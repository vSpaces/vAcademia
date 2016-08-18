
#include "StdAfx.h"
#include "Sprite.h"
#include "FakeObject.h"
#include "SkeletonAnimationObject.h"
#include "AddNCacheDataTask.h"

unsigned int GetStringHash(std::string& str)
{
	unsigned int res = 0;

	for (unsigned int i = 0; i < str.length(); i++)
	{
		res += str[i];
	}

	return res;
}

CCamera3D* CAvatarObject::CreateRenderCamera(const std::string& asCamera, float upKoef)
{
	int cameraID = g->cm.GetObjectNumber("avatar_render_camera");
	if (-1 == cameraID)
	{
		cameraID = g->cm.AddObject("avatar_render_camera");
	}
	CCamera3D* camera = g->cm.GetObjectPointer(cameraID);

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	CModel* model = sao->GetModel();

	float tmp1, tmp2, scale;
	m_obj3d->GetScale(&tmp1, &tmp2, &scale);
	float maxZ = model->GetModelMaxZ();
	float avatarHeight = maxZ;
	if ( avatarHeight < 100 || avatarHeight > 200 ){avatarHeight = 180;}
	CVector3D avatarPosition = m_obj3d->GetCoords();
	if ((!((CSkeletonAnimationObject*)m_obj3d->GetAnimation())->IsUsualOrder()) && (maxZ < 160))
	{
		avatarPosition.z -= 100;
	}
	CVector3D avatarFaceVector = sao->GetFaceVector()*(-1.0f);
	CQuaternion q = m_obj3d->GetRotation()->GetAsDXQuaternion();
	if ((q.x == 0.0f) && (q.y == 0.0f) && (q.z == 0.0f) && (q.w == 0.0f))
	{
		q.z = 1.0f;
	}
	avatarFaceVector *= q;
	CVector3D avatarUpVector(0.0f, 0.0f, -1.0f);
	CVector3D avatarLeftVector = avatarFaceVector.Cross(avatarUpVector);

	CalBone* pBone = NULL;
	if (sao->GetCalModel()->getSkeleton())
	{
		pBone = sao->GetCalModel()->getSkeleton()->getBone("head");
	}
	CVector3D headPosition(0.0f, 0.0f, 0.0f);
	if (pBone)
	{
		CVector3D scl = m_obj3d->GetScale3D();

		CalVector hp = pBone->getTranslationAbsolute();
		headPosition.Set(hp.x, hp.y, hp.z);

		headPosition *= q;
		headPosition.x *= scl.x;
		headPosition.y *= scl.y;
		headPosition.z *= scl.z;
		headPosition.z += avatarHeight / 14.0f;
	}
	std::string _cameraStr = asCamera;
	int ii = _cameraStr.find("_");
	if ( ii > 0 )
	{
		int _spin = 0;
		std::string _cameraSpinStr = _cameraStr.substr(ii+1,_cameraStr.size()-ii);
		_cameraStr = _cameraStr.substr(0,ii);
        _spin = rtl_atoi(_cameraSpinStr.c_str());
		CVector3D headCameraAt;
		CVector3D headCameraEye;
		if ( "avatar" == _cameraStr )
		{
			headCameraAt = avatarPosition + CVector3D(0.0f,0.0f,avatarHeight*0.5f);
			headCameraEye = headCameraAt;
			headCameraEye.z += (avatarFaceVector + avatarLeftVector).z * 1.8f * avatarHeight;			
			headCameraEye.x += (avatarFaceVector + avatarLeftVector).x * 1.8f * avatarHeight * cosf(_spin / 180.0f * 3.14f);
			headCameraEye.y += (avatarFaceVector + avatarLeftVector).y * 1.8f * avatarHeight * sinf(_spin / 180.0f * 3.14f);
		}
		else if ( "torso" == _cameraStr )
		{
			headCameraAt = avatarPosition + CVector3D(0.0f,0.0f,avatarHeight*0.75f);
			headCameraEye = headCameraAt;
			headCameraEye.z += (avatarFaceVector + avatarLeftVector).z * 0.7f * avatarHeight;
			headCameraEye.x += (avatarFaceVector + avatarLeftVector).x * 0.7f * avatarHeight * cosf(_spin / 180.0f * 3.14f);
			headCameraEye.y += (avatarFaceVector + avatarLeftVector).y * 0.7f * avatarHeight * sinf(_spin / 180.0f * 3.14f);
		}
		else if ("head" == _cameraStr)
		{	
			headCameraAt = avatarPosition + CVector3D(0.0f,0.0f,avatarHeight*0.92f);
			if ((!((CSkeletonAnimationObject*)m_obj3d->GetAnimation())->IsUsualOrder()) && (maxZ < 160))
			{	
				headCameraAt.z -= 5;
			}
			headCameraEye = headCameraAt;
			headCameraEye.z += (avatarFaceVector + avatarLeftVector).z * 0.45f * avatarHeight + g->rs.GetInt(AE_Z) + 3;
			headCameraEye.x += (avatarFaceVector + avatarLeftVector).x * 0.45f * avatarHeight * cosf(_spin / 180.0f * 3.14f);
			headCameraEye.y += (avatarFaceVector + avatarLeftVector).y * 0.45f * avatarHeight * sinf(_spin / 180.0f * 3.14f);
		}
		else if ("headavatar" == _cameraStr)
		{
			headCameraAt = avatarPosition + CVector3D(0.0f,0.0f,avatarHeight*0.8f);
			if ((!((CSkeletonAnimationObject*)m_obj3d->GetAnimation())->IsUsualOrder()) && (maxZ < 160))
			{	
				headCameraAt.z -= 5;
			}
			headCameraEye = headCameraAt;
			headCameraEye.z += (avatarFaceVector + avatarLeftVector).z * 0.35f * avatarHeight;
			headCameraEye.x += (avatarFaceVector + avatarLeftVector).x * 0.35f * avatarHeight * cosf(_spin / 180.0f * 3.14f);
			headCameraEye.y += (avatarFaceVector + avatarLeftVector).y * 0.35f * avatarHeight * sinf(_spin / 180.0f * 3.14f);
		}
		else if ("legs" == _cameraStr)
		{
			headCameraAt = avatarPosition + CVector3D(0.0f,0.0f,avatarHeight*0.28f);
			headCameraEye = headCameraAt;
			headCameraEye.z += (avatarFaceVector + avatarLeftVector).z * 1.15f * avatarHeight;
			headCameraEye.x += (avatarFaceVector + avatarLeftVector).x * 1.15f * avatarHeight * cosf(_spin / 180.0f * 3.14f);
			headCameraEye.y += (avatarFaceVector + avatarLeftVector).y * 1.15f * avatarHeight * sinf(_spin / 180.0f * 3.14f);
		}
		else
		{
			headCameraAt = avatarPosition + CVector3D(0.0f,0.0f,avatarHeight*0.6f);
			headCameraEye = headCameraAt;
			headCameraEye.z += (avatarFaceVector + avatarLeftVector).z * 1.5f * avatarHeight;
			headCameraEye.x += (avatarFaceVector + avatarLeftVector).x * 1.5f * avatarHeight * cosf(_spin / 180.0f * 3.14f);
			headCameraEye.y += (avatarFaceVector + avatarLeftVector).y * 1.5f * avatarHeight * sinf(_spin / 180.0f * 3.14f);		
		}
		camera->SetLookAt(headCameraAt);			
		camera->SetEyePosition(headCameraEye);
		camera->SetUp(avatarUpVector * upKoef);
		camera->SetFov(-10);

	}
	
	if (("0" == asCamera) || ("figure" == asCamera))
	{
		// body camera
		CVector3D headCameraAt;
		headCameraAt = avatarPosition + headPosition + CVector3D(0.0f, 0.0f, -avatarHeight * 2.1f * (1.0f / 4.5f));
		camera->SetLookAt(headCameraAt);
		camera->SetEyePosition(headCameraAt - avatarFaceVector * 8.0f * avatarHeight * (1.0f / 4.5f));
		camera->SetUp(avatarUpVector);
		camera->SetFov(-10);
	}

	if (("1" == asCamera) || ("body" == asCamera))
	{
		// body camera
		CVector3D headCameraAt;
		headCameraAt = avatarPosition + headPosition + CVector3D(0.0f,0.0f,-avatarHeight * 0.65f);
		camera->SetLookAt(headCameraAt);
		camera->SetEyePosition(headCameraAt - (avatarFaceVector + avatarLeftVector) * 0.2f *5.0f *avatarHeight);
		camera->SetUp(avatarUpVector);
		camera->SetFov(-10);
	}
	else if (("2" == asCamera) || ("face" == asCamera))
	{
		CVector3D headCameraAt;
		headCameraAt = avatarPosition + headPosition;
		camera->SetLookAt(headCameraAt);
		camera->SetEyePosition(headCameraAt - (avatarFaceVector + avatarLeftVector) * 0.15f * 5.0f * avatarHeight);
		camera->SetUp(avatarUpVector);
		camera->SetFov(-10);
	}
	else if (("3" == asCamera) || ("foots" == asCamera))
	{
		// legs camera
		CVector3D headCameraAt;
		headCameraAt = avatarPosition + headPosition + CVector3D(0.0f,0.0f,-avatarHeight*2.0f);
		camera->SetLookAt(headCameraAt);
		camera->SetEyePosition(headCameraAt - (avatarFaceVector + avatarLeftVector) * 0.5f * 5.0f * avatarHeight);
		camera->SetUp(avatarUpVector);
		camera->SetFov(-10);
	}

	return camera;
}

