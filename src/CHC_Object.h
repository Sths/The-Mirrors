#ifndef CHC_OBJECT_H
#define CHC_OBJECT_H

#include <Windows.h>
#include <glut.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <glaux.h>
#include <iostream> 
#include <fstream>
#include <string>

#include "CHC_Constants.h"
#include "CHC_Vector3.h"

using namespace std;

class CHC_Object {
protected:
	static bool Loaded; // �Ƿ��Ѿ��������벢��������ʾ�б�
	static GLuint ViewList;
	static int TextureIndex;
	CHC_Vector3 Position, Normal;
	string ModelPath;
public:
	void Load();
	void Draw();
	void Draw(int x, int y, int dir);
	CHC_Object() {}
	CHC_Object(CHC_Vector3 _postion, CHC_Vector3 _normail, string _ModelPath);
};


#endif