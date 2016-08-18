// AnimSequencer.h: interface for the CalAnimSequencer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMSEQUENCER_H__1A09B750_33B8_4AC1_9EB2_2B3E59EE612C__INCLUDED_)
#define AFX_ANIMSEQUENCER_H__1A09B750_33B8_4AC1_9EB2_2B3E59EE612C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef void (*pcbfSeqAccomplished)(int,int,LPVOID);

class CalAnimSequencer  
{
	friend class CalMixer;
public:
	void blendCycle(int id, float delay=0.0f);
	void executeAction(int id, float delay=0.0f);
	void resume();
	void animExecuted(int mId);
	bool create(CalMixer* pm, int sid, int mid, int eid);
	void stop();
	void play();
	CalAnimSequencer();
	virtual ~CalAnimSequencer();

protected:
	int			m_seqID;
	int			m_startID;
	int			m_cycleID;
	int			m_endID;
	int			mode;
	float		m_fadeDelay;
	CalMixer*	m_pMixer;
	pcbfSeqAccomplished	pfAccomlished;
	LPVOID				pdAccomlished;
};

#endif // !defined(AFX_ANIMSEQUENCER_H__1A09B750_33B8_4AC1_9EB2_2B3E59EE612C__INCLUDED_)