void CAvatarObject::setSnapshotColor(std::string strR, std::string strG, std::string strB, std::string strA)
{
	m_dwSnaphotBgColorR = (unsigned char)rtl_atoi(strR.c_str());
	m_dwSnaphotBgColorG = (unsigned char)rtl_atoi(strG.c_str());
	m_dwSnaphotBgColorB = (unsigned char)rtl_atoi(strB.c_str());
	m_dwSnaphotBgColorA = (unsigned char)rtl_atoi(strA.c_str());
}

void	CAvatarObject::MakeSnapshot(const std::string& asCamera)
{
	IAnyDataContainer*	anyDataContainer = rGet("__SnapshotImage");
	assert(anyDataContainer);
	if (!anyDataContainer)	
	{
		return;
	}

	CSprite* sprite = (CSprite*)anyDataContainer->GetMediaPointer();
	assert(sprite);
	if (!sprite)
	{
		return;
	}

	// получаем текущие параметры картинки
	int	currentWidth = 0;
	int	currentHeight = 0;
	int	currentX = 0;
	int	currentY = 0;
	sprite->GetSize(currentWidth, currentHeight);
	sprite->GetCoords(currentX, currentY);

	int newWidth = 2;
	int newHeight = 2;
	while (newWidth < currentWidth)
	{
		newWidth *= 2;
	}
	while (newHeight < currentHeight)
	{
		newHeight *= 2;
	}

	int textureID = sprite->GetTextureID(0);
	assert(textureID != -1);
	CTexture* texture = g->tm.GetObjectPointer(textureID);

	if ((currentWidth != newWidth) || (currentHeight != newHeight))
	{
		if (!sprite->IsUniqueTextureUsed())
		{
			unsigned int textureID = CFrameBuffer::CreateColorTexture(GL_TEXTURE_2D, newWidth, newHeight, GL_RGBA, GL_RGBA, false);

			int engineTextureID = g->tm.ManageGLTexture("avatar__snapshot", textureID, newWidth, newWidth);
			CTexture* newTexture = g->tm.GetObjectPointer(engineTextureID);
			newTexture->UpdateCurrentFiltration(GL_NEAREST, GL_NEAREST);
			newTexture->SetTextureWidth(currentWidth);
			newTexture->SetTextureHeight(currentHeight);
			newTexture->SetTextureRealWidth(newWidth);
			newTexture->SetTextureRealHeight(newHeight);
			newTexture->SetTransparentStatus(true);
			newTexture->SetXInvertStatus(texture->IsXInvert());
			newTexture->SetYInvertStatus(texture->IsYInvert());
			currentWidth = newWidth;
			currentHeight = newHeight;
		
			sprite->SetUniqueTextureStatus(true);
			sprite->SetTextureID(engineTextureID);

			texture = newTexture;
		}
		else
		{
			currentWidth = newWidth;
			currentHeight = newHeight;
		}
	}

	int leadVersion, majorVersion, minorVersion;
	g->gi.GetDriverVersion(leadVersion, majorVersion, minorVersion);
	bool isFramebufferUsed = ((g->ei.IsExtensionSupported(GLEXT_ext_framebuffer_object)) && (!((g->gi.GetVendorID() == VENDOR_ATI) && (majorVersion < 8))));

	if (isFramebufferUsed)
	{
		if (!m_fbo)
		{
			assert((currentWidth & (currentWidth - 1)) == 0);
			assert((currentHeight & (currentHeight - 1)) == 0);

			MP_NEW_V3(m_fbo, CFrameBuffer, currentWidth, currentHeight, CFrameBuffer::depth32);
			m_fbo->Create(true);
		}

		m_fbo->Bind();
		if (!m_fbo->AttachColorTexture(texture->GetNumber()))
		{
			g->lw.WriteLn("[ERROR] Failed to attach color texture to FBO in Avatar Editor!");
			m_fbo->Unbind();
			MP_DELETE(m_fbo);
			m_fbo = NULL;
			return;
		}

		bool res = m_fbo->IsOk();
		assert(res);
		if (!res)
		{
			g->lw.WriteLn("[ERROR] Failed to create FBO in Avatar Editor!");
			m_fbo->Unbind();
			MP_DELETE(m_fbo);
			m_fbo = NULL;
			return;
		}
	}
	else
	{
		// do nothing
	}

	g->stp.PushMatrix();
	g->stp.SetViewport(0, 0, texture->GetTextureWidth(), texture->GetTextureHeight());

	// fill by green color

	if (isFramebufferUsed)
	{
		GLFUNC(glClearColor)(m_dwSnaphotBgColorR / 255.0f, m_dwSnaphotBgColorG / 255.0f, m_dwSnaphotBgColorB / 255.0f, m_dwSnaphotBgColorA / 255.0f);
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		g->ne.EndFrame();
		GLFUNC(glClear)(GL_DEPTH_BUFFER_BIT);
		GLFUNC(glDepthMask)(GL_FALSE);
		//r    g   b
		//g->sp.PutOneColorQuad(-1.0f, -1.0f, 1.0f, 1.0f, 11, 115, 15);
		g->sp.PutOneColorQuad(-1, -1, 1, 1, m_dwSnaphotBgColorR, m_dwSnaphotBgColorG, m_dwSnaphotBgColorB,m_dwSnaphotBgColorA, NULL);
		GLFUNC(glDepthMask)(GL_TRUE);
	}

	CCamera3D* camera = CreateRenderCamera(asCamera);
	camera->Bind();
	std::string _cameraStr = asCamera;
	int ii = _cameraStr.find("_");
	if ( ii>0 )
	{
		g->cm.SetFov(0,m_fCameraFov);
	}
	else
		g->cm.SetFov(0);

	g->cm.ExtractFrustum();

	g->tm.EnableTexturing();
	g->tm.RefreshTextureOptimization();
	GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	g->stp.SetColor(255, 255, 255);

	g->shdw.DisableShadows();

	g->mr.SetMode(MODE_RENDER_ALL);

	m_obj3d->GetLODGroup()->Update(true, true);
	m_obj3d->GetLODGroup()->ClearVisibilityCheckResults();
	m_obj3d->GetLODGroup()->Draw();

	g->shdw.EnableShadows();
	
	g->stp.RestoreViewport();
	g->stp.PopMatrix();

	GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);	

	if (isFramebufferUsed)
	{
		m_fbo->DetachColorTexture();

		m_fbo->Unbind();
	}
	else
	{
		GLFUNC(glFlush)();
		GLFUNC(glFinish)();
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
		GLFUNC(glCopyTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, 0, 0, currentWidth, currentHeight);
		GLFUNC(glClear)(GL_DEPTH_BUFFER_BIT);
	}
}

