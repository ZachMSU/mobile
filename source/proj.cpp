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

#define NODE_FOLDER_1 "../../../mobiletree/1"
#define NODE_FOLDER_2 "../../../mobiletree/2"

//animation speeds
#define FLY_SPEED 120 //how many frames the fly to will take
#define SPEED_RANGE 0.2 //pictures will rotate a max of SPEED_RANGE degrees per frame
#define WOBBLE 2 // This is how much of a wobble when looking at a picture in degrees
#define FRAMES_PER_CHAR 2 //how many frames between each character when displaying in the description

//style properties
#define STICK_COLOR 0, 1, 1
#define STICK_WIDTH 3
#define TEXT_COLOR 0, 1, 0
#define TEXT_BACKGROUND 0, 0, 0, 0.6
#define HIGHLIGHT_COLOR 1, 1, 0
#define FONT_HEIGHT 50 //space between lines
#define TEXT_POS_X -.9 //text starting position from -1 to 1. 0 is the middle of the screen
#define TEXT_POS_Y -.2 
#define FONT GLUT_BITMAP_TIMES_ROMAN_24
#define DROP_DISTANCE -230  // For the mobile drop distance for each hanging/raised(if > 0) piece

bool showHelp = false,
	 showCoords = true,
	 tracking = false,
	 manifest = false,
	 spinning = true;

using namespace std;

