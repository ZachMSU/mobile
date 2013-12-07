/* Provides helper functions and global defines
 *
 */

#ifndef HELPERS_H
#define HELPERS_H

#define DEBUG

#define _USE_MATH_DEFINES
using namespace std;
#include <iostream>
#include <GL/glut.h>				// include GLUT library
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include "soil\SOIL.h"
#include <time.h>

//GLfloat random();

void pause();

double toDegree(double);
//converts radains to degrees

double toRadian(double);
//converts degrees to radians

void renderBitmapString(double, double, double, void*, string);
//displays the string on the screen (xcoord, ycoord, font, diplaystring)

void coordinates(GLfloat);
//draws the coordinate axis to the distance specifyed
#endif