#pragma once

#include "mlDict.h"
#include <string>

namespace rmml {

#undef xmlChar
#define xmlChar wchar_t

class mlHashTable;
typedef mlHashTable *mlHashTablePtr;

class mlHashTable {

	struct mlHashEntry {
		mlHashEntry *next;
		wchar_t *name;
		wchar_t *name2;
		wchar_t *name3;
		void *payload;
		int valid;
		mlHashEntry(){
			next = NULL;
			name = NULL;
			name2 = NULL;
			name3 = NULL;
			payload = NULL;
			valid = 0;
		}

	};
	typedef mlHashEntry* mlHashEntryPtr;

	mlHashEntry *table;
	int size;
	int nbElems;
	mlDict* dict;

	unsigned long ComputeQKey(
		   const wchar_t *prefix, const wchar_t *name,
		   const wchar_t *prefix2, const wchar_t *name2,
		   const wchar_t *prefix3, const wchar_t *name3);

	unsigned long ComputeKey(const wchar_t *name,
	          const wchar_t *name2, const wchar_t *name3);

public:
	mlHashTable(int size);
	~mlHashTable();

	int AddEntry(const xmlChar *name, void *userdata) {
		return (AddEntry3(name, NULL, NULL, userdata));
	}

	int AddEntry2(const xmlChar *name,
	        const xmlChar *name2, void *userdata) {
		return (AddEntry3(name, name2, NULL, userdata));
	}

	int AddEntry3(const xmlChar *name,
	         const xmlChar *name2, const xmlChar *name3, void *userdata);

	int Grow(int size);
	typedef void (*mlHashDeallocator)(void *payload, xmlChar *name);
	int RemoveEntry(const xmlChar *name, mlHashDeallocator f) {
		return(RemoveEntry3(name, NULL, NULL, f));
	}

	int RemoveEntry2(const xmlChar *name,
				const xmlChar *name2, mlHashDeallocator f) {
		return RemoveEntry3(name, name2, NULL, f);
	}
	int RemoveEntry3(const xmlChar *name,
		const xmlChar *name2, const xmlChar *name3, mlHashDeallocator f);

	void* Lookup(const xmlChar *name) {
		return(Lookup3(name, NULL, NULL));
	}

	void* Lookup2(const xmlChar *name, const xmlChar *name2) {
		return(Lookup3(name, name2, NULL));
	}

	void* Lookup3(const xmlChar *name, 
	       const xmlChar *name2, const xmlChar *name3);

};

}