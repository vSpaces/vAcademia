
#include "StdAfx.h"
#include "TinyXML.h"
#include "FilterSequence.h"
#include "GlobalSingletonStorage.h"

_SFilterInfo::~_SFilterInfo()
{
		std::vector<SImageParameter *>::iterator itI = imageParams.begin();
		for ( ; itI != imageParams.end(); itI++)
		{
			MP_DELETE(*itI);
		}

		std::vector<SFloatParameter *>::iterator itF = floatParams.begin();
		for ( ; itF != floatParams.end(); itF++)
		{
			MP_DELETE(*itF);
		}
}

_SImageDescription::_SImageDescription():
	MP_VECTOR_INIT(references),
	MP_STRING_INIT(name),
	MP_STRING_INIT(like)
{
	isRenderTarget = false;
	isSelfCreated = true;
	renderTarget = NULL;
	textureID = 0xFFFFFFFF;
}

_SImageParameter::_SImageParameter():
	MP_STRING_INIT(name),
	MP_STRING_INIT(textureName)
{
	slotID = 0;
	textureID = 0xFFFFFFFF;
}

_SFloatParameter::_SFloatParameter():
	MP_STRING_INIT(name)
{
	slotID = 0;
	value = 0.0f;
}

_SFilterInfo::_SFilterInfo():
	MP_VECTOR_INIT(imageParams),
	MP_VECTOR_INIT(floatParams)
{
	filter = NULL;
}

CFilterSequence::CFilterSequence():
	m_isLoaded(false),
	m_isPrepared(false),
	m_isIndependent(false),
	MP_VECTOR_INIT(m_filters),
	MP_VECTOR_INIT(m_tempImages),
	MP_VECTOR_INIT(m_sourceImages),
	MP_VECTOR_INIT(m_resultImages),
	MP_MAP_INIT(m_imagesMap),
	MP_VECTOR_INIT(m_renderTargets)
{
}

bool CFilterSequence::Load(std::string fileName, bool isIndependent)
{
	m_isIndependent = isIndependent;
	
	TiXmlDocument doc( fileName.c_str());
	bool isLoaded = g->rl->LoadXml( (void*)&doc, fileName);

	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] Loading filter sequence ", fileName, " failed...");
		return false;
	}

	TiXmlNode* sequence = doc.FirstChild("sequence");

	TiXmlNode* source = sequence->FirstChild("source");
	TiXmlNode* image = source->FirstChild("image");
	while (image != NULL)
	{
		SImageDescription* imageDesc = MP_NEW(SImageDescription);
		imageDesc->name = image->ToElement()->GetText();
		m_sourceImages.push_back(imageDesc);
		m_imagesMap.insert(MP_MAP<MP_STRING, SImageDescription *>::value_type(MAKE_MP_STRING(imageDesc->name), imageDesc));
		image = source->IterateChildren("image", image);
	}

	TiXmlNode* temps = sequence->FirstChild("temp");
	if (temps)
	{
		image = temps->FirstChild("image");
	}
	else
	{
		image = NULL;
	}

	while (image != NULL)
	{
		SImageDescription* imageDesc = MP_NEW(SImageDescription);
		imageDesc->name = image->ToElement()->GetText();
		if (image->ToElement()->Attribute("like"))
		{
			imageDesc->like = image->ToElement()->Attribute("like");
		}
		m_tempImages.push_back(imageDesc);
		m_imagesMap.insert(MP_MAP<MP_STRING, SImageDescription *>::value_type(MAKE_MP_STRING(imageDesc->name), imageDesc));
		image = temps->IterateChildren("image", image);
	}

	TiXmlNode* result = sequence->FirstChild("result");
	image = result->FirstChild("image");
	while (image != NULL)
	{
		std::string name = image->ToElement()->GetText();
		SImageDescription* imageDesc = GetImageDescription(name);
		if (imageDesc)
		{
			m_resultImages.push_back(imageDesc);
		}
		else
		{
			imageDesc = MP_NEW(SImageDescription);
			imageDesc->name = name;
			if (image->ToElement()->Attribute("like"))
			{
				imageDesc->like = image->ToElement()->Attribute("like");
			}
			m_resultImages.push_back(imageDesc);
			m_imagesMap.insert(MP_MAP<MP_STRING, SImageDescription *>::value_type(MAKE_MP_STRING(imageDesc->name), imageDesc));
		}		
		image = result->IterateChildren("image", image);
	}

	TiXmlNode* filters = sequence->FirstChild("filters");
	TiXmlNode* filter = filters->FirstChild("filter");
	while (filter != NULL)
	{
		std::string name = filter->ToElement()->Attribute("name");
		std::string result = filter->ToElement()->Attribute("result");

		SFilterInfo* filterInfo = MP_NEW(SFilterInfo);
		int filterID = g->fp.GetObjectNumber(name);
		filterInfo->filter = g->fp.GetObjectPointer(filterID);
		filterInfo->resultImage = GetImageDescription(result);
		((SImageDescription *)filterInfo->resultImage)->isRenderTarget = true;

		image = filter->FirstChild("image");
		while (image != NULL)
		{
			std::string name = image->ToElement()->Attribute("name");
			
			SImageParameter* imageParam = MP_NEW(SImageParameter);
			imageParam->name = name;
			imageParam->slotID = filterInfo->filter->GetImageID(name);
			imageParam->textureName = image->ToElement()->GetText();

			AddImageReference(imageParam->textureName, filterInfo, imageParam->slotID);

			filterInfo->imageParams.push_back(imageParam);

			image = filter->IterateChildren("image", image);
		}

		TiXmlNode* fl = filter->FirstChild("float");
		while (fl != NULL)
		{
			std::string name = fl->ToElement()->Attribute("name");
			
			SFloatParameter* floatParam = MP_NEW(SFloatParameter);
			floatParam->name = name;
			floatParam->slotID = filterInfo->filter->GetFloatID(name);
			floatParam->value = (float)atof(fl->ToElement()->GetText());

			filterInfo->floatParams.push_back(floatParam);

			fl = filter->IterateChildren("float", fl);
		}
		
		m_filters.push_back(filterInfo);

		filter = filters->IterateChildren("filter", filter);
	}
	
	return true;
}

