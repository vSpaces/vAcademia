#pragma once

#include "mlRMML.h"

namespace rmml {

class mlCachedSynchProp
{
public:
	std::string msName;
	mlSynchData moValue;
public:
	mlCachedSynchProp(const char* apcName, mlSynchData& oData);
	mlCachedSynchProp(const mlCachedSynchProp& src){
		*this = src;
	}
	void SetValue(mlSynchData& oData);
	const mlCachedSynchProp& operator=(const mlCachedSynchProp& src){
		msName = src.msName;
		moValue = src.moValue;
		return *this;
	}
};

// список кэшированных свойств для отдельного объекта
class mlObjectCachedSynchProps : public std::vector<mlCachedSynchProp>
{
public:
	unsigned int muID; // идентификатор объекта
	unsigned int muBornRealityID; // идентификатор реальности рождения
	mlSysSynchProps mSysProps; // синхронизируемые системные свойства
	std::string msSubobjPath; // путь к подобъекту (потому что у них нет собственного идентификатора)
	typedef std::vector<mlObjectCachedSynchProps> VecSubobjProps;
	VecSubobjProps* mpvSOSPs; // вектор синхронизируемых свойств подобъектов
public:
	void AddProperty(const char* apcName, mlSynchData& oData);
	mlObjectCachedSynchProps():muID(0), muBornRealityID(0), mpvSOSPs(NULL){}
	~mlObjectCachedSynchProps(){
		if(mpvSOSPs != NULL)
			delete mpvSOSPs;
	}
	mlObjectCachedSynchProps(const mlObjectCachedSynchProps &src):mpvSOSPs(NULL){ *this = src;}
	mlObjectCachedSynchProps &operator=(const mlObjectCachedSynchProps &src)
	{
		clear();
		insert(begin(),src.begin(),src.end());
		muID = src.muID;
		muBornRealityID = src.muBornRealityID;
		mSysProps = src.mSysProps;
		msSubobjPath = src.msSubobjPath;
		if(mpvSOSPs != NULL)
			delete mpvSOSPs;
		if(src.mpvSOSPs == NULL){
			mpvSOSPs = NULL;
		}else{
			mpvSOSPs = new VecSubobjProps;
			mpvSOSPs->insert(mpvSOSPs->begin(), src.mpvSOSPs->begin(), src.mpvSOSPs->end());
		}
		return *this;
	}
	
};

enum EAddToCacheResult
{
	CR_ERROR, CR_NOT_FULL, CR_DISCONNECTED, CR_ADDED_NEW, CR_UPDATED
};

// Кэш для синхронизируемых свойств, 
// для которых еще нет объектов
// (?? надо постараться сделать так, чтобы можно было их пускать по той же веточке
//  pMLEl->SynchM3Props(*vi, oEventData))
class mlSynchPropCache : public std::vector<mlObjectCachedSynchProps>
{
	mlSynchedObjProps moSynchedObjProps; // для GetProperties
	VecSynchedObjProps moSubobjProps; // для GetProperties
	mlSynchData moData; // для GetProperties
	JSContext* mcx;
public:
	mlSynchPropCache():
	  MP_VECTOR_INIT(moSubobjProps)
	{ mcx = NULL; }
	void SetJSContext(JSContext* cx){ mcx = cx; }
	// добавить синхронизируемые свойства объекта в кэш
	// (обновить значения уже присутствующих свойств)
	// (!!! устарела !!!)
	void AddProperties(mlSynchedObjProps& aSynchedObjProps, mlSynchData& oData);

	// добавить синхронизируемые свойства объекта в кэш (для одного объекта во 2м формате)
	// (обновить значения уже присутствующих свойств)
	EAddToCacheResult AddProperties2(mlSynchData& oData);
	// получить синхронизируемые свойства в готовом для синхронизации виде
	bool GetProperties(unsigned int auObjectID, unsigned int auBornRealityID, mlSynchedObjProps* &aSynchedObjProps, mlSynchData* &oData, /*mlSysSynchProps* &apSysProps, */ VecSynchedObjProps* &aSubobjProps);
private:
	std::vector<mlObjectCachedSynchProps>::iterator FindSynchPropsObject(
		unsigned int auObjectID, unsigned int auBornRealityID,
		mlObjectCachedSynchProps*& aSynchProps);
};

}
