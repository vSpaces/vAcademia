#if !defined(AFX_TRANSLS_H__93462C7D_05B5_40EC_B07F_23CB65874903__INCLUDED_)
#define AFX_TRANSLS_H__93462C7D_05B5_40EC_B07F_23CB65874903__INCLUDED_


#define	VECTOR2MLPOSITION(v, p)	{(p).x=(double)(v).x;	(p).y=(double)(v).y;	(p).z=(double)(v).z;}
#define	MLPOSITION2VECTOR(v, p)	{(p).x=(float)(v).x;	(p).y=(float)(v).y;	(p).z=(float)(v).z;}

/*#define	QUAT2MLROTATION(right, r)	{			\
	CalVector	axis;	float	angle;		\
	right.extract( axis, angle);				\
	(r).x = axis.x;	(r).y = axis.y;	(r).z = axis.z;	\
	(r).a = (right).w*180/3.14f;}*/

#define	MLROTATION2QUAT(r, right)	{ right.x=(float)r.x;right.y=(float)r.y;right.z=(float)r.z;right.w=(float)r.a;}
#define	QUAT2MLROTATION(right, r)	{ r.x=(double)right.x;r.y=(double)right.y;r.z=(double)right.z;r.a=(double)right.w;}

#endif