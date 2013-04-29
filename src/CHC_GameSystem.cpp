#include "CHC_GameSystem.h"
#include <stdio.h>

extern bool MouseLeftDown;
extern int	MousePickMirror;
extern CHC_Mirror	DrawMirror;
extern CHC_Laser	DrawLaser;
int  PickMirrorState;

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
				if (now_map[i][j].col[now_map[i][j].dir / 2] >= ori_map[i][j].col[now_map[i][j].dir / 2]) {
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
					if (d < 0) break;
				} else
					if (now_map[x][y].isReceive() || now_map[x][y].isSender()) break;
			}
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
	if (!L.istYequal0(P)) return;
		
	int x, y; 
	xyToBoardxy(P[0], P[2], x, y);
	
	if (InMap(x, y)) Insert_mirror(x, y, PickMirrorState);
}

void CHC_GameSystem::ClickMouse(CHC_Line & L)
{
	CHC_Vector3 P;
	if (!L.istYequal0(P)) return;
		
	int x, y; 
	xyToBoardxy(P[0], P[2], x, y);
	
	SelectX = 255, SelectY = 255;
	if (InToolBar(x, y) >= 0) {
		if (MouseLeftDown && now_nm > 0) {
			MousePickMirror = y - MIRROR_TOOLBAR_Y;
			PickMirrorState = START_MIRROR_STATE;
		} else MousePickMirror = -1;
	} else
	if (InMap(x, y)) {
		if (now_map[x][y].isMirror()) {
			SelectX = x, SelectY = y;
			
		}
	}
}
