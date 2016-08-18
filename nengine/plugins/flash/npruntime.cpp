#include "stdafx.h"

#include <map>
#include <string>

//#include "config.h"

//#if ENABLE(NETSCAPE_PLUGIN_API)
/*
#include "npruntime_internal.h"
*/
//#include "npapi.h"
#include "npruntime.h"
#include "npruntime_impl.h"

#define ASSERT(expr)

/*
#include "npruntime_priv.h"

#include "c_utility.h"
#include <runtime/Identifier.h>
#include <runtime/JSLock.h>
#include <wtf/Assertions.h>
#include <wtf/HashMap.h>

using namespace JSC::Bindings;

typedef HashMap<RefPtr<JSC::UString::Rep>, PrivateIdentifier*> StringIdentifierMap;
*/

typedef std::map<std::wstring, PrivateIdentifier*> StringIdentifierMap;
typedef std::map<int, PrivateIdentifier*> IntIdentifierMap;
typedef std::pair<std::wstring, PrivateIdentifier*> StringIdentifierPair;
typedef std::pair<int, PrivateIdentifier*> IntIdentifierPair;

StringIdentifierMap gStringIdentifierMap;
IntIdentifierMap gIntIdentifierMap;

class CMapsEraser
{
public:
	CMapsEraser(){}
	~CMapsEraser(){
		// чистим gStringIdentifierMap и StringIdentifierMap
		// ??
	}
}gMapsEraser;

static StringIdentifierMap* getStringIdentifierMap()
{
	return &gStringIdentifierMap;
}

static IntIdentifierMap* getIntIdentifierMap(){
	return &gIntIdentifierMap;
}

/*
static StringIdentifierMap* getStringIdentifierMap()
{
    static StringIdentifierMap* stringIdentifierMap = 0;
    if (!stringIdentifierMap)
        stringIdentifierMap = new StringIdentifierMap;
    return stringIdentifierMap;
}

typedef HashMap<int, PrivateIdentifier*> IntIdentifierMap;

static IntIdentifierMap* getIntIdentifierMap()
{
    static IntIdentifierMap* intIdentifierMap = 0;
    if (!intIdentifierMap)
        intIdentifierMap = new IntIdentifierMap;
    return intIdentifierMap;
}
*/

bool UTF82String(const char* autf8Str, int anLen, std::wstring &asStr){
	asStr.erase();

	if (anLen == -1){
		anLen = (int)strlen(autf8Str);
	}

	unsigned uLength;
	char chTemp;
	for (; *autf8Str != '\0' && anLen > 0; anLen--){
		wchar_t wch;
		if ((*autf8Str & 0x80) == 0){
			wch = *autf8Str;
			autf8Str++;
		}else{
			// Calculate bits
			//chTemp = (char)0xF0;
			if ((*autf8Str & 0xF0) == 0xF0){
				uLength = 4; chTemp = (char) 0xF0;
			}
			else if ((*autf8Str & 0xE0) == 0xE0){
				uLength = 3; chTemp = (char) 0xE0;
			}
			else if ((*autf8Str & 0xC0) == 0xC0){
				uLength = 2; chTemp = (char) 0xC0;
			}
			else
				return false;
			anLen -= (uLength - 1);

			// Build character
			// The first byte needs to clear part that has count of bytes for the UTF8 character.
			// This is done by clearing the number bits for each byte + 1 from right that the character
			// are taken.
			// Then we shift the final character one octal bitlenght for each byte that is added.
			uLength--;
			wch = (*autf8Str & ~chTemp) << (uLength * 6);

			autf8Str++; uLength--;
			wch |= (*autf8Str & 0x3F) << (uLength * 6);
			if (uLength){
				autf8Str++; uLength--;
				wch |= (*autf8Str & 0x3F) << (uLength * 6);
				if (uLength){
					autf8Str++; uLength--;
					wch |= (*autf8Str & 0x3F);
				}
			}
			autf8Str++;
		}
		asStr += wch;
	}
	return true;
}

std::wstring identifierFromNPIdentifier(const NPUTF8* name)
{
	std::wstring wsName;
	UTF82String(name, -1, wsName);
	return wsName;
}

NPIdentifier _NPN_GetStringIdentifier(const NPUTF8* name)
{
    ASSERT(name);
    
    if (name) {
        PrivateIdentifier* identifier = NULL;
        
		StringIdentifierMap::const_iterator it = getStringIdentifierMap()->find(identifierFromNPIdentifier(name));
		if(it != getStringIdentifierMap()->end())
			identifier = it->second;
        if (identifier == 0) {
            identifier = new PrivateIdentifier;
            // We never release identifier names, so this dictionary will grow, as will
            // the memory for the identifier name strings.
            identifier->isString = true;
            identifier->value.string = strdup(name);

            getStringIdentifierMap()->insert(StringIdentifierPair(identifierFromNPIdentifier(name), identifier));
        }
        return (NPIdentifier)identifier;
    }
    
    return 0;
}

