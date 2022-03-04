#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Source.h"
#include "Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"


using namespace std;

struct RGBType {
	double r;
	double g;
	double b;
};

void savebmp (const char *filename, int w, int h, int dpi, RGBType *data) {
	FILE *f;
	int k = w*h;
	int s = 4*k;
	int filesize = 54 + s;
	
	double factor = 39.375;
	int m = static_cast<int>(factor);
	
	int ppm = dpi*m;
	
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};
	
	bmpfileheader[ 2] = (unsigned char)(filesize);
	bmpfileheader[ 3] = (unsigned char)(filesize>>8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);
	
	bmpinfoheader[ 4] = (unsigned char)(w);
	bmpinfoheader[ 5] = (unsigned char)(w>>8);
	bmpinfoheader[ 6] = (unsigned char)(w>>16);
	bmpinfoheader[ 7] = (unsigned char)(w>>24);
	
	bmpinfoheader[ 8] = (unsigned char)(h);
	bmpinfoheader[ 9] = (unsigned char)(h>>8);
	bmpinfoheader[10] = (unsigned char)(h>>16);
	bmpinfoheader[11] = (unsigned char)(h>>24);
	
	bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s>>8);
	bmpinfoheader[23] = (unsigned char)(s>>16);
	bmpinfoheader[24] = (unsigned char)(s>>24);
	
	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm>>8);
	bmpinfoheader[27] = (unsigned char)(ppm>>16);
	bmpinfoheader[28] = (unsigned char)(ppm>>24);
	
	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm>>8);
	bmpinfoheader[31] = (unsigned char)(ppm>>16);
	bmpinfoheader[32] = (unsigned char)(ppm>>24);
	
	f = fopen(filename,"wb");
	
	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
	
	for (int i = 0; i < k; i++) {
		RGBType rgb = data[i];
		
		double red = (data[i].r)*255;
		double green = (data[i].g)*255;
		double blue = (data[i].b)*255;
		
		unsigned char color[3] = {(int)floor(blue),(int)floor(green),(int)floor(red)};
		
		fwrite(color,1,3,f);
	}
	
	fclose(f);
}

int winningObjectIndex(vector<double> object_intersections) {
	// retourner l'indice de l'intersection gagnante
	int index_of_minimum_value;
	
	// éviter les calculs inutiles
	if (object_intersections.size() == 0) {
		// s'il n'y a pas d'intersections
		return -1;
	}
	else if (object_intersections.size() == 1) {
		if (object_intersections.at(0) > 0) {
			// si cette intersection est supérieure à zéro, alors c'est notre indice de valeur minimale
			return 0;
		}
		else {
			// sinon la seule valeur d'intersection est négative
			return -1;
		}
	}
	else {
		// sinon, il y a plus d'une intersection
		// trouver d'abord la valeur maximale
		
		double max = 0;
		for (int i = 0; i < object_intersections.size(); i++) {
			if (max < object_intersections.at(i)) {
				max = object_intersections.at(i);
			}
		}
		
		// puis, en partant de la valeur maximale, trouver la valeur positive minimale
		if (max > 0) {
			// nous ne voulons que des intersections positives
			for (int index = 0; index < object_intersections.size(); index++) {
				if (object_intersections.at(index) > 0 && object_intersections.at(index) <= max) {
					max = object_intersections.at(index);
					index_of_minimum_value = index;
				}
			}
			
			return index_of_minimum_value;
		}
		else {
			// tous les intersections sont négatives
			return -1;
		}
	}
}

