#include "camera.h"

camera::camera() {
	currentPos.x = currentPos.y = currentPos.z = 0;
	currentPos.lookatX = currentPos.lookatY = currentPos.lookatZ = 0;
	toPos.x = toPos.y = toPos.z = 0;
	toPos.lookatX = toPos.lookatY = toPos.lookatZ = 0;
	fromPos.x = fromPos.y = fromPos.z = 0;
	fromPos.lookatX = fromPos.lookatY = fromPos.lookatZ = 0;
	framesLeft = 0;
}