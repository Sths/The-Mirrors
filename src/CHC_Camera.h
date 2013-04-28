#ifndef CHC_Camera_H
#define CHC_Camera_H

#include "CHC_Vector3.h"

class CHC_Camera
{
public:
	CHC_Camera() {
		CHC_Camera::set(0, 0, 0, 1, 0, 0, 0, 1, 0);
	}

	//Position, view, upVector
	CHC_Camera(float pX, float pY, float pZ, float vX, float vY, float vZ,
		   float uX, float uY, float uZ) { CHC_Camera::set(pX, pY, pZ, vX, vY, vZ, uX, uY, uZ); }
	//~CHC_Camera();

	void set(float pX, float pY, float pZ, float vX, float vY, float vZ,
		   float uX, float uY, float uZ);
	void set(CHC_Vector3 pos, CHC_Vector3 view);
	
	CHC_Vector3 getPosition() const { return m_P; }
	void setPosition(const CHC_Vector3 &ve) {
		m_P = ve;
	}

	CHC_Vector3 getView() const { return m_pV; }
	void setView(const CHC_Vector3 &ve) { 
		m_pV = ve;
	}

	void moveStraight(float dis);	// �����ӵ㷽��ǰ��
	void strafe(float dis);			// �����ӵ㷽������߶�
	void verticalM(float dis);		// �����ӵ㷽��ֱ��up��(ֱ������)
	void rotateView(float angle, float x, float y, float z);	// ��ת����
	void yaw(float angle);			// ��y��
	void pitch(float angle);		// ��x��
	void worldUp(float angle);		// ���������ϵ��ֱ������
	void setLook();					// ����OpenGL����LookAt
	void setViewByMouse(int x, int y, float sensX, float sensY); //������
	void setLook(float discenter);					// ����OpenGL����LookAt
	
//private:
	CHC_Vector3 m_P, m_pV, m_U;
};

#endif