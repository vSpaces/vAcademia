
#pragma once

#define OCCLUSION_POINT				1
#define OCCLUSION_BOUNDING_BOX		2

class COcclusionQuery
{
public:
	COcclusionQuery();
	~COcclusionQuery();

	void SetQueryPoint(float x, float y, float z);
	void SetQueryBoundingBox(float x, float y, float z, float xSize, float ySize, float zSize);
	void DoQuery();

	void StartQuery();
	void EndQuery();

	int GetFragmentCount();
	bool IsResultAvailable();

	void UpdateQueryObject();

private:
	unsigned int m_queryID;
	unsigned int m_fragmentCount;

	int m_type;

	bool m_isResultAvailable;
	bool m_isQueryStarted;
	unsigned char m_status;

	float m_x, m_y, m_z;
	float m_xSize, m_ySize, m_zSize;
};