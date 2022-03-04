#ifndef _Light_H
#define _Light_H

#include "Source.h"
#include "Vect.h"
#include "Color.h"

class Light : public Source {
	Vect position;
	Color color;
	
	public:
	
	Light ();
	
	Light (Vect, Color);
	
	// les fonctions des methodes
	virtual Vect getLightPosition () { return position; }
	virtual Color getLightColor () { return color; }
	
};

Light::Light () {
	position = Vect(0,0,0);
	color = Color(1,1,1, 0);
}

Light::Light (Vect p, Color c) {
	position = p;
	color = c;
}

#endif
