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
#include "camera.h"
#include "tinydir.h"

#define editor_title "3D Mobile"
#define DEFAULT_MANIFEST "C:\\temp\\project\\manifest.txt"
#define SPEED_RANGE 0.5
#define WOBBLE 2
#define DROP_DISTANCE -100  // For the mobile drop distance for each hanging/raised(if > 0) piece
#define NODE_FOLDER_1 "../../../mobiletree/1"
#define NODE_FOLDER_2 "../../../mobiletree/2"
#define FONT GLUT_BITMAP_TIMES_ROMAN_24
#define FONT_HEIGHT 50 //space between lines
using namespace std;

static GLbitfield bitmask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
static bool spinning = true;
static int FPS = 60;
static GLfloat currentAngleOfRotation[4] = { 0, 30, 90, 180 };
picture* lookat;
struct timeStat{
	WORD num;
	WORD sum;
	timeStat(){
		num = 0;
		sum = 0;
	}
};
timeStat timestat;


GLfloat random(){
	GLfloat out = ((GLfloat) rand() / RAND_MAX) * (SPEED_RANGE * 2) - SPEED_RANGE;
	cout<<"->Random:"<<out<<"\n";
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

//camera stuff
int height = 600,
	width  = 1000,
	depth = 600;
double distanceMultiplier = 5;
camera cam(70.0, 1000, 600, 1, depth*distanceMultiplier*1000); //initalize the camera object
cameraPos initialPos = {0, -500, depth*distanceMultiplier*1.5, 0, -1500, 0};

bool hideHelp = true,
	 showCoords = true,
	 tracking = false;

void reshape(int w, int h){
	cam.changePerspective(70.0, w, h, 1, depth*distanceMultiplier*1000);
}
void resetCamera() {
	cam.set(initialPos, true);
}
void track(){
	cam.set(lookat->x - (1200 * sin(toRadian(lookat->angle + 180 + WOBBLE))),
			lookat->y - lookat->height,
			lookat->z - (1200 * cos(toRadian(lookat->angle + 180 - WOBBLE))),
			lookat->x, lookat->y - lookat->height, lookat->z, false);
}
void displayDescriptionNew() {
	//Set the matricies to absolute coords
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//Set blending functions
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0, 0, 0, 0.6);

	//render the semi-transparent background
	glBegin(GL_POLYGON);
		glVertex3f(-1, 0, 0);	
		glVertex3f(1, 0, 0);
		glVertex3f(1, -1, 0);
		glVertex3f(-1, -1, 0);
	glEnd();

	//render the text
	glColor3f(1, 1, 1);
	renderBitmapString(-1, 0, 0, FONT, lookat->description, FONT_HEIGHT / cam.height);

	//reset blending
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	//reset the matricies
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
void displayDescription() {
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0, 0, 0, 0.6);
        glPushMatrix();
        glTranslatef(lookat->x, lookat->y - height, lookat->z);
        glRotatef(lookat->angle, 0.0, 1.0, 0.0);
        glBegin(GL_POLYGON);
                glVertex3f(width, height - lookat->height * 1.5, 0);        
                glVertex3f(-width, height - lookat->height * 1.5, 0);
                glVertex3f(-width, -height*2, 0);
                glVertex3f(width, -height*2, 0);
        glEnd();
        glPopMatrix();
        glColor3f(1, 1, 1);
        renderBitmapString(lookat->x, lookat->y - lookat->height * 2, lookat->z, GLUT_BITMAP_TIMES_ROMAN_24, lookat->description);
        
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
}
void drawTree(treeNode* tree) {	
	//This is all recursive up in here!
	if (tree->pic) {
		//leaf(with picture)
		//GLfloat oldClickDepth;

		//Find the current z buffer value at the mouse coord
		//glReadPixels(click.x, click.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &oldClickDepth);

		//Render the picture
		tree->pic->display(tree->xpos, tree->ypos, tree->zpos, tree->angle, tree->pic == click.lastPic);
	
		//Find the new z buffer value at the mouse coord, if it has changed, then this picture is the one the mouse is over
		GLfloat newClickDepth;

		glReadPixels(click.x, click.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &newClickDepth);
		//if (newClickDepth != oldClickDepth) {
		if (newClickDepth < click.lastZ){
			click.lastPic = tree->pic;
		}
		click.lastZ = newClickDepth;
	} else {
		GLfloat x1 = tree->xpos + (cos(toRadian(tree->angle)) * tree->radius);
		GLfloat z1 = tree->zpos + (sin(toRadian(tree->angle)) * tree->radius);
		GLfloat x2 = tree->xpos + (cos(toRadian(tree->angle + 180)) * tree->radius);
		GLfloat z2 = tree->zpos + (sin(toRadian(tree->angle + 180)) * tree->radius);
		glColor3f(0, 1, 1);
		
		//draw the sticks
		glLineWidth(3);
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
	glClear(bitmask);

	if (tracking) track();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (showCoords) coordinates(1000);

	drawTree(root);
	cam.touch();
	if (click.clicked) {
		tracking = true;
		click.clicked = false;
	}

	if (tracking) {
		//displayDescription();
		displayDescriptionNew();
	}

	glFlush();
	glutSwapBuffers();
}
void myDisplayCallback(){
	//don't put stuff here, have everything redraw off the timer tick
	// prevents window refreshes from making the thing spin out of control
}
void keyboardCallback(unsigned char key, int cursorX, int cursorY) {
	switch (key) {
	case 't':
		cout << "Avg render time of last " << timestat.num << " frames is " << (timestat.sum / timestat.num) << " milliseconds.\n";
		timestat.sum = 0;
		timestat.num = 0;
		break;
	case '+':
		FPS += 10;
	case '-':
		FPS -= 5;
		if (FPS <= 0) FPS = 5;
		cout<<"Framerate(FPS):"<<FPS<<"\n";
		break;
	case 'b'://this one is for fun
		bitmask ^= GL_COLOR_BUFFER_BIT;
		if (bitmask & GL_COLOR_BUFFER_BIT) {
			cout<<"Clearing buffer(normal)\n";
		} else {
			cout<<"No longer clearing buffer\n";
		}
		break;
	case 'd'://also for fun
		if (glIsEnabled(GL_DEPTH_TEST)) {
			glDisable(GL_DEPTH_TEST);
			cout<<"Disabled Depth Test\n";
		} else {
			glEnable(GL_DEPTH_TEST);
			cout<<"Enabled Depth Test\n";
		}
		break;
	case ' ':
		redraw();
		break;
	case 's':
		if (spinning) {
			cout << "Stopping Animation(use space to advance frame)\n";
			spinning = false;
		} else {
			cout << "Starting Animation\n";
			spinning = true;
		}
		break;
	case 'c': 
		if (showCoords) {
			cout<<"Hiding Coordinate Axis\n";
			showCoords = false;
		} else {
			cout<<"Showing Coordinat Axis\n";
			showCoords = true;
		}
		break;
	case 'q': 
	case 27://escape
		exit(EXIT_SUCCESS);
		cout<<"Bye!\n";
		break;
	default: break;
	}
}
void mouseCallback(int buttonName, int state, int cursorX, int cursorY) {
#ifdef DEBUG
	cout<<"->MouseClick X:"<<cursorX<<" Y:"<<cursorY<<"\n";
#endif
	cam.animate(120);
	if (buttonName == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (tracking) {
			resetCamera();
			tracking = false;
		} else {
			click.clicked = true;
			click.x = cursorX;
			click.y = width - cursorY;
			lookat = click.lastPic;
		}
	}
}
void myInit(){
	glEnable(GL_DEPTH_TEST); 
	cam.set(initialPos);
	cam.touch();
}
void timer(int v) {
	SYSTEMTIME before, after;
	GetSystemTime(&before);

	if (spinning) redraw();

	GetSystemTime(&after);
	WORD frameTime = after.wMilliseconds - before.wMilliseconds;
	if (before.wSecond < after.wSecond) frameTime += 1000; //in case frame renders over a second boundry
	timestat.sum += frameTime;
	timestat.num ++;

	glutTimerFunc(1000/FPS, timer, v);	
}
void passiveMove(int cursorX, int cursorY) {
	if (!tracking) {
		click.x = cursorX;
		click.y = (int) cam.height - cursorY;
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
	if (file.is_open()) {
		while (!file.eof()) {
			getline(file, temp);
			text.append(temp);
			text.append("\n");
		}
		file.close();
		char * ret = new char[strlen(text.c_str())+1]();
		strcpy(ret, text.c_str());
		return ret;
	} else {
		cout << " ( ! ) Missing file: " << path << " ( ! )\n\n";
	}
	
	return "???";
}
void printDirectory(const char *path) {
	tinydir_dir dir;
	tinydir_open(&dir, path);

	while (dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (file.is_dir) {
			if (strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0) {
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
	depth += DROP_DISTANCE;

	while (dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (file.is_dir) {
			if (strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0) {
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
			char* description;
			sscanf(file.name, "%[^.].%s", fileName, extension);
			if (strcmp(extension, "txt") != 0) {
				sprintf(textPath, "%s/%s%s", path, fileName, ".txt");
				printf("->Preparing files:\n  %s\n  %s\n\n", file.path, textPath);
				description = (char*)parseTextFile(textPath);
				if (hasLeftPic) {
					leaf->right->pic = new picture(file.path, file.name, description);
				} else {
					leaf->left->pic = new picture(file.path, file.name, description);
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
	searchDirectory(NODE_FOLDER_1, root->left, DROP_DISTANCE); //Check left

	root->right = new treeNode(0, -100, 0, 800); // Right child
	searchDirectory(NODE_FOLDER_2, root->right, DROP_DISTANCE); // Check right
}
void main(int argc, char ** argv){
	//initialize glut and openGL
	glutInit(& argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(600, 0);			// specify a window position
	glutCreateWindow(editor_title);	// create a titled window
	myInit();									// setting up

	//if (argc < 2) {
	//	//no command line parameters given
	//	loadManifest(DEFAULT_MANIFEST);
	//} else {
	//	loadManifest(argv[1]);
	//}
	//initialize project stuff
	srand(time(NULL));
	//root = new treeNode(0, 0, 0, 3000);

	click.clicked = false;
	//loadHardTree();
	constructMobileTree();

	glutKeyboardFunc(keyboardCallback);
	glutDisplayFunc(myDisplayCallback);		// register a callback
	glutTimerFunc(100, timer, 0);
	
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMovement);
	glutPassiveMotionFunc(passiveMove);
	glutReshapeFunc(reshape);
	
	glutMainLoop();							// get into an infinite loop
}