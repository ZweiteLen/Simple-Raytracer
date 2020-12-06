#ifndef TORUSH
#define TORUSH

#include "hittable.h"
#include "algebra.h"

class torus: public hittable {
    public:
        torus() {}
        torus(vec3 cen, vec3 n, double br, double sr, material* m) : 
        center(cen), normal(n), R1(br), R2(sr), mat_ptr(m) {}
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
        
    public:
        vec3 center;
        vec3 normal;
        double R1;
        double R2;
        material *mat_ptr;
};

bool torus::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 rD = r.direction();
    vec3 rO = r.origin();

    double dX = rD.x();
    double dY = rD.y();
    double dZ = rD.z();
    double oX = rO.x();
    double oY = rO.y();
    double oZ = rO.z();
    
    double g = 4.0*R1*R1*((dX*dX) + (dY*dY));
    double h = 8.0*R1*R1*((oX*dX) + (oY*dY));
    double i = 4.0*R1*R1*((oX*oX) + (oY*oY));
    double j = r.direction().squared_length();
    double k = 2.0*dot(r.origin(), r.direction());
    double l = r.origin().squared_length() + ((R1*R1) - (R2*R2));

    double solutions[4];
    int numRealRoots = Algebra::SolveQuarticEquation(
        j*j, 
        2.0*j*k,
        (2.0*j*l) + (k*k) - g,
        (2.0*k*l) - h,
        (l*l) - i,
        solutions
    );
    if (numRealRoots == 0) { return false; }
    int numPosRoots = 0;
    for (int c = 0; c < numRealRoots; ++c)
    { if(solutions[c] > 1.0e-4) { solutions[numPosRoots++] = solutions[c]; } }
    if (numPosRoots == 0) { return false; }
    double t = solutions[0];
    for (int c = 1; c < numPosRoots; c++)
    { if(solutions[c] < t) { t = solutions[c]; } }
    if (t < t_max && t > t_min) {
        rec.t = t;
        rec.p = r.point_at_parameter(rec.t);
        double a = 1.0 - (R1 / sqrt(rec.p.x()*rec.p.x() + rec.p.y()*rec.p.y()));
        rec.normal = unit_vector(vec3(a*rec.p.x(), a*rec.p.y(), rec.p.z()));
        rec.mat_ptr = mat_ptr;
        return true;
    }
    return false;
}

#endif