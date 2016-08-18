// AAnim.cpp: implementation of the CAAnim class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include "AAnim.h"
#include "zlibengn.h"
#include "lzoengn.h"
#include "macros.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ALIGNL(i) ((i+3)&(~3)) /* ULONG aligned ! */

//---------------------------------------------------------
CAnim::CAnim()
{
 m_pFli=NULL;
}

//---------------------------------------------------------
CAnim::CAnim(const char* filename,fli_source playtype,int direct)
{
 m_pFli=NULL;
 OpenAnim(filename,playtype,direct);
}

//---------------------------------------------------------
CAnim::CAnim(CFile* aFile,fli_source playtype,int direct)
{
 m_pFli=NULL;
 OpenAnim(aFile,playtype,direct);
}

//---------------------------------------------------------
CAnim::~CAnim()
{
 CloseAnim();
}

//---------------------------------------------------------
BOOL CAnim::OpenAnim(const char* filename,fli_source playtype,int dir) 
{
	return OpenAnim( new CFile( filename, CFile::modeRead), playtype, dir);
/*
    HMMIO handle;
    fli *ptr; 
    char *ptr1;
    fli_header header;
    flc_header *flc;
    DWORD size=0;
    CloseAnim();
    
    // load the 128 byte fli/flc header
    if((handle = mmioOpen((LPSTR)filename,NULL, MMIO_READ|MMIO_ALLOCBUF))==NULL) 
        return FALSE;
    mmioRead(handle,(char*)&header,sizeof(fli_header));
    flc=(flc_header *)&header;
    
    switch (header.magic){
    case fli_magic:
        break;
	case zip_magic:
    case dta_magic:
    case flc_magic:
        mmioSeek(handle,flc->oframe1,SEEK_SET);
        break;
    default       : 
        mmioClose(handle,0);
        return FALSE;
    }
    
    if(playtype!=FLI_DISK) {
        // try to allocate ram for the entire fli
		switch (header.magic){
		case fli_magic:
			size = header.size-sizeof(fli_header)+sizeof(fli);
			break;
		case zip_magic:
		case dta_magic:
		case flc_magic:
			size = header.size - flc->oframe1+sizeof(fli);
			break;
		}

        if((ptr=(fli *)GAllocPtr(size))==NULL){
            if (playtype==FLI_RAM){
                mmioClose(handle,0);
                return FALSE;
            }
            playtype=FLI_DISK;
        } else 
            playtype=FLI_RAM;
    }
    
    // try to allocate ram just for the fli descriptor
	if(playtype==FLI_DISK) {
        if((ptr=(fli *)GAllocPtr(sizeof(fli)))==NULL) {
            mmioClose(handle,0);
            return FALSE;
        }
	}

	if (header.magic==fli_magic) {
		ptr->frames=header.frames;
		ptr->width=header.width;
		ptr->height=header.height;
		ptr->speed=header.speed;
		if (ptr->speed<1)
			ptr->speed=1;
		ptr->depth=8;
		ptr->bits=1;
		ptr->speed=ptr->speed*100/7;
	} else {
		ptr->frames=flc->frames;
		ptr->width=flc->width;
		ptr->height=flc->height;
		ptr->speed=flc->speed;
		ptr->depth=flc->depth;
		ptr->bits=(flc->depth+1)/8;
		if (ptr->speed<1)
			ptr->speed=1;
	}
	m_nDirect=dir;
	ptr->current=0;
	ptr->crntoff=(long)sizeof(fli);
	ptr->magic = flc->magic;

	if (playtype==FLI_RAM)
		ptr->handle=0;
	else 
		ptr->handle=handle;
	
	if (playtype==FLI_RAM) {
		ptr1 = (char *)ptr + sizeof(fli);
		mmioRead(handle, ptr1, size - sizeof(fli));		
		mmioClose(handle,0);
	}
	
	// return the pointer to the fli
	m_pFli=ptr;
	memcpy(&m_pFli->header,&header,sizeof(fli_header));
	return TRUE;*/
}

//---------------------------------------------------------
BOOL CAnim::OpenAnim(CFile* aFile,fli_source playtype,int dir) {
    fli* ptr; 
    char* ptr1;
    fli_header header;
    flc_header *flc;
    DWORD size=0;
    CloseAnim();
    
    // load the 128 byte fli/flc header
    aFile->Read((char*)&header,sizeof(fli_header));
    flc=(flc_header *)&header;
    
    switch (header.magic){
    case fli_magic:
        break;
	case fln_magic:
	case zip_magic:
    case dta_magic:
    case flc_magic:
        aFile->Seek( flc->oframe1, CFile::begin);
        break;
    default       : 
		DELETEP( aFile);
        return FALSE;
    }
    
    if(playtype!=FLI_DISK) {
        // try to allocate ram for the entire fli
		switch (header.magic){
		case fli_magic:
			size = header.size-sizeof(fli_header)+sizeof(fli);
			break;
		case fln_magic:
		case zip_magic:
		case dta_magic:
		case flc_magic:
			size = header.size - flc->oframe1+sizeof(fli);
			break;
		}

        if((ptr=(fli *)GAllocPtr(size))==NULL){
            if (playtype==FLI_RAM){
				DELETEP( aFile);
                return FALSE;
            }
            playtype=FLI_DISK;
        } else 
            playtype=FLI_RAM;
    }
    
    // try to allocate ram just for the fli descriptor
	if(playtype==FLI_DISK) {
        if((ptr=(fli *)GAllocPtr(sizeof(fli)))==NULL) {
			DELETEP( aFile);
            return FALSE;
        }
	}

	if (header.magic==fli_magic) {
		ptr->frames=header.frames;
		ptr->width=header.width;
		ptr->height=header.height;
		ptr->speed=header.speed;
		if (ptr->speed<1)
			ptr->speed=1;
		ptr->depth=8;
		ptr->bits=1;
		ptr->speed=ptr->speed*100/7;
	} else {
		ptr->frames=flc->frames;
		ptr->width=flc->width;
		ptr->height=flc->height;
		ptr->speed=flc->speed;
		ptr->depth=flc->depth;
		ptr->bits=(flc->depth+1)/8;
		if (ptr->speed<1)
			ptr->speed=1;
	}
	m_nDirect=dir;
	ptr->current=0;
	ptr->crntoff=(long)sizeof(fli);
	ptr->magic = flc->magic;

	if (playtype==FLI_RAM)
		ptr->pFile = NULL;
	else 
		ptr->pFile = aFile;
	
	if (playtype==FLI_RAM) {
		ptr1 = (char *)ptr + sizeof(fli);
		TRY{
			aFile->Read(ptr1, size - sizeof(fli));		
		}CATCH( CFileException, e ){
//			if( e->m_cause == CFileException::fileNotFound )
//				printf( "ERROR: File not found\n");
		}
		END_CATCH
		DELETEP( aFile);
	}
	
	// return the pointer to the fli
	m_pFli=ptr;
	memcpy(&m_pFli->header,&header,sizeof(fli_header));
	return TRUE;
}

