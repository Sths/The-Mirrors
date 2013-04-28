#include "CHC_Line.h"

CHC_Line::CHC_Line() {
	S.set(0, 0, 0);		
	T.set(0, 0, 0);
}

CHC_Line::CHC_Line(const CHC_Vector3 & _S, const CHC_Vector3 & _T) {
	S = _S;
	T = _T;
}

CHC_Vector3 CHC_Line::operator[](unsigned int id) {
	if (id == 0) return S;
	if (id == 1) return T;
	else return CHC_Vector3(0, 0, 0);
}

BOOL CHC_Line::istYequal0(CHC_Vector3 & P) {
	if (S[1] == T[1]) return FALSE;

	float r = S[1] / (S[1] - T[1]);
	P = (T - S) * r + S;
	return TRUE;
}
