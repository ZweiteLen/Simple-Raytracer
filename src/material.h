#ifndef MATERIALH
#define MATERIALH

#include <vector>
#include "ray.h"
#include "hittable.h"

struct hit_record;

struct light {
    // 0 = directional
    // 1 = point
    int type;
    vec3 lightVector;
    vec3 lightColour;
};
double max(double a, double b) {
    return (a>b) ? a : b; 
}

class material {
    public:
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, double c, vec3& attenuation, ray& scattered
        ) const = 0;
        virtual void blinn(
            const hit_record& rec, const light l, vec3& viewVector, ray& shadowRay, vec3& specular, double& NLAngle
        ) const = 0;
};

class lambertian : public material {
    public: 
        lambertian(const vec3& a) : albedo(a) {}
        virtual bool scatter
        (const ray& r_in, const hit_record& rec, double c, vec3& attenuation, ray& scattered) 
        const {
            vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            scattered = ray(rec.p, target-rec.p);
            attenuation = albedo;
            attenuation *= c;
            return true;
        }
        virtual void blinn(
            const hit_record& rec, const light l, vec3& viewVector, ray& shadowRay, vec3& specular, double& NLAngle
        ) const {
            vec3 lightPos = l.lightVector;
            vec3 shadowLightPos = lightPos + random_in_unit_sphere();
            if (l.type == 1) {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos - rec.p));
            } else {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos));
            }
            specular = vec3();
            NLAngle = 1.0;
        }
    
    public: 
        vec3 albedo;
};

class metal : public material {
    public: 
        metal(const vec3& a, double f) : albedo(a) { 
            if (f < 1) {
                fuzz = f;
            } else {
                fuzz = 1;
            }
        }
        virtual bool scatter
        (const ray& r, const hit_record& rec, double c, vec3& attenuation, ray& scattered) 
        const {
            vec3 reflected = reflect(unit_vector(r.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            attenuation *= c;
            return (dot(scattered.direction(), rec.normal));
        }
        virtual void blinn(
            const hit_record& rec, const light l, vec3& viewVector, ray& shadowRay, vec3& specular, double& NLAngle
        ) const {
            vec3 lightPos = l.lightVector;
            vec3 shadowLightPos = lightPos + random_in_unit_sphere();
            if (l.type == 1) {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos - rec.p));
            } else {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos));
            }
            specular = vec3();
            NLAngle = 0.0;
        }
    
    public: 
        vec3 albedo;
        double fuzz;
};

class dielectric : public material {
    public: 
        dielectric(double ri) : ref_inx(ri) {}

        virtual bool scatter
        (const ray& r, const hit_record& rec, double c, vec3& attenuation, ray& scattered)  
        const {
            vec3 outward_normal;
            vec3 reflected = reflect(r.direction(), rec.normal);
            double ni_over_nt;
            attenuation = vec3(1,1,1);
            vec3 refracted;
            double reflect_prob;
            double cosine;
            if (dot(r.direction(), rec.normal) > 0) {
                outward_normal = -rec.normal;
                ni_over_nt = ref_inx;
                cosine = ref_inx * dot(r.direction(), rec.normal / r.direction().length());
            } 
            else {
                outward_normal = rec.normal;
                ni_over_nt = 1.0 / ref_inx;
                cosine = -dot(r.direction(), rec.normal / r.direction().length());
            }
            if (refract(r.direction(), outward_normal, ni_over_nt, refracted)) {
                reflect_prob = schlick(cosine, ref_inx);
                scattered = ray(rec.p, refracted);
            } 
            else {
                reflect_prob = 1.0;
            }
            if (random_double() < reflect_prob) {
                scattered = ray(rec.p, reflected);
            }
            else {
                scattered = ray(rec.p, refracted);
            }
            return true;
        }
        virtual void blinn(
            const hit_record& rec, const light l, vec3& viewVector, ray& shadowRay, vec3& specular, double& NLAngle
        ) const {
            vec3 lightPos = l.lightVector;
            vec3 shadowLightPos = lightPos + random_in_unit_sphere();
            if (l.type == 1) {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos - rec.p));
            } else {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos));
            }
            specular = vec3();
            NLAngle = 1.0;
        }
    
    public: 
        double ref_inx;

    private:
        static double schlick(double cosine, double ref_inx) {
            double r0 = (1-ref_inx) / (1+ref_inx);
            r0 = r0 * r0;
            return r0 + (1-r0) * pow((1 - cosine), 5);
        }
};