//---------------------------------------------------------
void CAnim::flicolor(char *data)
{
	short int i, j, k, change;
	//0 unsigned char current = 0;
	i = *data;
	data += 2;
	for (j=0; j<i; j++) {
		//0 	current += *data++;
		*data++;//0
		change = *data++;
		if (!change)
			change = 256;
		for (k = 0; k < change ; k++){
			m_pFli->Colors[k].rgbRed   = 4*(*data++);
			m_pFli->Colors[k].rgbGreen = 4*(*data++);
			m_pFli->Colors[k].rgbBlue  = 4*(*data++);
			m_pFli->Colors[k].rgbReserved = 0;
		}
	}
}

//---------------------------------------------------------
void CAnim::flicolor256to64(char *data)
{
	short int i, j, k, change;
	i = *data;
	data += 2;
	for (j = 0; j < i; j++) {
		data++;
		change = *data++;
		if (!change)
			change=256;
		for (k = 0; k < change ; k++){
			m_pFli->Colors[k].rgbRed   = *data++;
			m_pFli->Colors[k].rgbGreen = *data++;
			m_pFli->Colors[k].rgbBlue  = *data++;
			m_pFli->Colors[k].rgbReserved = 0;
		}
	}
}

//---------------------------------------------------------
void CAnim::flibrun(char *data, char *dest, short int xsize, short int ysize)
{
	short int i, j, k;
	char  dt, *dest1;
	BYTE ppl;
	signed char sc;
	for (i = 0; i < ysize; i++) {
		ppl   = *data++;
		dest1 = dest;
		for (j = 0; j < ppl; j++) {
			sc = *(data++);
			if (sc < 0)
				for(k = 0; k < -sc; k++)
					*(dest++) = *(data++);
				else {  
					dt = *(data++);
					for(k = 0; k < sc; k++)
						*(dest++) = dt;
				}
		}
		dest = dest1 + xsize * m_nDirect;
	}
}

//---------------------------------------------------------
void CAnim::dta_brun16(char *data, char *dest, short int xsize, short int ysize) 
{
	short int i, j, k;
	char  *dest1;
	short int  dt;
	signed char sc;
	BYTE ppl;
	for (i = 0; i < ysize; i++) {
		ppl = *((BYTE*)data++);
		dest1 = dest;
 		for (j = 0; j < ppl; j++) {
			sc = *data++;
			if (sc < 0){
				for(k = 0; k < -sc; k++){
					*((short int*)dest)=*((short int*)data);
					data+=2;
					dest+=2;
				}
			}
			else {  
				dt = *((short int*)data);
				data+=2;
				for(k = 0; k < sc; k++) {
					*((short int*)dest)=dt;
					dest+=2;
				}
			}
		}
		dest = dest1 + xsize * m_nDirect;
	}
}

//---------------------------------------------------------
void CAnim::dta_brun24(char *data, char *dest, short int xsize, short int ysize) {
	short int i, j, k;
	char  *dest1;
	signed char sc;
	BYTE ppl,r,g,b;
	for (i = 0; i < ysize; i++) {
		ppl = *((BYTE*)data++);
		dest1 = dest;
		for (j = 0; j < ppl; j++) {
			sc = *data++;
			if (sc < 0){
				for(k = 0; k < -sc; k++){
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
				}
			}
			else {  
				r = *(data++);
				g = *(data++);
				b = *(data++);
				for(k = 0; k < sc; k++) {
					*(dest++) = r;
					*(dest++) = g;
					*(dest++) = b;					
				}
			}
		}
		dest = dest1 + xsize * m_nDirect;
	}
}


void CAnim::dta_brun32(char *data, char *dest, short int xsize, short int ysize) {
	short int i, j, k;
	char  *dest1;
	signed short int sc;
	BYTE ppl,a,r,g,b;
	for (i = 0; i < ysize; i++) {
		ppl = *((BYTE*)data++);
		dest1 = dest;
		for (j = 0; j < ppl; j++) {
			sc = *(signed short int*)data;
			data += 2;
			if (sc < 0){
				for(k = 0; k < -sc; k++){
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
				}
			}
			else {  
				a = *(data++);
				r = *(data++);
				g = *(data++);
				b = *(data++);
				for(k = 0; k < sc; k++) {
					*(dest++) = a;
					*(dest++) = r;
					*(dest++) = g;
					*(dest++) = b;					
				}
			}
		}
		dest = dest1 + xsize * m_nDirect;
	}
}

//---------------------------------------------------------
void CAnim::flilc(char *data,char *dest,short int xsize,char *finaldata)
{
	short int j,k,l,m;
	char dt,*dest1;
	signed char sc;
	BYTE ppl;
	do {
		j = *(short int *)(data);//line number or line skip count
		data += 2;
		dest += j * xsize * m_nDirect; //new
		m = *((short int *)data);//number lines
		data += 2;
		for (l = 0; l < m; l++) {
			ppl = *data++; //packet count
			dest1 = dest;
			for (j = 0; j < ppl; j++) {
				dest += (unsigned char)*(data++);
				sc = *(data++);
				if (sc > 0) { 
					for(k = 0; k < sc; k++)
						*(dest++) = *(data++);
				}
				if (sc < 0) {  
					dt = *(data++);
					for(k = 0; k < -sc; k++)
						*(dest++) = dt;
				}
				if (data >= finaldata)
					return;
			}
			dest = dest1 + xsize * m_nDirect;
		}
	}while(data < finaldata);
}

//---------------------------------------------------------
void CAnim::fliss2(char *data,char *dest,short int xsize)
{
	short int  k, cl, l,p, ppl, ppl1, dt;
	char *dest1;	
	signed char sc;
	
	cl = *(short int *)data;
	data += 2;
	for (l = 0; l < cl; l++) {
		ppl = *(short int *)data;
		data += 2;
		while ((ppl & LINE_SKIP) == LINE_SKIP) {
			dest -= xsize * ppl * m_nDirect;
			ppl = *(short int *)data;
			data += 2;
		}
		dest1 = dest;
		ppl1 = ppl;
		if (ppl < 0) {
			ppl1 = *(short int *)data;
			data += 2;
		}
		for (p = 0; p < ppl1; p++) {
			dest += (unsigned char)*(data++);
			sc = *(data++);
			if (sc > 0) {
				for(k = 0; k < sc; k++) {
					*(short int *)dest =
						*(short int *)data;
					dest += 2;
					data += 2;
				}
			}
			if (sc < 0) {
				dt = *(short int *)data;
				data += 2;
				for(k = 0; k < -sc; k++) {
					*(short int *)dest = dt;
					dest += 2;
				}
			}
		}
		if (ppl < 0)
			*dest = (char) ppl;
		dest = dest1 + xsize * m_nDirect;
	}
}

