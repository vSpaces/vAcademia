
#include "StdAfx.h"
#include <TinyXML.h>
#include "LensFlares.h"
#include "PlatformDependent.h"
#include "GlobalSingletonStorage.h"

#define DEFAULT_SOURCE_FLARE_COUNT	10

#define FADEIN_TIME					1000
#define FADEOUT_TIME				1000

CLensFlares::CLensFlares():
	MP_VECTOR_INIT(m_sources),
	m_flareTextureCount(0),
	m_isLoaded(false)
{
}

void CLensFlares::LoadIfNeeded()
{
	if (m_isLoaded)
	{
		return;
	}


	m_isLoaded = true;

	std::wstring path = GetApplicationRootDirectory();
	path += L"lensflares.xml";

	USES_CONVERSION;
	TiXmlDocument doc( W2A(path.c_str()));
	bool isLoaded = g->rl->LoadXml( (void*)&doc, path);

	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] error loading lens flares configuration");
		return;
	}

	TiXmlNode* flares = doc.FirstChild("lensflares");

	m_sources.clear();
	m_sources.reserve(DEFAULT_SOURCE_FLARE_COUNT);

	TiXmlNode* position = flares->FirstChild("position");
	while (position != NULL)
	{
		if ((!position->ToElement()->Attribute("x")) ||
			(!position->ToElement()->Attribute("y")) ||
			(!position->ToElement()->Attribute("z")))
		{
			return;
		}

		AddPosition(StringToFloat(position->ToElement()->Attribute("x")), StringToFloat(position->ToElement()->Attribute("y")),
			StringToFloat(position->ToElement()->Attribute("z")));

		position = flares->IterateChildren("position", position);
	}

	TiXmlNode* flare = flares->FirstChild("flare");
	while (flare != NULL)
	{
		if ((!flare->ToElement()->Attribute("src")) ||
			(!flare->ToElement()->Attribute("offset")) ||
			(!flare->ToElement()->Attribute("scale")) ||
			(!flare->ToElement()->Attribute("opacity")))
		{
			return;
		}

		SetFlare(m_flareTextureCount, flare->ToElement()->Attribute("src"), StringToFloat(flare->ToElement()->Attribute("offset")),
			StringToFloat(flare->ToElement()->Attribute("scale")), StringToFloat(flare->ToElement()->Attribute("opacity")));
		m_flareTextureCount++;

		flare = flare->IterateChildren("flare", flare);
	}
}

void CLensFlares::SetFlare(int flareID, std::string fileName, float offset, float scale, float opacity)
{
	if ((flareID >= 0) && (flareID < MAX_FLARE_TEXTURE_COUNT))
	{
		m_flareTextureID[flareID].textureID = g->tm.GetObjectNumber(fileName);

		if (m_flareTextureID[flareID].textureID != 0xFFFFFFFF)
		{
			CTexture* texture = g->tm.GetObjectPointer(m_flareTextureID[flareID].textureID);

			m_flareTextureID[flareID].width = texture->GetTextureWidth();
			m_flareTextureID[flareID].height = texture->GetTextureHeight();
			m_flareTextureID[flareID].width2Scaled = (int)(m_flareTextureID[flareID].width / 2.0f * scale);
			m_flareTextureID[flareID].height2Scaled = (int)(m_flareTextureID[flareID].height / 2.0f * scale);
		}

		m_flareTextureID[flareID].opacity = (int)(opacity * 255);
		m_flareTextureID[flareID].offset = offset;
	}
}

void CLensFlares::AddPosition(float x, float y, float z)
{
	SFlareSource* flareSource = MP_NEW(SFlareSource);
	flareSource->position.Set(x, y, z);
	if (g->ei.IsExtensionSupported(GLEXT_arb_occlusion_query))
	{
		if (!flareSource->query)
		{
			flareSource->query = MP_NEW(COcclusionQuery);
		}
	}
	m_sources.push_back(flareSource);
}

void CLensFlares::SetVisibleFlare(SFlareSource* source, bool isVisible)
{
	if (isVisible)
	{
		if (!source->isVisibleOnLastFrame)
		{
			source->startVisibleTime = (int)g->ne.time; 
		}		
	}
	else
	{
		if (source->isVisibleOnLastFrame)
		{
			source->startInvisibleTime = (int)g->ne.time; 
		}		
	}

	source->isVisibleOnLastFrame = isVisible;
}

