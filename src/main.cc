#include <iostream>
#include <random>
#include <limits>
#include <algorithm>

#include "torus.h"
#include "sphere.h"
#include "cube.h"
#include "triangle.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"


vec3 ray_color(const ray& r, hittable *world, light l, int depth) {
    hit_record rec;
    if (depth > 50)
        return vec3(0,0,0);
    if (world->hit(r, 0.001, std::numeric_limits<double>::infinity(), rec)) {
        ray scattered;
        vec3 attenuation;

        vec3 lightPos = l.lightVector;
        vec3 shadowLightPos = lightPos + random_in_unit_sphere();
        vec3 lightColour = l.lightColour;
        vec3 viewVector = unit_vector(-r.direction());
        vec3 specular;
        double NLAngle;
        ray shadowRay;

        rec.mat_ptr->blinn(rec, l, viewVector, shadowRay, specular, NLAngle);
        double contribution = 1.0;

        hit_record temp = rec;
        if (world->hit(shadowRay, 0.001, std::numeric_limits<double>::infinity(), temp)) {
            contribution = 0.2;
            specular *= 0.0;
        }
        contribution = (NLAngle == 1.0) ? contribution : NLAngle;
        if (rec.mat_ptr->scatter(r, rec, contribution, attenuation, scattered)) {
            return attenuation * ray_color(scattered, world, l, depth+1) + specular;
        } 
        return vec3(0,0,0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}

hittable *random_scene() {
    int n = 500;
    hittable **list = new hittable*[n+1];
    list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian((vec3(0.5,0.5,0.5))));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double choose_mat = random_double();
            vec3 center = vec3(a+0.9*random_double(), 0.2, b+0.9*random_double());
            if ((center - vec3(4,2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {     
                    //diffuse
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(
                        random_double()*random_double(),random_double()*random_double(),random_double()*random_double())));
                } 
                else if (choose_mat < 0.95) {   
                    //metal
                    list[i++] = new sphere(center, 0.2, new metal(vec3(
                        0.5*(1+random_double()),0.5*(1+random_double()),0.5*(1+random_double())), 0.5*random_double()));
                }
                else { 
                    //glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0,1,0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4,1,0), 1.0, new lambertian(vec3(0.4,0.2,0.1)));
    list[i++] = new sphere(vec3(4,1,0), 1.0, new metal(vec3(0.7,0.6,0.0), 0.0));

    return new hittable_list(list, i);
}

int main () {
    int nx = 600; 
    int ny = 300;
    int ns = 150;

    std::cout << "P3\n" << nx << ' ' << ny << "\n255\n";

    light l = {1, vec3(0,6,0), vec3(1,1,1)};
    hittable *list[5];
    list[0] = new sphere(vec3(0,-101.5,-2), 100, new lambertian(vec3(0.8,0.8,0.0)));
    list[1] = new sphere(vec3(2,-1,-1), 0.5, new blinn_metal(vec3(0.8,0.6,0.2), 0, 20.0));
    list[2] = new sphere(vec3(-2,-1,-1), 0.5, new blinn_dielectric(1.5,20.0));
    list[3] = new sphere(vec3(0,-1,1), 0.5, new blinn_lambertian(vec3(0.2,0.2,0.8), 20.0));
    list[4] = new cube(vec3(-0.5,-0.5,-2),vec3(-0.5,-1.5,-2),vec3(0.5,-1.5,-2),new blinn_lambertian(vec3(1.0,0,0), 20.0));
    //list[1] = new torus(vec3(0,0,0),vec3(0,0,1), 1, 0.5, new blinn_lambertian(vec3(0.7,0.7,0.9), 20.0));
    hittable *world = new hittable_list(list, 5);
    //world = random_scene();

    vec3 lookfrom = vec3(-3, 1, 5);
    vec3 lookat = vec3(0,-0.5,-1);
    double dist_to_focus = (lookfrom-lookat).length();
    double apeture = 0.1;
    camera cam = camera(lookfrom, lookat, vec3(0,1,0), 40, double(nx)/double(ny), apeture, dist_to_focus);

    for (int j = ny-1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    	for (int i = 0; i < nx; i++) {
            vec3 col = vec3(0,0,0);
            for (int s = 0; s < ns; s++) {
                double u = double(i + random_double()) / double(nx);
                double v = double(j + random_double()) / double(ny);
                ray r = cam.get_ray(u,v);
                vec3 p = r.point_at_parameter(2.0);
                col += ray_color(r, world, l, 0);
            }
            col /= double(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
    		int ir = int(255.99*col[0]);
    		int ig = int(255.99*col[1]);
    		int ib = int(255.99*col[2]);
    		std::cout << ir << " " << ig << " " << ib << "\n";
    	}
    }
	
    return 0;
}