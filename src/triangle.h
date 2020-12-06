#ifndef TRIANGLEH
#define TRIANGLEH

#include "hittable.h"

class triangle: public hittable {
    public:
        triangle() {}
        triangle(vec3& c1, vec3& c2, vec3& c3, material* m) : p1(c1), p2(c2), p3(c3), mat_ptr(m) { normal = cross((p1 - p2), (p3 - p2)); };
        triangle(vec3& c1, vec3& c2, vec3& c3, vec3& n, material* m) : p1(c1), p2(c2), p3(c3), normal(n), mat_ptr(m) {};
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
        
    public:
        vec3 p1, p2, p3, normal;
        material *mat_ptr;
};

bool in_zero_one(double x) {
    return (x >= 0 && x <= 1);
}

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 w = p1 - r.origin();
    double a = dot(w, normal);
    double b = dot(r.direction(), normal);
    if (b == 0) { return false; }
    double k = a / b;
    if (k < t_min || k > t_max) { return false; }
    vec3 i = (r.direction() * k) + r.origin();

    vec3 ab = p2 - p1;
    vec3 ac = p3 - p1;
    vec3 pa = p1 - i;
    vec3 pb = p2 - i;
    vec3 pc = p3 - i;
    double At = cross(ab, ac).length() / 2;
    double Aa = cross(pb, pc).length() / 2;
    double Ab = cross(pa, pc).length() / 2;
    double Ac = cross(pa, pb).length() / 2;
    double alpha = Aa / At;
    double beta = Ab / At;
    double gamma = Ac / At;

    if (!in_zero_one(alpha) || !in_zero_one(beta) || !in_zero_one(gamma)) {
        return false;
    }
    if ((alpha + beta + gamma) > 1.001) { return false; }

    rec.t = k;
    rec.p = i;
    rec.normal = normal;
    rec.mat_ptr = mat_ptr;
    return true;
}

#endif