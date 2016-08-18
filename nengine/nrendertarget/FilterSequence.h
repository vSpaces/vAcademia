
#pragma once

#include "CommonRenderTargetHeader.h"

#include "Filter.h"
#include "SimpleDrawingTarget.h"

typedef struct _SImageParameter
{
	MP_STRING name;
	unsigned int slotID;
	unsigned int textureID;
	MP_STRING textureName;

	_SImageParameter();
} SImageParameter;

typedef struct _SFloatParameter
{
	MP_STRING name;
	unsigned int slotID;
	float value;

	_SFloatParameter();
} SFloatParameter;

typedef struct _SFilterInfo
{
	CFilter* filter;
	MP_VECTOR<SImageParameter *> imageParams;
	MP_VECTOR<SFloatParameter *> floatParams;
	void* resultImage;	

	_SFilterInfo();
	~_SFilterInfo();
} SFilterInfo;

typedef struct _SImageReference
{
	SFilterInfo* filterInfo;
	unsigned int slotID;

	_SImageReference()
	{
		filterInfo = NULL;
		slotID = 0;
	}
} SImageReference;

typedef struct _SImageDescription
{
	MP_STRING name;
	MP_STRING like;
	bool isRenderTarget;
	CSimpleDrawingTarget* renderTarget;
	bool isSelfCreated;
	unsigned int textureID;
	MP_VECTOR<SImageReference *> references;

	_SImageDescription();
} SImageDescription;

class CFilterSequence : public CCommonObject
{
public:
	CFilterSequence();
	~CFilterSequence();

	bool Load(std::string fileName, bool isIndependent);
	bool IsLoaded()const;
	bool IsIndependent()const;

	void PrepareForExecution();
	bool Execute();
	
	void SetSourceImage(unsigned int slotID, unsigned int textureID);
	void SetResultImage(unsigned int slotID, unsigned int textureID);
	void SetTempImage(unsigned int slotID, unsigned int textureID);

	unsigned int GetResultImage(unsigned int slotID);
	unsigned int DetachResultImage(unsigned int slotID);

	void AddImageReference(std::string& imageName, SFilterInfo* filterInfo, unsigned int slotID);

private:
	void PrepareForExecutionIfNeeded();
	void PrepareImage(SImageDescription* imageDesc);

	SImageDescription* GetImageDescription(std::string& name);

	void UpdateImageDescription(SImageDescription* imageDescription, unsigned int textureID);

	CSimpleDrawingTarget* GetRenderTarget(unsigned int width, unsigned int height);

	bool m_isIndependent;
	bool m_isPrepared;
	bool m_isLoaded;

	MP_VECTOR<SFilterInfo *> m_filters;
	MP_VECTOR<SImageDescription *> m_tempImages;
	MP_VECTOR<SImageDescription *> m_sourceImages;
	MP_VECTOR<SImageDescription *> m_resultImages;
	MP_MAP<MP_STRING, SImageDescription *> m_imagesMap;

	MP_VECTOR<CSimpleDrawingTarget *> m_renderTargets;
};