std::string CAvatarObject::GetBillboardCacheName()
{
	std::string tuneString = GetTuneString();
	unsigned int hash = GetStringHash(tuneString);

	std::string cacheFileName = "___billboard_avatarv2_";
	USES_CONVERSION;
	if (m_obj3d)
	{
		cacheFileName += W2A(m_obj3d->GetName());
	}
	cacheFileName += "_";
	cacheFileName += IntToStr(hash);

	return cacheFileName;
}

bool	CAvatarObject::CanTakeBillboardFromCache()
{
	std::string cacheFileName = GetBillboardCacheName();

	if (g->dc.CheckIfDataExists(cacheFileName))
	{
		return true;
	}

	return false;
}

bool CAvatarObject::FixBillboardData(unsigned char* data, unsigned int textureSize)
{
	if (!g->rs.GetBool(SHADERS_USING))
	{
		for (unsigned int i = 0; i < textureSize; i += 4)
		if (data[i + 3] > 0)
		{
			data[i + 3] = 255;
		}

		return true;
	}

	return false;
}

bool	CAvatarObject::IsBillboardDataCorrect(void* _data, unsigned int dataSize)
{
	unsigned char* data = (unsigned char*)_data;
	if (data[3] != 0)
	{
		return false;
	}

	for (unsigned int i = 0; i < dataSize; i += 4, data += 4)
	{
		bool isCorrect = ((data[2] != 255) || (data[1] != 255) || (data[0] != 255)); 
		if (isCorrect)
		{
			return true;
		}
	}

	return false;
}

