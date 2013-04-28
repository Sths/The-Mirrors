#ifndef CHC_MIRROR_H
#define CHC_MIRROR_H

#include "CHC_Object.h"
#include "CHC_Vector3.h"
#include "CHC_Constants.h"

class CHC_Mirror:CHC_Object {
public:
	CHC_Mirror() {}
	CHC_Mirror(int x, int y, int dir);
	void Set_Position(int x, int y);
	void Set_Direction(int dir);
	void Load();
	void Draw();
};


#endif