class blinn_lambertian : public material {
    public: 
        blinn_lambertian(const vec3& a, double s) : albedo(a), shininess(s) {}
        virtual bool scatter
        (const ray& r, const hit_record& rec, double c, vec3& attenuation, ray& scattered) 
        const {
            vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            scattered = ray(rec.p, target-rec.p);
            attenuation = albedo;
            attenuation *= c;
            return true;
        }
        virtual void blinn(
            const hit_record& rec, const light l, vec3& viewVector, ray& shadowRay, vec3& specular, double& NLAngle
        ) const {
            vec3 lightPos = l.lightVector;
            vec3 shadowLightPos = lightPos + random_in_unit_sphere();
            if (l.type == 1) {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos - rec.p));
                lightPos -= rec.p;
            } else {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos));
            }
            vec3 n = unit_vector(rec.normal);
            lightPos = unit_vector(lightPos);
            NLAngle = max(0.1, dot(n, lightPos));
            vec3 halfVector = lightPos + viewVector;
            double specAmount = pow(max(0.0, dot(rec.normal, unit_vector(halfVector))), shininess);
            specular = specAmount * l.lightColour;
        }
    
    public: 
        vec3 albedo;
        double shininess;
};

class blinn_metal : public material {
    public: 
        blinn_metal(const vec3& a, double f, double s) : albedo(a), shininess(s) { 
            if (f < 1) {
                fuzz = f;
            } else {
                fuzz = 1;
            }
        }
        virtual bool scatter
        (const ray& r, const hit_record& rec, double c, vec3& attenuation, ray& scattered) 
        const {
            vec3 reflected = reflect(unit_vector(r.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            attenuation *= c;
            return (dot(scattered.direction(), rec.normal));
        }
        virtual void blinn(
            const hit_record& rec, const light l, vec3& viewVector, ray& shadowRay, vec3& specular, double& NLAngle
        ) const {
            vec3 lightPos = l.lightVector;
            vec3 shadowLightPos = lightPos + random_in_unit_sphere();
            if (l.type == 1) {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos - rec.p));
                lightPos -= rec.p;
            } else {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos));
            }
            vec3 n = unit_vector(rec.normal);
            lightPos = unit_vector(lightPos);
            NLAngle = 1.0;
            vec3 halfVector = unit_vector(lightPos) + viewVector;
            double specAmount = pow(max(0.0, dot(rec.normal, unit_vector(halfVector))), shininess);
            specular = specAmount * l.lightColour;
        }
    
    public: 
        vec3 albedo;
        double fuzz;
        double shininess;
};

class blinn_dielectric : public material {
    public: 
        blinn_dielectric(double ri, double s) : ref_inx(ri), shininess(s) {}

        virtual bool scatter
        (const ray& r, const hit_record& rec, double c, vec3& attenuation, ray& scattered) 
        const {
            vec3 outward_normal;
            vec3 reflected = reflect(r.direction(), rec.normal);
            double ni_over_nt;
            attenuation = vec3(1,1,1);
            vec3 refracted;
            double reflect_prob;
            double cosine;
            if (dot(r.direction(), rec.normal) > 0) {
                outward_normal = -rec.normal;
                ni_over_nt = ref_inx;
                cosine = ref_inx * dot(r.direction(), rec.normal / r.direction().length());
            } 
            else {
                outward_normal = rec.normal;
                ni_over_nt = 1.0 / ref_inx;
                cosine = -dot(r.direction(), rec.normal / r.direction().length());
            }
            if (refract(r.direction(), outward_normal, ni_over_nt, refracted)) {
                reflect_prob = schlick(cosine, ref_inx);
                scattered = ray(rec.p, refracted);
            } 
            else {
                reflect_prob = 1.0;
            }
            if (random_double() < reflect_prob) {
                scattered = ray(rec.p, reflected);
            }
            else {
                scattered = ray(rec.p, refracted);
            }
            return true;
        }
        virtual void blinn(
            const hit_record& rec, const light l, vec3& viewVector, ray& shadowRay, vec3& specular, double& NLAngle
        ) const {
            vec3 lightPos = l.lightVector;
            vec3 shadowLightPos = lightPos + random_in_unit_sphere();
            if (l.type == 1) {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos - rec.p));
                lightPos -= rec.p;
            } else {
                shadowRay = ray(rec.p, unit_vector(shadowLightPos));
            }
            vec3 n = unit_vector(rec.normal);
            lightPos = unit_vector(lightPos);
            NLAngle = max(0.0, dot(n, lightPos));
            vec3 halfVector = unit_vector(lightPos) + viewVector;
            double specAmount = pow(max(0.0, dot(rec.normal, unit_vector(halfVector))), shininess);
            specular = specAmount * l.lightColour;
        }
    
    public: 
        double ref_inx;
        double shininess;

    private:
        static double schlick(double cosine, double ref_inx) {
            double r0 = (1-ref_inx) / (1+ref_inx);
            r0 = r0 * r0;
            return r0 + (1-r0) * pow((1 - cosine), 5);
        }
};

#endif
