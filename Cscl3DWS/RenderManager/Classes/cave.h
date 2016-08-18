
#pragma once

class CCave
{
public:
	CCave();
	~CCave();

	bool Init();

	float GetX();
	float GetY();

	float GetDeltaX();
	float GetDeltaY();
	void FixPosition();

	void Update();

private:
	float m_x, m_y;
	float m_fixX, m_fixY;
	int m_trackerKey;
	void* m_tracker;
};