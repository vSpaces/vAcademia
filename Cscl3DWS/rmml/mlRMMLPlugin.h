#pragma once

namespace rmml {

/**
 * Класс элмента RMML, загружающего 
 * OMS-Plugin
 */

class mlRMMLPlugin :
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlIPlugin
{
	bool mbInMyCall;
protected:
	class CInMyCall
	{
		mlRMMLPlugin* mpThis;
		bool mbOldInMyCall;
	public:
		CInMyCall(mlRMMLPlugin* apThis){
		  mpThis = apThis;
		  mbOldInMyCall = mpThis->mbInMyCall;
		  mpThis->mbInMyCall = true;
		}
		~CInMyCall(){
		  mpThis->mbInMyCall = mbOldInMyCall;
		}
	};
	friend class CInMyCall;
public:
	mlRMMLPlugin(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLPlugin(void);

	bool IsInMyCall(){ return mbInMyCall; }
	
MLJSCLASS_DECL
private:
	enum {
		JSPROP_uid = 1
	};
	MP_WSTRING uid;
		
	JSFUNC_DECL(call)

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация mlILoadable
MLILOADABLE_IMPL
	const wchar_t* mlRMMLPlugin::GetSrc();

// реализация mlIPlugin
public:
	mlIPlugin* GetIPlugin(){ return this; }
	const wchar_t*	GetUID(){ return uid.c_str();}
	omsresult Call(char* apProcName, char* aArgTypes, va_list aArgsV);
};

}