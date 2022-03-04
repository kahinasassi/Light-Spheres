# light sphere
Our solution is categorized as 9 classes and a main menu. Hence each class represents an essential element to achieve the desired scene.
The classes are as follows:
- Camera: this is the camera of the scene from which we are going to create.
- Color: the representation of colors (shape: rgba);
- Light: to represent a point source of light (lamp);
- Vect: base class for geometry (for manipulation of
objects). It gathers all the information on the intersection of a
ray with an object.
- Sphere: a concrete class to represent a sphere;
- Source: in order to have the position of the light and its color;
- Plane: to define the ground;
- Ray: it is for the rays launched such that for each pixel of
the image, we launch a ray towards the scene (starting from the camera and passing through the pixel). AND at each intersection of ray with a
surface, we cast a ray in the direction of the light source
 to see if the surface is illuminated;
- Object: so that each wrapped OpenGL object supports the
reference counting. It is common to all the objects of the
scene; gathers its geometry, its material and its primitive
OpenGL.
And the “main” which contains several definitions of the data structures (record to define the colors, record to keep the image of the scene, etc.) and calls to the different functions.
▪ To generate shadows: just check that the light source is clearly visible from the point of intersection. For that, we launch a ray from the point of intersection in the direction of the light, and we test if there is an intersection.
