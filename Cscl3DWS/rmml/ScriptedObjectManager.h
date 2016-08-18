#pragma once

#include <map>
#include <vector>

namespace rmml {

class mlRMMLElement;
typedef std::vector<mlRMMLElement*> ObjectsVector;

/*
CScriptedObjectManager
  - хранит ссылки на скритовые объекты,
  - предоставляет средства поиска объектов по их идентификаторам,
  - предоставляет средства перебора объектов.

Скриптовый объект мира - это 3D объект мира, у которого задан скриптовый класс. 
Соответсвенно, для него создается rmml-объект.
По принципу создания вложенных записей скриптовые объекты разделяются на две катагории:
  - принадлежащие только одной реальности -- все объекты, кроме аватаров;
  - перемещающиеся по реальностям -- пока это только автары.
При создании записи создаются копии всех объектов, участвовавших в записи.
В случай с перемещающимися по реальностям объектами (аватарами) при воспрозводении записии в одной 
реальности могут оказаться две копии одного объекта - записанный и переместившийся в 
реальность воспроизведения (записанный аватар и автар просматривающего запись пользователя).
При создании записей на основе просмотра другой записи копий одного перемещающегося по
реальностям объекта может быть более двух.

Таким образом, все объекты одной реальности, кроме перемещающихся по реальностям объектов 
(аватаров), могут быть идентифицированы одним идентификатором - objectID.
Для идентификации перемещающихся по реальностям объектов (аватаров) необходимо два
идентификатора - objectID и bornRealityID

Исходя из этого, элементы хранятся в двух хешах (map):
  - по ключу objectID,
  - по сложному ключу - objectID, bornRealityID
*/

class CScriptedObjectManager
{
public:
	CScriptedObjectManager(void);
	virtual ~CScriptedObjectManager(void);

	void RegisterObject( mlRMMLElement* apMLEl);
	void ChangeObjectRealityAndBornReality( mlRMMLElement* apMLEl, unsigned int auRealityID, unsigned int auBornRealityID);
	void UnregisterObject( mlRMMLElement* apMLEl);
	mlRMMLElement* GetObject(unsigned int auObjID, unsigned int auRealityID, bool aAccurately = true);
	mlRMMLElement* GetOneRealityObject(unsigned int auObjID);
	void GetObjects( ObjectsVector& anObjects);
	// Возвращает живых аватаров. Исправление #84
	void GetRealAvatars( ObjectsVector& anObjects);
	// Возвращает записанных аватаров, которые могли остаться при выходе из записи
	void GetRecordedAvatars( ObjectsVector& anObjects);
	// Возвращает объекты, не являющиеся аватары
	void GetNoAvatarSyncObjects( ObjectsVector& anObjects);

private:
	void JoinScriptedObject( std::string& aJoinedObject);

	// Карта скритуемых объектов
	typedef MP_MAP<unsigned __int64, mlRMMLElement*> CMovableRealityObjectMap;
	CMovableRealityObjectMap movableRealityObjects;

	// Карта синхронизируемых объектов, которыми не владеют эксклюзивно
	typedef MP_MAP<unsigned int, mlRMMLElement*> COneRealityObjectMap;
	COneRealityObjectMap oneRealityObjects;

	// Структура идентификаторов объекта
	struct CObjectIdentifiers 
	{
		unsigned int objectID;
		unsigned int bornRealityID;

		CObjectIdentifiers() {}
		CObjectIdentifiers( unsigned int anObjectID, unsigned int aBornRealityID) :
			objectID( anObjectID), bornRealityID( aBornRealityID) 
		{
		}
		bool operator==( const CObjectIdentifiers& aRight)
		{
			if (objectID != aRight.objectID)
				return false;
			if (bornRealityID != aRight.bornRealityID)
				return false;
			return true;
		}
		bool operator!=( const CObjectIdentifiers& aRight)
		{
			return !(*this == aRight);
		}
	};

	// Карта всех зарегистрированных объектов - указателю объекта сопоставляется его иднтификаторы,
	// которые были, когда объект был зарегистрирован
	typedef MP_MAP< mlRMMLElement*, CObjectIdentifiers> CAllObjectMap;
	CAllObjectMap allObjectMap;
};

}