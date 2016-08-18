#include "mlRMML.h" // for precompile heades only

#include "mlDict.h"

namespace rmml {

#define MAX_HASH_LEN 4
#define MIN_DICT_SIZE 128
#define MAX_DICT_HASH 8 * 2048

// #define ALLOW_REMOVAL
// #define DEBUG_GROW

/*
 * xmlDictAddString:
 * @dict: the dictionnary
 * @name: the name of the userdata
 * @len: the length of the name, if -1 it is recomputed
 *
 * Add the string to the array[s]
 *
 * Returns the pointer of the local string, or NULL in case of error.
 */
const wchar_t* mlDict::AddString(const wchar_t *name, int namelen) {
    mlDictStringsPtr pool;
    const wchar_t *ret;
    int size = 0; /* + sizeof(_xmlDictStrings) == 1024 */

    pool = this->strings;
    while (pool != NULL) {
		if (pool->end - pool->free > namelen)
			goto found_pool;
		if (pool->size > size) size = pool->size;
		pool = pool->next;
    }
    // Not found, need to allocate
    if (pool == NULL) {
        if (size == 0) size = 1000;
	else size *= 4; // exponential growth
        if (size < 4 * namelen) 
	    size = 4 * namelen; // just in case !
	//pool = (mlDictStringsPtr) new char[sizeof(mlDictStrings) + size];
	pool = (mlDictStringsPtr) MP_NEW_ARR( char, sizeof(mlDictStrings) + size);
	if (pool == NULL)
	    return(NULL);
	pool->size = size;
	pool->nbStrings = 0;
	pool->free = &pool->array[0];
	pool->end = &pool->array[size];
	pool->next = this->strings;
	this->strings = pool;
    }
found_pool:
    ret = pool->free;
    memcpy(pool->free, name, namelen);
    pool->free += namelen;
    *(pool->free++) = 0;
    return(ret);
}

/*
 * xmlDictAddQString:
 * @dict: the dictionnary
 * @prefix: the prefix of the userdata
 * @name: the name of the userdata
 * @len: the length of the name, if -1 it is recomputed
 *
 * Add the QName to the array[s]
 *
 * Returns the pointer of the local string, or NULL in case of error.
 */
const wchar_t* mlDict::AddQString(const wchar_t *prefix,
                 const wchar_t *name, int namelen)
{
    mlDictStringsPtr pool;
    const wchar_t *ret;
    int size = 0; /* + sizeof(_xmlDictStrings) == 1024 */
    int plen;

    if (prefix == NULL) return(AddString(name, namelen));
    plen = wcslen(prefix);

    pool = this->strings;
    while (pool != NULL) {
	if (pool->end - pool->free > namelen)
	    goto found_pool;
	if (pool->size > size) size = pool->size;
	pool = pool->next;
    }
    /*
     * Not found, need to allocate
     */
    if (pool == NULL) {
        if (size == 0) size = 1000;
	else size *= 4; // exponential growth
        if (size < 4 * namelen) 
	    size = 4 * namelen; // just in case ! 
	pool = (mlDictStringsPtr) MP_NEW_ARR( char, sizeof(mlDictStrings) + size);
	if (pool == NULL)
	    return(NULL);
	pool->size = size;
	pool->nbStrings = 0;
	pool->free = &pool->array[0];
	pool->end = &pool->array[size];
	pool->next = this->strings;
	this->strings = pool;
    }
found_pool:
    ret = pool->free;
    memcpy(pool->free, prefix, plen);
    pool->free += plen;
    *(pool->free++) = ':';
    namelen -= plen + 1;
    memcpy(pool->free, name, namelen);
    pool->free += namelen;
    *(pool->free++) = 0;
    return(ret);
}

/*
 * xmlDictComputeKey:
 * Calculate the hash key
 */
unsigned long ComputeKey(const wchar_t *name, int namelen)
{
    unsigned long value = 0L;
    
    if (name == NULL) return(0);
    value = *name;
    value <<= 5;
    if (namelen > 10) {
        value += name[namelen - 1];
        namelen = 10;
    }
    switch (namelen) {
        case 10: value += name[9];
        case 9: value += name[8];
        case 8: value += name[7];
        case 7: value += name[6];
        case 6: value += name[5];
        case 5: value += name[4];
        case 4: value += name[3];
        case 3: value += name[2];
        case 2: value += name[1];
        default: break;
    }
    return(value);
}

/*
 * xmlDictComputeQKey:
 * Calculate the hash key
 */
unsigned long ComputeQKey(const wchar_t *prefix, const wchar_t *name, int len)
{
    unsigned long value = 0L;
    int plen;
    
    if (prefix == NULL)
        return(ComputeKey(name, len));

    plen = wcslen(prefix);
    if (plen == 0)
	value += 30 * (unsigned long) ':';
    else
	value += 30 * (*prefix);
    
    if (len > 10) {
        value += name[len - (plen + 1 + 1)];
        len = 10;
	if (plen > 10)
	    plen = 10;
    }
    switch (plen) {
        case 10: value += prefix[9];
        case 9: value += prefix[8];
        case 8: value += prefix[7];
        case 7: value += prefix[6];
        case 6: value += prefix[5];
        case 5: value += prefix[4];
        case 4: value += prefix[3];
        case 3: value += prefix[2];
        case 2: value += prefix[1];
        case 1: value += prefix[0];
        default: break;
    }
    len -= plen;
    if (len > 0) {
        value += (unsigned long) ':';
	len--;
    }
    switch (len) {
        case 10: value += name[9];
        case 9: value += name[8];
        case 8: value += name[7];
        case 7: value += name[6];
        case 6: value += name[5];
        case 5: value += name[4];
        case 4: value += name[3];
        case 3: value += name[2];
        case 2: value += name[1];
        case 1: value += name[0];
        default: break;
    }
    return(value);
}

/**
 * xmlDictCreate:
 *
 * Create a new dictionary
 *
 * Returns the newly created dictionnary, or NULL if an error occured.
 */
mlDict::mlDict() {

    ref_counter = 1;

    size = MIN_DICT_SIZE;
	nbElems = 0;
    dict = MP_NEW_ARR( mlDictEntry, MIN_DICT_SIZE);
	strings = NULL;
	subdict = NULL;
    return;
}

/**
 * xmlDictCreateSub:
 * @sub: an existing dictionnary
 *
 * Create a new dictionary, inheriting strings from the read-only
 * dictionnary @sub. On lookup, strings are first searched in the
 * new dictionnary, then in @sub, and if not found are created in the
 * new dictionnary.
 *
 * Returns the newly created dictionnary, or NULL if an error occured.
 */
//xmlDictPtr
//xmlDictCreateSub(xmlDictPtr sub) {
//    xmlDictPtr dict = xmlDictCreate();
//  
//    if ((dict != NULL) && (sub != NULL)) {
//        dict->subdict = sub;
//	xmlDictReference(dict->subdict);
//    }
//    return(dict);
//}

/**
 * xmlDictReference:
 * @dict: the dictionnary
 *
 * Increment the reference counter of a dictionary
 *
 * Returns 0 in case of success and -1 in case of error
 */
int mlDict::AddRef() {
    ref_counter++;
    return(0);
}

/**
 * xmlDictGrow:
 * @dict: the dictionnary
 * @size: the new size of the dictionnary
 *
 * resize the dictionnary
 *
 * Returns 0 in case of success, -1 in case of failure
 */
int mlDict::Grow(int size) {
    unsigned long key;
    int oldsize, i;
    mlDictEntryPtr iter, next;
    mlDictEntry *olddict;
#ifdef DEBUG_GROW
    unsigned long nbElem = 0;
#endif
  
    if (size < 8)
        return(-1);
    if (size > 8 * 2048)
	return(-1);

    oldsize = size;
    olddict = dict;
    if (olddict == NULL)
        return(-1);
  
    dict = (mlDictEntry*) MP_NEW_ARR( char, size * sizeof(mlDictEntry)); 
    if (dict == NULL){
		dict = olddict;
		return(-1);
    }
    memset(this->dict, 0, size * sizeof(mlDictEntry));
    this->size = size;

    /*	If the two loops are merged, there would be situations where
	a new entry needs to allocated and data copied into it from 
	the main dict. So instead, we run through the array twice, first
	copying all the elements in the main array (where we can't get
	conflicts) and then the rest, so we only free (and don't allocate)
    */
    for (i = 0; i < oldsize; i++) {
	if (olddict[i].valid == 0) 
	    continue;
	key = ComputeKey(olddict[i].name, olddict[i].len) % this->size;
	memcpy(&(this->dict[key]), &(olddict[i]), sizeof(mlDictEntry));
	this->dict[key].next = NULL;
#ifdef DEBUG_GROW
	nbElem++;
#endif
    }

    for (i = 0; i < oldsize; i++) {
	iter = olddict[i].next;
	while (iter) {
	    next = iter->next;

	    /*
	     * put back the entry in the new dict
	     */

	    key = ComputeKey(iter->name, iter->len) % this->size;
	    if (this->dict[key].valid == 0) {
			(this->dict[key]) = *iter; // memcpy(&(this->dict[key]), iter, sizeof(mlDictEntry));
			this->dict[key].next = NULL;
			this->dict[key].valid = 1;
			 MP_DELETE( iter);
	    } else {
	    	iter->next = this->dict[key].next;
	    	this->dict[key].next = iter;
	    }

#ifdef DEBUG_GROW
	    nbElem++;
#endif

	    iter = next;
	}
    }

     MP_DELETE_ARR( olddict);

#ifdef DEBUG_GROW
    xmlGenericError(xmlGenericErrorContext,
	    "xmlDictGrow : from %d to %d, %d elems\n", oldsize, size, nbElem);
#endif

    return(0);
}

/**
 * xmlDictFree:
 * @dict: the dictionnary
 *
 * Free the hash @dict and its contents. The userdata is
 * deallocated with @f if provided.
 */
void mlDict::Release() {

    // decrement the counter, it may be shared by a parser and docs 
    ref_counter--;
    if (ref_counter > 0) {
        return;
    }
	 MP_DELETE_THIS;
}

mlDict::~mlDict(){
    if (subdict != NULL) {
        subdict->Release();
    }

    int i;
    mlDictEntryPtr iter;
    mlDictEntryPtr next;
    int inside_dict = 0;
    mlDictStringsPtr pool, nextp;

    if (dict) {
		for(i = 0; ((i < size) && (nbElems > 0)); i++) {
			iter = &(dict[i]);
			if (iter->valid == 0)
				continue;
			inside_dict = 1;
			while (iter) {
				next = iter->next;
				if (!inside_dict)
					 MP_DELETE( iter);
				nbElems--;
				inside_dict = 0;
				iter = next;
			}
			inside_dict = 0;
		}
		 MP_DELETE( dict);
    }
    pool = strings;
    while (pool != NULL) {
		nextp = pool->next;
		MP_DELETE( pool);
		pool = nextp;
    }
}

/**
 * xmlDictLookup:
 * @dict: the dictionnary
 * @name: the name of the userdata
 * @len: the length of the name, if -1 it is recomputed
 *
 * Add the @name to the dictionnary @dict if not present.
 *
 * Returns the internal copy of the name or NULL in case of internal error
 */
const wchar_t* mlDict::Lookup(const wchar_t *name, int len) {
    unsigned long key, okey, nbi = 0;
    mlDictEntryPtr entry;
    mlDictEntryPtr insert;
    const wchar_t *ret;

    if ((dict == NULL) || (name == NULL))
	return(NULL);

    if (len < 0)
        len = wcslen(name);

    /*
     * Check for duplicate and insertion location.
     */
    okey = ComputeKey(name, len);
    key = okey % this->size;
    if (this->dict[key].valid == 0) {
	insert = NULL;
    } else {
	for (insert = &(this->dict[key]); insert->next != NULL;
	     insert = insert->next) {
#ifdef __GNUC__
	    if (insert->len == len) {
		if (!memcmp(insert->name, name, len))
		    return(insert->name);
	    }
#else
	    if ((insert->len == len) &&
	        (!wcsncmp(insert->name, name, len)))
		return(insert->name);
#endif
	    nbi++;
	}
#ifdef __GNUC__
	if (insert->len == len) {
	    if (!memcmp(insert->name, name, len))
		return(insert->name);
	}
#else
	if ((insert->len == len) &&
	    (!wcsncmp(insert->name, name, len)))
	    return(insert->name);
#endif
    }

    if (this->subdict) {
	key = okey % this->subdict->size;
	if (this->subdict->dict[key].valid != 0) {
	    mlDictEntryPtr tmp;

	    for (tmp = &(this->subdict->dict[key]); tmp->next != NULL;
		 tmp = tmp->next) {
#ifdef __GNUC__
		if (tmp->len == len) {
		    if (!memcmp(tmp->name, name, len))
			return(tmp->name);
		}
#else
		if ((tmp->len == len) &&
		    (!wcsncmp(tmp->name, name, len)))
		    return(tmp->name);
#endif
		nbi++;
	    }
#ifdef __GNUC__
	    if (tmp->len == len) {
		if (!memcmp(tmp->name, name, len))
		    return(tmp->name);
	    }
#else
	    if ((tmp->len == len) &&
		(!wcsncmp(tmp->name, name, len)))
		return(tmp->name);
#endif
	}
	key = okey % this->size;
    }

    ret = AddString(name, len);
    if (ret == NULL)
        return(NULL);
    if (insert == NULL) {
	entry = &(this->dict[key]);
    } else {
	entry = MP_NEW( mlDictEntry);
	if (entry == NULL)
	     return(NULL);
    }
    entry->name = ret;
    entry->len = len;
    entry->next = NULL;
    entry->valid = 1;


    if (insert != NULL) 
	insert->next = entry;

    this->nbElems++;

    if ((nbi > MAX_HASH_LEN) &&
        (this->size <= ((MAX_DICT_HASH / 2) / MAX_HASH_LEN)))
	Grow(MAX_HASH_LEN * 2 * this->size);
    /* Note that entry may have been freed at this point by xmlDictGrow */

    return(ret);
}

/**
 * xmlDictExists:
 * @dict: the dictionnary
 * @name: the name of the userdata
 * @len: the length of the name, if -1 it is recomputed
 *
 * Check if the @name exists in the dictionnary @dict.
 *
 * Returns the internal copy of the name or NULL if not found.
 */
const wchar_t* mlDict::Exists(const wchar_t *name, int len) {
    unsigned long key, okey, nbi = 0;
    mlDictEntryPtr insert;

    if ((dict == NULL) || (name == NULL))
	return(NULL);

    if (len < 0)
        len = wcslen(name);

    /*
     * Check for duplicate and insertion location.
     */
    okey = ComputeKey(name, len);
    key = okey % this->size;
    if (this->dict[key].valid == 0) {
	insert = NULL;
    } else {
	for (insert = &(this->dict[key]); insert->next != NULL;
	     insert = insert->next) {
#ifdef __GNUC__
	    if (insert->len == len) {
		if (!memcmp(insert->name, name, len))
		    return(insert->name);
	    }
#else
	    if ((insert->len == len) &&
	        (!wcsncmp(insert->name, name, len)))
		return(insert->name);
#endif
	    nbi++;
	}
#ifdef __GNUC__
	if (insert->len == len) {
	    if (!memcmp(insert->name, name, len))
		return(insert->name);
	}
#else
	if ((insert->len == len) &&
	    (!wcsncmp(insert->name, name, len)))
	    return(insert->name);
#endif
    }

    if (this->subdict) {
	key = okey % this->subdict->size;
	if (this->subdict->dict[key].valid != 0) {
	    mlDictEntryPtr tmp;

	    for (tmp = &(this->subdict->dict[key]); tmp->next != NULL;
		 tmp = tmp->next) {
#ifdef __GNUC__
		if (tmp->len == len) {
		    if (!memcmp(tmp->name, name, len))
			return(tmp->name);
		}
#else
		if ((tmp->len == len) &&
		    (!wcsncmp(tmp->name, name, len)))
		    return(tmp->name);
#endif
		nbi++;
	    }
#ifdef __GNUC__
	    if (tmp->len == len) {
		if (!memcmp(tmp->name, name, len))
		    return(tmp->name);
	    }
#else
	    if ((tmp->len == len) &&
		(!wcsncmp(tmp->name, name, len)))
		return(tmp->name);
#endif
	}
	key = okey % this->size;
    }

    // not found
    return(NULL);
}
/*
__forceinline bool isEqual(const wchar_t* apch1, const wchar_t* apch2){
	for(;;){
		if(*apch1!=*apch2) return false;
		if(*apch1==L'\0') return true;
		apch1++, apch2++;
	}
}
*/
int IsQEqual(const wchar_t *pref, const wchar_t *name, const wchar_t *str) {
    if (pref == NULL) return(isEqual(name, str));
    if (name == NULL) return(0);
    if (str == NULL) return(0);

    do {
        if (*pref++ != *str) return(0);
    } while ((*str++) && (*pref));
    if (*str++ != ':') return(0);
    do {
        if (*name++ != *str) return(0);
    } while (*str++);
    return(1);
}

/**
 * xmlDictQLookup:
 * @dict: the dictionnary
 * @prefix: the prefix 
 * @name: the name
 *
 * Add the QName @prefix:@name to the hash @dict if not present.
 *
 * Returns the internal copy of the QName or NULL in case of internal error
 */
const wchar_t* mlDict::QLookup(const wchar_t *prefix, const wchar_t *name) {
    unsigned long okey, key, nbi = 0;
    mlDictEntryPtr entry;
    mlDictEntryPtr insert;
    const wchar_t *ret;
    int len;

    if ((dict == NULL) || (name == NULL))
	return(NULL);

    len = wcslen(name);
    if (prefix != NULL)
        len += 1 + wcslen(prefix);

    /*
     * Check for duplicate and insertion location.
     */
    okey = ComputeQKey(prefix, name, len);
    key = okey % this->size;
    if (this->dict[key].valid == 0) {
	insert = NULL;
    } else {
	for (insert = &(this->dict[key]); insert->next != NULL;
	     insert = insert->next) {
	    if ((insert->len == len) &&
	        (IsQEqual(prefix, name, insert->name)))
		return(insert->name);
	    nbi++;
	}
	if ((insert->len == len) &&
	    (IsQEqual(prefix, name, insert->name)))
	    return(insert->name);
    }

    if (this->subdict) {
	key = okey % this->subdict->size;
	if (this->subdict->dict[key].valid != 0) {
	    mlDictEntryPtr tmp;
	    for (tmp = &(this->subdict->dict[key]); tmp->next != NULL;
		 tmp = tmp->next) {
		if ((tmp->len == len) &&
		    (IsQEqual(prefix, name, tmp->name)))
		    return(tmp->name);
		nbi++;
	    }
	    if ((tmp->len == len) &&
		(IsQEqual(prefix, name, tmp->name)))
		return(tmp->name);
	}
	key = okey % this->size;
    }

    ret = AddQString(prefix, name, len);
    if (ret == NULL)
        return(NULL);
    if (insert == NULL) {
	entry = &(this->dict[key]);
    } else {
	entry = MP_NEW( mlDictEntry);
	if (entry == NULL)
	     return(NULL);
    }
    entry->name = ret;
    entry->len = len;
    entry->next = NULL;
    entry->valid = 1;

    if (insert != NULL) 
	insert->next = entry;

    this->nbElems++;

    if ((nbi > MAX_HASH_LEN) &&
        (this->size <= ((MAX_DICT_HASH / 2) / MAX_HASH_LEN)))
	Grow(MAX_HASH_LEN * 2 * this->size);
    // Note that entry may have been freed at this point by xmlDictGrow

    return(ret);
}

/**
 * xmlDictOwns:
 * @dict: the dictionnary
 * @str: the string
 *
 * check if a string is owned by the disctionary
 *
 * Returns 1 if true, 0 if false and -1 in case of error
 * -1 in case of error
 */
int mlDict::Owns(const wchar_t *str) {
    mlDictStringsPtr pool;

    if ((dict == NULL) || (str == NULL))
	return(-1);
    pool = this->strings;
    while (pool != NULL) {
        if ((str >= &pool->array[0]) && (str <= pool->free))
	    return(1);
	pool = pool->next;
    }
    if (this->subdict)
        return(subdict->Owns(str));
    return(0);
}

/**
 * xmlDictSize:
 * @dict: the dictionnary
 *
 * Query the number of elements installed in the hash @dict.
 *
 * Returns the number of elements in the dictionnary or
 * -1 in case of error
 */
int mlDict::DictSize() {
    if (subdict)
        return(nbElems + subdict->nbElems);
    return(nbElems);
}

}