Color getColorAt(Vect intersection_position, Vect intersecting_ray_direction, vector<Object*> scene_objects, int index_of_winning_object, vector<Source*> light_sources, double accuracy, double ambientlight) {
	
	Color winning_object_color = scene_objects.at(index_of_winning_object)->getColor();
	Vect winning_object_normal = scene_objects.at(index_of_winning_object)->getNormalAt(intersection_position);
	
	if (winning_object_color.getColorSpecial() == 2) {
		//  motif de carreaux de sol
		
		int square = (int)floor(intersection_position.getVectX()) + (int)floor(intersection_position.getVectZ());
		
		if ((square % 2) == 0) {
			// tuile noire
			winning_object_color.setColorRed(0);
			winning_object_color.setColorGreen(0);
			winning_object_color.setColorBlue(0);
		}
		else {
			// tuile blanche
			winning_object_color.setColorRed(1);
			winning_object_color.setColorGreen(1);
			winning_object_color.setColorRed(1);
		}
	}
	
	Color final_color = winning_object_color.colorScalar(ambientlight);
	
	if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
		// réflexion d'objets à intensité spéculaire
		double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
		Vect scalar1 = winning_object_normal.vectMult(dot1);
		Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
		Vect scalar2 = add1.vectMult(2);
		Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
		Vect reflection_direction = add2.normalize();
		
		Ray reflection_ray (intersection_position, reflection_direction);
		
		// déterminer ce que le rayon croise en premier
		vector<double> reflection_intersections;
		
		for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
			reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
		}
		
		int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);
		
		if (index_of_winning_object_with_reflection != -1) {
			// rayon de réflexion a manqué tout le reste
			if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
				// déterminer la position et la direction au point d'intersection avec le rayon de réflexion
				// le rayon n'affecte la couleur que s'il est réfléchi par quelque chose
				
				Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
				Vect reflection_intersection_ray_direction = reflection_direction;
				
				Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);
				
				final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));
			}
		}
	}
	
	for (int light_index = 0; light_index < light_sources.size(); light_index++) {
		Vect light_direction = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
		
		float cosine_angle = winning_object_normal.dotProduct(light_direction);
		
		if (cosine_angle > 0) {
			//test pour les ombres
			bool shadowed = false;
			
			Vect distance_to_light = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
			float distance_to_light_magnitude = distance_to_light.magnitude();
			
			Ray shadow_ray (intersection_position, light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize());
			
			vector<double> secondary_intersections;
			
			for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++) {
				secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
			}
			
			for (int c = 0; c < secondary_intersections.size(); c++) {
				if (secondary_intersections.at(c) > accuracy) {
					if (secondary_intersections.at(c) <= distance_to_light_magnitude) {
						shadowed = true;
					}
				}
				break;
			}
			
			if (shadowed == false) {
				final_color = final_color.colorAdd(winning_object_color.colorMultiply(light_sources.at(light_index)->getLightColor()).colorScalar(cosine_angle));
				
				if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
					// special [0-1]
					double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
					Vect scalar1 = winning_object_normal.vectMult(dot1);
					Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
					Vect scalar2 = add1.vectMult(2);
					Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
					Vect reflection_direction = add2.normalize();
					
					double specular = reflection_direction.dotProduct(light_direction);
					if (specular > 0) {
						specular = pow(specular, 10);
						final_color = final_color.colorAdd(light_sources.at(light_index)->getLightColor().colorScalar(specular*winning_object_color.getColorSpecial()));
					}
				}
				
			}
			
		}
	}
	
	return final_color.clip();
}

int thisone;


	
	
