#ifndef SPHEREH
#define SPHEREH

#include "hittable.h"

class sphere: public hittable {
    public:
        sphere() {}
        sphere(vec3 cen, double r, material* m) : center(cen), radius(r), mat_ptr(m) {}
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
        
    public:
        vec3 center;
        double radius;
        material *mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    double a = r.direction().squared_length();
    double b = dot(oc, r.direction());
    double c = oc.squared_length() - radius*radius;
    double discriminant = b*b - a*c;
    if (discriminant > 0) {
        double temp = (-b - sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = unit_vector((rec.p - center) / radius);
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = unit_vector((rec.p - center) / radius);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

#endif