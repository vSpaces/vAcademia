#include "mlRMML.h"
#include "mlRMMLModule.h"
#include "mlLogRecord.h"
#include "mlLogSeance.h"
#include "config/prolog.h"
#include <Windows.h>

wchar_t*	wchNumbers[10] = {L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9"};
namespace rmml
{
	mlString Int2MlString( int iStrN)
	{
		mlString sStrN;
		if(iStrN==0)
		{
			sStrN=L"0";
		}
		else
		{
			for(; iStrN != 0; iStrN/=10)
				sStrN.insert(0, wchNumbers[iStrN % 10]);
		}
		return sStrN;
	}
}

mlStyle::~mlStyle(){
	if(fontFamily!=NULL) MP_DELETE_ARR( fontFamily);
}

#define ML_STL_COPY_PROP(P) P=aStyle.P;
#define ML_STL_COPY_STR_PROP(SP) \
	if(SP!=NULL)  MP_DELETE_ARR( SP); SP=NULL; \
	if(aStyle.SP==NULL){ \
	SP=NULL; \
	}else{ \
	char* psz=aStyle.SP; while(*psz++); \
	int iLen=psz-aStyle.SP; \
	SP = MP_NEW_ARR( char, iLen+1); \
	char* pszS=aStyle.SP; \
	char* pszD=SP; \
	SP[iLen]='\0'; \
	while((iLen--)>0) *pszD++=*pszS++; \
	}

mlStyle& mlStyle::operator=(mlStyle& aStyle){
	ML_STL_COPY_PROP(fontStyle);
	ML_STL_COPY_PROP(fontWeight);
	ML_STL_COPY_PROP(fontSize);
	ML_STL_COPY_PROP(lineHeight);
	ML_STL_COPY_STR_PROP(fontFamily);
	ML_STL_COPY_PROP(textAlign);
	ML_STL_COPY_PROP(textIndent);
	ML_STL_COPY_PROP(textDecoration);
	ML_STL_COPY_PROP(color);
	ML_STL_COPY_PROP(antialiasing);
	return *this;
}

namespace rmml {

mlRMML::mlRMML(void)
{
}

mlRMML::~mlRMML(void)
{
}

#define CREATE_JSMLEL2(NM,TYP) \
	}else if(aiType==TYP || isEqual(#NM,apszClassName)){ \
		return mlRMML##NM::newJSObject(cx);

JSObject* mlRMML::CreateElement(JSContext* cx, const char* apszClassName, int aiType){
	if(false){
	CREATE_JSMLEL2(Image,MLMT_IMAGE);
	CREATE_JSMLEL2(Text,MLMT_TEXT);
	}else if(isEqual("Synch",apszClassName)){ 
		return mlSynch::newJSObject(cx);
	CREATE_JSMLEL2(Animation,MLMT_ANIMATION);
	CREATE_JSMLEL2(Composition,MLMT_COMPOSITION);
	CREATE_JSMLEL2(Button,MLMT_BUTTON);
	CREATE_JSMLEL2(Sequencer,MLMT_SEQUENCER);
	CREATE_JSMLEL2(Object,MLMT_OBJECT);
	}else if(aiType==MLMT_OBJECT || isEqual("Object3D",apszClassName)){ \
		return mlRMMLObject::newJSObject(cx);
	CREATE_JSMLEL2(Motion,MLMT_MOTION);
	CREATE_JSMLEL2(Group,MLMT_GROUP);
	CREATE_JSMLEL2(Speech,MLMT_SPEECH);
	CREATE_JSMLEL2(Audio,MLMT_AUDIO);
	CREATE_JSMLEL2(Camera,MLMT_CAMERA);
	CREATE_JSMLEL2(Light,MLMT_LIGHT);
	CREATE_JSMLEL2(Video,MLMT_VIDEO);
	CREATE_JSMLEL2(Input,MLMT_INPUT);
	CREATE_JSMLEL2(ActiveX,MLMT_ACTIVEX);
	CREATE_JSMLEL2(Flash,MLMT_FLASH);
	CREATE_JSMLEL2(Browser,MLMT_BROWSER);
	}else if(aiType==MLMT_QT || isEqual("QuickTime",apszClassName)){ 
		return mlRMMLQT::newJSObject(cx);
	CREATE_JSMLEL2(Plugin,MLMT_PLUGIN);
	CREATE_JSMLEL2(Scene3D,MLMT_SCENE3D);
	CREATE_JSMLEL2(Character,MLMT_CHARACTER);
	CREATE_JSMLEL2(Viewport,MLMT_VIEWPORT);
	CREATE_JSMLEL2(Movement,MLMT_MOVEMENT);
	CREATE_JSMLEL2(Visemes,MLMT_VISEMES);
	CREATE_JSMLEL2(Cloud,MLMT_CLOUD);
	CREATE_JSMLEL2(Styles,MLMT_STYLES);
	CREATE_JSMLEL2(Hint,MLMT_HINT);
	CREATE_JSMLEL2(XML,MLMT_XML);
	CREATE_JSMLEL2(Path3D,MLMT_PATH3D);
	CREATE_JSMLEL2(Idles,MLMT_IDLES);
	CREATE_JSMLEL2(Idle,MLMT_IDLE);
	CREATE_JSMLEL2(Shadows3D,MLMT_SHADOWS3D);
	CREATE_JSMLEL2(Shadow3D,MLMT_SHADOW3D);
	CREATE_JSMLEL2(Map,MLMT_MAP);
	CREATE_JSMLEL2(Material,MLMT_MATERIAL);
	CREATE_JSMLEL2(Materials,MLMT_MATERIALS);
	CREATE_JSMLEL2(Resource,MLMT_RESOURCE);
	CREATE_JSMLEL2(ImageFrame,MLMT_IMAGE_FRAME);
	CREATE_JSMLEL2(Line,MLMT_LINE);
	}else{
		JSObject* jso=mlRMMLXML::CreateElementByClassName(cx, apszClassName);
		if(jso!=NULL) return jso;
	}
	return NULL;
}

mlresult mlRMML::InitGlobalJSObjects(JSContext* cx , JSObject* obj)
{
	mlRMMLImage::InitJSClass(cx,obj);
	mlRMMLAnimation::InitJSClass(cx,obj);
	mlRMMLVideo::InitJSClass(cx,obj);
	mlRMMLText::InitJSClass(cx,obj);
	mlRMMLInput::InitJSClass(cx,obj);
	mlRMMLActiveX::InitJSClass(cx,obj);
	mlRMMLFlash::InitJSClass(cx,obj);
	mlRMMLBrowser::InitJSClass(cx,obj);
	mlRMMLQT::InitJSClass(cx,obj);
	mlRMMLPlugin::InitJSClass(cx,obj);
	mlRMMLButton::InitJSClass(cx,obj);
	mlRMMLAudio::InitJSClass(cx,obj);
	mlRMMLSequencer::InitJSClass(cx,obj);
	mlRMMLXML::InitJSClasses(cx,obj);
	mlRMMLComposition::InitJSClass(cx,obj);
	mlRMMLLine::InitJSClass(cx,obj);
	// 3D
	mlPosition3D::InitJSClass(cx,obj);
	mlScale3D::InitJSClass(cx,obj);
	mlRotation3D::InitJSClass(cx,obj);
	mlRMMLViewport::InitJSClass(cx,obj);
	mlRMMLScene3D::InitJSClass(cx,obj);
	mlRMMLCamera::InitJSClass(cx,obj);
	mlRMMLLight::InitJSClass(cx,obj);
	mlRMMLObject::InitJSClass(cx,obj);
	mlRMMLParticles::InitJSClass(cx,obj);
	mlRMMLMotion::InitJSClass(cx,obj);
	mlRMMLGroup::InitJSClass(cx,obj);
	mlRMMLCharacter::InitJSClass(cx,obj);
	mlRMMLSpeech::InitJSClass(cx,obj);
	mlRMMLMovement::InitJSClass(cx,obj);
	mlRMMLCloud::InitJSClass(cx,obj);
	mlRMMLVisemes::InitJSClass(cx,obj);
	mlRMMLStyles::InitJSClass(cx,obj);
	mlRMMLHint::InitJSClass(cx,obj);
	mlRMMLPath3D::InitJSClass(cx,obj);
	mlRMMLIdles::InitJSClass(cx,obj);
	mlRMMLIdle::InitJSClass(cx,obj);
	mlRMMLShadow3D::InitJSClass(cx,obj);
	mlRMMLShadows3D::InitJSClass(cx,obj);
	mlRMMLMap::InitJSClass(cx,obj);
	mlRMMLMaterial::InitJSClass(cx,obj);
	mlRMMLMaterials::InitJSClass(cx,obj);
	mlMotionTrackFrame::InitJSClass(cx,obj);
	mlMotionTrackFrames::InitJSClass(cx,obj);
	mlMotionTracks::InitJSClass(cx,obj);
	mlMotionTrack::InitJSClass(cx,obj);
	mlColorJSO::InitJSClass(cx,obj);
	mlRectangleJSO::InitJSClass(cx,obj);
	mlSoapJSO::InitJSClass(cx,obj);
	mlLogRecordJSO::InitJSClass(cx,obj);
	mlLogRecordsJSO::InitJSClass(cx,obj);
	mlLogSeanceJSO::InitJSClass(cx,obj);
	mlLogSeancesJSO::InitJSClass(cx,obj);
	mlSynch::InitJSClass(cx,obj);
	mlSynchLinkJSO::InitJSClass(cx,obj);
	//
	mlRMMLModule::InitJSClass(cx,obj);
	mlRMMLResource::InitJSClass(cx,obj);

	mlRMMLImageFrame::InitJSClass(cx,obj);

	// ??
	return ML_OK;
}


#define RETURN_TYPE_NAME(TYPE) \
	case MLMT_##TYPE:	\
		return #TYPE;

// получить название типа (по константе MLMT_*)
std::string mlRMML::GetTypeName(int aiRMMLType){
	std::string sScriptNotSet;
	if((aiRMMLType & 0xFF) == 0xFF){ // MLMT_SCRIPT
		aiRMMLType &= ~MLSTF_NOTSET;
		sScriptNotSet = " ns";
	}
	switch(aiRMMLType){
		RETURN_TYPE_NAME(UNDEFINED);
		RETURN_TYPE_NAME(IMAGE);
		RETURN_TYPE_NAME(ANIMATION);
		RETURN_TYPE_NAME(VIDEO);
		RETURN_TYPE_NAME(TEXT);
		RETURN_TYPE_NAME(BUTTON);
		RETURN_TYPE_NAME(ACTIVEX);
		RETURN_TYPE_NAME(PLUGIN);
		RETURN_TYPE_NAME(AUDIO);
		RETURN_TYPE_NAME(XML);
		RETURN_TYPE_NAME(VIEWPORT);
		RETURN_TYPE_NAME(SCENE3D);
		RETURN_TYPE_NAME(CAMERA);
		RETURN_TYPE_NAME(LIGHT);
		RETURN_TYPE_NAME(OBJECT);
		RETURN_TYPE_NAME(MOTION);
		RETURN_TYPE_NAME(CHARACTER);
		RETURN_TYPE_NAME(SPEECH);
		RETURN_TYPE_NAME(GROUP);
		RETURN_TYPE_NAME(CLOUD);
		RETURN_TYPE_NAME(VISEMES);
		RETURN_TYPE_NAME(HINT);
		RETURN_TYPE_NAME(PATH3D);
		RETURN_TYPE_NAME(MAP);
		RETURN_TYPE_NAME(MATERIAL);
		RETURN_TYPE_NAME(MATERIALS);
		RETURN_TYPE_NAME(FLASH);
		RETURN_TYPE_NAME(BROWSER);
		RETURN_TYPE_NAME(QT);
		RETURN_TYPE_NAME(INPUT);
		RETURN_TYPE_NAME(IMAGE_FRAME);
		RETURN_TYPE_NAME(COMPOSITION);
		RETURN_TYPE_NAME(RESOURCE);
		RETURN_TYPE_NAME(SHADOW3D);
		RETURN_TYPE_NAME(IDLE);
		RETURN_TYPE_NAME(IDLES);
		RETURN_TYPE_NAME(MOVEMENT);
		RETURN_TYPE_NAME(STYLES);
		RETURN_TYPE_NAME(SEQUENCER);
		RETURN_TYPE_NAME(SCENE);
		RETURN_TYPE_NAME(SCENECLASS);
		RETURN_TYPE_NAME(LINE);
		RETURN_TYPE_NAME(SCRIPT);
		case MLST_EVENT:
			return std::string("SCRIPT(event")+sScriptNotSet+")";
		case MLST_WATCH:
			return std::string("SCRIPT(watch")+sScriptNotSet+")";
		case MLST_SYNCHMS:
			return std::string("SCRIPT(synchms")+sScriptNotSet+")";
		case MLST_EVEXPR:
			return std::string("SCRIPT(evexpr")+sScriptNotSet+")";
		case MLST_SYNCH:
			return std::string("SCRIPT(synch")+sScriptNotSet+")";
		case MLST_SYNCHPROP:
			return std::string("SCRIPT(synchProp")+sScriptNotSet+")";
		case MLST_SYNCHEVENT:
			return std::string("SCRIPT(synchEvent")+sScriptNotSet+")";
	}	
	return "UNKNOWN";
} 

void sortchars (char *lo, char *hi){
    while (hi > lo) {
        char* max = lo;
        for (char* p = lo+1; p <= hi; p ++) {
            if (*p > *max) max = p;
        }
		char tmp=*max; *max=*hi; *hi=tmp;
        hi--;
    }
}

//class mlOutString{
//	mlString mstr;
//public:
//	mlOutString& operator=(const wchar_t* apwc);
//};

mlOutString& mlOutString::operator=(const wchar_t* apwc){
	mstr=apwc;
	return (*this);
}

mlOutString& mlOutString::operator=(const char* apsz){
	mstr = cLPWCSTR(apsz);
	return (*this);
}

mlOutString& mlOutString::operator=(mlString as){
	mstr=as;
	return (*this);
}

double WStringToDouble(const wchar_t *p)
{
#define white_space(c) ((c) == L' ' || (c) == L'\t')
#define valid_digit(c) ((c) >= L'0' && (c) <= L'9')

    int frac = 0;
    double sign, value, scale;

    // Skip leading white space, if any.

    while (white_space(*p) ) {
        p += 1;
    }

    // Get sign, if any.

    sign = 1.0;
    if (*p == L'-') {
        sign = -1.0;
        p += 1;

    } else if (*p == L'+') {
        p += 1;
    }

    // Get digits before decimal point or exponent, if any.

    value = 0.0f;
    while (valid_digit(*p)) {
        value = value * 10.0f + (*p - L'0');
        p += 1;
    }

    // Get digits after decimal point, if any.

    if ((*p == L'.') || (*p == L',')) {
        float pow10 = 10.0f;
        p += 1;

        while (valid_digit(*p)) {
            value += (*p - L'0') / pow10;
            pow10 *= 10.0f;
            p += 1;
        }
    }

    // Handle exponent, if any.

    scale = 1.0f;
    if ((*p == L'e') || (*p == L'E')) {
        unsigned int expon;
        p += 1;

        // Get sign of exponent, if any.

        frac = 0;
        if (*p == L'-') {
            frac = 1;
            p += 1;

        } else if (*p == L'+') {
            p += 1;
        }

        // Get digits of exponent, if any.

        expon = 0;
        while (valid_digit(*p)) {
            expon = expon * 10 + (*p - L'0');
            p += 1;
        }
        if (expon > 308) expon = 308;

        // Calculate scaling factor.

        while (expon >= 25) { scale *= 1E25; expon -= 25; }
        while (expon >=  8) { scale *= 1E8;  expon -=  8; }
        while (expon >   0) { scale *= 10.0; expon -=  1; }
    }

    // Return signed and scaled floating point result.

    return sign * (frac ? (value / scale) : (value * scale));
}

}