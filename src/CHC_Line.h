#ifndef CHC_LINE_H
#define CHC_LINE_H

#include "CHC_Vector3.h"

class CHC_Line
{
public:
	CHC_Line();
	CHC_Line(const CHC_Vector3 & _S, const CHC_Vector3 & _T);

	CHC_Vector3 operator[](unsigned int i);			// [0]: S,  [1]: T
	BOOL istYequal0(CHC_Vector3 & P);				// intersection with Y = 0 Plane

private:
	CHC_Vector3 S, T;
};

#endif