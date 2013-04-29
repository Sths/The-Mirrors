#include "CHC_GameSystem.h"
#include <stdio.h>

extern bool MouseLeftDown;
extern int	MousePickMirror;
extern CHC_Mirror	DrawMirror;
extern CHC_Laser	DrawLaser;
int  PickMirrorState;
static bool mark[MAP_WIDTH][MAP_HEIGHT][8];		//hash

void CHC_GameSystem::Restart()
{
	now_nm = num_mirror;
	for (int i = 0; i != map_w; i++)
		for (int j = 0; j != map_h; j++)
			now_map[i][j] = ori_map[i][j];
	MousePickMirror = -1;
}

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
		fscanf(fin, "%d%d", &x, &y);
		ori_map[x][y].obj = RECEIVER_ID;
		fscanf(fin, "%d%d%d", &r, &g, &b);
		ori_map[x][y].col[0].setColor(r, g, b);
	}
	fscanf(fin, "%d", &num_mirror);

	Restart();

	return true;
}

int  CHC_GameSystem::InToolBar(int x, int y)
{
	if (MIRROR_TOOLBAR_X <= x && x < MIRROR_TOOLBAR_X + MIRROR_TOOLBAR_LEN_X) {
		if (MIRROR_TOOLBAR_Y <= y && y < MIRROR_TOOLBAR_Y + MIRROR_TOOLBAR_LEN_Y) {
			return y - MIRROR_TOOLBAR_Y;
		}
	}
	return -1;
}

bool CHC_GameSystem::InMap(int x, int y)
{
	return (0<=x)&&(x<map_w)&&(0<=y)&&(y<map_h);
}

bool CHC_GameSystem::Insert_mirror(int x, int y, int sort, int d)
{
	if (!InMap(x, y) || now_nm == 0 || !now_map[x][y].isBlank()) return false;
	now_map[x][y].obj = MIRROR_ID;
	now_map[x][y].mirror_s = sort;
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
			if (now_map[i][j].isReceive() && !(now_map[i][j].col[0] == ori_map[i][j].col[0])) {
				return false;
			}
	return true;
}

int CHC_GameSystem::Reflection(int inDir, int mDir, int mirrorType)
{
	return DirChange[mirrorType][inDir>>1][mDir];
}

/* Load Map from file */
void CHC_GameSystem::DrawMirrorToolbar()
{
	if (now_nm > 0) {
		DrawMirror.Set_Direction(START_MIRROR_STATE);
		DrawMirror.Set_Position(MIRROR_TOOLBAR_X, MIRROR_TOOLBAR_Y);
		DrawMirror.Draw();
	}
}

