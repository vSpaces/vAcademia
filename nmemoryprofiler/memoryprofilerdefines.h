
#pragma once

#include <Assert.h>

#ifdef	MEMORY_PROFILING
#	define	MP_NEW(type)									\
		mp_alloc<type>(MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);

#	define	MP_NEW_Z(type)									\
		mp_alloc<type>(MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__, -1, 0, true);

#	define	MP_GLOBAL_NEW(type)									\
		mp_alloc<type>(MP_DLL_ID_INTERNAL, __FILE__, __LINE__, NULL);

#	define	MP_SPECIAL_NEW(type, location)									\
		mp_alloc<type>(MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__, -1, location);	\

#	define	MP_NEW_MEMBER_P(var,type, param)	\
	var = new type(param);								 \
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_P(var, type, param)							\
		type* var = new type(param);							\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_V(var, type, param)						\
	{														\
		var = new type(param);									\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\
	}	\

#	define	MP_NEW_P2(var, type, param1, param2)			\
	type* var = new type(param1, param2);					\
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);	\

#	define	MP_NEW_V2(var, type, param1, param2)			\
	{														\
		var = new type(param1, param2);							\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\
	}	\

#	define	MP_NEW_P3(var, type, param1, param2, param3)	\
	type* var = new type(param1, param2, param3);			\
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_V3(var, type, param1, param2, param3)	\
	{														\
		var = new type(param1, param2, param3);					\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\
	}	\

#	define	MP_NEW_P4(var, type, param1, param2, param3, param4)\
	type* var = new type(param1, param2, param3, param4);	\
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_V4(var, type, param1, param2, param3, param4)\
	{												\
		var = new type(param1, param2, param3, param4);	\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\
	}	\

#	define	MP_NEW_P5(var, type, param1, param2, param3, param4, param5)\
	type* var = new type(param1, param2, param3, param4, param5);	\
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_V5(var, type, param1, param2, param3, param4, param5)\
	{																\
		var = new type(param1, param2, param3, param4, param5);	\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\
	}	\

#	define	MP_NEW_P6(var, type, param1, param2, param3, param4, param5, param6)\
	type* var = new type(param1, param2, param3, param4, param5, param6);	\
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_V6(var, type, param1, param2, param3, param4, param5, param6)\
	{																	\
		var = new type(param1, param2, param3, param4, param5, param6);	\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\
	}	\

#	define	MP_NEW_P7(var, type, param1, param2, param3, param4, param5, param6, param7)\
	type* var = new type(param1, param2, param3, param4, param5, param6, param7);	\
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_V7(var, type, param1, param2, param3, param4, param5, param6, param7)\
	{	\
		var = new type(param1, param2, param3, param4, param5, param6, param7);	\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\
	}	\

#	define	MP_NEW_P8(var, type, param1, param2, param3, param4, param5, param6, param7, param8)\
	type* var = new type(param1, param2, param3, param4, param5, param6, param7, param8);	\
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_V8(var, type, param1, param2, param3, param4, param5, param6, param7, param8)\
	{	\
		var = new type(param1, param2, param3, param4, param5, param6, param7, param8);	\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\
	}	\

#	define	MP_NEW_P9(var, type, param1, param2, param3, param4, param5, param6, param7, param8, param9)\
	type* var = new type(param1, param2, param3, param4, param5, param6, param7, param8, param9);	\
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_V9(var, type, param1, param2, param3, param4, param5, param6, param7, param8, param9)\
	{	\
		var = new type(param1, param2, param3, param4, param5, param6, param7, param8, param9);	\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\
	}	\

#	define	MP_NEW_P10(var, type, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10)\
	type* var = new type(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);	\
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_P16(var, type, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16)\
	type* var = new type(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16);	\
	mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\

#	define	MP_NEW_V10(var, type, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10)\
	{	\
		var = new type(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);	\
		mp_manage_var<type>(MP_DLL_ID_INTERNAL, var, __FILE__, __LINE__, __FUNCTION__);			\
	}	\

#	define	MP_NEW_ARR(type, _size)							\
		mp_alloc<type>(MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__, _size);			\

#	define MP_NEW_Z_ARR(type, _size)							\
		mp_alloc<type>(MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__, _size, 0, true);			\

