/*===================================================================================
Zach Hudson
Folder: Hudson923
Term Project

PROGRAMMER:				Zach Hudson and Chase Coates
PATH:					
COURSE:					CSC 525/625
LAST MODIFIED DATE:		Nov. 22, 2013
DESCRIPTION:			3D Mobile
NOTE:					N/A
FILES:					proj.cpp, (hwProject.sln, ...)
IDE/COMPILER:			MicroSoft Visual Studio 2012
INSTRUCTION FOR COMPILATION AND EXECUTION:
1.		Double click on hwProject.sln	to OPEN the project
2.		Press Ctrl+F7					to COMPILE
3.		Press Ctrl+Shift+B				to BUILD (COMPILE+LINK)
4.		Press Ctrl+F5					to EXECUTE
===================================================================================*/

#include <sstream>
#include "helpers.h"
#include "picture.h"
#include "tinydir.h"

#define editor_title "3D Mobile"
#define DEFAULT_MANIFEST "C:\\temp\\project\\manifest.txt"
#define SPEED_RANGE 0.5
#define WOBBLE 2
using namespace std;

//picture* pics[300];
//int picCount = 0;
//double myangle = 0;

static bool spinning = true;
static const int FPS = 30;
static GLfloat currentAngleOfRotation[4] = { 0, 30, 90, 180 };
picture* lookat;

GLfloat random(){
	GLfloat out = ((GLfloat) rand() / RAND_MAX) * (SPEED_RANGE * 2) - SPEED_RANGE;
	//cout<<"->Random:"<<out<<"\n";
	return out;
}

struct mouseClick{
	bool clicked;
	int x;
	int y;
	GLfloat lastZ;
	picture* lastPic;
	mouseClick(){
		clicked = false;
		x = 0;
		y = 0;
		lastZ = 0;
		lastPic = NULL;
	}
};

mouseClick click;

struct treeNode{
	picture* pic;
	treeNode* right;
	treeNode* left;
	GLfloat xpos;
	GLfloat ypos;
	GLfloat zpos;
	GLfloat angle;
	GLfloat radius;
	GLfloat speed;
	treeNode(GLfloat x, GLfloat y, GLfloat z, GLfloat r){
		pic = NULL;
		right = NULL;
		left = NULL;
		xpos = x;
		ypos = y;
		zpos = z;
		angle = 0.0;
		speed = random();
		radius = r;
	}
	treeNode() {
		treeNode(0.0, 0.0, 0.0, 0.0);
	}
};

treeNode* root;

int height = 600,
	width  = 1000,
	depth = 600;

double distanceMultiplier = 6;

int globalX, globalY;
//double angle = 0;

//int followPicIndex = -1;

bool hideHelp = true,
	 hideCoord = false,
	 tracking = false;

double camera[9] =  {0, -500, depth*distanceMultiplier*1.5, 0, -1500, 0, 0, 1, 0};

//cameraPos initalPosition = {0, -500, depth*distanceMultiplier*1.5, 0, -1500, 0};


void reshape(int w, int h){
    width = w;
    height = h;
    glViewport(0, 0, w, h);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(70.0, width/height, 1, depth*distanceMultiplier*1000);
}
void resetCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera[0] = camera[3] = camera[5] = 0;
	camera[4] = -1500;
	camera[1] = -500;
	camera[2] = depth*distanceMultiplier*1.5;
	//gluPerspective(70.0, width/height, 1, depth*distanceMultiplier*1000);
#ifdef DEBUG
	cout<<"->Aspect:("<<width<<","<<height<<")\n";
