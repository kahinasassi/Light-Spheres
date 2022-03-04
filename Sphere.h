#ifndef _SPHERE_H
#define _SPHERE_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"

class Sphere : public Object {
	Vect center;
	double radius;
	Color color;
	
	public:
	
	Sphere ();
	
	Sphere (Vect, double, Color);
	
	// les fonctions des methodes
	Vect getSphereCenter () { return center; }
	double getSphereRadius () { return radius; }
	virtual Color getColor () { return color; }
	
	virtual Vect getNormalAt(Vect point) {
		// la normale pointe toujours loin du centre d'une sphère
		Vect normal_Vect = point.vectAdd(center.negative()).normalize();
		return normal_Vect;
	}
	
	virtual double findIntersection(Ray ray) {
		Vect ray_origin = ray.getRayOrigin();
		double ray_origin_x = ray_origin.getVectX();
		double ray_origin_y = ray_origin.getVectY();
		double ray_origin_z = ray_origin.getVectZ();
		
		Vect ray_direction = ray.getRayDirection();
		double ray_direction_x = ray_direction.getVectX();
		double ray_direction_y = ray_direction.getVectY();
		double ray_direction_z = ray_direction.getVectZ();
		
		Vect sphere_center = center;
		double sphere_center_x = sphere_center.getVectX();
		double sphere_center_y = sphere_center.getVectY();
		double sphere_center_z = sphere_center.getVectZ();
		
		double a = 1; // normalisé
		double b = (2*(ray_origin_x - sphere_center_x)*ray_direction_x) + (2*(ray_origin_y - sphere_center_y)*ray_direction_y) + (2*(ray_origin_z - sphere_center_z)*ray_direction_z);
		double c = pow(ray_origin_x - sphere_center_x, 2) + pow(ray_origin_y - sphere_center_y, 2) + pow(ray_origin_z - sphere_center_z, 2) - (radius*radius);
		
		double discriminant = b*b - 4*c;
		
		if (discriminant > 0) {
			/// le rayon croise la sphère
			
			// la premiere racine
			double root_1 = ((-1*b - sqrt(discriminant))/2) - 0.000001;
			
			if (root_1 > 0) {
				// la première racine est la plus petite racine positive
				return root_1;
			}
			else {
				// la deuxieme racine est la plus petite racine positive
				double root_2 = ((sqrt(discriminant) - b)/2) - 0.000001;
				return root_2;
			}
		}
		else {
			// le rayon a manqué la sphère
			return -1;
		}
	}
	
};

Sphere::Sphere () {
	center = Vect(0,0,0);
	radius = 1.0;
	color = Color(0.5,0.5,0.5, 0);
}

Sphere::Sphere (Vect centerValue, double radiusValue, Color colorValue) {
	center = centerValue;
	radius = radiusValue;
	color = colorValue;
}

#endif