void _NPN_GetStringIdentifiers(const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
{
    ASSERT(names);
    ASSERT(identifiers);
    
    if (names && identifiers)
        for (int i = 0; i < nameCount; i++)
            identifiers[i] = _NPN_GetStringIdentifier(names[i]);
}

NPIdentifier _NPN_GetIntIdentifier(int32_t intid)
{
    PrivateIdentifier* identifier = NULL;

    if (intid == 0 || intid == -1) {
        static PrivateIdentifier* negativeOneAndZeroIdentifiers[2];

        identifier = negativeOneAndZeroIdentifiers[intid + 1];
        if (!identifier) {
            identifier = new PrivateIdentifier;
            identifier->isString = false;
            identifier->value.number = intid;

            negativeOneAndZeroIdentifiers[intid + 1] = identifier;
        }
    } else {
		IntIdentifierMap::const_iterator it = getIntIdentifierMap()->find(intid);
		if(it != getIntIdentifierMap()->end())
			identifier = it->second;
        if (!identifier) {
            identifier = (PrivateIdentifier*)malloc(sizeof(PrivateIdentifier));
            // We never release identifier names, so this dictionary will grow.
            identifier->isString = false;
            identifier->value.number = intid;

            getIntIdentifierMap()->insert(IntIdentifierPair((int)intid, identifier));
        }
    }
    return (NPIdentifier)identifier;
}

bool _NPN_IdentifierIsString(NPIdentifier identifier)
{
    PrivateIdentifier* i = (PrivateIdentifier*)identifier;
    return i->isString;
}

NPUTF8 *_NPN_UTF8FromIdentifier(NPIdentifier identifier)
{
    PrivateIdentifier* i = (PrivateIdentifier*)identifier;
    if (!i->isString || !i->value.string)
        return NULL;
        
    return (NPUTF8 *)strdup(i->value.string);
}

int32_t _NPN_IntFromIdentifier(NPIdentifier identifier)
{
    PrivateIdentifier* i = (PrivateIdentifier*)identifier;
    if (i->isString)
        return 0;
    return i->value.number;
}

void NPN_InitializeVariantWithStringCopy(NPVariant* variant, const NPString* value)
{
    variant->type = NPVariantType_String;
    variant->value.stringValue.UTF8Length = value->UTF8Length;
    variant->value.stringValue.UTF8Characters = (NPUTF8 *)malloc(sizeof(NPUTF8) * value->UTF8Length);
    memcpy((void*)variant->value.stringValue.UTF8Characters, value->UTF8Characters, sizeof(NPUTF8) * value->UTF8Length);
}

void _NPN_ReleaseVariantValue(NPVariant* variant)
{
    ASSERT(variant);

    if (variant->type == NPVariantType_Object) {
        _NPN_ReleaseObject(variant->value.objectValue);
        variant->value.objectValue = 0;
    } else if (variant->type == NPVariantType_String) {
        free((void*)variant->value.stringValue.UTF8Characters);
        variant->value.stringValue.UTF8Characters = 0;
        variant->value.stringValue.UTF8Length = 0;
    }

    variant->type = NPVariantType_Void;
}

NPObject *_NPN_CreateObject(NPP npp, NPClass* aClass)
{
    ASSERT(aClass);

    if (aClass) {
        NPObject* obj;
		if (aClass->allocate != NULL){
            obj = aClass->allocate(npp, aClass);
		}else{
//            obj = (NPObject*)malloc(sizeof(NPObject));
			  obj = new NPObject();
		}

        obj->_class = aClass;
        obj->referenceCount = 1;

        return obj;
    }

    return 0;
}

NPObject* _NPN_RetainObject(NPObject* obj)
{
    ASSERT(obj);

    if (obj)
        obj->referenceCount++;

    return obj;
}

void _NPN_DeallocateObject(NPObject *obj);

void _NPN_ReleaseObject(NPObject* obj)
{
    ASSERT(obj);
    ASSERT(obj->referenceCount >= 1);

    if (obj && obj->referenceCount >= 1) {
        if (--obj->referenceCount == 0)
            _NPN_DeallocateObject(obj);
    }
}

void _NPN_DeallocateObject(NPObject *obj)
{
    ASSERT(obj);

    if (obj) {
		if (obj->_class->deallocate){
            obj->_class->deallocate(obj);
		}else{
            delete obj;
		}
    }
}

// from NP_jsobject.cpp

//#endif // ENABLE(NETSCAPE_PLUGIN_API)
