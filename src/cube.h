#ifndef BOXH
#define BOXH

#include "hittable.h"
#include "hittable_list.h"
#include "triangle.h"

class cube: public hittable {
    public:
        cube() {}
        cube(vec3& p1, vec3& p2, vec3& p3, material* m) {
            /*
                 c4 ______ c3
                /          /
                p1 ______ c2
                 c8 ______ c7
                /          /
                p2 ______ p3
            */
            vec3 up = p1 - p2;
            vec3 side = p3 - p2;
            vec3 back = unit_vector(cross(up, side));
            back *= side.length();
            vec3 corners[8] = { p1, p1 + side, (p1 + side) + back, p1 + back, p2, p3, p3 + back, p2 + back };
            vec3 a, b;
            vec3 n;
            for (int i = 0; i < 8; i++){   
                if (i % 2 == 0)
                {
                    int j = i/2;
                    int s = (5 + j < 8) ? 5 + j : 4;
                    a = corners[s] - corners[4+j];
                    b = corners[0+j] - corners[4+j];
                    n = unit_vector(cross(a, b));
                    n = (i > 1) ? -n : n;
                    triangles[i] = new triangle(corners[0+j], corners[4+j], corners[s], n, m);
                } else {
                    int j = (i-1)/2;
                    int s = (5 + j < 8) ? 5 + j : 4;
                    int c = (1 + j < 3) ? 1 + j : 0;
                    a = corners[0+j] - corners[(1+j) % 4];
                    b = corners[s] - corners[(1+j) % 4];
                    n = unit_vector(cross(a, b));
                    n = (i > 1) ? -n : n;
                    triangles[i] = new triangle(corners[0+j], corners[c], corners[s], n, m);
                }
            }
            // top face
            a = corners[1] - corners[0];
            b = corners[3] - corners[0];
            n = unit_vector(cross(a, b));
            triangles[8] = new triangle(corners[3], corners[0], corners[1], n, m);
            triangles[9] = new triangle(corners[3], corners[2], corners[1], n, m);
            // bottom face
            triangles[10] = new triangle(corners[7], corners[4], corners[5], -n, m);
            triangles[11] = new triangle(corners[7], corners[6], corners[5], -n, m);
            triangle_list = hittable_list(triangles, 12);
           
            mat_ptr = m;
        }
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
        
    public:
        hittable_list triangle_list;
        hittable *triangles[12];
        material *mat_ptr;
};

bool cube::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return triangle_list.hit(r, t_min, t_max, rec);
}

#endif