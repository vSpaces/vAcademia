
#pragma once

#include "CommonEngineHeader.h"

class CAuxilaryProcessor
{
friend class CGlobalSingletonStorage;

public:
	void SetEdgesSrcFile(std::string path);
	void DrawEdges();
	void AskEndWork(unsigned int maxTime);
	void FadeIn(int localTime, int allTime);
	void FadeOut(int localTime, int allTime);
	void WhiteIn(int localTime, int allTime);
	void WhiteOut(int localTime, int allTime);

private:
	CAuxilaryProcessor();
	CAuxilaryProcessor(const CAuxilaryProcessor&);
	CAuxilaryProcessor& operator=(const CAuxilaryProcessor&);
	~CAuxilaryProcessor();

	MP_STRING m_edgesSrcFile;
};
