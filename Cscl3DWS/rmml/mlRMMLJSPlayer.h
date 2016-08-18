//
// $RCSfile: mlRMMLJSPlayer.h,v $
// $Revision: 1.4 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2008/08/23 07:48:38 $
//              $Author: bdima $
//
//////////////////////////////////////////////////////////////////////

namespace rmml {

enum {
//	JSPROP_window;
	JSPROP_mode=1,
	JSPROP_module,
	JSPROP_soundMode,
	JSPROP_soundVolume,
	JSPROP_stencilBuffer,
	JSPROP_bigCursor,
	JSPROP_version,
	JSPROP_language,
	JSPROP_debugVersion,
	JSPROP_arguments,
	JSPROP_logRestoreProgress,
	JSPROP_reality,
	JSPROP_preloadingMode,
	JSPROP_loadingMode,
	JSPROP_testMode,
	JSPROP_testResDir,
	TIDE_mlRMMLPlayer
};

enum { EVID_mlRMMLPlayer=TIDE_mlRMMLPlayer-1,
	EVID_onRestoreUnsynched,
	EVID_onRestoreStateUnsynched,
	EVID_onDisconnect,
	EVID_onErrorReport,
	EVID_onActivated,
	EVID_onDeactivated,
	EVID_onShowInfoState,
	EVID_onWindowSizeChanged,
	EVID_onKinectFound,
	EVID_onOculusRiftFound,
	EVID_onWindowMoved,
	EVID_onVisibilityDistanceKoefChanged,
	EVID_onVersionReceived,
	EVID_onControllerFound,
	TEVIDE_mlRMMLPlayer // ML_PLAYER_EVENTS_COUNT = TEVIDE_mlRMMLPlayer = 9
}; // опх днаюбкемхх мнбцн янашрхъ наъгюрекэмн сбекхвхбюрэ гмювемхе оепелеммни 'ML_PLAYER_EVENTS_COUNT'

}