#endif
	gluPerspective(70.0, 1, width/height, depth*distanceMultiplier*1000);
	gluLookAt(camera[0], camera[1], camera[2], 
		      camera[3], camera[4], camera[5], 
			  camera[6], camera[7], camera[8]);
}
void track(){
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(70.0, width/height, 1, depth*distanceMultiplier*1000);

	gluLookAt(lookat->x - (1200 * sin(toRadian(lookat->angle + 180 + WOBBLE))),
		lookat->y - lookat->height,
		lookat->z - (1200 * cos(toRadian(lookat->angle + 180 - WOBBLE))),
			  lookat->x, lookat->y - lookat->height, lookat->z,
			  camera[6], camera[7], camera[8]);
}
void drawTree(treeNode* tree) {	
	//This is all recursive up in here!
	if (tree->pic) {
		//leaf(with picture)
		float oldClickDepth;
		//if (click.clicked) {
		if (true){ //debug, change this back
			glReadPixels(click.x, click.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &oldClickDepth);
		}
		if (lookat == tree->pic) {
			tree->pic->display(tree->xpos, tree->ypos, tree->zpos, tree->angle, true);
		} else {
			tree->pic->display(tree->xpos, tree->ypos, tree->zpos, tree->angle);
		}

		//if (click.clicked) {
		if (true) { //debug, change this back
			float newClickDepth;
			glReadPixels(click.x, click.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &newClickDepth);
			if (newClickDepth != oldClickDepth) {
				click.lastPic = tree->pic;
				lookat = tree->pic;
			}
		}

	} else {
		GLfloat x1 = tree->xpos + (cos(toRadian(tree->angle)) * tree->radius);
		GLfloat z1 = tree->zpos + (sin(toRadian(tree->angle)) * tree->radius);
		GLfloat x2 = tree->xpos + (cos(toRadian(tree->angle + 180)) * tree->radius);
		GLfloat z2 = tree->zpos + (sin(toRadian(tree->angle + 180)) * tree->radius);
		glColor3f(0, 1, 1);
		
		//draw the sticks
		glBegin(GL_LINES);		
		glVertex3f(x1, tree->left->ypos, z1);
		glVertex3f(x1, tree->ypos, z1);

		glVertex3f(x1, tree->ypos, z1);
		glVertex3f(x2, tree->ypos, z2);

		glVertex3f(x2, tree->ypos, z2);
		glVertex3f(x2, tree->right->ypos, z2);
		glEnd();

		//process left subtree
		if (tree->left) {
			tree->left->xpos = x1;
			tree->left->zpos = z1;
			drawTree(tree->left);
		}
		//process right subtree
		if (tree->right) {
			tree->right->xpos = x2;
			tree->right->zpos = z2;
			drawTree(tree->right);
		}

	}
	tree->angle += tree->speed; //add speed to the angle each fram
	//check for angle overflow
	if (tree->angle > 360) {
		tree->angle -= 360;
	}
	if (tree->angle < 0) {
		tree->angle += 360;
	}
}
void redraw(){
	glEnable(GL_DEPTH_TEST); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);//this one is for fun

	if (tracking) track();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (!hideCoord) coordinates(1000);

	drawTree(root);
	if (click.clicked) {
		tracking = true;
		click.clicked = false;
	}

	glFlush();
	glutSwapBuffers();
}
void myDisplayCallback(){
	//don't put stuff here, have everything redraw off the timer tick
	// prevents window refreshes from making the thing spin out of control
}
void translate(double x, double y) {
	myDisplayCallback();
}
void rotate(double rads) {
	myDisplayCallback();
}

void keyboardCallback(unsigned char key, int cursorX, int cursorY) {
	switch (key) {
		case 't':
			tracking = !tracking;
			break;
		case '1': //followPicIndex = 0;
				  break;
		case '2': //followPicIndex = 1;
				  break;
		case '3': //followPicIndex = 2;
				  break;
		case '4': //followPicIndex = 3;
				  break;
		case 'r': if (tracking) {
					  tracking = !tracking;
					  resetCamera();
				  }
				  break;
		case 'c': hideCoord = !hideCoord;
				  break;
		case 'q': exit(EXIT_SUCCESS);
				  break;
		default: break;
	}
}
//void mouseCallback(int buttonName, int state, int cursorX, int cursorY) {
//	if (buttonName == GLUT_LEFT_BUTTON && state == GLUT_UP && followPicIndex != -1) {
//		tracking = !tracking;
//		if (!tracking) resetCamera();
//	}
//}
void mouseCallback(int buttonName, int state, int cursorX, int cursorY) {
#ifdef DEBUG
	cout<<"->MouseClick X:"<<cursorX<<" Y:"<<cursorY<<"\n";
#endif
	if (buttonName == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (tracking) {
			resetCamera();
			tracking = false;
		} else {
			click.clicked = true;
			click.x = cursorX;
			click.y = width - cursorY;
		}
	}
}
void myInit(){
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, width/height, 1, depth*distanceMultiplier*1000);
	gluLookAt(camera[0], camera[1], camera[2], 
		      camera[3], camera[4], camera[5], 
			  camera[6], camera[7], camera[8]);
}
void timer(int v) {

  //glutPostRedisplay();

	redraw();
  glutTimerFunc(1000/FPS, timer, v);
}
void passiveMove(int cursorX, int cursorY) {
	// No use yet
#ifdef DEBUG
	//cout<<"->passiveMove X:"<<cursorX<<" Y:"<<cursorY<<"\n";
#endif
			//click.clicked = true;
	if (!tracking) {
		click.x = cursorX;
		click.y = height - cursorY;
	}
}
void mouseMovement(int cursorX, int cursorY) {
	// No use yet
}

