// Recognize.cpp: implementation of the CRecognize class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Recognize.h"
#include <math.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define M_PI 3.1415926535

CRecognize::CRecognize()
{
	index_time=0;

}

CRecognize::~CRecognize()
{

}

/* Процедура БПФ */
void CRecognize::BPF(float *x, float *y, int N)
{

 register float  c,s,t1,t2,t3,t4,u1,u2,u3;
 register int	 i,j,p,l,L,M,M1,K,I = 0;

 L = N;
 M = N/2;
 M1 = N - 1;

 while (L >= 2)
 {
	l = L/2;
	u1 = 1.;
	u2 = 0.;
	t1 = (float)M_PI/(float)l;

	c = cos(t1);
	s = (-1)*I*sin(t1);

	for (j = 0; j < l; j++)
	{

	 for (i = j; i < N; i += L)
	 {
		p = i + l;
		t1 = *(x+i) + *(x+p);
		t2 = *(y+i) + *(y+p);
		t3 = *(x+i) - *(x+p);
		t4 = *(y+i) - *(y+p);
		*(x+p) = t3*u1 - t4*u2;
		*(y+p) = t4*u1 + t3*u2;
		*(x+i) = t1;
		*(y+i) = t2;
	 }

	u3 = u1*c - u2*s;
	u2 = u2*c + u1*s;
	u1 = u3;
       }

       L /= 2;

 } // "while ..." end

 j = 0;

 for (i = 0; i < M1; i++)
 {

  if (i > j)
  {
	t1 = *(x+j);
	t2 = *(y+j);

	*(x+j) = *(x+i);
	*(y+j) = *(y+i);

	*(x+i) = t1;
	*(y+i) = t2;
  }

  K = M;

  while (j >= K)
  {
	j -= K;
	K /= 2;
  }

  j += K;

 } // "for ..." end

}

/*  ********************************************************
              F I R      F I L T E R 
 ***************************************************************
  >--(Xn) >--- T ---(Xnm1)>--- T ---(Xnm2)>--- T ---(Xnm3)
       |              |               |               |
       H0             H1              H2              H3
       |              |               |               |
       --------------(+)-------------(+)-------------(+)--->(Yn) 
 ************************************************************** */

/* compute coefficients */
void CRecognize::FIR_coef(float Wc, int NoT)
{
int N,n;
float Hleft, Hright;
float SIG = 1;
float High_fc, Low_fc;

High_fc  = (float)(((PI - Wc) * SAMPLE_RATE)/(2*PI));
Low_fc   = (float)((Wc * SAMPLE_RATE)/(2*PI));

N = 2 * NoT + 1;

ATLTRACE(_T("# of coefficient h{i}(i=-n..0..+n); n = %3d\n"),NoT);
ATLTRACE(_T("                     Total TAP number = %3d\n"),N);
ATLTRACE(_T("Sampling freq:        fs = %d[Hz]\n"),SAMPLE_RATE);
ATLTRACE(_T("Cut off freq:        fc = Wc*fs/2pi\n"));
ATLTRACE(_T("fc(High) = %5.2f KHz (derived)    fc (Low) = %5.2f KHz\n"), High_fc/1000,  Low_fc/1000);
ATLTRACE(_T("(normalized by 2 pi) Wc  = 2*pi*fc/fs  (0 <  Wc < 2 pi)\n Wc(High) = %f * PI [rad/sec] \n Wc(Low)  = %f * PI [rad/sec]\n"), 2*High_fc/SAMPLE_RATE,2*Low_fc/SAMPLE_RATE);

SIG = 1;
for (n = 0; n <= NoT; n++) {
   Hleft = (float)(1.0/(n * PI) * sin( n * Wc));
   Hright = SIG * Hleft;
   if (n == 0) {
	     ATLTRACE(_T("coefficient of h%2d/h-%2d    = %8.5f,        %8.5f\n"),         n, n,2 * Low_fc/SAMPLE_RATE, 1 * (2 * Low_fc/SAMPLE_RATE));
         HL[NoT+1]=HR[NoT+1]=(float)(2.0 * Low_fc/SAMPLE_RATE);
   } else {
      ATLTRACE(_T("coefficient of h%2d/h-%2d    = %8.5f,        %8.5f\n"),                        n,    n,       Hleft,          Hright);
      HR[NoT+1+n] = HR[NoT+1-n] = Hright;
      HL[NoT+1+n] = HL[NoT+1-n] = Hleft;
   }
   SIG = -1 * SIG;
   }

return;
}