void	CAvatarObject::MakeBillboard(CReadingCacheFileTask* task)
{	
	std::string cacheFileName = GetBillboardCacheName();

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if (sao->GetBillboardTextureID() != -1)
	{
		CTexture* texture = g->tm.GetObjectPointer(sao->GetBillboardTextureID());
		texture->MarkDeleted();		
	}

	const int newWidth = 64;
	const int newHeight = 64;
		
	unsigned int textureID = CFrameBuffer::CreateColorTexture(GL_TEXTURE_2D, newWidth, newHeight, GL_RGBA, GL_RGBA, true);

	int engineTextureID = g->tm.ManageGLTexture("billboard", textureID, newWidth, newHeight);
	CTexture* texture = g->tm.GetObjectPointer(engineTextureID);
	texture->UpdateCurrentFiltration(GL_LINEAR, GL_LINEAR);
	texture->SetTextureWidth(newWidth);
	texture->SetTextureHeight(newHeight);
	texture->SetTextureRealWidth(newWidth);
	texture->SetTextureRealHeight(newHeight);
	texture->SetTransparentStatus(true);
	texture->SetXInvertStatus(true);
	texture->SetYInvertStatus(true);

	if (task)	
	{
		void* data = task->GetData();
		int size = task->GetDataSize();
		
		if ((data) && (size > 4))
		{
			bool isCorrect = IsBillboardDataCorrect(data, size); 

			if (isCorrect)
			{				
				CTexture* texture = g->tm.GetObjectPointer(engineTextureID);
				GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
				GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				texture->SetHasNoMipmaps(true);
				texture->SetCurrentFiltration(GL_LINEAR, GL_LINEAR, 0, 0);
				sao->SetBillboardTextureID(engineTextureID);
			}

			MP_DELETE_ARR(data);
			MP_DELETE(task);
	
			if (isCorrect)
			{
				return;
			}
		}
		else if (data)
		{
			MP_DELETE(task);
			MP_DELETE_ARR(data);			
		}
	}

	int leadVersion, majorVersion, minorVersion;
	g->gi.GetDriverVersion(leadVersion, majorVersion, minorVersion);
	//bool isFramebufferUsed = ((g->ei.IsExtensionSupported(GLEXT_ext_framebuffer_object)) && (!((g->gi.GetVendorID() == VENDOR_ATI) && (majorVersion < 8))));
	bool isFramebufferUsed = ((g->cc.AreExtendedAbilitiesSupported()) && (g->gi.GetVendorID() != VENDOR_INTEL));

	if (isFramebufferUsed)
	{
		if (!m_fbo)
		{
			MP_NEW_V3(m_fbo, CFrameBuffer, newWidth, newHeight, CFrameBuffer::depth32);
			m_fbo->Create(true);
		}

		m_fbo->Bind();
		if (!m_fbo->AttachColorTexture(texture->GetNumber()))
		{
			g->lw.WriteLn("[ERROR] Failed to attach color texture to FBO in Avatar Editor!");
			m_fbo->Unbind();
			MP_DELETE(m_fbo);
			m_fbo = NULL;
			return;
		}

		bool res = m_fbo->IsOk();
		assert(res);
		if (!res)
		{
			g->lw.WriteLn("[ERROR] Failed to create FBO in Avatar Editor!");
			m_fbo->Unbind();
			MP_DELETE(m_fbo);
			m_fbo = NULL;
			return;
		}
	}
	else
	{
		// do nothing
	}

	g->stp.PushMatrix();
	g->stp.SetViewport(0, 0, texture->GetTextureWidth(), texture->GetTextureHeight());

	if (isFramebufferUsed)
	{
		GLFUNC(glClearColor)(1.0f, 1.0f, 1.0f, 0.0f);
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		g->ne.EndFrame();		
		GLFUNC(glClearColor)(1.0f, 1.0f, 1.0f, 0.0f);
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	CCamera3D* camera = CreateRenderCamera("0");
	camera->Bind();
	g->cm.SetFov(-10, (float)newWidth / (float)newHeight);

	g->cm.ExtractFrustum();

	g->tm.EnableTexturing();
	g->tm.RefreshTextureOptimization();
	g->stp.SetColor(255, 255, 255);

	g->shdw.DisableShadows();

	g->mr.SetMode(MODE_RENDER_ALL);

	m_obj3d->GetLODGroup()->Update(true, true);
	m_obj3d->GetLODGroup()->ClearVisibilityCheckResults();
	m_obj3d->GetLODGroup()->Draw();

	unsigned int textureSize = texture->GetTextureWidth() * texture->GetTextureHeight() * 4;
	unsigned char* data = MP_NEW_ARR(unsigned char, textureSize);
	GLFUNC(glReadPixels)(0, 0, texture->GetTextureWidth(), texture->GetTextureHeight(), GL_RGBA, GL_UNSIGNED_BYTE, data);	
	
	g->shdw.EnableShadows();
	
	g->stp.RestoreViewport();
	g->stp.PopMatrix();	

	if (isFramebufferUsed)
	{
		m_fbo->DetachColorTexture();

		m_fbo->Unbind();
	}
	else
	{
		GLFUNC(glFlush)();
		GLFUNC(glFinish)();
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
		GLFUNC(glCopyTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, 0, 0, newWidth, newHeight);
		GLFUNC(glClear)(GL_DEPTH_BUFFER_BIT);
	}

	texture->SetHasNoMipmaps(true);
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
	texture->SetCurrentFiltration(GL_LINEAR, GL_LINEAR, 0, 0);	
	if (IsBillboardDataCorrect(data, textureSize))
	{
		if (FixBillboardData(data, textureSize))
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

		g->dc.AddDataAsynch(data, textureSize, cacheFileName);

		sao->SetBillboardTextureID(engineTextureID);		
	}	

	MP_DELETE_ARR(data);

	if (m_fbo)
	{
		MP_DELETE(m_fbo);
		m_fbo = NULL;
	}

	g->tm.RefreshTextureOptimization();
	g->stp.SetDefaultStates();
	g->stp.SetMaterial(-1);
	g->stp.PrepareForRender();
}