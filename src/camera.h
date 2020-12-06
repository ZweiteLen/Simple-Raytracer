#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

const double pi = 3.1415926535897932385;

vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0 * vec3(random_double(), random_double(), 0) - vec3(1,1,0);
    } while (dot(p,p) >= 1.0);
    return p;
}

class camera {
    public:
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, double vfov, double aspect, double apeture, double focus_dist) {
            lens_radius = apeture / 2;
            double theta = vfov * pi / 180;
            double half_height = tan(theta / 2);
            double half_width = aspect * half_height;
            origin = lookfrom;
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            lower_left_corner = origin - half_width*focus_dist*u - half_height*focus_dist*v - focus_dist*w;
            horizonal = 2*half_width*focus_dist*u;
            vertical = 2*half_height*focus_dist*v;
        }
        ray get_ray(double s, double t) {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
            return ray(origin+offset, lower_left_corner + s*horizonal + t*vertical - origin - offset);
        }

    public: 
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizonal;
        vec3 vertical;
        vec3 u, v, w;
        double lens_radius;
};

#endif