int main (int argc, char *argv[]) {
	cout << "rendering ..." << endl;
	
	clock_t t1, t2;
	t1 = clock();
	
	int dpi = 72;
	int width = 640;
	int height = 480;
	int n = width*height;
	RGBType *pixels = new RGBType[n];
	
	int aadepth = 1;
	double aathreshold = 0.1;
	double aspectratio = (double)width/(double)height;
	double ambientlight = 0.2;
	double accuracy = 0.00000001;
	
	 /**************************** scène 1: les sphères sur le sol *************************/
	Vect O (0,0,0);
	Vect X (1,0,0);
	Vect Y (0,1,0);
	Vect Z (0,0,1);
	
	Vect new_sphere_location (1.75, -0.25, 0);
	
	Vect campos (3, 1.5, -4);
	
	Vect look_at (0, 0, 0);
	Vect diff_btw (campos.getVectX() - look_at.getVectX(), campos.getVectY() - look_at.getVectY(), campos.getVectZ() - look_at.getVectZ());
	
	Vect camdir = diff_btw.negative().normalize();
	Vect camright = Y.crossProduct(camdir).normalize();
	Vect camdown = camright.crossProduct(camdir);
	Camera scene_cam (campos, camdir, camright, camdown);
	
	Color white_light (1.0, 1.0, 1.0, 0);
	Color pretty_green (0.5, 1.0, 0.5, 0.3);
	Color maroon (0.5, 0.25, 0.25, 0);
	Color tile_floor (1, 1, 1, 2);
	Color gray (0.5, 0.5, 0.5, 0);
	Color black (0.0, 0.0, 0.0, 0);
	
	Vect light_position (-7,10,-10);
	Light scene_light (light_position, white_light);
	vector<Source*> light_sources;
	light_sources.push_back(dynamic_cast<Source*>(&scene_light));
	
	// scene objects
	Sphere scene_sphere (O, 1, pretty_green);
	Sphere scene_sphere2 (new_sphere_location, 0.5, maroon);
	Plane scene_plane (Y, -1, tile_floor);
	vector<Object*> scene_objects;
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere2));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));
	
	int thisone, aa_index;
	double xamnt, yamnt;
	double tempRed, tempGreen, tempBlue;
	
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			thisone = y*width + x;
			
			// commencer par un pixel vide
			double tempRed[aadepth*aadepth];
			double tempGreen[aadepth*aadepth];
			double tempBlue[aadepth*aadepth];
			
			for (int aax = 0; aax < aadepth; aax++) {
				for (int aay = 0; aay < aadepth; aay++) {
			
					aa_index = aay*aadepth + aax;
					
					srand(time(0));
					
					// créer le rayon de la caméra vers ce pixel
					if (aadepth == 1) {
					
						// commencer sans anti-crénelage
						if (width > height) {
							// l'image est plus large que haute
							xamnt = ((x+0.5)/width)*aspectratio - (((width-height)/(double)height)/2);
							yamnt = ((height - y) + 0.5)/height;
						}
						else if (height > width) {
							// l'imageur est plus grand que large
							xamnt = (x + 0.5)/ width;
							yamnt = (((height - y) + 0.5)/height)/aspectratio - (((height - width)/(double)width)/2);
						}
						else {
							// l'image est carrée
							xamnt = (x + 0.5)/width;
							yamnt = ((height - y) + 0.5)/height;
						}
					}
					else {
						// anti-crénelage
						if (width > height) {
							// l'image est plus large que haute
							xamnt = ((x + (double)aax/((double)aadepth - 1))/width)*aspectratio - (((width-height)/(double)height)/2);
							yamnt = ((height - y) + (double)aax/((double)aadepth - 1))/height;
						}
						else if (height > width) {
							// l'imageur est plus grand que large
							xamnt = (x + (double)aax/((double)aadepth - 1))/ width;
							yamnt = (((height - y) + (double)aax/((double)aadepth - 1))/height)/aspectratio - (((height - width)/(double)width)/2);
						}
						else {
							// l'image est carrée
							xamnt = (x + (double)aax/((double)aadepth - 1))/width;
							yamnt = ((height - y) + (double)aax/((double)aadepth - 1))/height;
						}
					}
					
					Vect cam_ray_origin = scene_cam.getCameraPosition();
					Vect cam_ray_direction = camdir.vectAdd(camright.vectMult(xamnt - 0.5).vectAdd(camdown.vectMult(yamnt - 0.5))).normalize();
					
					Ray cam_ray (cam_ray_origin, cam_ray_direction);
					
					vector<double> intersections;
					
					for (int index = 0; index < scene_objects.size(); index++) {
						intersections.push_back(scene_objects.at(index)->findIntersection(cam_ray));
					}
					
					int index_of_winning_object = winningObjectIndex(intersections);
					
					if (index_of_winning_object == -1) {
						// mettre le fond noir
						tempRed[aa_index] = 0;
						tempGreen[aa_index] = 0;
						tempBlue[aa_index] = 0;
					}
					else{
						// l'index correspond à un objet de notre scène
						if (intersections.at(index_of_winning_object) > accuracy) {
							// déterminer les vecteurs de position et de direction au point d'intersection
							
							Vect intersection_position = cam_ray_origin.vectAdd(cam_ray_direction.vectMult(intersections.at(index_of_winning_object)));
							Vect intersecting_ray_direction = cam_ray_direction;
		
							Color intersection_color = getColorAt(intersection_position, intersecting_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight);
							
							tempRed[aa_index] = intersection_color.getColorRed();
							tempGreen[aa_index] = intersection_color.getColorGreen();
							tempBlue[aa_index] = intersection_color.getColorBlue();
						}
					}
				}
			}
			
			// faire la moyenne de la couleur du pixel
			double totalRed = 0;
			double totalGreen = 0;
			double totalBlue = 0;
			
			for (int iRed = 0; iRed < aadepth*aadepth; iRed++) {
				totalRed = totalRed + tempRed[iRed];
			}
			for (int iGreen = 0; iGreen < aadepth*aadepth; iGreen++) {
				totalGreen = totalGreen + tempGreen[iGreen];
			}
			for (int iBlue = 0; iBlue < aadepth*aadepth; iBlue++) {
				totalBlue = totalBlue + tempBlue[iBlue];
			}
			
			double avgRed = totalRed/(aadepth*aadepth);
			double avgGreen = totalGreen/(aadepth*aadepth);
			double avgBlue = totalBlue/(aadepth*aadepth);
			
			pixels[thisone].r = avgRed;
			pixels[thisone].g = avgGreen;
			pixels[thisone].b = avgBlue;
		}
	}
	
	savebmp("SCENE.bmp",width,height,dpi,pixels);
	
	delete pixels, tempRed, tempGreen, tempBlue;
	
	t2 = clock();
	float diff = ((float)t2 - (float)t1)/1000;
	
	cout << diff << " seconds" << endl;
	
	return 0;
}
