#pragma once

namespace rmml {

/**
* JS-класс для синхронизируемой ссылки на объект
*/

// 0 - статус не определен, 
#define SLS_UNDEFINED 0
// 1 - объект существует в сцене и есть на него ссылка
#define SLS_EXISTS 1
// 2 - отправлен запрос на получение статуса объекта, но ответ еще не пришел
#define SLS_WAITING_STATUS 2
// 3 - объект существует на сервере и находится в поле видимости, но еще не загружен
#define SLS_NOT_LOADED_YET 3
// -1 - "битая ссылка" (объект не существует вообще)
#define SLS_BROKEN -1
// -2 - объект вне поля видимости или в другой реальности, поэтому не будет загружен и создан
#define SLS_NOT_VISIBLE -2
// -3 - возникла какая-то ошибка при загрузке или создании объекта
#define SLS_LOAD_ERROR -3
// -4 - статус объекта не может быть определен
#define SLS_CANT_BE_DEFINED -4
// -5 - ссылка на объект была, но теперь ее нет, потому что объект удален из сцены
#define SLS_LOST -5

/*
  Синхронизируемая ссылка - это ссылка на объект, которая может быть значением свойства подобъекта synch,
т.е. содержит некий идентификатор объекта, по которому можно инициировать его воссоздание, если он еще 
не загружен или получить реальную ссылку на объект (getRef), если он уже существует.

*/

class mlSynchLinkJSO:	
				public mlJSClass
{
friend class mlSynchData;
friend class mlSynch;
	unsigned int muObjectID;
	unsigned int muBornRealityID;
	mlRMMLElement* mpParentElem;
	mlSynch* mpParent;
	bool mbLinkIsTracking;	// ссылка отслеживается на предмет потери при удалении объекта
public:
	mlSynchLinkJSO(void);
	void destroy() { MP_DELETE_THIS; }
	~mlSynchLinkJSO(void);
	static JSBool AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	MLJSCLASS_DECL2(mlSynchLinkJSO)
private:
	enum {
//		JSPROP_ref = 1, // реальная ссылка на реальный объект (null, если объекта еще нет в сцене)
		JSPROP_status = 1 	
					// 0 - статус не определен, (SLS_UNDEFINED)
					// 1 - объект существует в сцене и есть на него ссылка (SLS_EXISTS)
					// 2 - отправлен запрос на получение статуса объекта, но ответ еще не пришел (SLS_WAITING_STATUS)
					// 3 - объект существует на сервере и находится в поле видимости, но еще не загружен (SLS_NOT_LOADED_YET)
					// -1 - "битая ссылка" (объект не существует вообще) (SLS_BROKEN)
					// -2 - объект вне поля видимости, поэтому не будет загружен и создан (SLS_NOT_VISIBLE)
					// -3 - возникла какая-то ошибка при загрузке или создании объекта (SLS_LOAD_ERROR)
					// -4 - статус объекта не может быть определен (SLS_CANT_BE_DEFINED)
					// -5 - ссылка на объект была, но теперь ее нет, потому что объект удален из сцены (SLS_LOST)
	};
	JSObject* ref;
	int status;
	JSFUNC_DECL(getRef)	// получить реальную ссылку на реальный объект
private:
	void UpdateObjectID();
	void GetStatus();
	unsigned int CorrectBornRealityID();
	void CallEventJSFunc(const wchar_t* apwcName);
public:
	void Init(mlSynchLinkJSO* apSrc);
	void SetParents(mlRMMLElement* apParentElem, mlSynch* apParent = NULL){
		mpParentElem = apParentElem;
		mpParent = apParent;
	}
	JSObject* GetRef();
	// ищет объект по muObjectID и устанавливает ref
	void UpdateRef(bool abElemDestroyed = false);
	int GetObjectID(unsigned int& aBornRealityID);
	// вызывается SceneManager-ом, когда получен ответ с MapServer-а
	// auObjectID и auBornRealityID - для проверки, что в этом Link-е все еще ожидается статус именно этого объекта
	void onStatusReceived(unsigned int auObjectID, unsigned int auBornRealityID, int aiStatus);
};

}