#	define	MP_REALLOC_ARR(type, _ptr, _size)							\
		mp_realloc<type>(MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__, _size, 0, _ptr);			\

#	define	MP_FAST_NEW_ARR(type, _size)							\
		mp_fast_alloc_arr<type>(MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__, _size);			\

#	define	MP_SPECIAL_NEW_ARR(type, _size, location)							\
		mp_alloc<type>(MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__, _size, location);			\

#	define	MP_DELETE_THIS				{assert(gMP); if (gMP) gMP->DeleteInfo(MP_DLL_ID_INTERNAL, this, __FUNCTION__, MEMORY_TYPE_NOT_ARRAY, sizeof(*this));delete this;}
														
#	define	MP_DELETE(ptr)				{assert(gMP); if (gMP) gMP->DeleteInfo(MP_DLL_ID_INTERNAL, ptr, __FUNCTION__, MEMORY_TYPE_NOT_ARRAY, sizeof(*ptr)); delete ptr; ptr = NULL;}

#	define	MP_DELETE_UNSAFE(ptr)		{assert(gMP); if (gMP) gMP->DeleteInfo(MP_DLL_ID_INTERNAL, ptr, __FUNCTION__, MEMORY_TYPE_NOT_ARRAY, sizeof(*ptr)); delete ptr;}
														
#	define	MP_DELETE_ARR(ptr)								\
	{	assert(gMP); \
		if (gMP) gMP->DeleteInfo(MP_DLL_ID_INTERNAL, ptr, __FUNCTION__, MEMORY_TYPE_ARRAY, 0);						\
		delete[] ptr;										\
		ptr = NULL;											\
	}	\

#	define	MP_FAST_DELETE_ARR(ptr)								\
	{	\
		if (gMP) gMP->DeleteInfo(MP_DLL_ID_INTERNAL, ptr, __FUNCTION__, MEMORY_TYPE_FAST_MALLOC, sizeof(*ptr));						\
		if ((!gBMC) || (!gBMC->FreeMemory(ptr))) delete[] ptr;									\
		ptr = NULL;											\
	}	\

#	define	MP_DELETE_ARR_UNSAFE(ptr)								\
	{	assert(gMP); \
		if (gMP) gMP->DeleteInfo(MP_DLL_ID_INTERNAL, ptr, __FUNCTION__, MEMORY_TYPE_ARRAY, 0);	\
		delete[] ptr;										\
	}	\

#	define MP_MAP				CMapMP
#	define MP_MAP_DC			CMapMP_DC
#	define MP_STACK				CStackMP
#	define MP_STACK_DC			CStackMP_DC
#	define MP_VECTOR			CVectorMP
#	define MP_VECTOR_DC			CVectorMP_DC
#	define MP_LIST				CListMP
#	define MP_LIST_DC			CListMP_DC
#	define MP_STRING			CStringMP
#	define MP_STRING_DC			CStringMP_DC
#	define MP_WSTRING			CWideStringMP
#	define MP_WSTRING_DC		CWideStringMP_DC
#	define MP_MAP_INIT(m)		m(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_MAP_DC_INIT(m)	m.Init(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);
#	define MP_MAP_INIT_THIS		Init(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);
#	define MP_VECTOR_INIT(v)	v(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_STACK_INIT(s)		s(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_LIST_INIT(l)		l(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_STACK_DC_INIT(s)	s.Init(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);
#	define MP_STACK_INIT_THIS	Init(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);
#	define MP_VECTOR_DC_INIT(v)	v.Init(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);
#	define MP_VECTOR_INIT_THIS	Init(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);
#	define MP_LIST_DC_INIT(l)	l.Init(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);
#	define MP_LIST_INIT_THIS	Init(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);
#	define MP_STRING_INIT(s)	s(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_STRING_DC_INIT(s)	s.Init(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);
#	define MP_WSTRING_INIT(ws)	ws(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_WSTRING_DC_INIT(s)	s.Init(gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__);
#	define MAKE_MP_STRING(s)	CStringMP(s, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MAKE_MP_WSTRING(s)	CWideStringMP(s, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MAKE_MP_VECTOR(v, T)	CVectorMP<T>(v, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MAKE_MP_LIST(l, T)	CListMP<T>(l, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MAKE_MP_MAP(m, T)	CMapMP<T>(m, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MAKE_MP_STACK(s, T)	CStackMP<T>(s, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_STRING(s)			MP_NEW_P5(s, MP_STRING, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_WSTRING(ws)		MP_NEW_P5(ws, MP_WSTRING, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_MAP(m, t)			MP_NEW_P5(m, t, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_VECTOR(v, t)		MP_NEW_P5(v, t, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_STACK(s, t)		MP_NEW_P5(s, t, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_LIST(l, t)		MP_NEW_P5(l, t, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_STRING_V(s)		MP_NEW_V5(s, MP_STRING, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_WSTRING_V(ws)		MP_NEW_V5(ws, MP_WSTRING, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_MAP_V(m, t)		MP_NEW_V5(m, t, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_VECTOR_V(v, t)	MP_NEW_V5(v, t, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_STACK_V(s, t)		MP_NEW_V5(s, t, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)
#	define MP_NEW_LIST_V(l, t)		MP_NEW_V5(l, t, gMP, MP_DLL_ID_INTERNAL, __FILE__, __LINE__, __FUNCTION__)

