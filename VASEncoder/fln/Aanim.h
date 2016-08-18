// AAnim.h: interface for the CAAnim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AANIM_H__B982152E_4026_11D1_A1B4_C3AC308F154C__INCLUDED_)
#define AFX_AANIM_H__B982152E_4026_11D1_A1B4_C3AC308F154C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <mmsystem.h>
#include "gmem.h"

#define ALIGNL(i) ((i+3)&(~3)) // ULONG aligned!

#define FORMAT565
//#define FORMAT555

// fli file header, have to use byte structure packing
#pragma pack(1)

enum fli_source { FLI_RAM, FLI_DISK, FLI_AUTO };
// FLI_RAM forces load_fli to load the entire fli to memory and play it from ram
// FLI_DISK forces play from disk, (uses from 128 bytes up to just over 64Kb of ram)
// FLI_AUTO first tries to load file into memory then tries the disk option

typedef struct 
	{
	unsigned long size;
	unsigned short int magic;
	unsigned short int frames;
	unsigned short int width;
	unsigned short int height;
	unsigned short int depth;
	unsigned short int flags;
	unsigned short int speed;
	unsigned long    next;
	unsigned long    frit;
	char    reserved[102];
} fli_header;


// flc file header
typedef struct 
	{
	unsigned long size;
	unsigned short int magic;
	unsigned short int frames;
	unsigned short int width;
	unsigned short int height;
	unsigned short int depth;
	unsigned short int flags;
	unsigned long speed;
	char    reserved0[2];
	long    created;
	char    creator[4];
	long    updated;
	char    updater[4];
	short int     aspectx;
	short int     aspecty;
	char    reserved1[38];
	long    oframe1;
	long    oframe2;
	char    reserved[40];
	}flc_header;

typedef struct  
	{
	unsigned short int     magic;
	unsigned short int     frames;
	unsigned short int     width;
	unsigned short int     height;
	unsigned long		   speed;
	unsigned short int     current;
	unsigned long          crntoff;
	CFile*			       pFile;
	unsigned short int     depth;
	unsigned short int     bits;
	flc_header			   header;
	//PALETTEENTRY Colors[256]; //PALETTE 
	RGBQUAD Colors[256];
	}fli;
/*	description of a fli structure
	Any of these variables can be used directly, by fliptr->variable
	where fliptr is the pointer returned by the load_fli function.
	To go to the first frame execute the following code:
		fliptr->current=0;
		fliptr->crntoff=(long)sizeof(fli);
	(this only works if all of the fli was loaded into the ram)

	The data can only be used if the fli was loaded with the FLI_RAM 
    or possibly	the FLI_AUTO option.

	Handle is equal to 0 if all of the fli was loaded into the ram.

byte offset     size    name    meaning
0               2       frames  Number of frames in fly, a maximum of 4000
2               2       width   Screen width (320)
4               2       height  Screen height (200)
6               2       speed   Number of video ticks between frames
8               2       current Current frame (=0 initilly)
10              4       crntoff offset from begining to current frame (=14 initially)
14              2       handle  handle of the file if playing from disk
16              ?       data    data for the frames (only if handle>1)

*/

typedef struct  
	{
	unsigned long   size;
	unsigned short int magic;
	unsigned short int     chunks;
	char    expand[8];
	}frame_header;

typedef struct  
	{
	unsigned long size;
	unsigned short int     type;
	}chunk_header;

typedef struct 
	{
	unsigned long size;	// size of block incuding size of this header
	unsigned short int x;
	unsigned short int y;
	unsigned short int width;
	unsigned short int height;
} fln_block_header;

#define fln_magic       0xAF89
#define zip_magic       0xAF88
#define dta_magic       0xAF44
#define fli_magic       0xAF11
#define flc_magic       0xaf12
#define prefix_id       0xf100
#define frame_magic     0xF1Fa

#define FLI_COLOR256    4
#define FLI_SS2         7
#define FLI_COLOR       11
#define FLI_LC          12
#define FLI_BLACK       13
#define FLI_BRUN        15
#define FLI_COPY        16
#define FLI_PSTAMP      18
#define DTA_BRUN        25
#define DTA_COPY        26
#define DTA_LC          27
#define SSN_ZIP         31
#define FLN_ZIP         33
#define FLN_LZO         34
#define FLN_VID         35
#define FLN_TCC         36
#define LINE_SKIP		0xc000        
#pragma pack() //use the default packing method

class CAnim  
{
public:
	CAnim();
	CAnim(const char* filename,fli_source playtype=FLI_DISK,int direct=-1);
	CAnim(CFile* aFile,fli_source playtype=FLI_DISK,int direct=-1);
	
	virtual ~CAnim();
	
	BOOL	OpenAnim (const char* filename,fli_source playtype=FLI_DISK,int direct=-1);
	BOOL	OpenAnim(CFile* aFile,fli_source playtype,int dir);
	int		PlayFrame(char *dest,int lPitch = -1);  //распаковывает в Dest;
	void	CloseAnim();
	BOOL	Seek(int Nframe, char*dest,int lPitch = -1); //from 0 to Frames-1
	BOOL	GetPalette(RGBQUAD*pal);//указатель на палитру
	LONG	GetSizeBuffer();		//размер дл€ буфера
	inline  fli*  GetFormat()   {return m_pFli;}
//atributs
protected:
	//–аспоковщики 
    void flicolor       (char *data);
	void flicolor256to64(char *data);
	void flibrun		(char *data, char *dest, short int xsize, short int ysize);
	void flilc			(char *data, char *dest, short int xsize, char *finaldata);
	void fliss2			(char *data, char *dest, short int xsize);
	void flicopy		(char *data, char *dest, short int xsize, short int ysize,short int bits); 
	void ssn_unzip		(char *data, char *dest, unsigned long* nZip, unsigned long* nUnzip);
	void ssn_zip		(char *data, char *dest, unsigned long* nZip, unsigned long* nUnzip,int level=6);
public:
	void dta_lc16		(char *data, char *dest, short int xsize);
	void dta_lc24		(char *data, char *dest, short int xsize);
	void dta_lc32		(char *data, char *dest, short int xsize, short int ysize);
	void dta_brun16		(char *data, char *dest, short int xsize, short int ysize);
	void dta_brun24		(char *data, char *dest, short int xsize, short int ysize);
	void dta_brun32		(char *data, char *dest, short int xsize, short int ysize);
	void flndecode		(char *data, char *dest, short int xsize, bool abAlpha);
	void flndecodevid	(char *data, char *dest, short int xsize, bool abAlpha,bool bKeyFrame);
	void flndecodetcc	(char *data, char *dest, short int xsize, bool abAlpha,bool bKeyFrame);
		int  m_nDirect;	// -1 дл€ перевернутого изображени€
					// +1 дл€ пр€мого изображени€
	fli* m_pFli;	// описатель анимации
	CList<CRect,CRect> rlChngBlcks;
};

#endif // !defined(AFX_AANIM_H__B982152E_4026_11D1_A1B4_C3AC308F154C__INCLUDED_)
