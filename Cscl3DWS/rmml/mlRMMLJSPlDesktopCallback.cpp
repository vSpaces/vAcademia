#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"
#include "mlRMMLJSPlDesktop.h"
#include "../../desktopMan/WebCamGrabber/errorCodes.h"

namespace rmml {

	mlDesktopCallbackManager::mlDesktopCallbackManager(mlSceneManager* apSM):
		MP_STRING_INIT(sTarget)
	{
		mpSM = apSM;
		mbCallbacksIsSet = false;
		imageBits = 0;
		imageQuality = 50;
	}

	void mlDesktopCallbackManager::SetCallbacks(JSContext *cx)
	{
		if(mbCallbacksIsSet) return;
		omsContext* pOMSContext = mpSM->GetContext();
		if(pOMSContext == NULL) return;
		desktop::IDesktopManager* pDesktopMan = pOMSContext->mpDesktopMan;
		if(pDesktopMan == NULL)
			return;
		mbCallbacksIsSet = pDesktopMan->AddCallback(this);

		desktop::IDesktopSharingManager* pSharingMan = pOMSContext->mpSharingMan;
		if(pSharingMan != NULL)
			pSharingMan->AddCallback(this);
	}

	void mlDesktopCallbackManager::onEventEntered( unsigned int uiRealityId, unsigned int uiPlayingRecordID,  unsigned int uiBaseRecordId)
	{
		omsContext* pOMSContext = mpSM->GetContext();
		if(pOMSContext == NULL) return;

		// Записи для воспроизведения нет
		if( uiPlayingRecordID == 0 || uiPlayingRecordID == -1)
			return;

		unsigned int playingRecID = uiPlayingRecordID;
		if (uiBaseRecordId != 0 && uiBaseRecordId != INCORRECT_RECORD_ID)
			playingRecID = uiBaseRecordId;

		if( pOMSContext->mpSharingMan)
		{
			pOMSContext->mpSharingMan->StartPlayingSharing( playingRecID, uiRealityId);
		}
	}
	
	void mlDesktopCallbackManager::onEventLeaved()
	{
		omsContext* pOMSContext = mpSM->GetContext();
		if(pOMSContext == NULL) return;

		if( pOMSContext->mpSharingMan)
			pOMSContext->mpSharingMan->StopPlayingSharing();
	}

	//! Real calls to RMML
	void mlDesktopCallbackManager::onApplyFrameRequestInternal()
	{
		omsContext* pOMSContext = mpSM->GetContext();
		if(pOMSContext == NULL) return;
		if( pOMSContext->mpPreviewRecorder == NULL) return;

		wchar_t* fileName = pOMSContext->mpPreviewRecorder->ApplyFrameToTarget( (char*)sTarget.data(), bitsPP, imageWidth, imageHeight, imageBits, imageQuality);
		if( fileName != NULL)
		{
			mpSM->NotifyOnDesktopFileSaved( fileName, (char*)sTarget.data());
		}
	}