bool CFilterSequence::IsLoaded()const
{
	return m_isLoaded;
}

bool CFilterSequence::IsIndependent()const
{
	return m_isIndependent;
}

void CFilterSequence::PrepareForExecution()
{
	PrepareForExecutionIfNeeded();
}

bool CFilterSequence::Execute()
{
	PrepareForExecutionIfNeeded();

	std::vector<SFilterInfo *>::iterator it = m_filters.begin();
	std::vector<SFilterInfo *>::iterator itEnd = m_filters.end();

	for ( ; it != itEnd; it++)
	{
		SImageDescription* imageDesc = (SImageDescription *)(*it)->resultImage;

		{
			std::vector<SFloatParameter *>::iterator itf = (*it)->floatParams.begin();
			std::vector<SFloatParameter *>::iterator itfEnd = (*it)->floatParams.end();

			for ( ; itf != itfEnd; itf++)
			{
				(*it)->filter->SetFloatArgument((*itf)->slotID, (*itf)->value);
			}
		}

		{
			std::vector<SImageParameter *>::iterator iti = (*it)->imageParams.begin();
			std::vector<SImageParameter *>::iterator itiEnd = (*it)->imageParams.end();

			for ( ; iti != itiEnd; iti++)
			{
				(*it)->filter->SetSourceImage((*iti)->slotID, (*iti)->textureID);
			}
		}

		imageDesc->renderTarget->AttachTexture(g->tm.GetObjectPointer(imageDesc->textureID));
		imageDesc->renderTarget->ApplyFilter((*it)->filter);
	}

	return true;
}
	
void CFilterSequence::SetSourceImage(unsigned int slotID, unsigned int textureID)
{
	if (m_sourceImages.size() > slotID)
	{
		UpdateImageDescription(m_sourceImages[slotID], textureID);
	}
}

void CFilterSequence::SetResultImage(unsigned int slotID, unsigned int textureID)
{
	if (m_resultImages.size() > slotID)
	{
		UpdateImageDescription(m_resultImages[slotID], textureID);
	}
}

void CFilterSequence::SetTempImage(unsigned int slotID, unsigned int textureID)
{
	if (m_tempImages.size() > slotID)
	{
		UpdateImageDescription(m_tempImages[slotID], textureID);
	}
}

