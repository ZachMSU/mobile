#ifndef CAMERA_H
#define CAMERA_H
#include "helpers.h"

struct cameraPos{
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat lookatX;
	GLfloat lookatY;
	GLfloat lookatZ;
};

class camera{
	//this is a singleton
private:
	cameraPos currentPos;
	cameraPos toPos;
	cameraPos fromPos;
	int framesLeft;
public:
	
	void animate(int frame);
	camera();
	void set(GLfloat x, GLfloat y, GLfloat z, GLfloat lookatX, GLfloat lookatY, GLfloat lookatZ);
	void set(cameraPos position);
	void setPos(GLfloat x, GLfloat y, GLfloat z);
	void setPos(cameraPos position);
	void setLookat(GLfloat lookatX, GLfloat lookatY, GLfloat lookatZ);
	void setLookat(cameraPos position);
	//possibly add FPS controls
};

#endif