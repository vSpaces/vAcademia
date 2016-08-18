#pragma once

#include <string>

namespace rmml {

#undef xmlRealloc
#undef xmlMallocAtomic

class mlDict {
	struct mlDictEntry {
		mlDictEntry *next;
		const wchar_t *name;
		int len;
		int valid;
		mlDictEntry(){
			next = NULL;
			name = NULL;
			len = 0;
			valid = 0;
		}
	};
	typedef mlDictEntry *mlDictEntryPtr;

	struct mlDictStrings {
		mlDictStrings* next;
		wchar_t *free;
		wchar_t *end;
		int size;
		int nbStrings;
		wchar_t array[1];
	};
	typedef mlDictStrings *mlDictStringsPtr;

    int ref_counter;
//    mlRMutexPtr mutex;

    mlDictEntry *dict;
    int size;
    int nbElems;
    mlDictStringsPtr strings;

    mlDict *subdict;

public:
	mlDict();
	~mlDict();
	int AddRef();
	void Release();
	const wchar_t* AddString(const wchar_t *name, int namelen);
	const wchar_t* AddQString(const wchar_t *prefix,
                 const wchar_t *name, int namelen);
	int Grow(int size);
	const wchar_t* Lookup(const wchar_t *name, int len);
	const wchar_t* Exists(const wchar_t *name, int len);
	const wchar_t* QLookup(const wchar_t *prefix, const wchar_t *name);
	int DictSize();
	int Owns(const wchar_t *str);
};

inline wchar_t* Strndup(const wchar_t *cur, int len) {
    wchar_t *ret;
    
    if ((cur == NULL) || (len < 0)) return(NULL);
	ret = MP_NEW_ARR(wchar_t, len + 1);
	wcsncpy_s(ret, len + 1, cur, len);
    ret[len] = 0;
    return(ret);
}

inline wchar_t* Strdup(const wchar_t *cur) {
    wchar_t *ret;

    if ((cur == NULL)) return(NULL);
	int len = wcslen(cur);
	ret = MP_NEW_ARR(wchar_t, len + 1);
	wcsncpy_s(ret, len + 1, cur, len);
    ret[len] = 0;
    return(ret);
}

}