//---------------------------------------------------------
void CAnim::dta_lc16(char *data,char *dest,short int xsize) {
	short int k, cl, l, p, ppl, ppl1;
	char *dest1;
	signed char sc;
    WORD dt;
	cl = *(short int *)data;
	data += 2;
	for (l = 0; l < cl; l++) {
		ppl = *(short int *)data;
		data += 2;
		while ((ppl & LINE_SKIP) == LINE_SKIP) {
			dest -= xsize * ppl * m_nDirect;
			ppl = *(short int *)data;
			data += 2;
		}
		dest1 = dest;
		ppl1 = ppl;
		if (ppl < 0) {
			ppl1 = *(short int *)data;
			data += 2;
		}
		for (p = 0; p < ppl1; p++) {
			dest += (unsigned char)*(data++)*2;
			sc = *(data++);
			if (sc > 0){
				for(k = 0; k < sc; k++) {
					*(short int *)dest =
						*(short int *)data;
					dest += 2;
					data += 2;
				}
			}
			if (sc < 0) {
				dt = *(WORD *)data;
				data += 2;
				for(k = 0; k < -sc; k++) {
					*(short int *)dest = dt;
					dest += 2;
				}
			}
		}
		if (ppl < 0)
			*dest = (char) ppl;
		dest = dest1 + xsize  * m_nDirect;
	}
}

//---------------------------------------------------------
void CAnim::dta_lc24(char *data,char *dest,short int xsize) {
	short int k, cl, l, p, ppl, ppl1;
	char *dest1;
	signed char sc;
	BYTE r,g,b;
	cl = *(short int *)data;
	data += 2;
	for (l = 0; l < cl; l++) {
		ppl = *(short int *)data;
		data += 2;
		while ((ppl & LINE_SKIP) == LINE_SKIP) {
			dest -= xsize * ppl * m_nDirect;
			ppl = *(short int *)data;
			data += 2;
		}
		dest1 = dest;
		ppl1 = ppl;
		if (ppl < 0) {
			ppl1 = *(short int *)data;
			data += 2;
		}
		for (p = 0; p < ppl1; p++) {
			dest += (unsigned char)*(data++)*3;
			sc = *(data++);
			if (sc > 0) {
				for(k = 0; k < sc; k++) {
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
				}
			}
			if (sc < 0) {
				r = *(data++);
				g = *(data++);
				b = *(data++);
				for(k = 0; k < -sc; k++) {
					*(dest++) = r;
					*(dest++) = g;
					*(dest++) = b;					
				}
			}
		}
		if (ppl < 0)
			*dest = (char )ppl;
		dest = dest1 + xsize * m_nDirect;
	}
}

void CAnim::dta_lc32(char *data,char *dest,short int xsize,short int ysize) {
	short int i, j, k;
	char  *dest1;
	signed short int sc;
	BYTE ppl,a,r,g,b;
	for (i = 0; i < ysize; i++) {
		ppl = *((BYTE*)data++);
		dest1 = dest;
		for (j = 0; j < ppl; j++) {
			sc = *(signed short int*)data;
			data += 2;
			if (sc < 0){
				for(k = 0; k < -sc; k++){
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
				}
			}
			else {  
				a = *(data++);
				r = *(data++);
				g = *(data++);
				b = *(data++);
				for(k = 0; k < sc; k++) {
					*(dest++) = a;
					*(dest++) = r;
					*(dest++) = g;
					*(dest++) = b;					
				}
			}
		}
		dest = dest1 + xsize * m_nDirect;
	}

/*	short int k, cl, l, p, ppl, ppl1;
	char *dest1;
	signed short int sc;
	BYTE a,r,g,b;
	cl = *(short int *)data;
	data += 2;
	for (l = 0; l < cl; l++) {
		ppl = *(short int *)data;
		data += 2;
		while ((ppl & LINE_SKIP) == LINE_SKIP) {
			dest -= xsize * ppl * m_nDirect;
			ppl = *(short int *)data;
			data += 2;
		}
		dest1 = dest;
		ppl1 = ppl;
		if (ppl < 0) {
			ppl1 = *(short int *)data;
			data += 2;
		}
		for (p = 0; p < ppl1; p++) {
			dest += (unsigned char)*(data++)*4;
			sc = *((signed short int*)data);
			data += 2;
			if (sc > 0) {
				for(k = 0; k < sc; k++) {
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
					*(dest++) =	*(data++);
				}
			}
			if (sc < 0) {
				a = *(data++);
				r = *(data++);
				g = *(data++);
				b = *(data++);
				for(k = 0; k < -sc; k++) {
					*(dest++) = a;
					*(dest++) = r;
					*(dest++) = g;
					*(dest++) = b;					
				}
			}
		}
		if (ppl < 0)
			*dest = (char )ppl;
		dest = dest1 + xsize * m_nDirect;
	} */
}

void CAnim::flicopy (char *data, char *dest, short int xsize, short int ysize,short int bits) 
{
	for (int  i = 0; i < ysize; i++) {
		memcpy(dest,data,xsize*bits);
		dest += xsize * bits * m_nDirect;
		data += xsize * bits ;
	}
}

void CAnim::ssn_unzip(char *data, char *dest,unsigned long* nZip, unsigned long *nUnzip)
{
	ZlibEngine* p_decoder;
	p_decoder = new ZlibEngine();
	
	p_decoder->decompmem(data,nZip,dest,nUnzip);
	delete p_decoder;
}

void CAnim::ssn_zip	(char *data, char *dest, unsigned long* nZip, unsigned long* nUnzip,int level)
{
	ZlibEngine* p_decoder;
	p_decoder = new ZlibEngine();
	
	p_decoder->compmem(data,nZip,dest,nUnzip,level);
	delete p_decoder;
}