// float ampMultiplier
int CRecognize::DoFilter(DWORD aDataSize, BYTE *aData, DWORD position, float limit, float ampMultiplier)
{
  DWORD i;

 DWORD tmp=0,distance=0;
 char pr[2];
 bool minus=FALSE;
 index_time=0;

// Отладка . Запись в raw файл
  static int file_num = 0;
	
//  ATLTRACE("Position=%d DataSize=%d",position,aDataSize);
  	/*for(i=position; i < aDataSize; i+=2) 
	{  
		pr[0]=aData[i]; pr[1]=aData[i+1]; 
		//memcpy((void *)(&tmp), pr,2);
		tmp = *((WORD*)pr)
		minus=FALSE;
		if (tmp & 0x8000)
		{ 
			tmp^=0xFFFF;
			tmp++;
			minus=TRUE;
		}
		time[index_time].value=tmp;
		time[index_time].sign=minus;
		index_time++;
	}*/

	for(i=position; i < aDataSize; i+=2) 
	{  
		pr[0]=aData[i]; pr[1]=aData[i+1]; 
		//memcpy((void *)(&tmp), pr,2);
		tmp = *((WORD*)pr);
		minus=FALSE;
		if (tmp & 0x8000)
		{ 
			tmp^=0xFFFF;
			tmp++;
			minus=TRUE;
		}
		tmp = (DWORD)(tmp * ampMultiplier);
		if( tmp > 0x7FFFFFF)
			tmp = 0x7FFFFFF;
		time[index_time].value=tmp;
		time[index_time].sign=minus;
		index_time++;
	}

	file_num++;
	
	WORD interval=50;
	distance=(WORD)(index_time/interval);
	if (distance > 0)
	{
	// Поиск пауз и лексем
	double prom; 
	int num_pausa=0;
	int num_lexema=0;
	DWORD temp;
	for (DWORD j=0; j<=distance; j++)
	{ 
		DWORD s=j*interval;
		temp=0;
		if (s+interval<=(DWORD)index_time) 
		{ 
			for (DWORD  k=s; k<s+interval; k++)	
				temp+=time[k].value;
			temp/=interval;
			prom=(temp*100.0)/32767.0;
  			if (prom < limit)
				num_pausa++;
			else
				num_lexema++;
		}
	}
 
	static int count=0;
	if (num_pausa>num_lexema)
	{
		#ifdef DEBUG_INFO
		ATLTRACE(_T("Silence\n"));
		#endif
		return -1;
	}
	else 
	{ 
		#ifdef DEBUG_INFO
		ATLTRACE(_T("Lexema = %d"), count);
		#endif
		int d=rand();
		int s=d%4;
		#ifdef DEBUG_INFO
		ATLTRACE("Count = %d\n",s);
		#endif

		if (count<3)
			count++;
		else
			count=0;
		return s; 
	}
   }
   return -1;
}

/* write a "standard" sun header with an unspecified length */
#define writeLong(fp, ul) putc((ul >> 24) & 0xff, fp); \
    putc((ul >> 16) & 0xff, fp); putc((ul >> 8) & 0xff, fp); \
    putc(ul & 0xff, fp);

void CRecognize::write_header()
{
    writeLong(fp, AUDIO_FILE_MAGIC);              /* Sun magic number*/
    writeLong(fp, 24);                            /* header size in bytes */
    writeLong(fp, ((unsigned)(~0)));              /* unspecified data size */
    writeLong(fp, AUDIO_FILE_ENCODING_LINEAR_16); /* data encoding format */
    writeLong(fp, SAMPLE_RATE);                   /* samples  per second */
    writeLong(fp, CHANNEL_NUMBER);                /* single channel */

}



float CRecognize::FIR(BYTE *aData, int Num)
{
  /* Multiply and add */
  float Yn = HL[0]*aData[Num]; 
  for (int i = 1; i <= FIR_TAPS; i++) 
  { Yn = Yn + HL[i]*aData[Num-i];
  }

  return(Yn); 
}



void CRecognize::fft(float *x,float *y,int order,int param)
{
 unsigned int n,l,e,f,i,j,o,o1,j1,i1,k;

 float u,v,z,c,s,p,q,r,t,w,a;


 n=1u<<order;


 for(l=1;l<=(unsigned int)order;l++)
 {

 u=1.0;
 v=0.0;
 e=1u<<(order-l+1);
 f=e/2;

 z=(float)(M_PI/f);

 c=cos(z);
 s=sin(z);

 if(param==FFT) s=-s;

 for(j=1;j<=f;j++)
  {
  for(i=j;i<=n;i+=e)
   {
   o=i+f-1;
   o1=i-1;
   p=x[o1]+x[o];
   r=x[o1]-x[o];
   q=y[o1]+y[o];
   t=y[o1]-y[o];
   x[o]=r*u-t*v;
   y[o]=t*u+r*v;
   x[o1]=p;
   y[o1]=q;
   }
  w=u*c-v*s;
  v=v*c+u*s;
  u=w;
  }
 }


 j=1;

 for(i=1;i<n;i++)
  {
  if(i<j)
   {
   j1=j-1;
   i1=i-1;
   p=x[j1];
   q=y[j1];
   x[j1]=x[i1];
   y[j1]=y[i1];
   x[i1]=p;
   y[i1]=q;
   }

  k=n/2;

  while(k<j)
   {
   j=j-k;
   k=k/2;
   }

  j+=k;
  }

 if(param==FFT) return;

 a=(float)(1.0/n);
 for(k=0;k<n;k++)
  {
  x[k]*=a;
  y[k]*=a;
  }
  return;

}
