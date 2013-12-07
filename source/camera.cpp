#include "camera.h"
void printCamPosStruct(cameraPos in){
	cout<<"(pos)X:"<<in.x<<" Y:"<<in.y<<" Z:"<<in.z;
	cout<<"\t(lookat)X:"<<in.lookatX<<" Y:"<<in.lookatY<<" Z:"<<in.lookatZ;
}
camera::camera(GLdouble ifovy, GLdouble iwidth, GLdouble iheight, GLdouble izNear, GLdouble izFar) {
	upx = 0;
	upy = 1;
	upz = 0;
	fovy = ifovy;
	width = iwidth;
	height = iheight;
	zNear = izNear;
	zFar = izFar;
	currentPos.x = currentPos.y = currentPos.z = 0;
	currentPos.lookatX = currentPos.lookatY = currentPos.lookatZ = 0;
	toPos.x = toPos.y = toPos.z = 0;
	toPos.lookatX = toPos.lookatY = toPos.lookatZ = 0;
	fromPos.x = fromPos.y = fromPos.z = 0;
	fromPos.lookatX = fromPos.lookatY = fromPos.lookatZ = 0;
	framesLeft = 0;
	changed = true;
}
void camera::changePerspective(GLdouble ifovy, GLdouble iwidth, GLdouble iheight, GLdouble izNear, GLdouble izFar, bool t) {
	fovy = ifovy;
	width = iwidth;
	height = iheight;
	zNear = izNear;
	zFar = izFar;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, width/height, zNear, zFar);
	if (t) {
		gluLookAt(currentPos.x, currentPos.y, currentPos.z,
			  currentPos.lookatX, currentPos.lookatY, currentPos.lookatZ,
			  upx, upy, upz);
	}
}
void camera::copyStruct(cameraPos* from, cameraPos* to){
	to->x = from->x;
	to->y = from->y;
	to->z = from->z;
	to->lookatX = from->lookatX;
	to->lookatY = from->lookatY;
	to->lookatZ = from->lookatZ;
}
void camera::animate(int frames){
	framesLeft = frames;
	framesAnimate = frames;
	copyStruct(&currentPos, &fromPos);
}
void camera::setPos(GLfloat x, GLfloat y, GLfloat z, bool t){
	toPos.x = x;
	toPos.y = y;
	toPos.z = z;
	if (framesLeft <= 0) {
		currentPos.x = x;
		currentPos.y = y;
		currentPos.z = z;
	}
	changed = true;
	if (t) touch();
}
void camera::set(GLfloat x, GLfloat y, GLfloat z, GLfloat lookatX, GLfloat lookatY, GLfloat lookatZ, bool t){
	setPos(x, y, z);
	toPos.lookatX = lookatX;
	toPos.lookatY = lookatY;
	toPos.lookatZ = lookatZ;
	if (framesLeft <= 0) {
		currentPos.lookatX = lookatX;
		currentPos.lookatY = lookatY;
		currentPos.lookatZ = lookatZ;
	}
	if (t) touch();
}
void camera::set(cameraPos position, bool t){
	copyStruct(&position, &toPos);
	if (framesLeft <= 0) {
		copyStruct(&position, &currentPos);
	}
	changed = true;
	if (t) touch();
}
void camera::setPos(cameraPos position, bool t){
	toPos.x = position.x;
	toPos.y = position.y;
	toPos.z = position.z;
	if (framesLeft <= 0) {
		currentPos.x = position.x;
		currentPos.y = position.y;
		currentPos.z = position.z;
	}
	changed = true;
	if (t) touch();
}
void camera::touch(){
	if (framesLeft > 0) {
		//for animations
		if (framesAnimate == 0) {
			//prevent / 0
			framesLeft = 1;
		} else {
			float percentFrom = (float) framesLeft / framesAnimate;
			float percentTo = 1 - percentFrom;

			currentPos.x = (percentFrom * fromPos.x) + (percentTo * toPos.x);
			currentPos.y = (percentFrom * fromPos.y) + (percentTo * toPos.y);
			currentPos.z = (percentFrom * fromPos.z) + (percentTo * toPos.z);
			currentPos.lookatX = (percentFrom * fromPos.lookatX) + (percentTo * toPos.lookatX);
			currentPos.lookatY = (percentFrom * fromPos.lookatY) + (percentTo * toPos.lookatY);
			currentPos.lookatZ = (percentFrom * fromPos.lookatZ) + (percentTo * toPos.lookatZ);
		}
		framesLeft --;
		changed = true;
	}
	if (changed) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fovy, width/height, zNear, zFar);
		gluLookAt(currentPos.x, currentPos.y, currentPos.z,
				  currentPos.lookatX, currentPos.lookatY, currentPos.lookatZ,
				  upx, upy, upz);
		changed = false;
	}
}