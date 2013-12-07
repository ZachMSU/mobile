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

#include "helpers.h"
#include "picture.h"
#include "camera.h"

#define editor_title "3D Mobile"
#define DEFAULT_MANIFEST "C:\\temp\\project\\manifest.txt"
#define SPEED_RANGE 0.5
#define WOBBLE 2
using namespace std;

static GLbitfield bitmask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
static bool spinning = true;
static int FPS = 60;
static GLfloat currentAngleOfRotation[4] = { 0, 30, 90, 180 };
picture* lookat;

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
    //width = w;
    //height = h;
    //glViewport(0, 0, w, h);
	cam.changePerspective(70.0, w, h, 1, depth*distanceMultiplier*1000);
}
void resetCamera() {
	cam.set(initialPos, true);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	camera[0] = camera[3] = camera[5] = 0;
//	camera[4] = -1500;
//	camera[1] = -500;
//	camera[2] = depth*distanceMultiplier*1.5;
//#ifdef DEBUG
//	cout<<"->Aspect:("<<width<<","<<height<<")\n";
//#endif
//	gluPerspective(70.0, 1, width/height, depth*distanceMultiplier*1000);
//	gluLookAt(camera[0], camera[1], camera[2], 
//		      camera[3], camera[4], camera[5], 
//			  camera[6], camera[7], camera[8]);
}
void track(){
	//glMatrixMode(GL_PROJECTION);

	//glLoadIdentity();
	//gluPerspective(70.0, width/height, 1, depth*distanceMultiplier*1000);

	//gluLookAt(lookat->x - (1200 * sin(toRadian(lookat->angle + 180 + WOBBLE))),
	//	lookat->y - lookat->height,
	//	lookat->z - (1200 * cos(toRadian(lookat->angle + 180 - WOBBLE))),
	//		  lookat->x, lookat->y - lookat->height, lookat->z,
	//		  camera[6], camera[7], camera[8]);
	cam.set(lookat->x - (1200 * sin(toRadian(lookat->angle + 180 + WOBBLE))),
			lookat->y - lookat->height,
			lookat->z - (1200 * cos(toRadian(lookat->angle + 180 - WOBBLE))),
			lookat->x, lookat->y - lookat->height, lookat->z, false);
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
		//if (lookat == tree->pic) {
			tree->pic->display(tree->xpos, tree->ypos, tree->zpos, tree->angle, tree->pic == click.lastPic);
		//} else {
		//	tree->pic->display(tree->xpos, tree->ypos, tree->zpos, tree->angle);
		//}

		//if (click.clicked) {
		if (true) { //debug, change this back
			float newClickDepth;
			glReadPixels(click.x, click.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &newClickDepth);
			if (newClickDepth != oldClickDepth) {
				click.lastPic = tree->pic;
				//lookat = tree->pic;
			}
		}

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
	glFlush();
	glutSwapBuffers();
}
void myDisplayCallback(){
	//don't put stuff here, have everything redraw off the timer tick
	// prevents window refreshes from making the thing spin out of control
}
void keyboardCallback(unsigned char key, int cursorX, int cursorY) {
	switch (key) {
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
	if (spinning) redraw();
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
void loadManifest(const char* manifestFilename){
	/* Loads the configuration data from the manifest file. 
	 * See example manifest.txt for more deets
	 * manifestFilename : filename of the config file
	 * NOTES: This whole thing could be better.
	 */

	//known bug: will crash if comment is at the end
//	ifstream file;
//	file.open(manifestFilename);
//	string filename;
//	GLfloat width;
//	GLfloat height;
//	string name;
//	string description;
//	string token;
//	while (!file.eof()) {
//		file>>token;
//		if (token == "/*") {
//			//itereate over comments
//			while (true){
//				file>>token;
//				if (token == "*/") break;
//			}
//			file>>token;
//		}
//		filename = token;
//		file>>width>>height>>name>>description;
//#ifdef DEBUG
//		cout<<"->IDX:"<<picCount<<"FN:"<<filename<<" W:"<<width<<" H:"<<height<<" NAME:"<<name<<" DESCRIPTION:"<<description<<"\n"; //debug
//#endif
//		pics[picCount] = new picture(filename, width, height, name, description);
//		picCount ++;
//	}
}
void loadHardTree(){
	//this is just a hardcoded tree. For debug purposes
	//Trees are hard.
	root->left = new treeNode(0, -100, 0, 1200);
	root->right = new treeNode(0, -100, 0, 800);

	root->left->left = new treeNode(0, -500, 0, 600);
	root->left->right = new treeNode(0, -200, 0, 800);

	root->right->left = new treeNode(0, -200, 0, 800);
	root->right->right = new treeNode(0, -200, 0, 800);

	root->left->left->left = new treeNode(0, -900, 0, 600);
	root->left->left->right = new treeNode(0, -900, 0, 600);

	root->left->left->left->pic = new picture("C:\\temp\\project\\pics\\1.jpg", 600, 500, "bard", "thing");
	root->left->left->right->pic = new picture("C:\\temp\\project\\pics\\5.jpg", 600, 500, "bard", "thing");
	
	root->left->right->pic = new picture("C:\\temp\\project\\pics\\6.jpg", 800, 1300, "batoro", "kids");


	root->right->left->pic = new picture("C:\\temp\\project\\pics\\3.jpg", 800, 600, "bard", "thing is beans");
	root->right->right->pic = new picture("C:\\temp\\project\\pics\\4.jpg", 800, 600, "sette", "haters");

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

	//if (argc < 2) {
	//	//no command line parameters given
	//	loadManifest(DEFAULT_MANIFEST);
	//} else {
	//	loadManifest(argv[1]);
	//}
	//initialize project stuff
	srand(time(NULL));
	root = new treeNode(0, 0, 0, 3000);

	click.clicked = false;
	loadHardTree();

	glutKeyboardFunc(keyboardCallback);
	glutDisplayFunc(myDisplayCallback);		// register a callback
	glutTimerFunc(100, timer, 0);
	
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMovement);
	glutPassiveMotionFunc(passiveMove);
	glutReshapeFunc(reshape);
	
	glutMainLoop();							// get into an infinite loop
}