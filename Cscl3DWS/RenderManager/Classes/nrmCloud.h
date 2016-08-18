
#pragma once

#include "nrmanager.h"
#include "nrmObject.h"
#include "nrmViewport.h"
#include "nrm3DObject.h"
#include "LinkSubmitter.h"
#include "OcclusionQuery.h"

// выравнивание сноски относительно облачка
#define ALIGN_ID_LEFT		1
#define ALIGN_ID_CENTER		2
#define ALIGN_ID_RIGHT		3

#define MAX_VISIBLE_DIST	1500.0f
#define MAX_VISIBLE_DIST_SQ	MAX_VISIBLE_DIST * MAX_VISIBLE_DIST

#define MAX_POINT_VISIBLE_DIST		10000.0f
#define MAX_POINT_VISIBLE_DIST_SQ	MAX_POINT_VISIBLE_DIST * MAX_POINT_VISIBLE_DIST

class nrmCloud : public nrm3DObject,
				 public moICloud,
				 public CLinkSubmitter
{
public:
	nrmCloud(mlMedia* apMLMedia);
	virtual ~nrmCloud();

	void update(DWORD dwTime, void* data);
	void OnSetRenderManager();

	void VisibleChanged();
	void ContentChanged();
	ml3DPosition GetPos();
	bool IsSortOnDepth();
	void SetPos(ml3DPosition &aPos);
	void PosChanged(ml3DPosition &pos);
	void FootnoteChanged();
	void AlignChanged();
	void MoveOutOfScreenChanged();
	void AllowOverlapChanged();
	void SortOnDepthChanged();
	void AttachedToChanged(mlMedia* pmlMedia);
	void MaxVisibleDistanceChanged();
	void SetSpecParameters(int adeltaNoChangePos, int ashiftAtAvatarPosZ);
	void ViewVecChanged(ml3DPosition &viewVec);

	moICloud* GetICloud();

	bool SrcChanged();

	void SetActiveViewport(nrmViewport* pViewport);

	void SetScreenSize(int width, int height);

	void SetDepth(float depth);

	void MarginXChanged();
	void MarginYChanged();

	void SetDistSq(float distSq);
	float GetDistSq()const;

	bool IsVisible()const;
	void SetVisible(bool isShow);
	
	int GetCurrentLevel() { return 0; }
	void LevelChanged(int level) {}

private:
	void AutoSetZPosition();

	ml3DPosition m_position;
	ml3DPosition m_viewVector;
	mlPoint m_oldPos;

	nrmViewport* m_pViewport;

	nrmImage* m_footnoteImage;

	COcclusionQuery m_occlusionQuery;

	mlMedia* m_content;
	mlMedia* m_footnote;
	mlMedia* m_attachedTo;

	int m_screenWidth, m_screenHeight;
	int m_contentWidth, m_contentHeight;
	int m_alignID;
	int m_marginX, m_marginY;
	float m_maxVisibleDistance;
	float m_maxVisibleDistanceSq;

	bool m_moveOutOfScreenAllowed;
	bool m_overlapAllowed;
	bool m_sortOnDepthNeeded;
	bool m_isVisible;

	bool m_isShow;
	int m_contentX, m_contentY;
	float m_depth;
	float m_distSq;

	int m_deltaNoChangePos;
	int m_shiftAtAvatarPosZ;
};