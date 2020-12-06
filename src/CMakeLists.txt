cmake_minimum_required ( VERSION 3.1.0 )

project (RayTracer
            VERSION 3.0.0
            LANGUAGES CXX
)

#Set to c++11
set ( CMAKE_CXX_STANDARD 11 )

#Sources

set ( SOURCE_ONE_WEEKEND
	algebra.h
	algebra.cpp
	camera.h
	ray.h
	vec3.h
	hittable.h
	hittable_list.h
	sphere.h
	cube.h
	torus.h
	triangle.h
	main.cc
)

#Executables
add_executable(Raytracer ${SOURCE_ONE_WEEKEND})