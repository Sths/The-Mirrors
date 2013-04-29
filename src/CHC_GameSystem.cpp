#include "CHC_GameSystem.h"
#include <stdio.h>

CHC_Mirror	DrawMirror;
CHC_Laser	DrawLaser;

void CHC_GameSystem::Restart()
{
	now_nm = num_mirror;
	for (int i = 0; i != map_w; i++)
		for (int j = 0; j != map_h; j++)
			now_map[i][j] = ori_map[i][j];
}

/* Load Map from file */
bool CHC_GameSystem::LoadMap(char * filename) {
	FILE * fin;
	if ((fin = fopen(filename, "r")) == NULL) return false;

	fscanf(fin, "%d%d", &map_w, &map_h);	
	for (int i = 0; i != map_w; i++)
		for (int j = 0; j != map_h; j++)
			ori_map[i][j].setBlank();

	fscanf(fin, "%d%d", &num_sender, &num_receiver);
	int x, y, d, r, g, b;
	for (int i = 0; i != num_sender; i++) {
		fscanf(fin, "%d%d%d", &x, &y, &d);
		ori_map[x][y].dir = d;
		ori_map[x][y].obj = SENDER_ID;
		fscanf(fin, "%d%d%d", &r, &g, &b);
		ori_map[x][y].col[d / 2].setColor(r, g, b);
	}
	for (int i = 0; i != num_receiver; i++) {
		fscanf(fin, "%d%d%d", &x, &y, &d);
		ori_map[x][y].dir = d;
		ori_map[x][y].obj = RECEIVER_ID;
		fscanf(fin, "%d%d%d", &r, &g, &b);
		ori_map[x][y].col[d / 2].setColor(r, g, b);
	}
	fscanf(fin, "%d", &num_mirror);

	Restart();

	return true;
}

bool CHC_GameSystem::InMap(int x, int y)
{
	return (0<=x)&&(x<map_w)&&(0<=y)&&(y<map_h);
}

bool CHC_GameSystem::Insert_mirror(int x, int y, int d)
{
	if (!InMap(x, y) || now_nm == 0 || !now_map[x][y].isBlank()) return false;
	now_map[x][y].obj = MIRROR_ID;
	now_map[x][y].dir = d;
	--now_nm;
	return true;
}

bool CHC_GameSystem::Delete_mirror(int x, int y)
{
	if (!InMap(x, y) || !now_map[x][y].isMirror()) return false;
	now_map[x][y].obj = BLANK_ID;
	++now_nm;
	return true;
}

bool CHC_GameSystem::Win() {
	for (int i = 0; i != map_w; i++)
		for (int j = 0; j != map_h; j++)
			if (now_map[i][j].isReceive() && !(now_map[i][j].col[ori_map[i][j].dir / 2] == ori_map[i][j].col[ori_map[i][j].dir / 2])) {
				return false;
			}
	return true;
}

int CHC_GameSystem::Reflection(int inDir, int mDir, int mirrorType)
{
	return DirChange[mirrorType][inDir>>1][mDir];
}

void CHC_GameSystem::GameDraw()
{
	for (int i = 0; i != map_w; i++)
		for (int j = 0; j != map_h; j++) {
			if (now_map[i][j].isMirror()) {
				DrawMirror.Set_Direction(now_map[i][j].dir);
				DrawMirror.Set_Position(i, j);
				DrawMirror.Draw();
			} else 
			if (now_map[i][j].isSender()) {
			} else 
			if (now_map[i][j].isReceive()) {
			}
		}
	for (int i = 0; i != map_w; i++)
		for (int j = 0; j != map_h; j++) {
			if (now_map[i][j].col[1].notZero()) {
				DrawLaser.set(i, j, i+1, j, now_map[i][j].col[1].changeToRGB());
				DrawLaser.Draw();
			}
			if (now_map[i][j].col[0].notZero()) {
				DrawLaser.set(i, j, i, j+1, now_map[i][j].col[0].changeToRGB());
				DrawLaser.Draw();
			}
		}
}

void CHC_GameSystem::Refresh()
{
	/* Refresh */
	for (int i = 0; i != map_w; i++)
		for (int j = 0; j != map_h; j++) 
		if (!now_map[i][j].isSender()) {
			for (int k = 0; k != 4; k++)
				now_map[i][j].col[k].setColor(0, 0, 0); 
		}

	bool mark[MAP_WIDTH][MAP_HEIGHT][8];		//hash
	for (int stx = 0; stx != map_w; stx++)
		for (int sty = 0; sty != map_h; sty++) 
		if (now_map[stx][sty].isSender()) {
			int x = stx, y = sty, d = now_map[stx][sty].dir;
			CHC_Color col = now_map[stx][sty].col[d / 2];

			memset(mark, 0, sizeof(mark));
			while (true) {
				mark[x][y][d] = true;
				now_map[x][y].col[d / 2].add(col);
				x += dx[d / 2], y += dy[d / 2];
				if (!InMap(x, y) || mark[x][y][d]) break;

				now_map[x][y].col[(d / 2 + 2) % 4].add(col);
				if (now_map[x][y].isMirror()) {
					d = Reflection(d, now_map[x][y].dir, 0);
				} else
					if (now_map[x][y].isReceive() || now_map[x][y].isSender()) break;
			}
		}

	GameDraw();
	win_flag = Win();
	
	if (win_flag) {
		/* Win */
	}
}

void CHC_GameSystem::xyToBoardxy(float fx, float fy, int & x, int &y)
{
	x = floor(fx);
	y = floor(fy);
}

void CHC_GameSystem::ClickMouse(CHC_Line & L, int State)
{
	CHC_Vector3 P;
	if (!L.istYequal0(P)) return;
	if (State == MOUSE_OTHER_STATE) return;
	int x, y;
	xyToBoardxy(P[0], P[2], x, y);
	if (!InMap(x, y)) return;
	if (State == 8) {
		// Delete
		Delete_mirror(x, y);
	} else
	if (State >= 0 && State < 8) {
		// Insert
		Insert_mirror(x, y, State);
	}
}

void CHC_GameSystem :: getnearest(CHC_Line L , CHC_Vector3 & p)
{
	CHC_Vector3 tempP,point;
	float t = L.istYequal0(tempP);
	int markx = int(tempP[0]) ,marky = int(tempP[2]), mark = 0;
	for (int i = 0 ; i < MAP_WIDTH; i++)
		for (int j = 0 ; j < MAP_HEIGHT; i++)
			if (now_map[i][j].isMirror()){
				CHC_Mir_Info temp(now_map[i][j].dir,i,j);
				float t1 = L.isitersectionplane(temp.x[0],temp.n,tempP);
				if (L.isonmirror(tempP,temp.x[0],temp.x[1],temp.x[2],temp.x[3]) && t1 < t){
					t = t1;markx = i;marky = j;mark = 1;
				}
			}
			p = CHC_Vector3(markx,marky,mark);
}