void CLensFlares::Draw()
{
	if (g->rs.IsOculusRiftEnabled())
	{
		return;
	}

	g->stp.SetColor(255, 255, 255);
	g->stp.SetState(ST_ZTEST, true);
	g->stp.SetState(ST_ZWRITE, true);
	g->stp.SetState(ST_CULL_FACE, false);

	g->stp.PrepareForRender();

	if (g->scm.GetActiveScene())
	{
		LoadIfNeeded();

		std::vector<SFlareSource *>::iterator it = m_sources.begin();
		std::vector<SFlareSource *>::iterator itEnd = m_sources.end();
		for ( ; it != itEnd; it++)
		{
			(*it)->isPointInFrustum = g->cm.IsPointInFrustum((*it)->position.x, (*it)->position.y, (*it)->position.z);
			if (!(*it)->isPointInFrustum)
			{
				SetVisibleFlare(*it, false);
			}
		}		

		it = m_sources.begin();
		
		for ( ; it != itEnd; it++)
		if (((*it)->isPointInFrustum) || (g->ne.time - (*it)->startInvisibleTime < FADEOUT_TIME))
		{
			bool isVisibleForFading = !(*it)->isPointInFrustum;
			CViewport* viewport = g->scm.GetActiveScene()->GetViewport();

			if (!isVisibleForFading)
			{
				bool isVisible = ((*it)->query->GetFragmentCount() > 0);
				            			
				(*it)->query->SetQueryPoint((*it)->position.x, (*it)->position.y, (*it)->position.z);
				g->stp.PushMatrix();
				viewport->GetCamera()->Bind();
				(*it)->query->DoQuery();
				g->stp.PopMatrix();
		
				if (!isVisible)
				{
					SetVisibleFlare(*it, false);
					if (g->ne.time - (*it)->startInvisibleTime < FADEOUT_TIME)
					{
						isVisibleForFading = true;
					}
					else
					{
						continue;
					}
				}
			}
					
			float screenX, screenY, screenZ;
			if ((viewport->GetScreenCoords((*it)->position.x, (*it)->position.y, (*it)->position.z, &screenX, &screenY, &screenZ)) || (isVisibleForFading))
			{
				if (!isVisibleForFading)
				{
					SetVisibleFlare(*it, true);
				}

				CColor3 clr(255, 255, 255);
				float centerX = viewport->GetWidth() / 2.0f; 
				float centerY = viewport->GetHeight() / 2.0f;

				g->stp.PushMatrix();
				GLFUNC(glLoadIdentity)();
				g->cm.SetCamera2d();
				g->sm.UnbindShader();

				for (unsigned int i = 0; i < m_flareTextureCount; i++)
				{
					float x = screenX + (centerX - screenX) * m_flareTextureID[i].offset * 2.0f;
					float y = screenY + (centerY - screenY) * m_flareTextureID[i].offset * 2.0f;

					unsigned int textureID = m_flareTextureID[i].textureID;
					if (0xFFFFFFFF == textureID)
					{
						continue;
					}

					int width = m_flareTextureID[i].width;
					int height = m_flareTextureID[i].height;
					int width2Scaled = m_flareTextureID[i].width2Scaled;
					int height2Scaled = m_flareTextureID[i].height2Scaled;

					float opacityKoef = 1.0f;

					if (((*it)->isVisibleOnLastFrame) && (g->ne.time - (*it)->startVisibleTime < FADEOUT_TIME))
					{
						opacityKoef = (float)(g->ne.time - (*it)->startVisibleTime) / (float)FADEOUT_TIME;
					}

					if ((!(*it)->isVisibleOnLastFrame) && (g->ne.time - (*it)->startInvisibleTime < (float)FADEIN_TIME))					
					{
						opacityKoef = 1.0f - (float)(g->ne.time - (*it)->startInvisibleTime) / (float)FADEIN_TIME;
					}

					g->stp.SetState(ST_ZWRITE, false);
					g->stp.SetState(ST_ZTEST, false);
					g->sp.PutAddSprite((int)x - width2Scaled, (int)y - height2Scaled, (int)x + width2Scaled, (int)y + height2Scaled, 0, 0, width, height, textureID, (unsigned char)(m_flareTextureID[i].opacity * opacityKoef), clr);					
					g->stp.SetState(ST_ZWRITE, true);
					g->stp.SetState(ST_ZTEST, true);
					g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}

				g->stp.PopMatrix();
			}
			else
			{
				SetVisibleFlare(*it, false);
			}
		}		
	}

	g->stp.SetColor(255, 255, 255, 255);
	g->stp.PrepareForRender();
}

CLensFlares::~CLensFlares()
{
}