//---------------------------------------------------------
int CAnim::PlayFrame(char *dest, int lPitch)
{
	frame_header *frame,frame1;
	chunk_header *chunk, *chunk1;
	short int     i;
	
	if (m_pFli==NULL)
		return 1;
	
	if (m_pFli->pFile == NULL) {
		// if playing from ram, setup pointers for the next frame
		frame=(frame_header *)((char *)m_pFli+m_pFli->crntoff);
		chunk=(chunk_header *)((char *)frame+sizeof(frame_header));
		m_pFli->current++;
	}
	else {
		// if playing from disk, read the next frame
		m_pFli->pFile->Read((char*)&frame1,sizeof(frame_header));
		frame=&frame1;
		if (frame1.chunks>0) {
			// if there are chunks to load, load em
			if((chunk=(chunk_header *)GAllocPtr(frame1.size-sizeof(frame_header)))==NULL)
                return 1;
            chunk1 = chunk;
			m_pFli->pFile->Read((char*)chunk,frame1.size-sizeof(frame_header));
		}
		m_pFli->current++;
		if (m_pFli->current==m_pFli->frames) {
			m_pFli->current=0;
			switch(m_pFli->magic){
			case fli_magic:
			if (m_pFli->pFile->Seek(sizeof(fli_header), CFile::begin)==-1)
				return FALSE;
				break;
			case fln_magic:
			case zip_magic:
			case dta_magic:
			case flc_magic:
				if (m_pFli->pFile->Seek(m_pFli->header.oframe1, CFile::begin)==-1)
					return FALSE;
				break;
			}
		}
	}
	
	if (frame->magic == frame_magic)
		for (i=0; i<frame->chunks;i++) {
			switch(chunk->type) {
			case FLN_ZIP:
				{
					char* pcCompData=(char *)chunk+sizeof(chunk_header);
					unsigned long ulUncompSize;
					ulUncompSize=*((unsigned long*)pcCompData);
					char *pcData=new char[ulUncompSize+5];
					pcCompData+=sizeof(unsigned long);
					unsigned long ulCompSize;
					ulCompSize=chunk->size - sizeof(chunk_header) - sizeof (unsigned long);
					ZlibEngine zLibEng;
					zLibEng.decompmem(pcCompData,&ulCompSize,pcData,&ulUncompSize);
					flndecode(pcData,dest,lPitch==-1 ? ALIGNL(m_pFli->width*m_pFli->bits):lPitch,m_pFli->bits == 4);
					delete pcData;
				}
				break;
			case FLN_LZO:
				{
					char* pcCompData=(char *)chunk+sizeof(chunk_header);
					unsigned long ulUncompSize;
					ulUncompSize=*((unsigned long*)pcCompData);
					char *pcData=new char[ulUncompSize+5];
					pcCompData+=sizeof(unsigned long);
					unsigned long ulCompSize;
					ulCompSize=chunk->size - sizeof(chunk_header) - sizeof (unsigned long);
					LZOEngine LZOEng;
					VERIFY(LZOEng.decompmem((unsigned char*)pcCompData,&ulCompSize,(unsigned char*)pcData,&ulUncompSize));
					flndecode(pcData,dest,lPitch==-1 ? ALIGNL(m_pFli->width*m_pFli->bits):lPitch,m_pFli->bits == 4);
					delete pcData;
				}
				break;
			case SSN_ZIP:
				{
					chunk_header locchunk;
					char* ptr1 = (char *)chunk+sizeof(chunk_header)+sizeof(DWORD);
					char* ptr3 = (char *)chunk+sizeof(chunk_header);
					DWORD zip = chunk->size - sizeof(chunk_header) - sizeof (DWORD);
					DWORD unzip = *(DWORD*)ptr3;
				    char* ptr2 = (char *)GAllocPtr(unzip+1000);
					ssn_unzip(ptr1,ptr2,&zip,&unzip);
					memcpy(&locchunk,ptr2,sizeof(chunk_header));
					switch(locchunk.type)
					{
					case FLI_COLOR256:
						flicolor256to64((char *)ptr2+sizeof(chunk_header));
						break;
					case FLI_SS2:
						fliss2((char *)ptr2+sizeof(chunk_header),dest, lPitch==-1 ? lPitch==-1 ? ALIGNL(m_pFli->width):lPitch:lPitch);
						break;
					case FLI_COLOR:
						flicolor((char *)ptr2+sizeof(chunk_header));
						break;
					case FLI_LC:
						flilc((char *)ptr2+sizeof(chunk_header),dest, lPitch==-1 ? ALIGNL(m_pFli->width):lPitch,(char *)ptr2+locchunk.size-1);
						break;
					case FLI_BLACK:
						break; // have to make this one day
					case FLI_BRUN:
						flibrun((char *)ptr2+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width):lPitch, m_pFli->height);
						break;
					case FLI_COPY:
					case DTA_COPY:
						flicopy((char *)ptr2+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width):lPitch, m_pFli->height,m_pFli->bits);
						break; // have to make this one day
					case FLI_PSTAMP:
						break; // ignore the stamps
					case DTA_BRUN://(25)
						switch(m_pFli->bits){
						case 2:
							dta_brun16((char *)ptr2+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width*m_pFli->bits):lPitch,m_pFli->height);
							break;
						case 3:
							dta_brun24((char *)ptr2+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width*m_pFli->bits):lPitch,m_pFli->height);
							break;
						case 4:
							dta_brun32((char *)ptr2+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width*m_pFli->bits):lPitch,m_pFli->height);
							break;
						}
						break; 
					case DTA_LC:  //(27)
						switch(m_pFli->bits){
						case 2:
							dta_lc16((char *)ptr2+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width*m_pFli->bits):lPitch);
							break;
						case 3:
							dta_lc24((char *)ptr2+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width*m_pFli->bits):lPitch);
							break;
						case 4:
							dta_lc32((char *)ptr2+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width*m_pFli->bits):lPitch,m_pFli->height);
							break;
						}
						break; // ignore the stamps
							
					default:
						// undefined chunk type, ignore it
						break;
					}
					GFreePtr(ptr2);
				}
				break;
			case FLI_COLOR256:
				flicolor256to64((char *)chunk+sizeof(chunk_header));
				break;
			case FLI_SS2:
				fliss2((char *)chunk+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width):lPitch);
				break;
			case FLI_COLOR:
				flicolor((char *)chunk+sizeof(chunk_header));
				break;
			case FLI_LC:
				flilc((char *)chunk+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width):lPitch,(char *)chunk+chunk->size-1);
				break;
			case FLI_BLACK:
				break; // have to make this one day
			case FLI_BRUN:
				flibrun((char *)chunk+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width):lPitch,m_pFli->height);
				break;
			case FLI_COPY:
			case DTA_COPY:
				flicopy((char *)chunk+sizeof(chunk_header),dest,lPitch==-1 ? ALIGNL(m_pFli->width):lPitch,m_pFli->height,m_pFli->bits);
				break; // have to make this one day
			case FLI_PSTAMP:
				break; // ignore the stamps
			case DTA_BRUN://(25)
				switch(m_pFli->bits){
				case 2:
					dta_brun16((char *)chunk+sizeof(chunk_header),dest,lPitch/*ALIGNL(m_pFli->width*m_pFli->bits)*/,m_pFli->height);
					break;
				case 3:
					dta_brun24((char *)chunk+sizeof(chunk_header),dest,lPitch/*ALIGNL(m_pFli->width*m_pFli->bits)*/,m_pFli->height);
					break;
				case 4:
					dta_brun32((char *)chunk+sizeof(chunk_header),dest,lPitch/*ALIGNL(m_pFli->width*m_pFli->bits)*/,m_pFli->height);
					break;
				}
				break; 
			case DTA_LC:  //(27)
					switch(m_pFli->bits){
					case 2:
						dta_lc16((char *)chunk+sizeof(chunk_header),dest,lPitch/*ALIGNL(m_pFli->width*m_pFli->bits)*/);
						break;
					case 3:
						dta_lc24((char *)chunk+sizeof(chunk_header),dest,lPitch/*ALIGNL(m_pFli->width*m_pFli->bits)*/);
						break;
					case 4:
						dta_lc32((char *)chunk+sizeof(chunk_header),dest,lPitch/*ALIGNL(m_pFli->width*m_pFli->bits)*/,m_pFli->height);
						break;
					}
					break; // ignore the stamps
					
			default:
				ASSERT(FALSE);
					// undefined chunk type, ignore it
					break;
			}
			chunk = (chunk_header *)((char *)chunk + chunk->size);
		}
		
		// if playing from the disk, free the previous frame data
		if (m_pFli->pFile != NULL) {
			if (frame1.chunks!=0) 
				GFreePtr(chunk1);
		}
		else {
			if (m_pFli->current >= m_pFli->frames) {
				m_pFli->current = 0;
				m_pFli->crntoff = (long)sizeof(fli);
				frame=(frame_header *)((char *)m_pFli+m_pFli->crntoff);
			}else{
				m_pFli->crntoff+=frame->size;
			}
		}
		return 0;
}

