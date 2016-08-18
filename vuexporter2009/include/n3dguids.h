// n3dguids.h: interface for the CRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IN3DGUIDS_H__D9939FA9_60F6_411C_8565_6CF8904939FD__INCLUDED_)
#define AFX_IN3DGUIDS_H__D9939FA9_60F6_411C_8565_6CF8904939FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace natura3d
{
	typedef struct _N3DGUID
	{   // size is 16
		DWORD	Data1;
		WORD	Data2;
		WORD	Data3;
		BYTE	Data4[8];

		BOOL	operator<(const _N3DGUID& cmp);
		friend BOOL operator<(const _N3DGUID& v, const _N3DGUID& u);
	} N3DGUID;

	#define N3DHDEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
		extern N3DGUID NATURA3D_API name;

	#define N3DHEXTDEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
		extern N3DGUID name;
	
	#define n3dIsEqualGUID(rguid1, rguid2) (!memcmp(rguid1, rguid2, sizeof(N3DGUID)))
	#define	IMPLEMENT_RELEASE				void	free_object(){	delete this;}
	#define	DECLARE_GETIBASE				virtual	ibase*	get_ibase() = 0;
	#define	N3DDECLARE_INTERFACE(x)			virtual	x*	get_##x() = 0;
	#define	SUPPORT_INTERFACE(x,y)			(y->query_interface(iid_##x, NULL))
	#define	GET_INTERFACE(x,y,z)			(y->query_interface(iid_##x, (void**)z))
	#define	INTERFACE_SUPPORTED(x,y)		(SUPPORT_INTERFACE(x,y))
	#define	MUST_PROVIDE_INTERFACE(x,y)		assert(SUPPORT_INTERFACE(x,y));

	#define	N3DGET_INTERFACE(x,y)			y->get_##x()
	#define	N3DIMPLEMENT_INTERFACE(x)		virtual	x*	get_##x(){ return (##x*)this;}

	#define	BEGIN_INTERFACE_IMPLEMENTATION			bool	query_interface(const N3DGUID& name, void** ppinterface){	\
																void* apinterface=NULL;	\
													do{
	#define	IMPLEMENT_INTERFACE(x)					if(n3dIsEqualGUID(&name, &iid_##x)){	apinterface = (##x*)this; break;}	\

	#define	END_INTERFACE_IMPLEMENTATION			}while(false);				\
													if(apinterface)	{			\
															if( ppinterface)	\
															{					\
																*ppinterface = apinterface;		\
															}									\
															return true;						\
													}											\
													return false;								\
													}
};

#endif // !defined(AFX_IN3DGUIDS_H__D9939FA9_60F6_411C_8565_6CF8904939FD__INCLUDED_)