#pragma once

class IPhysics
{
public:

	// добавить примитив
	virtual void AddPrimitive(char type,
		float transX, float transY, float transZ,
		float rotX, float rotY, float rotZ, float rotW,
		float scaleX, float scaleY, float scaleZ,
		float x_length, float y_length, float z_length) = 0;
		//float r) = 0;

};
