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
	bool changed;

	int framesLeft, framesAnimate;
	GLdouble upx, upy, upz;

	void copyStruct(cameraPos* from, cameraPos* to);
	
public:
	cameraPos currentPos, toPos, fromPos;
	GLdouble fovy, width, height, zNear, zFar;	
	void animate(int frames);
	camera(GLdouble ifovy, GLdouble iwidth, GLdouble iheight, GLdouble izNear, GLdouble izfar);
	void changePerspective(GLdouble ifovy, GLdouble iwidth, GLdouble iheight, GLdouble izNear, GLdouble izFar, bool t = true);
	//set() sets the camera and lookat
	void set(GLfloat x, GLfloat y, GLfloat z, GLfloat lookatX, GLfloat lookatY, GLfloat lookatZ, bool t = false);
	void set(cameraPos position, bool t = false);
	//setPos() sets the location of the camera
	void setPos(GLfloat x, GLfloat y, GLfloat z, bool t = false);
	void setPos(cameraPos position, bool t = false);
	//setLookat() sets the lookat of the camera
	void setLookat(GLfloat lookatX, GLfloat lookatY, GLfloat lookatZ);
	void setLookat(cameraPos position);
	void touch();
	void camera::setPerspective(){
		gluPerspective(fovy, width/height, zNear, zFar);
				gluLookAt(currentPos.x, currentPos.y, currentPos.z,
			  currentPos.lookatX, currentPos.lookatY, currentPos.lookatZ,
			  upx, upy, upz);
	}
	//possibly add FPS controls
};

#endif