void CFilterSequence::UpdateImageDescription(SImageDescription* imageDescription, unsigned int textureID)
{
	imageDescription->textureID = textureID;
	std::vector<SImageReference *>::iterator it = imageDescription->references.begin();
	for ( ; it != imageDescription->references.end(); it++)
	{
		std::vector<SImageParameter *>::iterator itP = (*it)->filterInfo->imageParams.begin();
		std::vector<SImageParameter *>::iterator end = (*it)->filterInfo->imageParams.end();
		for ( ; itP != end; itP++)
		if ((*it)->slotID == (*itP)->slotID)
		{
			(*itP)->textureID = textureID;
			break;
		}
	}
}

unsigned int CFilterSequence::GetResultImage(unsigned int slotID)
{
	return (slotID >= m_resultImages.size()) ? 0xFFFFFFFF : m_resultImages[slotID]->textureID;
}

unsigned int CFilterSequence::DetachResultImage(unsigned int slotID)
{
	if (slotID >= m_resultImages.size())
	{
		return 0xFFFFFFFF;
	}

	unsigned int textureID = m_resultImages[slotID]->textureID;
	m_resultImages[slotID]->textureID = 0xFFFFFFFF;
	m_isPrepared = false;	

	return textureID;
}

SImageDescription* CFilterSequence::GetImageDescription(std::string& name)
{
	MP_MAP<MP_STRING, SImageDescription *>::iterator it = m_imagesMap.find(MAKE_MP_STRING(name));
	if (it != m_imagesMap.end())
	{
		return (*it).second;
	}
	else
	{
		return NULL;
	}
}

void CFilterSequence::PrepareForExecutionIfNeeded()
{
	if (m_isPrepared)
	{
		return;
	}

	std::vector<SImageDescription *>::iterator it = m_resultImages.begin();

	for ( ; it != m_resultImages.end(); it++)
	{
		PrepareImage(*it);
	}

	it = m_tempImages.begin();

	for ( ; it != m_tempImages.end(); it++)
	{
		PrepareImage(*it);
	}

	it = m_sourceImages.begin();

	for ( ; it != m_sourceImages.end(); it++)
	if ((*it)->isRenderTarget)
	{
		CTexture* texture = g->tm.GetObjectPointer((*it)->textureID);
		int width = texture->GetTextureWidth();
		int height = texture->GetTextureHeight();
		(*it)->renderTarget = GetRenderTarget(width, height);
	}

	m_isPrepared = true;
}

void CFilterSequence::PrepareImage(SImageDescription* imageDesc)
{
	int textureID = imageDesc->textureID;
	if (imageDesc->textureID == 0xFFFFFFFF)
	{
		SImageDescription* imageDescription = GetImageDescription(imageDesc->like);
		textureID = imageDescription->textureID;
	}

	CTexture* texture = g->tm.GetObjectPointer(textureID);
	int width = texture->GetTextureWidth();
	int height = texture->GetTextureHeight();

	imageDesc->renderTarget = GetRenderTarget(width, height);

	if (imageDesc->textureID == 0xFFFFFFFF)
	{
		imageDesc->textureID = g->tm.ManageGLTexture("image_desc", imageDesc->renderTarget->CreateTempTexture(), width, height);
		UpdateImageDescription(imageDesc, imageDesc->textureID);
	}
}

CSimpleDrawingTarget* CFilterSequence::GetRenderTarget(unsigned int width, unsigned int height)
{
	std::vector<CSimpleDrawingTarget *>::iterator it = m_renderTargets.begin();
	std::vector<CSimpleDrawingTarget *>::iterator itEnd = m_renderTargets.end();

	for ( ; it != itEnd; it++)
	if (((*it)->GetWidth() == width) && ((*it)->GetHeight() == height))
	{
		return *it;
	}

	CSimpleDrawingTarget* renderTarget = MP_NEW(CSimpleDrawingTarget);
	renderTarget->SetFormat(false, 0);
	renderTarget->SetWidth(width);
	renderTarget->SetHeight(height);
	renderTarget->Create();
	m_renderTargets.push_back(renderTarget);

	return renderTarget;
}

void CFilterSequence::AddImageReference(std::string& imageName, SFilterInfo* filterInfo, unsigned int slotID)
{
	MP_MAP<MP_STRING, SImageDescription *>::iterator it = m_imagesMap.find(MAKE_MP_STRING(imageName));
	if (it != m_imagesMap.end())
	{
		SImageReference* imageRef = MP_NEW(SImageReference);
		imageRef->filterInfo = filterInfo;
		imageRef->slotID = slotID;
		((*it).second)->references.push_back(imageRef);
	}
}

CFilterSequence::~CFilterSequence()
{
}