#else
#	define	MP_NEW(type) new type
#	define	MP_NEW_Z(type) (type*)memset(new type, 0, sizeof(type))
#	define	MP_GLOBAL_NEW(type) new type
#	define	MP_SPECIAL_NEW(type, location) new type
#	define	MP_NEW_MEMBER_P(var,type, param)	var = new type(param)
#	define	MP_NEW_P(var,type, param)	type* var = new type(param)
#	define	MP_NEW_P2(var,type, param1, param2)	type* var = new type(param1, param2)
#	define	MP_NEW_P3(var,type, param1, param2, param3)	type* var = new type(param1, param2, param3)
#	define	MP_NEW_P4(var,type, param1, param2, param3, param4)	type* var = new type(param1, param2, param3, param4)
#	define	MP_NEW_P5(var,type, param1, param2, param3, param4, param5)	type* var = new type(param1, param2, param3, param4, param5)
#	define	MP_NEW_P6(var,type, param1, param2, param3, param4, param5, param6)	type* var = new type(param1, param2, param3, param4, param5, param6)
#	define	MP_NEW_P7(var,type, param1, param2, param3, param4, param5, param6, param7)	type* var = new type(param1, param2, param3, param4, param5, param6, param7)
#	define	MP_NEW_P8(var,type, param1, param2, param3, param4, param5, param6, param7, param8)	type* var = new type(param1, param2, param3, param4, param5, param6, param7, param8)
#	define	MP_NEW_P9(var,type, param1, param2, param3, param4, param5, param6, param7, param8, param9)	type* var = new type(param1, param2, param3, param4, param5, param6, param7, param8, param9)
#	define	MP_NEW_P10(var,type, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10)	type* var = new type(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10)
#	define	MP_NEW_P16(var,type, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16)	type* var = new type(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16)
#	define	MP_NEW_V(var,type, param)	var = new type(param)
#	define	MP_NEW_V2(var,type, param1, param2)	var = new type(param1, param2)
#	define	MP_NEW_V3(var,type, param1, param2, param3)	var = new type(param1, param2, param3)
#	define	MP_NEW_V4(var,type, param1, param2, param3, param4)	var = new type(param1, param2, param3, param4)
#	define	MP_NEW_V5(var,type, param1, param2, param3, param4, param5)	var = new type(param1, param2, param3, param4, param5)
#	define	MP_NEW_V6(var,type, param1, param2, param3, param4, param5, param6)	var = new type(param1, param2, param3, param4, param5, param6)
#	define	MP_NEW_V7(var,type, param1, param2, param3, param4, param5, param6, param7)	var = new type(param1, param2, param3, param4, param5, param6, param7)
#	define	MP_NEW_V8(var,type, param1, param2, param3, param4, param5, param6, param7, param8)	var = new type(param1, param2, param3, param4, param5, param6, param7, param8)
#	define	MP_NEW_V9(var,type, param1, param2, param3, param4, param5, param6, param7, param8, param9)	var = new type(param1, param2, param3, param4, param5, param6, param7, param8, param9)
#	define	MP_NEW_V10(var,type, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10)	var = new type(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10)
#	define	MP_NEW_ARR(type, _size)	new type [_size]
#	define	MP_NEW_Z_ARR(type, _size)	zalloc<type>(_size)
#	define	MP_REALLOC_ARR(type, _ptr, _size)	_realloc<type>(_size, _ptr)
#	define	MP_FAST_NEW_ARR(type, _size)	fast_alloc_arr<type>(_size)
#	define	MP_SPECIAL_NEW_ARR(type, _size, location)	new type [_size]
#	define	MP_DELETE(ptr)	{ delete ptr; ptr = NULL; }
#	define	MP_DELETE_UNSAFE(ptr)	delete ptr;
#	define	MP_DELETE_ARR(ptr)	{ delete[] ptr; ptr = NULL; }
#	define	MP_FAST_DELETE_ARR(ptr)	if ((!gBMC) || (!gBMC->FreeMemory(ptr))) { delete[] ptr; ptr = NULL; }
#	define	MP_DELETE_ARR_UNSAFE(ptr)	delete[] ptr;
#	define	MP_DELETE_THIS				delete this;
#	define  MP_MAP				std::map
#	define  MP_MAP_DC			std::map
#	define  MP_VECTOR			std::vector
#	define	MP_VECTOR_DC		std::vector
#	define  MP_STACK			std::stack
#	define	MP_STACK_DC			std::stack
#	define  MP_LIST				std::list
#	define	MP_LIST_DC			std::list
#	define  MP_STRING			std::string
#	define  MP_STRING_DC		std::string
#	define  MP_WSTRING			std::wstring
#	define  MP_WSTRING_DC		std::wstring
#	define  MP_MAP_INIT(m)		m()
#	define	MP_MAP_DC_INIT(v)  /**/
#	define	MP_MAP_INIT_THIS  /**/
#	define  MP_VECTOR_INIT(v)	v()
#	define	MP_VECTOR_DC_INIT(v)  /**/
#	define	MP_VECTOR_INIT_THIS  /**/
#	define  MP_STACK_INIT(s)	s()
#	define	MP_STACK_DC_INIT(s)  /**/
#	define	MP_STACK_INIT_THIS  /**/
#	define  MP_LIST_INIT(l)		l()
#	define	MP_LIST_DC_INIT(l)  /**/
#	define	MP_LIST_INIT_THIS  /**/
#	define  MP_STRING_INIT(s)	s("")
#	define	MP_STRING_DC_INIT(v)  /**/
#	define  MP_WSTRING_INIT(ws)	ws(L"")
#	define	MP_WSTRING_DC_INIT(v)  /**/
#	define MAKE_MP_STRING(s)	s
#	define MAKE_MP_WSTRING(w)	w
#	define MAKE_MP_VECTOR(v, T)	v
#	define MAKE_MP_LIST(l, T)	l
#	define MAKE_MP_MAP(m, T)	m
#	define MAKE_MP_STACK(s, T)	s
#	define MP_NEW_STRING(s)			MP_STRING* s = MP_NEW(MP_STRING);
#	define MP_NEW_WSTRING(ws)		MP_WSTRING* ws = MP_NEW(MP_WSTRING);
#	define MP_NEW_MAP(m, t)			t* m = MP_NEW(t);
#	define MP_NEW_VECTOR(v, t)		t* v = MP_NEW(t);
#	define MP_NEW_LIST(l, t)		t* l = MP_NEW(t);
#	define MP_NEW_STACK(s, t)		t* s = MP_NEW(t);
#	define MP_NEW_STRING_V(s, t)	s = MP_NEW(MP_STRING);
#	define MP_NEW_WSTRING_V(ws, t)	ws = MP_NEW(MP_WSTRING);
#	define MP_NEW_MAP_V(m, t)		m = MP_NEW(t);
#	define MP_NEW_VECTOR_V(v, t)	v = MP_NEW(t);
#	define MP_NEW_LIST_V(l, t)		l = MP_NEW(t);
#	define MP_NEW_STACK_V(s, t)		s = MP_NEW(t);
#endif