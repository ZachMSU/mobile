#ifndef PICTURE_H
#define PICTURE_H

#define DEFAULT_WIDTH 600
#define DEFAULT_HEIGHT 600
#define DEFAULT_NAME "untitled"
#define DEFAULT_DESCRIPTION "no description"
#include "helpers.h"
class picture{
	/* This class provides storage and operations for each image.
	 * The constructor creates a picture object, and loads the image from the file
	 * display(x, y, z, angle, [selected]) renders the image on the screen at the coords
	 * and rotated at the specifyed angle(about it's own center)
	 */
private:
	//char name[100];
	//char description[1000];
	GLuint loadTexture(string);
	float texWidth;
	float texHeight;
	GLuint texture;
	bool loaded;
public:
	string name;
	string description;
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat angle;
	GLfloat width;
	GLfloat height;
	bool highlighted;
	//constructors
	//picture(string filename);
	//picture(string filename, GLfloat width, GLfloat height);
	//picture(string filename, string name, string description); //filename, name, description
	picture(string filename, string name = DEFAULT_NAME, string description = DEFAULT_DESCRIPTION);
	void display(GLfloat lx, GLfloat ly, GLfloat lz, GLfloat langle, bool lhighlighted = false){
		x = lx;
		y = ly;
		z = lz;
		angle = langle;
		highlighted = lhighlighted;
		display();
	}
	void display();
};
#endif