	void mlDesktopCallbackManager::onSharingServerFileSavedInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;
		char* alpcFileName = NULL;

		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);
		aData >> alpcFileName;
		std::string	sFileName( alpcFileName);

		mpSM->NotifyOnSharingFileSaved( sTarget.c_str(), sFileName.c_str());
	}

	void mlDesktopCallbackManager::onDesktopDestinationRequestInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;
		aData >> alpcTarget;

		desktop::IDesktopSharingDestination*	currentDestination = NULL;
		currentDestination = mpSM->GetContext()->mpRM->GetDesktopSharingDestination( alpcTarget);
		if( currentDestination)
		{
			mpSM->GetContext()->mpSharingMan->SetDestinationPointer( alpcTarget, currentDestination);
		}
	}

	void mlDesktopCallbackManager::onRaiseSessionStartedEventInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;
		char* alpcSessionKey = NULL;
		char* alpcFileName = NULL;

		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);
		aData >> alpcSessionKey;
		std::string	sSessionKey( alpcSessionKey);
		aData >> alpcFileName;
		std::string	sFileName( alpcFileName);

		mpSM->NotifyOnSharingSessionStarted( sTarget.c_str(), sSessionKey.c_str(), sFileName.c_str());
	}

	void mlDesktopCallbackManager::onRaisePreviewCancelEventInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;		
		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);		

		mpSM->NotifyOnSharingPreviewCancel( sTarget.c_str());
	}

	void mlDesktopCallbackManager::onRaisePreviewOKEventInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;		
		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);		

		mpSM->NotifyOnSharingPreviewOK( sTarget.c_str());
	}


	void mlDesktopCallbackManager::onRaiseViewSessionStartedEventInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;
		char* alpcSessionKey = NULL;
		bool canRemoteAccess;

		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);
		aData >> alpcSessionKey;
		std::string	sSessionKey( alpcSessionKey);
		aData >> canRemoteAccess;

		mpSM->NotifyOnSharingViewSessionStarted( sTarget.c_str(), sSessionKey.c_str(), canRemoteAccess);
	}

	void mlDesktopCallbackManager::onTextureSizeChangedEventInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;

		int width, height;

		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);
	
		aData >> width;
		aData >> height;

		mpSM->NotifyOnTextureSizeChanged( sTarget.c_str(), width, height);
	}

	void mlDesktopCallbackManager::onSessionClosedEventInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;
		
		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);
		
		mpSM->NotifyOnSessionClosed( sTarget.c_str());

	}

	void mlDesktopCallbackManager::onViewSessionClosedEventInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;

		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);

		mpSM->NotifyOnViewSessionClosed( sTarget.c_str());
	}
	void mlDesktopCallbackManager::onWaitOwnerEventInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;

		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);

		mpSM->GetContext()->mpSharingMan->NotifyOnWaitOwnerEventInternal( sTarget.c_str());
	}

	void mlDesktopCallbackManager::onRemoteAccessModeChangedEventInternal(mlSynchData& aData)
	{
		char* alpcTarget = NULL;
		bool canRemoteAccess;

		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);
		aData >> canRemoteAccess;
		mpSM->NotifyOnRemoteAccessModeChanged(sTarget.c_str(), canRemoteAccess);
	}

	void mlDesktopCallbackManager::onVideoInputDeviceInitializedInternal(mlSynchData& aData)
	{
		int code;
		char* alpcTarget = NULL;

		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);
		aData >> code;
		
		if (code == SUCCESSFULL || code == 0 || code == USER_CHOSE_WEBCAMERA || code < 0) // success or code < 0 - оповещает не в скрипте
			mpSM->GetContext()->mpSharingMan->NotifyVideoDeviceInitialized(code);
		if (code > 0)
			mpSM->NotifyWebCameraState(sTarget.c_str(), code);
	}

	void mlDesktopCallbackManager::onSharingEventByCodeInternal(mlSynchData& aData)
	{
		int code;
		char* alpcTarget = NULL;

		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);
		aData >> code;

		mpSM->NotifySharingEventByCode(sTarget.c_str(), code);
	}

	void mlDesktopCallbackManager::onRaiseGlobalMouseEventInternal(mlSynchData& aData)
	{
		unsigned int wParam;
		float eventX, eventY;

		aData >> wParam;
		aData >> eventX;
		aData >> eventY;

		mpSM->NotifyGlobalMouseEvent(wParam, eventX, eventY);
	}

	void mlDesktopCallbackManager::onSetBarPositionEventInternal(mlSynchData& aData)
	{		
		char* alpcTarget = NULL;
		double second;
		double duration;
		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);
		aData >> second;
		aData >> duration;
		mpSM->NotifyOnSharingSetBarPosition( sTarget.c_str(), second, duration);
	}

	void mlDesktopCallbackManager::onVideoEndEventInternal(mlSynchData& aData)
	{		
		char* alpcTarget = NULL;
		aData >> alpcTarget;
		std::string	sTarget( alpcTarget);		
		mpSM->NotifyOnSharingVideoEnd( sTarget.c_str());
	}
	
	//! Calls to queue
	void mlDesktopCallbackManager::onApplyFrameRequest( LPCSTR lpcTarget, int bpp, int width, int height, const void* bits, int quality)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;
		if( imageBits != 0)
			MP_DELETE_ARR( imageBits);
		int memSize = width * height * bpp;
		imageBits = MP_NEW_ARR( BYTE, memSize);
		if( imageBits == NULL)
		{
			imageBits = 0;
			return;
		}
		memcpy( imageBits, bits, memSize);
		sTarget = lpcTarget;
		bitsPP = bpp;
		imageWidth = width;
		imageHeight = height;
		imageQuality = quality;

		mlSynchData LogData;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnApplyFrame, (unsigned char*)LogData.data(), LogData.size());
	}

	//! Calls to queue
	void mlDesktopCallbackManager::onDesktopDestinationRequest( LPCSTR lpcTarget)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;

		mlSynchData LogData;
		LogData << lpcTarget;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnDestinationRequest, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onRaiseSessionStartedEvent( LPCSTR lpcTarget, LPCSTR alpcSessionKey, LPCSTR alpcFileName)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;
		if( alpcSessionKey == NULL || *alpcSessionKey == 0)
			return;
		if( alpcFileName == NULL || *alpcFileName == 0)
			return;

		mlSynchData LogData;
		LogData << lpcTarget;
		LogData << alpcSessionKey;
		LogData << alpcFileName;

		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnRaiseSessionStartedEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onRaisePreviewCancelEvent( LPCSTR lpcTarget)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;		

		mlSynchData LogData;
		LogData << lpcTarget;		

		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnRaisePreviewCancelEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onRaisePreviewOKEvent( LPCSTR lpcTarget)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;		

		mlSynchData LogData;
		LogData << lpcTarget;		

		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnRaisePreviewOKEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onSharingServerFileSaved(LPCSTR lpcTarget, LPCSTR lpcFileName)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;
		if( lpcFileName == NULL || *lpcFileName == 0)
			return;

		mlSynchData LogData;
		LogData << lpcTarget;
		LogData << lpcFileName;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnSharingFileSaved, (unsigned char*)LogData.data(), LogData.size());
	}
	void mlDesktopCallbackManager::onRaiseViewSessionStartedEvent( LPCSTR lpcTarget, LPCSTR alpcSessionKey, bool canRemoteAccess)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;
		if( alpcSessionKey == NULL || *alpcSessionKey == 0)
			return;

		mlSynchData LogData;
		LogData << lpcTarget;
		LogData << alpcSessionKey;
		LogData << canRemoteAccess;

		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnRaiseViewSessionStartedEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onTextureSizeChangedEvent(LPCSTR lpcTarget, int width, int height)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;
	
		if (width <= 0)
			return;

		if (height <= 0)
			return;

		mlSynchData LogData;
		LogData << lpcTarget;
		LogData << width;
		LogData << height;

		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnTextureSizeChangedEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onSessionClosedEvent(LPCSTR lpcTarget)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;
	
		mlSynchData LogData;
		LogData << lpcTarget;
	
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnSessionClosedEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onViewSessionClosedEvent(LPCSTR lpcTarget)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;

		mlSynchData LogData;
		LogData << lpcTarget;

		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnViewSessionClosedEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onWaitOwnerEvent(LPCSTR lpcTarget)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;

		mlSynchData LogData;
		LogData << lpcTarget;

		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnWaitOwnerEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onRemoteAccessModeChangedEvent( LPCSTR lpcTarget, bool canRemoteAccess)
	{
		if( lpcTarget == NULL || *lpcTarget == 0)
			return;

		mlSynchData LogData;
		LogData << lpcTarget;
		LogData << canRemoteAccess;

		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnRemoteAccessModeChangedEvent, (unsigned char*)LogData.data(), LogData.size());
	}
	
	void mlDesktopCallbackManager::onVideoInputDeviceInitializedEvent(LPCSTR lpcTarget, int resultCode)
	{
		mlSynchData LogData;
		LogData << lpcTarget;
		LogData << resultCode; 
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnVideoInputDeviceInitializedEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onSharingEventByCode(LPCSTR alpcTarget, int aCode)
	{
		mlSynchData LogData;
		LogData << alpcTarget;
		LogData << aCode; 
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnSharingEventByCode, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onRaiseGlobalMouseEvent(unsigned int wParam, float aX, float aY)
	{
		mlSynchData LogData;
		LogData << wParam;
		LogData << aX;
		LogData << aY;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnGlobalMouseEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onSetBarPositionEvent(LPCSTR lpcTarget, double aSecond, double aDuration)
	{
		mlSynchData LogData;
		LogData << lpcTarget;
		LogData << aSecond;
		LogData << aDuration;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnSetBarPositionEvent, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlDesktopCallbackManager::onVideoEndEvent(LPCSTR lpcTarget)
	{
		mlSynchData LogData;
		LogData << lpcTarget;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_desktopOnVideoEndEvent, (unsigned char*)LogData.data(), LogData.size());
	}	
}

