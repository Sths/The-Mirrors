#include "CHC_Laser.h"


void CHC_Laser::set(int x0, int y0, int x1, int y1, int _color) {
	radius = 0.05;
	s = Grid(x0, y0) + CHC_Vector3(0, Gridy / 2.0, 0);
	t = Grid(x1, y1) + CHC_Vector3(0, Gridy / 2.0, 0);
	color = Color2Vector(_color);
}

CHC_Laser::CHC_Laser(int x0, int y0, int x1, int y1, int _color, GLfloat _radius = 0.05) {
	s = Grid(x0, y0) + CHC_Vector3(0, Gridy / 2.0, 0);
	t = Grid(x1, y1) + CHC_Vector3(0, Gridy / 2.0, 0);
	color = Color2Vector(_color);
	radius = _radius;
}

void CHC_Laser::Draw() {
	GLuint texture;
	LoadGLTextures(texture, "Reflection\\data\\Laser.bmp");
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	
	glColor4f(SEP(color), 0.5);
	glBegin(GL_QUADS);
		for (GLfloat i = 0; i < acos(-1.0); i += 0.5) {
			GLfloat y = cos(i) * radius;
			GLfloat x = sin(i) * radius;
			CHC_Vector3 temp = CHC_Vector3(s[2] - t[2], 0, t[0] - s[0]);
			temp.normalize();
			temp *= x;
			temp.v[1] += y; 
			glTexCoord2f(0.0f, 0.0f); glVertex3f(SEP(s + temp));
			glTexCoord2f(1.0f, 0.0f); glVertex3f(SEP(s - temp));
			glTexCoord2f(1.0f, 1.0f); glVertex3f(SEP(t - temp));
			glTexCoord2f(0.0f, 1.0f); glVertex3f(SEP(t + temp));
		}
	glEnd();

	glDepthMask(GL_TRUE);
}