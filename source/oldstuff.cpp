void cameraTrack() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, width/height, 1, depth*distanceMultiplier*1000);

	double z = pictures[followPicIndex][2]+1000;

	camera[0] = z * sin(currentAngleOfRotation[followPicIndex]*pictures[followPicIndex][5]*M_PI/180);
	camera[1] = pictures[followPicIndex][4];
	camera[2] = z * cos(currentAngleOfRotation[followPicIndex]*pictures[followPicIndex][5]*M_PI/180);
	camera[3] = 0;
	camera[4] = pictures[followPicIndex][4];
	camera[5] = 0;
	
	gluLookAt(camera[0], camera[1], camera[2], 
		      camera[3], camera[4], camera[5], 
			  camera[6], camera[7], camera[8]);
}

void loadImages() {
	cout << "Loading images...\n";
	glGenTextures(picCount, textures);
	for (int i = 0; i < picCount; i++) {
		textures[i] = loadTexture(load_me[i].c_str());

		if (textures[i] > 0) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			pictures[i][0] = GL_TEXTURE_WIDTH/width*100;
			cout << "gltexturewidth" << pictures[i][0] << "\n";
			pictures[i][1] = GL_TEXTURE_HEIGHT/height*100;
			cout << "Image: " << load_me[i].c_str() << " successfully loaded.\n";
		} else {
			cout << "Failure: " << load_me[i].c_str() << " : " << textures[i] << "\n";
			glDisable(GL_TEXTURE_2D);
		}

		//if (textures[i] > 0) {
		//	glEnable(GL_TEXTURE_2D);
		//	glBindTexture(GL_TEXTURE_2D, textures[i]);
		//	pictures[i][0] = GL_TEXTURE_WIDTH/width*100;
		//	pictures[i][1] = GL_TEXTURE_HEIGHT/height*100;
		//} else glDisable(GL_TEXTURE_2D);

		
	}
	cout << "Images loaded.\n";
}

void renderPictures(float x, float y, float z, float angle, float verse, float transX, float transY, float transZ, GLuint pic_index) {
	if (pic_index > 0) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, pic_index);
	} else glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotatef(angle*verse, 0.0, 1.0, 0.0);
	glTranslatef(transX, transY, transZ);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z);
		
	    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, y, z);

		glTexCoord2f(0.0f, 1.0f); glVertex3f(-x, -y, z);

		glTexCoord2f(1.0f, 1.0f); glVertex3f(x, -y, z);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void picture::display(float x, float y, float z, float angle, float verse, float transX, float transY, float transZ) {
	printf("s:%4.0f y:%4.0f z:%4.0f a:%4.0f v:%4.0f tX:%4.0f tY:%4.0f tZ%4.0f\n", x, y, z, angle, verse, transX, transY, transZ);

	
	GLuint pic_index = texture[0];
	if (pic_index > 0) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, pic_index);
	} else glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotatef(angle*verse, 0.0, 1.0, 0.0);
	glTranslatef(transX, transY, transZ);
	
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); 
		glVertex3f(x, y, z);
		
	    glTexCoord2f(0.0f, 0.0f); 
		glVertex3f(-x, y, z);

		glTexCoord2f(0.0f, 1.0f); 
		glVertex3f(-x, -y, z);

		glTexCoord2f(1.0f, 1.0f); 
		glVertex3f(x, -y, z);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void highlightPicture(float x, float y, float z, float angle, float verse, float transX, float transY, float transZ) {
	glPushMatrix();
	glRotatef(angle*verse, 0.0, 1.0, 0.0);
	glTranslatef(transX, transY, transZ);
	glColor3f(1, 1, 0);
	glLineWidth(5);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_POLYGON);
		glVertex3f(x+1, y+1, z);	
	    glVertex3f(-x-1, y+1, z);
		glVertex3f(-x-1, -y-1, z);
		glVertex3f(x+1, -y-1, z);
	glEnd();
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void loadManifest(const char* manifestFilename){
	/* Loads the configuration data from the manifest file. 
	 * See example manifest.txt for more deets
	 * manifestFilename : filename of the config file
	 * NOTES: This whole thing could be better.
	 */

	//known bug: will crash if comment is at the end
	ifstream file;
	file.open(manifestFilename);
	string filename;
	GLfloat width;
	GLfloat height;
	string name;
	string description;
	string token;
	while (!file.eof()) {
		file>>token;
		if (token == "/*") {
			//itereate over comments
			while (true){
				file>>token;
				if (token == "*/") break;
			}
			file>>token;
		}
		filename = token;
		file>>width>>height>>name>>description;
#ifdef DEBUG
		cout<<"->IDX:"<<picCount<<"FN:"<<filename<<" W:"<<width<<" H:"<<height<<" NAME:"<<name<<" DESCRIPTION:"<<description<<"\n"; //debug
#endif
		pics[picCount] = new picture(filename, width, height, name, description);
		picCount ++;
	}
}

void loadHardTree(){
	//this is just a hardcoded tree. For debug purposes
	//Trees are hard.
	root->left = new treeNode(0, -100, 0, 1200);
	root->right = new treeNode(0, -100, 0, 800);

	root->left->pic = new picture("C:\\temp\\project\\pics\\1.jpg", 600, 500, "bard", "thing");
	root->right->pic = new picture("C:\\temp\\project\\pics\\5.jpg", 600, 500, "bard", "thing");

	//root->left->left = new treeNode(0, -500, 0, 600);
	//root->left->right = new treeNode(0, -200, 0, 800);

	//root->right->left = new treeNode(0, -200, 0, 800);
	//root->right->right = new treeNode(0, -200, 0, 800);

	//root->left->left->left = new treeNode(0, -900, 0, 600);
	//root->left->left->right = new treeNode(0, -900, 0, 600);

	//root->left->left->left->pic = new picture("C:\\temp\\project\\pics\\1.jpg", 600, 500, "bard", "thing");
	//root->left->left->right->pic = new picture("C:\\temp\\project\\pics\\5.jpg", 600, 500, "bard", "thing");
	
	//root->left->right->pic = new picture("C:\\temp\\project\\pics\\6.jpg", 800, 1300, "batoro", "kids");


	//root->right->left->pic = new picture("C:\\temp\\project\\pics\\3.jpg", 800, 600, "bard", "thing is beans");
	//root->right->right->pic = new picture("C:\\temp\\project\\pics\\4.jpg", 800, 600, "sette", "haters");

	//lookat = root->left->left->pic;
	lookat = root->left->pic;
}

/* This is the manifest file for the mobile project. Comments start with "/" (no quotes) 
   and end with "*/" (also no qutos). There must be whitespace on both sides of comment thing.
   Each line is a picture entry. Pipes "|" in description will be turned into newlines.
   Filename, name, and description must be surrounded by quotes
   Filename width height "name" "description"    ex:
   C:\temp\project\pics\samp\bard.jpg 100 100
   This is a bard playing a thing
   
   Currently there can be no spaces in the filename, name, or description.
   */