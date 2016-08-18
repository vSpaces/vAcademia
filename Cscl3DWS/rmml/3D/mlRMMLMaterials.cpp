#include "mlRMML.h"

namespace rmml {

mlRMMLMaterials::mlRMMLMaterials(void)
:MP_VECTOR_INIT(mvChangeListeners)
{
	mRMMLType=MLMT_MATERIALS;
	mType=MLMT_MATERIALS;
}

mlRMMLMaterials::~mlRMMLMaterials(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLMaterials::~mlRMMLMaterials");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLMaterials::_JSPropertySpec[] = {
	// коды свойств 0..31 зарезервированы под установку материалов строкой
	// (думаю можно будет коды 29,30,31 использовать под специфические свойства)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLMaterials::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Materials,mlRMMLMaterials,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLMaterials)


///// JavaScript Set Property Wrapper
JSBool mlRMMLMaterials::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLMaterials);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:;
	SET_PROPS_END;
	if(JSVAL_IS_INT(id)){
		int iMatNum = JSVAL_TO_INT(id);
		if(iMatNum < TIDB_mlRMMLElement){
			if(JSVAL_IS_STRING(*vp)){
				JSString* jssMatDescr = JSVAL_TO_STRING(*vp);
				const wchar_t* jscMatDescr = wr_JS_GetStringChars(jssMatDescr);
				priv->SetMaterialDescription(iMatNum, jscMatDescr);
			}
		}
	}
	return JS_TRUE;
}

#define MAT_ID_MASK 0x100

///// JavaScript Get Property Wrapper
JSBool mlRMMLMaterials::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	if(JSVAL_IS_STRING(id)){
		mlString sID = (wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(id));
		if(sID.find(L"m") == 0){
			mlString sMatID = sID.substr(1);
			if(!sMatID.empty()){
				if ( sMatID == L"0"){
					id =  INT_TO_JSVAL( 0);
				}else{
					int iMatID = _wtoi(sMatID.c_str());
					if(iMatID > 0)
						id = INT_TO_JSVAL(iMatID | MAT_ID_MASK);
				}
			}
		}
	}
	GET_PROPS_BEGIN(mlRMMLMaterials);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:{
			// iID - material id
			mlString sMatName = L"__";
			wchar_t wsMatNum[20];
			iID &= ~MAT_ID_MASK;
			swprintf_s(wsMatNum, 20, L"%d", iID);
			sMatName += wsMatNum;
			//if(!bByMask)
			wr_JS_GetUCProperty(cx, obj, sMatName.c_str(), -1, vp);
			if(!JSVAL_IS_MLEL(cx,*vp)){
				mlRMMLElement* pMLEl3DO = priv->mpParent;
				if( pMLEl3DO)
				{
					if( !PMO_IS_NULL( pMLEl3DO->GetMO()))
					{
						moI3DObject* p3DO = pMLEl3DO->GetMO()->GetI3DObject();
						if( p3DO != NULL)
						{
							mlRMMLElement* pMLEl3DO = priv->mpParent;
							moI3DObject* p3DO = pMLEl3DO->GetMO()->GetI3DObject();
							if(p3DO != NULL){
								// создаем объект материала
								JSObject* jsoMat = mlRMMLMaterial::newJSObject(cx);
								// задаем ему имя __%d
								jsval vName = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sMatName.c_str()));
								JS_SetProperty(cx, jsoMat, MLELPN_NAME, &vName);
								mlRMMLMaterial* pMLElMat = (mlRMMLMaterial*)JS_GetPrivate(cx, jsoMat);
								// получаем значения свойств материала из реального объекта
								moIMaterial* pMat = NULL;
								if( iID >= 0)
									pMat = p3DO->GetMaterial(iID);

								if (pMat != NULL){
									pMLElMat->GetProps(pMat);
									// добавляем его в материалы
									priv->AddChild(pMLElMat);
									/* Tandy: это пока не знаю зачем
									for(int jj=0; jj<2; jj++){
										const wchar_t* pwcMapName = L"";
										switch(jj){
											case 0: pwcMapName = L"diffuse"; break;
											case 1: pwcMapName = L"specular"; break;
										}
										mlMedia* pMap = pMat->GetMap(pwcMapName);
										if(pMap != NULL){
											// ??
										}
									}*/
									// возвращаем ссылку на вновь созданный объект материала
									wr_JS_GetUCProperty(cx, obj, sMatName.c_str(), -1, vp);
								}
							}
						}
					}
				}
			}
		}
	GET_PROPS_END;
	return JS_TRUE;
}

