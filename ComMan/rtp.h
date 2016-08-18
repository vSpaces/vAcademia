// RTP_class.h: interface for the CRTP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTP_CLASS_H__8138B40D_7C5B_43CF_A973_E040FEF46A9D__INCLUDED_)
#define AFX_RTP_CLASS_H__8138B40D_7C5B_43CF_A973_E040FEF46A9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common_globals.h"

#define RTP_VERSION 2
#define bcopy(s, d, l)	memcpy(d, s, l)
#define RTP_MAX_SDES 256   /* maximum text length for SDES */
typedef unsigned char u_int8;
typedef unsigned short u_int16;
typedef unsigned long u_int32;
#define BUFL	1000							// Sound buffer maximum length (actually less)

typedef enum {
  RTCP_SR	= 200,
  RTCP_RR	= 201,
  RTCP_SDES = 202,
  RTCP_BYE	= 203,
  RTCP_APP	= 204
} rtcp_type_t;

typedef enum {
  RTCP_SDES_END    =  0,
  RTCP_SDES_CNAME  =  1,
  RTCP_SDES_NAME   =  2,
  RTCP_SDES_EMAIL  =  3,
  RTCP_SDES_PHONE  =  4,
  RTCP_SDES_LOC    =  5,
  RTCP_SDES_TOOL   =  6,
  RTCP_SDES_NOTE   =  7,
  RTCP_SDES_PRIV   =  8, 
  RTCP_SDES_IMG    =  9,
  RTCP_SDES_DOOR   = 10,
  RTCP_SDES_SOURCE = 11
} rtcp_sdes_type_t;

typedef struct {
  unsigned int version:2;  /* protocol version */
  unsigned int p:1; 	   /* padding flag */
  unsigned int x:1; 	   /* header extension flag */
  unsigned int cc:4;	   /* CSRC count */
  unsigned int m:1; 	   /* marker bit */
  unsigned int pt:7;	   /* payload type */
  u_int16 seq;			   /* sequence number */
  u_int32 ts;			   /* timestamp */
  u_int32 ssrc; 		   /* synchronization source */
  u_int32 csrc[1];		   /* optional CSRC list */
} rtp_hdr_t;

typedef struct {
  unsigned int version:2;  /* protocol version */
  unsigned int p:1; 	   /* padding flag */
  unsigned int count:5;    /* varies by payload type */
  unsigned int pt:8;	   /* payload type */
  u_int16 length;		   /* packet length in words, without this word */
} rtcp_common_t;

/* reception report */
typedef struct {
  u_int32 ssrc; 		   /* data source being reported */
  unsigned long fraction:8; /* fraction lost since last SR/RR */
  long lost:24;			   /* cumulative number of packets lost (signed!) */
  u_int32 last_seq; 	   /* extended last sequence number received */
  u_int32 jitter;		   /* interarrival jitter */
  u_int32 lsr;			   /* last SR packet from this source */
  u_int32 dlsr; 		   /* delay since last SR packet */
} rtcp_rr_t;

typedef struct {
  u_int8 type;			   /* type of SDES item (rtcp_sdes_type_t) */
  u_int8 length;		   /* length of SDES item (in octets) */
  char data[1]; 		   /* text, not zero-terminated */
} rtcp_sdes_item_t;

/* one RTCP packet */
typedef struct {
  rtcp_common_t common;    /* common header */
  union {
	/* sender report (SR) */
	struct {
	  u_int32 ssrc; 	   /* source this RTCP packet refers to */
	  u_int32 ntp_sec;	   /* NTP timestamp */
	  u_int32 ntp_frac;
	  u_int32 rtp_ts;	   /* RTP timestamp */
	  u_int32 psent;	   /* packets sent */
	  u_int32 osent;	   /* octets sent */ 
	  /* variable-length list */
	  rtcp_rr_t rr[1];
	} sr;

	/* reception report (RR) */
	struct {
	  u_int32 ssrc; 	   /* source this generating this report */
	  /* variable-length list */
	  rtcp_rr_t rr[1];
	} rr;

	/* BYE */
	struct {
	  u_int32 src[1];	   /* list of sources */
      /* can't express trailing text */
	} bye;

	/* source description (SDES) */
	struct rtcp_sdes_t {
	  u_int32 src;				/* first SSRC/CSRC */
	  rtcp_sdes_item_t item[1]; /* list of SDES items */
	} sdes;
  } r;
} rtcp_t;


struct soundbuf {
	LONG compression;
	char sendinghost[16];
	Sbuffer buffer;
};

enum {SSRC, TIMESTAMP, RTPDESRAND};

class CRTP  
{
	unsigned long ssrc;			  			// RTP synchronisation source identifier
	unsigned long timestamp; 	  			// RTP packet timestamp
	unsigned short seq;			  			// RTP packet sequence number
	unsigned long rtpdesrand;				// RTP DES random RTCP prefix

public:
	void change_seq(short val);
	void change_LONG(int id, LONG val);
	LONG rtp_out(soundbuf *sb_);
	int isrtp(unsigned char *pkt, int len);
	int isValidRTCPpacket(unsigned char *p, int len);
	int isRTCPByepacket(unsigned char *p, int len);
	int waProtUseLargerRTCPackets;	// Use large packets with RTP protocol
	void Init();
	CRTP();
	virtual ~CRTP();
	void sessionKeyGenerate(LPSTR key, BOOL binary);
	

private:
	int spurt;
};

#endif // !defined(AFX_RTP_CLASS_H__8138B40D_7C5B_43CF_A973_E040FEF46A9D__INCLUDED_)