char* parseTextFile(const char *path) {
	string text;
	string temp;
  
	ifstream file;
	file.open (path);

	while (!file.eof())
	{
		getline (file, temp);
		text.append (temp); 
	}
	file.close();
	char * ret = new char[strlen(temp.c_str())+1]();
	strcpy(ret, temp.c_str());
	return ret;
}

void printDirectory(const char *path) {
	tinydir_dir dir;
	tinydir_open(&dir, path);

	while (dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (file.is_dir) {
			if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0) {
				// Do Nothing otherwise it will loop for a long time
			} else {
				cout << file.name << "/\n";
				printDirectory(file.path);
			}
		} else {
			cout << file.name << "\n";
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
}

void searchDirectory(const char *path, treeNode *leaf, float depth) {
	tinydir_dir dir;
	tinydir_open(&dir, path);

	bool hasLeftNode = false;
	bool hasLeftPic = false;
	depth -= 100;

	while (dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (file.is_dir) {
			if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0) {
				// Do Nothing otherwise it will loop for a long time
			} else {
				if (hasLeftNode)  {//go Right
					leaf->right = new treeNode(0, depth, 0, 800);
					searchDirectory(file.path, leaf->right, depth); // Check right
				} else { //go Left
					leaf->left = new treeNode(0, depth, 0, 800);
					searchDirectory(file.path, leaf->left, depth); // Check left
					hasLeftNode = true;
				}
			}
		} else {
			char fileName[256], extension[20];
			char textPath[4096];
			char* picText;
			sscanf(file.name, "%[^.].%s", fileName, extension);
			if (strcmp(extension, "txt") != 0) {
				sprintf(textPath, "%s/%s%s", path, fileName, ".txt");
				picText = (char*)parseTextFile(textPath);
				if (hasLeftPic) {
					leaf->right->pic = new picture(file.path, file.name, picText);
				} else {
					leaf->left->pic = new picture(file.path, file.name, picText);
					hasLeftPic = true;
				}
			}
			printf("\n");
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
}

void constructMobileTree() {
	root = new treeNode(0, 0, 0, 2000);  // Main root

	root->left = new treeNode(0, -100, 0, 800); // Left child
	searchDirectory("../../../mobiletree/1", root->left, -100); //Check left

	root->right = new treeNode(0, -100, 0, 800); // Right child
	searchDirectory("../../../mobiletree/2", root->right, -100); // Check right
	lookat = root->left->left->pic;
}

void main(int argc, char ** argv){
	//initialize glut and openGL
	glutInit(& argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(600, 0);			// specify a window position
	glutCreateWindow(editor_title);	// create a titled window
	myInit();									// setting up

	//initialize project stuff
	srand(time(NULL));
	//click = new mouseClick();
	click.clicked = false;

	//printDirectory("../../../mobiletree");
	constructMobileTree();

	//pause();

	glutKeyboardFunc(keyboardCallback);
	glutDisplayFunc(myDisplayCallback);		// register a callback
	glutTimerFunc(100, timer, 0);
	
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMovement);
	glutPassiveMotionFunc(passiveMove);
	glutReshapeFunc(reshape);
	
	glutMainLoop();							// get into an infinite loop
}
