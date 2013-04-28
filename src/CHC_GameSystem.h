#ifndef CHC_GAMESYSTEM_H
#define CHC_GAMESYSTEM_H

#include "CHC_Constants.h"
#include "CHC_Vector3.h"
#include "CHC_Line.h"
#include "CHC_Color.h"
#include "CHC_Mirror.h"
#include "CHC_Laser.h"

#define MAP_WIDTH	16
#define MAP_HEIGHT	16
#define BLANK_ID	0
#define MIRROR_ID	1
#define SENDER_ID	2
#define RECEIVER_ID	3

static const int dx[4] = {0, 1, 0, -1};
static const int dy[4] = {1, 0, -1, 0};

static const int DirChange[1][4][8] = 
{ { {-1, -1, -1, 2, 4 ,6 , -1, -1},
	{-1, -1, -1, -1, -1, 4, 6, 0},
	{0, 2, -1 -1, -1, -1, -1, 6},
	{-1, 0, 2, 4, -1, -1, -1, -1}
	}
};


class CHC_Map_Info
{
public:
	int obj;					// object id
	int dir;					// direction
	CHC_Color col[4];				// need when obj is Sender or Receive 

	void operator=(const CHC_Map_Info & m) {
		obj = m.obj;
		dir = m.dir;
		for (int i = 0; i != 4; i++)
			col[i] = m.col[i];
	}
	void setBlank() {
		obj = BLANK_ID;
		for (int i = 0; i != 4; i++)
			col[i].setColor(0, 0, 0);
	}
	bool isMirror() {
		return obj == MIRROR_ID;
	}
	bool isBlank() {
		return obj == BLANK_ID;
	}
	bool isSender() {
		return obj == SENDER_ID;
	}
	bool isReceive() {
		return obj == RECEIVER_ID;
	}
};

class CHC_GameSystem
{
public:
	CHC_Map_Info ori_map[MAP_WIDTH][MAP_HEIGHT];
	CHC_Map_Info now_map[MAP_WIDTH][MAP_HEIGHT];

	bool InMap(int x, int y);
	bool LoadMap(char *filename);
	void Restart();
	void Refresh();
	bool Win();
	bool Insert_mirror(int x, int y, int d);
	bool Delete_mirror(int x, int y);
	int Reflection(int inDir, int mDir, int mirrorType = 0);
	void ClickMouse(CHC_Line & L, int State);
	void xyToBoardxy(float fx, float fy, int & x, int &y);

	void GameDraw();
	//static double intersectionPlane(CHC_Vector3 x0, CHC_Vector3 n, CHC_Vector3 x1, CHC_Vector3 n1);

private:
	bool win_flag;
	int map_w, map_h;		// map width, height
	int num_mirror, now_nm;				// number of mirror
	int num_sender, num_receiver;		// number of sender & receiver
};


#endif