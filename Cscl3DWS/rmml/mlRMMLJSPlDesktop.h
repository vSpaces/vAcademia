#ifndef _mlRMMLJSPlDesktop_h__
#define _mlRMMLJSPlDesktop_h__

#include "IDesktopMan.h"

namespace rmml{

	class mlDesktopCallbackManager: 
		public desktop::desktopIDesktopManCallbacks
	{
		bool mbCallbacksIsSet;
		mlSceneManager* mpSM;

		MP_STRING		sTarget;
		int				bitsPP;
		int				imageWidth;
		int				imageHeight;
		void*			imageBits;
		int				imageQuality;
	public:
		mlDesktopCallbackManager(mlSceneManager* apSM);
		void SetCallbacks(JSContext *cx);

		void onEventEntered( unsigned int uiRealityId, unsigned int uiPlayingRecordID, unsigned int uiBaseRecordID);
		void onEventLeaved();
		void onApplyFrameRequestInternal( );
		void onDesktopDestinationRequestInternal(mlSynchData& aData);
		void onRaiseSessionStartedEventInternal(mlSynchData& aData);
		void onRaisePreviewCancelEventInternal(mlSynchData& aData);
		void onRaisePreviewOKEventInternal(mlSynchData& aData);
		void onSharingServerFileSavedInternal(mlSynchData& aData);
		void onRaiseViewSessionStartedEventInternal(mlSynchData& aData);
		void onTextureSizeChangedEventInternal(mlSynchData& aData);
		void onSessionClosedEventInternal(mlSynchData& aData);
		void onViewSessionClosedEventInternal(mlSynchData& aData);
		void onWaitOwnerEventInternal(mlSynchData& aData);
		void onRemoteAccessModeChangedEventInternal(mlSynchData& aData);
		void onVideoInputDeviceInitializedInternal(mlSynchData& aData);
		void onSharingEventByCodeInternal(mlSynchData& aData);
		void onRaiseGlobalMouseEventInternal(mlSynchData& aData);
		void onSetBarPositionEventInternal(mlSynchData& aData);
		void onVideoEndEventInternal(mlSynchData& aData);

		// реализация voip::desktopIDesktopManCallbacks
	public:
		virtual void onApplyFrameRequest( LPCSTR lpcTarget, int bpp, int width, int height, const void* bits, int quality);
		virtual void onDesktopDestinationRequest( LPCSTR lpcTarget);
		virtual void onRaiseSessionStartedEvent( LPCSTR lpcTarget, LPCSTR alpcSessionKey, LPCSTR alpcFileName);
		virtual void onRaisePreviewCancelEvent( LPCSTR lpcTarget);
		virtual void onRaisePreviewOKEvent( LPCSTR lpcTarget);
		virtual void onSharingServerFileSaved(LPCSTR lpcTarget, LPCSTR lpcFileName);
		virtual void onRaiseViewSessionStartedEvent( LPCSTR lpcTarget, LPCSTR alpcSessionKey, bool canRemoteAccess);
		virtual void onTextureSizeChangedEvent(LPCSTR lpcTarget, int width, int height);
		virtual void onSessionClosedEvent(LPCSTR lpcTarget);
		virtual void onViewSessionClosedEvent(LPCSTR lpcTarget);
		virtual void onWaitOwnerEvent(LPCSTR lpcTarget);
		virtual void onRemoteAccessModeChangedEvent( LPCSTR lpcTarget, bool canRemoteAccess);
		virtual void onVideoInputDeviceInitializedEvent(LPCSTR lpcTarget, int resultCode);
		virtual void onSharingEventByCode(LPCSTR alpcTarget, int aCode);
		virtual void onRaiseGlobalMouseEvent(unsigned int wParam, float aX, float aY);
		virtual void onSetBarPositionEvent(LPCSTR lpcTarget, double aSecond, double aDuration);
		virtual void onVideoEndEvent(LPCSTR lpcTarget);		
	};

}
#endif