
#pragma once

class CGlobalKinect
{
public:
	CGlobalKinect();
	~CGlobalKinect();

	void SetDeskLeftCorner(CVector3D corner);
	void SetDeskRightCorner(CVector3D corner);

	CVector3D GetDeskLeftCorner();
	CVector3D GetDeskRightCorner();

	static CGlobalKinect* GetInstance();

private:
	CVector3D m_leftCorner;
	CVector3D m_rightCorner;
};