//---------------------------------------------------------
void CAnim::CloseAnim()
{
	if (m_pFli != NULL){
		if (m_pFli->pFile != NULL) 
			DELETEP( m_pFli->pFile);
		GFreePtr(m_pFli);
		m_pFli=NULL;
	}
}

//---------------------------------------------------------
BOOL CAnim::Seek(int Nframe, char*dest,int lPitch)
{
	frame_header *frame;
	short int     i;
	
	if (m_pFli==NULL) return FALSE;
	if (Nframe-1==m_pFli->current) return TRUE;
	
	if (Nframe>m_pFli->frames)	
		Nframe=m_pFli->frames;//конец
	if (Nframe<1)
		Nframe=1;				//начало
	
	if (Nframe-1>m_pFli->current) //требуемый кадр за текущим
	{
		for (i=m_pFli->current;i<Nframe-1;i++)
			PlayFrame(dest,lPitch);
		return TRUE;
	}//if
	
    if (Nframe-1<m_pFli->current) //требуемый кадр перед текущим
	{
		if (m_pFli->pFile) {
			m_pFli->current = 0;
			m_pFli->crntoff = (long)sizeof(fli);
			frame=(frame_header *)((char *)m_pFli+m_pFli->crntoff);
		}
		else{
			m_pFli->current=0;
			switch(m_pFli->magic){
			case fli_magic:
			if (m_pFli->pFile->Seek(sizeof(fli_header), CFile::begin)==-1)
				return FALSE;
				break;
			case zip_magic:
			case dta_magic:
			case flc_magic:
				if (m_pFli->pFile->Seek(m_pFli->header.oframe1, CFile::begin)==-1)
					return FALSE;
				break;
			}
		}
		for (i=0;i<Nframe-1;i++)
			PlayFrame(dest,lPitch);
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------------
BOOL CAnim::GetPalette(RGBQUAD* pal)
{
	frame_header *frame, frame1;
	chunk_header *chunk, *chunk1;
	short int     i;
	
	if (m_pFli==NULL)
		return TRUE;
	if (m_pFli->depth!=8)
		return FALSE; //Нет палитры
	Seek(1,NULL);
	for(i=0;i<m_pFli->frames;i++){
		if (m_pFli->pFile) {
			// if playing from ram, setup pointers for the next frame
			frame=(frame_header *)((char *)m_pFli+m_pFli->crntoff);
			chunk=(chunk_header *)((char *)frame+sizeof(frame_header));
			m_pFli->current++;
			m_pFli->crntoff+=frame->size;
		}
		else {
			// if playing from disk, read the next frame
			m_pFli->pFile->Read((char*)&frame1,sizeof(frame_header));
			frame=&frame1;
			if (frame1.chunks>0) {
				// if there are chunks to load, load em
				if((chunk=(chunk_header *)GAllocPtr(frame1.size-sizeof(frame_header)))==NULL)
					return 1;
				chunk1 = chunk;
				m_pFli->pFile->Read((char*)chunk,frame1.size-sizeof(frame_header));
			}
			m_pFli->current++;
		}
		
		if (frame->magic == frame_magic)
			for (i=0; i<frame->chunks;i++) {
				switch(chunk->type) {
				case SSN_ZIP:
					{
						chunk_header locchunk;
						char* ptr1 = (char *)chunk+sizeof(chunk_header)+sizeof(DWORD);
						char* ptr2 = (char *)chunk+sizeof(chunk_header);
						DWORD zip = chunk->size - sizeof(chunk_header) - sizeof (DWORD);
						DWORD unzip = *(DWORD*)ptr2 + 1000;
						ptr2 = (char *)GAllocPtr(unzip);
						ssn_unzip(ptr1,ptr2,&zip,&unzip);
						memcpy(&locchunk,ptr2,sizeof(chunk_header));
						switch(locchunk.type){
						case FLI_COLOR256:
							flicolor256to64((char *)ptr2+sizeof(chunk_header));
							Seek(1,NULL);
							memcpy(pal,m_pFli->Colors,sizeof(PALETTEENTRY)*256);
							GFreePtr(ptr2);
							goto lebel1;
							break;
						case FLI_COLOR:
							flicolor((char *)ptr2+sizeof(chunk_header));
							Seek(1,NULL);
							memcpy(pal,m_pFli->Colors,sizeof(PALETTEENTRY)*256);
							GFreePtr(ptr2);
							goto lebel1;
						}
						GFreePtr(ptr2);
					}
					break;
				case FLI_COLOR256:
					flicolor256to64((char *)chunk+sizeof(chunk_header));
					Seek(1,NULL);
					memcpy(pal,m_pFli->Colors,sizeof(PALETTEENTRY)*256);
					goto lebel1;
				case FLI_COLOR:
					flicolor((char *)chunk+sizeof(chunk_header));
					Seek(1,NULL);
					memcpy(pal,m_pFli->Colors,sizeof(PALETTEENTRY)*256);
					goto lebel1;
				default:
					// undefined chunk type, ignore it
					break;
				}
				chunk = (chunk_header *)((char *)chunk + chunk->size);
			}
lebel1:
		if (!m_pFli->pFile) {
			if (frame1.chunks!=0) 
				GFreePtr(chunk1);
		}
	}
	Seek(1,NULL);
	return FALSE;
}

//---------------------------------------------------------
LONG CAnim::GetSizeBuffer()
{
	if (m_pFli==NULL)
		return 0l;
	return m_pFli->height*ALIGNL(m_pFli->width*m_pFli->bits);
}

void CAnim::flndecode(char *data, char *dest, short int xsize, bool abAlpha){
	BYTE* pbtCur=(BYTE*)data;
	unsigned long uiNumOfBlocks=*((unsigned long *)pbtCur);
	pbtCur+=sizeof(unsigned long);
	unsigned long ulBytePP=2;
	if(abAlpha) ulBytePP=4;
	rlChngBlcks.RemoveAll();
	for(unsigned long ulBlckNum=0; ulBlckNum < uiNumOfBlocks; ulBlckNum++){
/*
	typedef struct 
		{
		unsigned long size;	// size of block incuding size of this header
		unsigned short int x;
		unsigned short int y;
		unsigned short int width;
		unsigned short int height;
	} fln_block_header;
*/
		fln_block_header oflnbh;
		memcpy(&oflnbh,pbtCur,sizeof(fln_block_header)); 
		pbtCur+=sizeof(fln_block_header);
		//
		rlChngBlcks.AddTail(
			CRect(oflnbh.x,oflnbh.y,oflnbh.x+oflnbh.width,oflnbh.y+oflnbh.height)
		);
		//
		BYTE* pbt1=(BYTE*)dest;
		unsigned long ulOffs;
		ulOffs=oflnbh.y; 
		ulOffs*=xsize; 
		pbt1+=ulOffs;
		for(int y=0; y<oflnbh.height; y++){
			BYTE* pbt2=pbt1;
			ulOffs=oflnbh.x; ulOffs*=ulBytePP;
			pbt2+=ulOffs;
			for(int x=0; x<oflnbh.width;){
				char sc=*(pbtCur++);
				if(sc<0){
					x+=(-sc);
					pbt2+=(ulBytePP*(-sc));
				}else{
					for(int i=0; i<sc; i++){
						unsigned short usHC=*((unsigned short*)pbtCur);
						pbtCur+=sizeof(unsigned short);
						if(abAlpha){ // with alpha
							BYTE r,g,b,a;
							unsigned short int hr=0,hg=0,hb=0;
							#ifdef FORMAT565
							r=(usHC>>(5+6-3))&0xF8;
							g=(usHC>>(5-2))&0xFC;
							#endif
							#ifdef FORMAT555
							r=(usHC>>(5+5-3))&0xF8;
							g=(usHC>>(5-3))&0xFC;
							#endif
							b=(usHC<<3)&0xF8;
							a=*(pbtCur++);
							pbt2[0]=b; pbt2[1]=g; pbt2[2]=r; pbt2[3]=a;
						}else{
							*((unsigned short*)pbt2)=usHC;
						}
						pbt2+=ulBytePP;
						x++;
					}
				}
			} // by x
			ulOffs=xsize; 
			pbt1+=ulOffs;
		} // by y
	} // by block
}

#define M2_MAX_LEN		8

#define M2_MAX_OFFSET	0x0800
#define M3_MAX_OFFSET	0x4000
#define M4_MAX_OFFSET	0xbfff

#define M4V_MARKER      8

inline void FindNextXRange(int &aiX0,int &aiY0,int aiW,int aiH,RECT* apChngBlcks,int aiChngBlcksCnt,UINT &auiOffs0,WORD &awBW){
	for(;;){
		RECT rcNearest; rcNearest.left=10000; 
		for(int ii=0; ii<aiChngBlcksCnt; ii++){
			RECT rc=apChngBlcks[ii];
			if(aiY0<rc.top || aiY0>rc.bottom) continue;
			if(rc.right<aiX0) continue;
			if((rc.left-aiX0)<(rcNearest.left-aiX0)) rcNearest=rc;
		}
		if(rcNearest.left!=10000){
			auiOffs0=(aiY0*aiW+rcNearest.left)<<1;
			awBW=(rcNearest.right-rcNearest.left+1)<<1;
			aiX0=rcNearest.right+1;
			return;
		}
		aiY0++; aiX0=0;
		if(aiY0>=aiH) break;
	}
}

void CAnim::flndecodevid(char *data, char *dest, short int xsize, bool abAlpha,bool bKeyFrame){
	//
	unsigned long ulBytePP=2;
	if(abAlpha) ulBytePP=4;
//	ASSERT(ulBytePP==2);
	//
	BYTE* ip=(BYTE*)data;
	BYTE* op=(BYTE*)dest;
	int iW=0,iH=0;
	RECT* pChngBlcks=NULL;
	WORD iCnt;
	iW=*(WORD*)ip; ip+=sizeof(WORD);
	iH=*(WORD*)ip; ip+=sizeof(WORD);
	if(!bKeyFrame){
		iCnt=*(WORD*)ip; ip+=sizeof(WORD);
		pChngBlcks=new RECT[iCnt];
		for(int ii=0; ii<iCnt; ii++){
			RECT rc;
			rc.left=*(WORD*)ip; ip+=sizeof(WORD);
			rc.top=*(WORD*)ip; ip+=sizeof(WORD);
			rc.right=*(WORD*)ip; ip+=sizeof(WORD);
			rc.bottom=*(WORD*)ip; ip+=sizeof(WORD);
			pChngBlcks[ii]=rc;
		}
	}
	//
	UINT t;
//	CRect rc;
	int x=0,y=0;
	UINT uiOffs0;
int iM=0;
int iCntUp=0;
int iCntNotUp=0;
	WORD wBW;
	BYTE* op0;
	if(!bKeyFrame){
		FindNextXRange(x,y,iW,iH,pChngBlcks,iCnt,uiOffs0,wBW);
		if(ulBytePP==4){ wBW<<=1; uiOffs0<<=1; }
		op0=op=(BYTE*)dest+uiOffs0;
	}
	//
	for(;;){
		t=*ip++;
		if(t<16){
			if(t==0){
				while(*ip==0){
					t+=255;
					ip++;
				}
				t+=15+*ip++;
			}
			ASSERT(t>0);
//			t+=3;
			// copy t pixels
			if(ulBytePP==4){
				for(; t>0; t--){
					WORD usHC=*(WORD*)ip; ip+=2;
					*(op++)=(usHC<<3)&0xF8; // B
					*(op++)=(usHC>>(5-2))&0xFC; // G
					*(op++)=(usHC>>(5+6-3))&0xF8; // R
					*(op++)=*(ip++); // alpha
				}
			}else{
				for(; t>0; t--){
					*(WORD*)op=*(WORD*)ip;
					ip+=2; op+=2;
				}
			}
			//
		}else{ // t >= 16
// [ 0x0..0x0F] - just pixels
// [0x10..0x1F] - M4 (eof)
// [0x20..0x3F] - M3
// [0x40..0xFF] - M2
			int iRLen=0;
			bool bUp=false;
			int iBack=0;
			UINT uiOffs;
if((op-(BYTE*)dest)/2==162)
int hh=0;
			if(t < 0x40){
				if(t<0x20){
iM=4;
					// M4
					if(t & M4V_MARKER){ 
						bUp=true; 
						t &= (~M4V_MARKER);
if(!bKeyFrame)
int hh=0;
					}
					t &= 7;
					if(t == 0){
						while(*ip == 0){
							t += 255;
							ip++;
						}
						t += 7 + *ip++;
					}
					t++;
					uiOffs=(*(unsigned short*)ip);
					if(uiOffs==0) break;
					iRLen=uiOffs&1;
					iBack=uiOffs&2;
//						if(uiOffs<2){
//							bMoreBlocks=uiOffs&1;
//							break;
//						}
					uiOffs>>=2;
					uiOffs--;
					uiOffs+=M3_MAX_OFFSET;
					ip+=2;
				}else{
iM=3;
					// M3
					t &= 31;
					if(t==0){
						while(*ip == 0){ t += 255; ip++; }
						t+=31+*ip++;
					}
					t++;
					uiOffs=(*(unsigned short*)ip);
					iRLen=uiOffs&1;
					iBack=uiOffs&2;
					uiOffs>>=2;
					if(uiOffs<M2_MAX_OFFSET) t+=(M2_MAX_LEN-2);
					ip+=2;
				}
			}else{
iM=2;
				// M2
				iRLen=t&1;
				iBack=t&2;
				uiOffs=(t>>2)&7;
				uiOffs+=((*ip++)<<3);
				t=t>>5;
			}
int idd=(int)(op-(BYTE*)(dest));
idd/=2;
int yy=idd/iW;
int xx=idd-yy*iW;
if(xx==52 && yy==4)
int hh=0;
			// copy match
			ASSERT(t>0);
int ttt0=t;
			if(bUp){
				uiOffs-=M3_MAX_OFFSET;
//
				int iOffsSign=-1;
				if((!bKeyFrame) && (uiOffs&1)){ // if Fwd
					iOffsSign=1;
				}
				if(!bKeyFrame) uiOffs>>=1;
//if(iOffsSign==1)
//int hh=0;
//
//				WORD* op2=(WORD*)(op+iOffsSign*((int)(uiOffs<<1)));
int iym=uiOffs/iW;
int ixm=uiOffs-iym*iW;
				if(iOffsSign==1){
					if(ulBytePP==4){
						ULONG* op2=(ULONG*)(op+uiOffs*ulBytePP);
						for(; t>0; t--){
							*(ULONG*)op=*op2;
							op2+=iW;
							op+=sizeof(ULONG);
						}
					}else{
						WORD* op2=(WORD*)(op+uiOffs*2);
						for(; t>0; t--){
							*(WORD*)op=*op2;
							op2+=iW;
							op+=2;
						}
					}
				}else{
					if(ulBytePP==4){
						ULONG* op2=(ULONG*)(op-uiOffs*ulBytePP);
						for(; t>0; t--){
							*(ULONG*)op=*op2;
							op2-=iW;
							op+=sizeof(ULONG);
						}
					}else{
						WORD* op2=(WORD*)(op-uiOffs*2);
						for(; t>0; t--){
							*(WORD*)op=*op2;
//if(!bKeyFrame){
//*(WORD*)op=iM*0x100;
//if(ttt0==t) *(WORD*)op=iM*0x100+0xF;
//}
							op2-=iW;
							op+=2;
						}
					}
				}
			}else{
				int iOffsSign=-1;
				if((!bKeyFrame) && (uiOffs&1)){ // if Fwd
					iOffsSign=1;
				}
				if(!bKeyFrame) uiOffs>>=1;
				// copy t pixels from x+/-uiOffs
				if(ulBytePP==4){
					ULONG* op2=(ULONG*)(op+iOffsSign*((int)(uiOffs*ulBytePP)));
					if(iBack){
						for(; t>0; t--){ 
							*(ULONG*)op=*(op2--); 
							op+=sizeof(ULONG); 
						}
					}else{
						for(; t>0; t--){
							*(ULONG*)op=*(op2++); 
							op+=sizeof(ULONG); 
						}
					}
				}else{
					WORD* op2=(WORD*)(op+iOffsSign*((int)(uiOffs<<1)));
					if(iBack){
						for(; t>0; t--){ *(WORD*)op=*(op2--); 
//if((!bKeyFrame)){
//*(WORD*)op=iM*0x100+0x7000;
//if(ttt0==t) *(WORD*)op=iM*0x100+0xF;
//}
							op+=2; 
						}
					}else{
						for(; t>0; t--){
							*(WORD*)op=*(op2++); 
//if((!bKeyFrame)){
//*(WORD*)op=iM*0x100;
//if(ttt0==t) *(WORD*)op=iM*0x100+0xF;
//}
							op+=2; 
						}
					}
				}
			}
			if(!bKeyFrame){
				ASSERT((op-op0)<=wBW);
				if((op-op0)==wBW){
					FindNextXRange(x,y,iW,iH,pChngBlcks,iCnt,uiOffs0,wBW);
					if(ulBytePP==4){ wBW<<=1; uiOffs0<<=1; }
					op0=op=(BYTE*)dest+uiOffs0;
				}
			}
			if(iRLen>0){
				if(ulBytePP==4){
					for(; iRLen>0; iRLen--){
						WORD usHC=*(WORD*)ip; ip+=2;
						*(op++)=(usHC<<3)&0xF8; // B
						*(op++)=(usHC>>(5-2))&0xFC; // G
						*(op++)=(usHC>>(5+6-3))&0xF8; // R
						*(op++)=*(ip++); // alpha
					}
				}else{
					for(; iRLen>0; iRLen--){
						*(WORD*)op=*(WORD*)ip;
						ip+=2; op+=2;
					}
				}
			}
		} // if(t<16)
		if(!bKeyFrame){
			ASSERT((op-op0)<=wBW);
			if((op-op0)==wBW){
				FindNextXRange(x,y,iW,iH,pChngBlcks,iCnt,uiOffs0,wBW);
				if(ulBytePP==4){ wBW<<=1; uiOffs0<<=1; }
				op0=op=(BYTE*)dest+uiOffs0;
			}
		}
	} // for(;;)
	if(pChngBlcks) delete pChngBlcks;
}

void CAnim::flndecodetcc(char *data, char *dest, short int xsize, bool abAlpha,bool bKeyFrame){
	//
	unsigned long ulBytePP=3;
	if(abAlpha) ulBytePP=4;
//	ASSERT(ulBytePP==2);
	//
	BYTE* ip=(BYTE*)data;
	BYTE* op=(BYTE*)dest;
	int iW=0,iH=0;
	RECT* pChngBlcks=NULL;
	WORD iCnt;
	iW=*(WORD*)ip; ip+=sizeof(WORD);
	iH=*(WORD*)ip; ip+=sizeof(WORD);
	if(!bKeyFrame){
		iCnt=*(WORD*)ip; ip+=sizeof(WORD);
		pChngBlcks=new RECT[iCnt];
		for(int ii=0; ii<iCnt; ii++){
			RECT rc;
			rc.left=*(WORD*)ip; ip+=sizeof(WORD);
			rc.top=*(WORD*)ip; ip+=sizeof(WORD);
			rc.right=*(WORD*)ip; ip+=sizeof(WORD);
			rc.bottom=*(WORD*)ip; ip+=sizeof(WORD);
			pChngBlcks[ii]=rc;
		}
	}
	//
	UINT t;
//	CRect rc;
	int x=0,y=0;
	UINT uiOffs0;
int iM=0;
int iCntUp=0;
int iCntNotUp=0;
	WORD wBW;
	BYTE* op0;
	if(!bKeyFrame){
		FindNextXRange(x,y,iW,iH,pChngBlcks,iCnt,uiOffs0,wBW);
		if(ulBytePP==4){ wBW<<=1; uiOffs0<<=1; }
		else{ wBW+=wBW>>1; uiOffs0+=uiOffs0>>1; }
		op0=op=(BYTE*)dest+uiOffs0;
	}
	//
	for(;;){
		t=*ip++;
		if(t<16){
			if(t==0){
				while(*ip==0){
					t+=255;
					ip++;
				}
				t+=15+*ip++;
			}
			ASSERT(t>0);
//			t+=3;
			// copy t pixels
			if(ulBytePP==4){
				for(; t>0; t--){
					*(long*)op=*(long*)ip;
					op+=4; ip+=4;
				}
			}else{ // ulBytePP==3
				for(; t>0; t--){
					*op++=*ip++;
					*op++=*ip++;
					*op++=*ip++;
				}
			}
			//
		}else{ // t >= 16
// [ 0x0..0x0F] - just pixels
// [0x10..0x1F] - M4 (eof)
// [0x20..0x3F] - M3
// [0x40..0xFF] - M2
			int iRLen=0;
			bool bUp=false;
			int iBack=0;
			UINT uiOffs;
if((op-(BYTE*)dest)/2==162)
int hh=0;
			if(t < 0x40){
				if(t<0x20){
iM=4;
					// M4
					if(t & M4V_MARKER){ 
						bUp=true; 
						t &= (~M4V_MARKER);
if(!bKeyFrame)
int hh=0;
					}
					t &= 7;
					if(t == 0){
						while(*ip == 0){
							t += 255;
							ip++;
						}
						t += 7 + *ip++;
					}
					t++;
					uiOffs=(*(unsigned short*)ip);
					if(uiOffs==0) break;
					iRLen=uiOffs&1;
					iBack=uiOffs&2;
//						if(uiOffs<2){
//							bMoreBlocks=uiOffs&1;
//							break;
//						}
					uiOffs>>=2;
					uiOffs--;
					uiOffs+=M3_MAX_OFFSET;
					ip+=2;
				}else{
iM=3;
					// M3
					t &= 31;
					if(t==0){
						while(*ip == 0){ t += 255; ip++; }
						t+=31+*ip++;
					}
					t++;
					uiOffs=(*(unsigned short*)ip);
					iRLen=uiOffs&1;
					iBack=uiOffs&2;
					uiOffs>>=2;
					if(uiOffs<M2_MAX_OFFSET) t+=(M2_MAX_LEN-2);
					ip+=2;
				}
			}else{
iM=2;
				// M2
				iRLen=t&1;
				iBack=t&2;
				uiOffs=(t>>2)&7;
				uiOffs+=((*ip++)<<3);
				t=t>>5;
			}
int idd=(int)(op-(BYTE*)(dest));
idd/=2;
int yy=idd/iW;
int xx=idd-yy*iW;
if(xx==52 && yy==4)
int hh=0;
			// copy match
			ASSERT(t>0);
int ttt0=t;
			if(bUp){
				uiOffs-=M3_MAX_OFFSET;
//
				int iOffsSign=-1;
				if((!bKeyFrame) && (uiOffs&1)){ // if Fwd
					iOffsSign=1;
				}
				if(!bKeyFrame) uiOffs>>=1;
//if(iOffsSign==1)
//int hh=0;
//
//				WORD* op2=(WORD*)(op+iOffsSign*((int)(uiOffs<<1)));
int iym=uiOffs/iW;
int ixm=uiOffs-iym*iW;
				if(iOffsSign==1){
					if(ulBytePP==4){
						ULONG* op2=(ULONG*)(op+uiOffs*ulBytePP);
						for(; t>0; t--){
							*(ULONG*)op=*op2;
							op2+=iW;
							op+=sizeof(ULONG);
						}
					}else{ // ulBytePP==3
						int iBPL=iW*ulBytePP;
						BYTE* op2=(op+uiOffs*ulBytePP);
						for(; t>0; t--){
							*op++=*op2++;
							*op++=*op2++;
							*op++=*op2;
							op2-=2;
							op2+=iBPL;
						}
					}
				}else{
					if(ulBytePP==4){
						ULONG* op2=(ULONG*)(op-uiOffs*ulBytePP);
						for(; t>0; t--){
							*(ULONG*)op=*op2;
							op2-=iW;
							op+=sizeof(ULONG);
						}
					}else{
						int iBPL=iW*ulBytePP;
						BYTE* op2=(op-uiOffs*ulBytePP);
						for(; t>0; t--){
							*op++=*op2++;
//if((!bKeyFrame)){
//op[-1]=0xE0;
//if(ttt0==t) op[-1]=0xFF;
//}
							*op++=*op2++;
							*op++=*op2;
							op2-=2;
							op2-=iBPL;
//if(!bKeyFrame){
//*(WORD*)op=iM*0x100;
//if(ttt0==t) *(WORD*)op=iM*0x100+0xF;
//}
						}
					}
				}
			}else{
				int iOffsSign=-1;
				if((!bKeyFrame) && (uiOffs&1)){ // if Fwd
					iOffsSign=1;
				}
				if(!bKeyFrame) uiOffs>>=1;
				// copy t pixels from x+/-uiOffs
				if(ulBytePP==4){
					ULONG* op2=(ULONG*)(op+iOffsSign*((int)(uiOffs*ulBytePP)));
					if(iBack){
						for(; t>0; t--){ 
							*(ULONG*)op=*(op2--); 
							op+=sizeof(ULONG); 
						}
					}else{
						for(; t>0; t--){
							*(ULONG*)op=*(op2++); 
							op+=sizeof(ULONG); 
						}
					}
				}else{
					BYTE* op2=(op+iOffsSign*((int)(uiOffs*ulBytePP)));
					if(iBack){
						for(; t>0; t--){ 
							*op++=*op2++; 
//if((!bKeyFrame)){
//op[-1]=0xE0;
//if(ttt0==t) op[-1]=0xFF;
//}
							*op++=*op2++; 
							*op++=*op2;
							op2-=5;
//if((!bKeyFrame)){
//*(WORD*)op=iM*0x100+0x7000;
//if(ttt0==t) *(WORD*)op=iM*0x100+0xF;
//}
						}
					}else{
						for(; t>0; t--){
							*op++=*op2++; 
//if((!bKeyFrame) && op2>=op){
//op[-1]=0xE0;
//if(ttt0==t) op[-1]=0xFF;
//*(WORD*)op=iM*0x100;
//if(ttt0==t) *(WORD*)op=iM*0x100+0xF;
//}
							*op++=*op2++; 
							*op++=*op2++; 
						}
					}
				}
			}
			if(!bKeyFrame){
				ASSERT((op-op0)<=wBW);
				if((op-op0)==wBW){
					FindNextXRange(x,y,iW,iH,pChngBlcks,iCnt,uiOffs0,wBW);
					if(ulBytePP==4){ wBW<<=1; uiOffs0<<=1; }
					else{ wBW+=wBW>>1; uiOffs0+=uiOffs0>>1; }
					op0=op=(BYTE*)dest+uiOffs0;
				}
			}
			if(iRLen>0){
				if(ulBytePP==4){
					for(; iRLen>0; iRLen--){
						*(long*)op=*(long*)ip;
						op+=4; ip+=4;
					}
				}else{
					for(; iRLen>0; iRLen--){
						*op++=*ip++; 
						*op++=*ip++; 
						*op++=*ip++; 
					}
				}
			}
		} // if(t<16)
		if(!bKeyFrame){
			ASSERT((op-op0)<=wBW);
			if((op-op0)==wBW){
				FindNextXRange(x,y,iW,iH,pChngBlcks,iCnt,uiOffs0,wBW);
				if(ulBytePP==4){ wBW<<=1; uiOffs0<<=1; }
				else{ wBW+=wBW>>1; uiOffs0+=uiOffs0>>1; }
				op0=op=(BYTE*)dest+uiOffs0;
			}
		}
	} // for(;;)
	if(pChngBlcks) delete pChngBlcks;
}



