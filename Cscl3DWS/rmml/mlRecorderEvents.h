#pragma once

namespace rmml {

enum {
	JSPROP_recordingStatus = 1,
	TIDE_mlRMMLSRecorder
};

enum { 
	EVID_onRecordConnected = 0,
	EVID_onSynchronizedObjectsLoaded,
	EVID_onDesktopImageFileSaved,
	EVID_onRecordInLocationFailed,
	EVID_onRecordDisconnected,
	EVID_onRecordStoped,
	EVID_onRecordInLocationNoneSound,
	EVID_onUpdatePlayingTime,
	EVID_onSeanceSeekPos,
	EVID_onSeancePaused,
	EVID_onSeancePlayed,
	EVID_onSeanceRewinded,
	EVID_onSeanceFinishedPlaying,
	EVID_onSeanceLoaded,
	EVID_onLessonLoaded,
	EVID_onSeanceLeaved,
	EVID_onSharingSessionStarted,
	EVID_onSharingViewSessionStarted,
	EVID_onSharingPreviewCancel,
	EVID_onSharingPreviewOK,
	EVID_onTextureSizeChanged,
	EVID_onSessionClosed,
	EVID_onSharingFileSaved,
	EVID_onViewSessionClosed,
	EVID_onRemoteAccessModeChanged,
	EVID_onWebCameraState,
	EVID_onAvatarImageFileSaved,
	EVID_onEventEntered,
	EVID_onEventLeaved,
	EVID_onEventUpdateInfo,	
	EVID_onEventUserRightsInfo,
	EVID_onEventUserRightUpdate,
	EVID_onEventUserBanUpdate,
	EVID_onSharingEventByCode,	
	EVID_onGlobalMouseEvent,
	EVID_onLocationEntered,
	EVID_onSharingEventSetBarPosition,
	EVID_onSharingEventVideoEnd,	
	EVID_RecorderEventsLast
};

#define DEF_AUTHOR 1
#define DEF_ASSISTANT 1

}