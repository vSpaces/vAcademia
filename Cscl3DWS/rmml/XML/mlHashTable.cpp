
#include "mlRMML.h" // for precompile heades only
#include "mlHashTable.h"

namespace rmml {

#undef xmlStrEqual
#define xmlStrEqual isEqual
/*
__forceinline bool isEqual(const wchar_t* apch1, const wchar_t* apch2){
	for(;;){
		if(*apch1!=*apch2) return false;
		if(*apch1==L'\0') return true;
		apch1++, apch2++;
	}
}
*/
#undef xmlStrndup
#define xmlStrndup Strndup

#undef xmlStrdup
#define xmlStrdup Strdup

#define MAX_HASH_LEN 8

unsigned long mlHashTable::ComputeKey(const wchar_t *name,
	          const wchar_t *name2, const wchar_t *name3) {
    unsigned long value = 0L;
    char ch;
    
    if (name != NULL) {
	value += 30 * (*name);
	while ((ch = *name++) != 0) {
	    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
	}
    }
    if (name2 != NULL) {
	while ((ch = *name2++) != 0) {
	    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
	}
    }
    if (name3 != NULL) {
	while ((ch = *name3++) != 0) {
	    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
	}
    }
    return (value % this->size);
}

unsigned long mlHashTable::ComputeQKey(
		   const wchar_t *prefix, const wchar_t *name,
		   const wchar_t *prefix2, const wchar_t *name2,
		   const wchar_t *prefix3, const wchar_t *name3) {
    unsigned long value = 0L;
    char ch;
    
    if (prefix != NULL)
	value += 30 * (*prefix);
    else
	value += 30 * (*name);

    if (prefix != NULL) {
	while ((ch = *prefix++) != 0) {
	    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
	}
	value = value ^ ((value << 5) + (value >> 3) + (unsigned long)':');
    }
    if (name != NULL) {
	while ((ch = *name++) != 0) {
	    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
	}
    }
    if (prefix2 != NULL) {
	while ((ch = *prefix2++) != 0) {
	    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
	}
	value = value ^ ((value << 5) + (value >> 3) + (unsigned long)':');
    }
    if (name2 != NULL) {
	while ((ch = *name2++) != 0) {
	    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
	}
    }
    if (prefix3 != NULL) {
	while ((ch = *prefix3++) != 0) {
	    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
	}
	value = value ^ ((value << 5) + (value >> 3) + (unsigned long)':');
    }
    if (name3 != NULL) {
	while ((ch = *name3++) != 0) {
	    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
	}
    }
    return (value % this->size);
}

mlHashTable::mlHashTable(int size) {
    if (size <= 0)
        size = 256;
  
    this->dict = NULL;
    this->size = size;
	this->nbElems = 0;
    this->table = MP_NEW_ARR( mlHashEntry, size);
}

mlHashTable::~mlHashTable(){
	MP_DELETE_ARR( table); 
}

int mlHashTable::AddEntry3(const xmlChar *name,
	         const xmlChar *name2, const xmlChar *name3,
		 void *userdata) {
    unsigned long key, len = 0;
    mlHashEntryPtr entry;
    mlHashEntryPtr insert;

    if ((table == NULL) || (name == NULL))
	return(-1);

    //
     // If using a dict internalize if needed
     
    if (this->dict) {
        if (!this->dict->Owns(name)) {
	    name = this->dict->Lookup(name, -1);
	    if (name == NULL)
	        return(-1);
	}
        if ((name2 != NULL) && (!this->dict->Owns(name2))) {
	    name2 = this->dict->Lookup(name2, -1);
	    if (name2 == NULL)
	        return(-1);
	}
        if ((name3 != NULL) && (!this->dict->Owns(name3))) {
	    name3 = this->dict->Lookup(name3, -1);
	    if (name3 == NULL)
	        return(-1);
	}
    }

    //
     // Check for duplicate and insertion location.
     
    key = ComputeKey(name, name2, name3);
    if (this->table[key].valid == 0) {
	insert = NULL;
    } else {
        if (this->dict) {
	    for (insert = &(this->table[key]); insert->next != NULL;
		 insert = insert->next) {
		if ((insert->name == name) &&
		    (insert->name2 == name2) &&
		    (insert->name3 == name3))
		    return(-1);
		len++;
	    }
	    if ((insert->name == name) &&
		(insert->name2 == name2) &&
		(insert->name3 == name3))
		return(-1);
	} else {
	    for (insert = &(this->table[key]); insert->next != NULL;
		 insert = insert->next) {
		if ((xmlStrEqual(insert->name, name)) &&
		    (xmlStrEqual(insert->name2, name2)) &&
		    (xmlStrEqual(insert->name3, name3)))
		    return(-1);
		len++;
	    }
	    if ((xmlStrEqual(insert->name, name)) &&
		(xmlStrEqual(insert->name2, name2)) &&
		(xmlStrEqual(insert->name3, name3)))
		return(-1);
	}
    }

    if (insert == NULL) {
		entry = &(this->table[key]);
    } else {
		entry = MP_NEW( mlHashEntry); 
		if (entry == NULL)
			return(-1);
    }

    if (this->dict != NULL) {
        entry->name = (xmlChar *) name;
        entry->name2 = (xmlChar *) name2;
        entry->name3 = (xmlChar *) name3;
    } else {
	entry->name = xmlStrdup(name);
	entry->name2 = xmlStrdup(name2);
	entry->name3 = xmlStrdup(name3);
    }
    entry->payload = userdata;
    entry->next = NULL;
    entry->valid = 1;


    if (insert != NULL) 
	insert->next = entry;

    this->nbElems++;

    if (len > MAX_HASH_LEN)
		Grow(MAX_HASH_LEN * this->size);

    return(0);
}

int mlHashTable::Grow(int size) {
    unsigned long key;
    int oldsize, i;
    mlHashEntryPtr iter, next;
    struct mlHashEntry *oldtable;
#ifdef DEBUG_GROW
    unsigned long nbElem = 0;
#endif
  
    if (table == NULL)
		return(-1);
    if (size < 8)
        return(-1);
    if (size > 8 * 2048)
		return(-1);

    oldsize = this->size;
    oldtable = this->table;
    if (oldtable == NULL)
        return(-1);
  
    this->table = MP_NEW_ARR( mlHashEntry, size); 
    if (this->table == NULL) {
		this->table = oldtable;
		return(-1);
    }
    // memset(this->table, 0, size * sizeof(xmlHashEntry));
    this->size = size;

    //	If the two loops are merged, there would be situations where
	// a new entry needs to allocated and data copied into it from 
	// the main table. So instead, we run through the array twice, first
	// copying all the elements in the main array (where we can't get
	// conflicts) and then the rest, so we only free (and don't allocate)
    
    for (i = 0; i < oldsize; i++) {
		if (oldtable[i].valid == 0) 
			continue;
		key = ComputeKey(oldtable[i].name, oldtable[i].name2,
					oldtable[i].name3);
		memcpy(&(this->table[key]), &(oldtable[i]), sizeof(mlHashEntry));
		this->table[key].next = NULL;
    }

    for (i = 0; i < oldsize; i++) {
	iter = oldtable[i].next;
	while (iter) {
	    next = iter->next;

	    //
	     // put back the entry in the new table

	    key = ComputeKey(iter->name, iter->name2,
		                    iter->name3);
	    if (this->table[key].valid == 0) {
		memcpy(&(this->table[key]), iter, sizeof(mlHashEntry));
		this->table[key].next = NULL;
		// xmlFree(iter);
	    } else {
	    	iter->next = this->table[key].next;
	    	this->table[key].next = iter;
	    }

#ifdef DEBUG_GROW
	    nbElem++;
#endif

	    iter = next;
	}
    }

    MP_DELETE_ARR( oldtable);

#ifdef DEBUG_GROW
    xmlGenericError(xmlGenericErrorContext,
	    "xmlHashGrow : from %d to %d, %d elems\n", oldsize, size, nbElem);
#endif

    return(0);
}

int mlHashTable::RemoveEntry3(const xmlChar *name,
    const xmlChar *name2, const xmlChar *name3, mlHashDeallocator f) {
    unsigned long key;
    mlHashEntryPtr entry;
    mlHashEntryPtr prev = NULL;

    if (table == NULL || name == NULL)
        return(-1);

    key = ComputeKey(name, name2, name3);
    if (this->table[key].valid == 0) {
        return(-1);
    } else {
        for (entry = &(this->table[key]); entry != NULL; entry = entry->next) {
            if (xmlStrEqual(entry->name, name) &&
                    xmlStrEqual(entry->name2, name2) &&
                    xmlStrEqual(entry->name3, name3)) {
                if ((f != NULL) && (entry->payload != NULL))
                    f(entry->payload, entry->name);
                entry->payload = NULL;
		if (this->dict == NULL) {
		    if(entry->name)
				MP_DELETE_ARR( entry->name);
		    if(entry->name2)
				MP_DELETE_ARR( entry->name2);
		    if(entry->name3)
				MP_DELETE_ARR( entry->name3);
		}
                if(prev) {
                    prev->next = entry->next;
		    MP_DELETE( entry);
		} else {
		    if (entry->next == NULL) {
				entry->valid = 0;
		    } else {
				entry = entry->next;
				memcpy(&(this->table[key]), entry, sizeof(mlHashEntry));
				MP_DELETE( entry);
		    }
		}
                this->nbElems--;
                return(0);
            }
            prev = entry;
        }
        return(-1);
    }
}

void* mlHashTable::Lookup3(const xmlChar *name, 
	       const xmlChar *name2, const xmlChar *name3) {
    unsigned long key;
    mlHashEntryPtr entry;

    if (table == NULL)
	return(NULL);
    if (name == NULL)
	return(NULL);
    key = ComputeKey(name, name2, name3);
    if (this->table[key].valid == 0)
	return(NULL);
    if (this->dict) {
	for (entry = &(this->table[key]); entry != NULL; entry = entry->next) {
	    if ((entry->name == name) &&
		(entry->name2 == name2) &&
		(entry->name3 == name3))
		return(entry->payload);
	}
    }
    for (entry = &(this->table[key]); entry != NULL; entry = entry->next) {
	if ((xmlStrEqual(entry->name, name)) &&
	    (xmlStrEqual(entry->name2, name2)) &&
	    (xmlStrEqual(entry->name3, name3)))
	    return(entry->payload);
    }
    return(NULL);
}

}
