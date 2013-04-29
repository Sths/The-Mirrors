#include "CHC_Mirror.h"
#include <vector>

extern vector <GLuint> FileTextures;

CHC_Mirror::CHC_Mirror(int x, int y, int dir) {
	Position = Grid(x, y);
	Normal = Dir2Normal(dir);
}

CHC_Mirror::CHC_Mirror(CHC_Vector3 _Position, int dir) {
	Position = _Position;
	Normal = Dir2Normal(dir);
}

void CHC_Mirror::Set_Position(int x, int y) {
	Position = Grid(x, y);
}

void CHC_Mirror::Set_Direction(int dir) {
	Normal = Dir2Normal(dir);
}

void CHC_Mirror::Load() {
	ifstream Fin(MirrorModelPath);
	CHC_Mirror::ViewList = glGenLists(1);
	
	Fin >> TextureIndex;

	glNewList(ViewList, GL_COMPILE);
		glBegin(GL_TRIANGLES);
			int n3; 
			Fin >> n3;
			while (n3 --) {
				float x, y, z;
				Fin >> x >> y >> z;
				glColor4f(x, y, z, 1.0);
				for (int i = 0; i < 3; i++) {
					Fin >> x >> y;
					glTexCoord2f(x, y);
					Fin >> x >> y >> z;
					glVertex3f(x, y, z);
				}
			}
		glEnd();
		glBegin(GL_QUADS);
			int n4;
			Fin >> n4;
			while (n4 --) {
				float x, y, z;
				Fin >> x >> y >> z;
				glColor4f(x, y, z, 1.0);
				for (int i = 0; i < 4; i++) {
					Fin >> x >> y;
					glTexCoord2f(x, y);
					Fin >> x >> y >> z;
					glVertex3f(x, y, z);
				}
			}
		glEnd();
	glEndList();
}

void CHC_Mirror::Draw() {
	if (!Loaded) {
		Load();
		Loaded = true;
	}
	glPushMatrix();
	//glLoadIdentity();
	glTranslatef(Position[0], Position[1], Position[2]);
	glRotatef(Normal[1] * 360, 0, 1, 0);
	//cout << Normal[1] << endl;
	//cout << FileTextures.size() << endl;
	if (TextureIndex != -1) glBindTexture(GL_TEXTURE_2D, FileTextures[TextureIndex]);
	glCallList(ViewList);
	glPopMatrix();
}