void CHC_GameSystem::GameDraw()
{
	CHC_Color RC;
	for (int i = 0; i != map_w; i++)
		for (int j = 0; j != map_h; j++) {
			if (now_map[i][j].isMirror()) {
				DrawMirror.Set_Direction(now_map[i][j].dir);
				DrawMirror.Set_Position(i, j);
				DrawMirror.Draw();
			} else 
			if (now_map[i][j].isSender()) {
				glPushMatrix();
					glTranslated((float)i, 0.5, (float)j);
					glColor3f(0.0, 0.0, 1.0);
					glutSolidCube(1);
				glPopMatrix();
			} else 
			if (now_map[i][j].isReceive()) {
				RC.setColor(0, 0, 0);
				for (int k = 0; k != 4; k++) {
					RC.add(now_map[i][j].col[k]);
				}
				if (RC >= ori_map[i][j].col[0]) {
					glPushMatrix();
						glTranslated((float)i, 0.5, (float)j);
						glColor3f(0.0, 1.0, 0.0);
						glutSolidCube(1);
					glPopMatrix();
				} else {
					glPushMatrix();
						glTranslated((float)i, 0.5, (float)j);
						glColor3f(1.0, 0.0, 0.0);
						glutSolidCube(1);
					glPopMatrix();
				}
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
	
	DrawMirrorToolbar();
}

void CHC_GameSystem::RefreshBackTrace(int x, int y, int d, CHC_Color & col)
{
	mark[x][y][d] = true;
	now_map[x][y].col[d/2].add(col);
	x += dx[d/2], y += dy[d/2];
	if (!InMap(x, y) || mark[x][y][d]) return;
	now_map[x][y].col[(d / 2 + 2) % 4].add(col);
	if (now_map[x][y].isMirror()) {
		d = Reflection(d, now_map[x][y].dir, 0);
		if (d < 0) return;
	} else
		if (now_map[x][y].isSender()) return;
	RefreshBackTrace(x, y, d, col);
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

	for (int stx = 0; stx != map_w; stx++)
		for (int sty = 0; sty != map_h; sty++) 
		if (now_map[stx][sty].isSender()) {
			int x = stx, y = sty, d = now_map[stx][sty].dir;
			CHC_Color col = now_map[stx][sty].col[d / 2];
			memset(mark, 0, sizeof(mark));
			RefreshBackTrace(x, y, d, col);
		}

	GameDraw();
	win_flag = Win();
	
	if (win_flag) {
		/* Win */
		/*
		glPushMatrix();
			glTranslated((float)map_w/2, 4, (float)map_h/2);
			glColor3f(0.1, 0.3, 0.3);
			glutSolidCube(map_w);
		glPopMatrix();
		*/
	}
}

void CHC_GameSystem::xyToBoardxy(float fx, float fy, int & x, int &y)
{
	x = floor(fx + 0.5f);
	y = floor(fy + 0.5f);
}

void CHC_GameSystem::KeyBoardDelete()
{
	if (InMap(SelectX, SelectY) && now_map[SelectX][SelectY].isMirror()) {
		Delete_mirror(SelectX, SelectY);
	}
}

void CHC_GameSystem::KeyBoardChangeState(int dx)
{
	if (MousePickMirror >= 0) {
		PickMirrorState = (PickMirrorState + dx + NUM_MIRROR_STATE) % NUM_MIRROR_STATE;
	} else 
	if (InMap(SelectX, SelectY) && now_map[SelectX][SelectY].isMirror()) {
		now_map[SelectX][SelectY].dir = (now_map[SelectX][SelectY].dir + dx + NUM_MIRROR_STATE) % NUM_MIRROR_STATE;
	}
}

void CHC_GameSystem::ClickMouseUp(CHC_Line & L)
{
	CHC_Vector3 P;
	int x, y; 

	L.istYequal0(P);
	xyToBoardxy(P[0], P[2], x, y);
	
	if (InMap(SelectX, SelectY)) {
		if (InMap(x, y)) {
			Delete_mirror(SelectX, SelectY);
			Insert_mirror(x, y, MousePickMirror, PickMirrorState);
			SelectX = x, SelectY = y;			// Select
		} else {
			Delete_mirror(SelectX, SelectY);
			SelectX = SelectY = 255;			// outside
		}
	} else 
	if (InToolBar(SelectX, SelectY) >= 0) {
		SelectX = SelectY = 255;
		if (InMap(x, y)) {
			Insert_mirror(x, y, MousePickMirror, PickMirrorState);
			SelectX = x, SelectY = y;			// Select
		}
	}
}

void CHC_GameSystem::ClickMouse(CHC_Line & L)
{
	CHC_Vector3 P;
	getnearest(L, P);
	int x, y; 
	xyToBoardxy(P[0], P[1], x, y);
	
	SelectX = 255, SelectY = 255;
	if (InToolBar(x, y) >= 0) {
		if (MouseLeftDown && now_nm > 0) {
			SelectX = x, SelectY = y;
			MousePickMirror = y - MIRROR_TOOLBAR_Y;
			PickMirrorState = START_MIRROR_STATE;
		} else MousePickMirror = -1;
	} else
	if (InMap(x, y)) {
		if (MouseLeftDown && now_map[x][y].isMirror()) {
			SelectX = x, SelectY = y;
			PickMirrorState = now_map[x][y].dir;
			MousePickMirror = now_map[x][y].mirror_s;
		} else MousePickMirror = -1;
	}
}

void CHC_GameSystem::getnearest(CHC_Line & L , CHC_Vector3 & p)
{
	CHC_Vector3 tempP,point;
	float t = L.istYequal0(tempP);
	float markx = tempP[0];
	float marky = tempP[2];
	float mark = 0;

	for (int i = 0 ; i < map_w; i++)
		for (int j = 0 ; j < map_h; j++)
			if ( now_map[i][j].isMirror() ) {
				printf("Find it !\n");
				CHC_Mir_Info temp(now_map[i][j].dir, i, j);
				float t1 = L.isItersectionplane(temp.x[0], temp.n, tempP);
				if (L.isOnMirror(tempP,temp.x[0],temp.x[1],temp.x[2],temp.x[3]) && t1 < t){
					t = t1;
					markx = i;
					marky = j;
					mark = 1;
				}
			}
	p = CHC_Vector3(markx, marky, mark);
}