static GLbitfield bitmask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
static int FPS = 60;
static int currentChars = 0;
static int framesSinceLastChar = 0;
static GLfloat currentAngleOfRotation[4] = { 0, 30, 90, 180 };
picture* lookat;
int pointingToAbyss = 0;
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
bool coinFlip(){
	//returns true or false
	return (rand() % 2) == 0;
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

int picCount = 0,
	picLookat = -1;
picture* pics[100];

void reshape(int w, int h){
	cam.changePerspective(70.0, w, h, 1, depth*distanceMultiplier*1000);
}
void resetCamera() {
	cam.set(initialPos, true);
}
void track(){
	double largestDimention = (lookat->height > lookat->width) ? lookat->height : lookat->width;
	double distance = largestDimention * 1.5;
	cam.set(lookat->x - (distance * sin(toRadian(lookat->angle + 180 + WOBBLE))),
			lookat->y - lookat->height,
			lookat->z - (distance * cos(toRadian(lookat->angle + 180 - WOBBLE))),
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
	glColor4f(TEXT_BACKGROUND);

	//render the semi-transparent background
	glBegin(GL_POLYGON);
		glVertex3f(-1, TEXT_POS_Y, 0);	
		glVertex3f(1, TEXT_POS_Y, 0);
		glVertex3f(1, -1, 0);
		glVertex3f(-1, -1, 0);
	glEnd();

	//render the text
	glColor3f(TEXT_COLOR);
	framesSinceLastChar ++;
	if (framesSinceLastChar >= FRAMES_PER_CHAR) {
		framesSinceLastChar = 0;
		currentChars ++;
	}
	renderBitmapString(TEXT_POS_X, TEXT_POS_Y, 0, FONT, lookat->description, FONT_HEIGHT / cam.height, currentChars);

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
		if (tree->pic != NULL) tree->pic->display(tree->xpos, tree->ypos, tree->zpos, tree->angle, tree->pic == click.lastPic);
	
		//Find the new z buffer value at the mouse coord, if it has changed, then this picture is the one the mouse is over
		GLfloat newClickDepth;

		glReadPixels(click.x, click.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &newClickDepth);
		//if (newClickDepth != oldClickDepth) {
		//if (newClickDepth < click.lastZ){
		if (newClickDepth == 1.0) {
			if (pointingToAbyss == picCount + 1) click.lastPic = nullptr;
			else if (pointingToAbyss < picCount + 1) pointingToAbyss++;
		} else if (newClickDepth < click.lastZ){
			pointingToAbyss = 0;
			click.lastPic = tree->pic;
		}
		click.lastZ = newClickDepth;
	} else {
		GLfloat x1 = tree->xpos + (cos(toRadian(tree->angle)) * tree->radius);
		GLfloat z1 = tree->zpos + (sin(toRadian(tree->angle)) * tree->radius);
		GLfloat x2 = tree->xpos + (cos(toRadian(tree->angle + 180)) * tree->radius);
		GLfloat z2 = tree->zpos + (sin(toRadian(tree->angle + 180)) * tree->radius);
		glColor3f(STICK_COLOR);
		
		//draw the sticks
		glLineWidth(STICK_WIDTH);
		glBegin(GL_LINES);		
			if (tree->left != NULL) {
				glVertex3f(x1, tree->left->ypos, z1);
				glVertex3f(x1, tree->ypos, z1);
			}

			glVertex3f(x1, tree->ypos, z1);
			glVertex3f(x2, tree->ypos, z2);

		
			if (tree->right != NULL) {
				glVertex3f(x2, tree->ypos, z2);
				glVertex3f(x2, tree->right->ypos, z2);
			}
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
	//click.lastPic = nullptr;

	if (tracking) track();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (showCoords) coordinates(1000);

	drawTree(root);
	cam.touch();
	if (click.clicked) {
		tracking = true;
		click.clicked = false;
		currentChars = 1;
		framesSinceLastChar = 0;
	}

	if (tracking && cam.framesLeft <= 0 && lookat->hasDescription) {
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
void constructRandomTree(){
	//construct the tree	
	const int nodeWidth = 10;
	root = new treeNode(0, 0, 0, nodeWidth);  // Main root
	//nodeWidth /= 2;
	//add the first pic
	root->left = new treeNode(0, DROP_DISTANCE, 0, nodeWidth);
	root->left->pic = pics[0];
	root->radius += pics[0]->width;
	//add the second pic
	root->right = new treeNode(0, DROP_DISTANCE, 0, nodeWidth);
	root->right->pic = pics[1];
	root->radius += pics[1]->width;

	//add the subsequent pics
	for (int i = 2; i < picCount; i++){
#ifdef DEBUG
		cout<<"Adding pics["<<i<<"] "<<pics[i]->filename<<"\n";
#endif
		//int curNodeWidth = nodeWidth;
		int treeHeight = 0;
		bool leafNotFound = true;
		
		treeNode* leafNode = root;
		
		while (leafNotFound) {
			treeHeight ++;
			
			leafNode->radius += pics[i]->width / 2;
			if (leafNode->pic == NULL) {
				
				leafNode = (coinFlip()) ? leafNode->left : leafNode->right;
				
				//nodeWidth /= 2;
			} else {
				//Found a leaf. Replace leaf with node, and move the found pic to the left.
				//picture* tempPic = leafNode->pic;
				leafNode->left = new treeNode(0, DROP_DISTANCE * treeHeight, 0, nodeWidth);
				leafNode->radius += leafNode->pic->width / 2;
				leafNode->left->pic = leafNode->pic;
				leafNode->pic = NULL;
				
				//Add current pics[i] to right side
				leafNode->right = new treeNode(0, DROP_DISTANCE * treeHeight, 0, nodeWidth);
				leafNode->right->pic = pics[i];
				leafNotFound = false;
			}
		}
	}
	//return true;
}
void keyboardCallback(unsigned char key, int cursorX, int cursorY) {
	switch (key) {
	case '.':
		cam.animate(FLY_SPEED);
		if (picLookat < picCount - 1) {
			picLookat ++;
			lookat = pics[picLookat];		
			click.clicked = true; //cause the animation to start
		}
		break;
	case ',':
		cam.animate(FLY_SPEED);
		if (picLookat > 0) {
			picLookat --;
			lookat = pics[picLookat];
			click.clicked = true; //cause the animation to start
		}
		break;
	case 'r':
		if (manifest) constructRandomTree();
		break;
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
	default: 
		//movement keys
		const int moveamt = 100;
		switch (key) {		
			case '1'://back
				initialPos.z += moveamt;
				break;
			case '4'://forth
				initialPos.z -= moveamt;
				break;
			case '2'://up
				initialPos.y -= moveamt;
				break;
			case '5'://down
				initialPos.y += moveamt;
				break;
			case '3'://lookat up
				initialPos.lookatY -= moveamt;
				break;
			case '6'://lookat down
				initialPos.lookatY += moveamt;
				break;

		}
		cam.set(initialPos);
		cout<<"Camera Z:"<<initialPos.z<<" Y:"<<initialPos.y<<" LookatY:"<<initialPos.lookatY<<"\n";
		break;
	}
}
void mouseCallback(int buttonName, int state, int cursorX, int cursorY) {
#ifdef DEBUG
	cout<<"->MouseClick X:"<<cursorX<<" Y:"<<cursorY<<"\n";
#endif
	cam.animate(FLY_SPEED);
	if (buttonName == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (tracking) {
			resetCamera();
			tracking = false;
		} else if (click.lastPic != nullptr) {
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
	
	return "";
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
	bool nodeNotFull = true;
	depth += DROP_DISTANCE;

	while (dir.has_next && nodeNotFull) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (file.is_dir) {
			if (strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0) {
				if (hasLeftNode)  {//go Right
					leaf->right = new treeNode(0, depth, 0, 800);
					searchDirectory(file.path, leaf->right, depth); // Check right
					nodeNotFull = false;
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
				//if (strcmp(description, "???") == 0) description = fileName;
				if (hasLeftNode) {
					if (leaf->right == NULL) leaf->right = new treeNode(0, depth, 0, 800);
					leaf->right->pic = new picture(file.path, file.name, description);
					nodeNotFull = false;
				} else {
					if (leaf->left == NULL) leaf->left = new treeNode(0, depth, 0, 800);
					leaf->left->pic = new picture(file.path, file.name, description);
					hasLeftNode = true;
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

	root->left = new treeNode(0, DROP_DISTANCE, 0, 800); // Left child
	searchDirectory(NODE_FOLDER_1, root->left, DROP_DISTANCE); //Check left

	root->right = new treeNode(0, DROP_DISTANCE, 0, 800); // Right child
	searchDirectory(NODE_FOLDER_2, root->right, DROP_DISTANCE); // Check right
}
bool loadManifest(const char* manifestFilename){
	/* Loads the configuration data from the manifest file. 
	 * See example manifest.txt for more deets
	 * manifestFilename : filename of the config file
	 * NOTES: This whole thing could be better.
	 */
	
	ifstream file;
	file.open(manifestFilename);
	if (file.fail()) {
#ifdef DEBUG
		cout<<"no manifest file found. Defaulting to folder mode.\n";
#endif
		return false;
	}
	string filename = "";
	GLfloat width;
	GLfloat height;
	string name;
	string description = "";
	string token;
	char ctoken[512];
	file>>token;
	while (!file.eof()) {		
		//file.get();
		//cout<<token;
		if (token == "#") {
			file>>width;
			file>>height;
			file.getline(ctoken, 512); //get filename
			file.getline(ctoken, 512); //Has to be 2. Dunno why.
			cout<<"w:"<<width<<"h:"<<height<<"\n";
			filename="";
			description="";
			filename.append(ctoken);
			while (!file.eof()) {
				//parse the description
				file>>token;
				if (token[0] == '#') break;
				description.append(token);
				file.getline(ctoken, 512);
				description.append(ctoken);
				description.append("\n");
			}
#ifdef DEBUG
			cout<<"pic["<<picCount<<"] filename:"<<filename<<"\n description:"<<description<<"\n";
#endif
			pics[picCount] = new picture(filename, "", description);
			if (pics[picCount]->loaded) picCount++;		
		}
	}
	file.close();
	return true;
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
	//constructMobileTree();
	if (loadManifest("C:\\temp\\project\\manifest.txt")){
		constructRandomTree();
		manifest = true;
	} else {
		constructMobileTree();
	}

	initialPos.z = root->radius * 2;
	cam.setPos(initialPos);
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