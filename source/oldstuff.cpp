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