// Реализация mlRMMLElement
mlresult mlRMMLMaterials::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLMaterials::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLMaterials::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. 
		return ML_ERROR_FAILURE;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLMaterials::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLMaterials* pNewEL=(mlRMMLMaterials*)GET_RMMLEL(mcx, mlRMMLMaterials::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

void mlRMMLMaterials::NotifyChangeListeners(mlRMMLMaterial* apMat, mlRMMLMap* apMap){
	v_elems::iterator vi;
	for(vi=mvChangeListeners.begin(); vi != mvChangeListeners.end(); vi++ ){
		mlRMML3DObject* pML3DO=(mlRMML3DObject*)*vi;
		unsigned uMatID = (unsigned)(-1);
		const wchar_t* wcsName = apMat->GetName();
		int iMatID = 0;
		if(ParseMatID(wcsName, &iMatID))
			uMatID = (unsigned)iMatID;
		if(apMap!=NULL){
			wchar_t* pMapName = apMap->GetName();
			mlString sMapName = pMapName;
			mlString::size_type pos = sMapName.find(L"_map");
			if(pos != mlString::npos){
				sMapName.erase(pos);
			}
			pML3DO->MaterialMapChanged(uMatID, sMapName.c_str());
		}else{
			pML3DO->MaterialChanged(uMatID);
		}
	}
}

void mlRMMLMaterials::AddChangeListener(mlRMML3DObject* ap3DO){
	mvChangeListeners.push_back((mlRMMLElement*)ap3DO);
}

void mlRMMLMaterials::RemoveChangeListener(mlRMML3DObject* ap3DO){
	v_elems::iterator vi;
	for(vi=mvChangeListeners.begin(); vi != mvChangeListeners.end(); vi++ ){
		if(*vi==(mlRMMLElement*)ap3DO){
			mvChangeListeners.erase(vi);
			break;
		}
	}
}

mlIMaterial* mlRMMLMaterials::GetMaterial(unsigned char auID){
	if(mpChildren==NULL)
		return NULL;
	for(v_elems_iter vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLMaterial* pMat = (mlRMMLMaterial*)(mlRMMLElement*)*vi;
		const wchar_t* wcsName = pMat->GetName();
		int iMatID = 0;
		if(ParseMatID(wcsName, &iMatID)){
			if((unsigned char)iMatID == auID)
				return (mlIMaterial*)pMat;
		}
	}
	return NULL;
}

void mlRMMLMaterials::SetMaterialDescription(int aiMatIdx, const wchar_t* apwcDescr){
	v_elems::iterator vi;
	for(vi=mvChangeListeners.begin(); vi != mvChangeListeners.end(); vi++ ){
		mlRMML3DObject* pML3DO=(mlRMML3DObject*)*vi;
		pML3DO->SetMaterialDescription(aiMatIdx, apwcDescr);
	}
}

bool mlRMMLMaterials::GetMaterialTextureSrc(mlRMMLMaterial* apMat, mlRMMLMap* apMap, mlString &asSrc){
	if(mpParent == NULL)
		return NULL;
	if(mpParent->Get3DObject() == NULL)
		return NULL;
	// ?? получить индекс материала apMat и имя apMap
	int iMatIdx = -1;
	for(v_elems_iter vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLMaterial* pMat = (mlRMMLMaterial*)(mlRMMLElement*)*vi;
		if(pMat != apMat)
			continue;
		const wchar_t* wcsName = pMat->GetName();
		int iMatID = 0;
		if(ParseMatID(wcsName, &iMatID)){
			iMatIdx = iMatID;
			break;
		}
	}
	if(iMatIdx == -1)
		return L"";
	mlString sMapName = apMat->GetMapName(apMap);
	if(sMapName.size() == 0)
		return L"";

	mlRMML3DObject* p3DO = mpParent->Get3DObject();
	mlOutString sSrc;
	bool bRes = p3DO->GetMaterialTextureSrc(iMatIdx, sMapName.c_str(), sSrc);
	asSrc = sSrc.mstr;
	return bRes;
}

}










