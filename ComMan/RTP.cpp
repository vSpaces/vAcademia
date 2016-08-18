// RTP_class.cpp: implementation of the CRTP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "rtp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//  Determine number of elements in an array
#define ELEMENTS(array) (sizeof(array)/sizeof((array)[0]))

//	Audio encoding types for RTP and VAT protocols
typedef enum {
	AE_PCMU, AE_PCMA, AE_G721, AE_IDVI, AE_G723, AE_GSM,
	AE_1016, AE_LPC, 
	AE_L8, AE_L16, AE_L24, AE_L32,
	AE_G728, AE_TRUE,
	AE_MAX
} audio_encoding_t;

/* Common audio description for files and workstations */
typedef struct {
	audio_encoding_t encoding;  /* type of encoding (differs) */
	unsigned sample_rate;       /* sample frames per second */
	unsigned channels;          /* number of interleaved channels */
} audio_descr_t;


static audio_descr_t adt[] = {
  {AE_PCMU,  8000, 1},	/*	0 PCMU */
  {AE_MAX,	 8000, 1},	/*	1 1016 */
  {AE_G721,  8000, 1},	/*	2 G721 */
  {AE_GSM,	 8000, 1},	/*	3 GSM */
  {AE_G723,  8000, 1},	/*	4 Unassigned */
  {AE_IDVI,  8000, 1},	/*	5 DVI4 */
  {AE_IDVI, 16000, 1},	/*	6 DVI4 */
  {AE_LPC,	 8000, 1},	/*	7 LPC */
  {AE_PCMA,  8000, 1},	/*	8 PCMA */
  {AE_MAX,		0, 1},	/*	9 G722 */
  {AE_L16,	44100, 2},	/* 10 L16 */
  {AE_L16,	44100, 1},	/* 11 L16 */
  {AE_MAX,		0, 1},	/* 12 */
};


CRTP::CRTP()
{
	waProtUseLargerRTCPackets = FALSE;	

}

CRTP::~CRTP()
{
	
}

void CRTP::sessionKeyGenerate(LPSTR key, BOOL binary)
{
	int j, k;
	char s[256];
    struct MD5Context md5c;
    unsigned char md5key[16], md5key1[16];
    POINT p;
	MEMORYSTATUS ms;
	
	/*	The following gets all kind of information likely
		to vary from moment to moment and uses it as the initial
		seed for the random number generator.  If any of these
		causes porting problems in the future, just delete them.  */
	char s_temp[256];
	LoadString(_Module.GetModuleInstance( ),IDS_FORMATS,s_temp,255);
	wsprintf(s,s_temp, GetTickCount());
	wsprintf(s + strlen(s), s_temp, time(NULL));	
	gethostname(s + strlen(s), 256);
	LoadString(_Module.GetModuleInstance( ),IDS_FORMATS+1,s_temp,255);
    wsprintf(s + strlen(s), s_temp, GetActiveWindow());
	LoadString(_Module.GetModuleInstance( ),IDS_FORMATS,s_temp,255);
    wsprintf(s + strlen(s), s_temp, GetFreeSpace(0));
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);
	wsprintf(s + strlen(s), s_temp, ms.dwMemoryLoad); 
	wsprintf(s + strlen(s), s_temp, ms.dwAvailPhys); 
	wsprintf(s + strlen(s), s_temp, ms.dwAvailPageFile); 

    GetCursorPos(&p);
 	LoadString(_Module.GetModuleInstance( ),IDS_FORMATS+2,s_temp,255);
    wsprintf(s + strlen(s), s_temp, p.x, p.y); 
	MD5Init(&md5c);
    MD5Update(&md5c, (unsigned char *)s, strlen(s));
    MD5Final(md5key, &md5c);
 	LoadString(_Module.GetModuleInstance( ),IDS_FORMATS+2,s_temp,255);
    wsprintf(s + strlen(s), s_temp, (time(NULL) + 65121) ^ 0x375F);
    MD5Init(&md5c);
    MD5Update(&md5c, (unsigned char *)s, strlen(s));
    MD5Final(md5key1, &md5c);

   
    for (j = k = 0; j < 16; j++) 
	{
		unsigned char rb = (md5key[j] ^ md5key1[j]);        
      
        if (binary) {  	key[j] = (char) rb;   } 
		else {
			/* We don't count on choosing where in the alphabet to
			start based on the hashes to stir in any more entropy
			than the fundamental 4 bits from the generated key
			(although it may), but primarily to spread out the
			letter distribution across the entire alphabet rather
			than just the first 16 letters. */
		#define Rad16(x)  ((((((md5key1[j] >> 1) ^ (md5key[15 - j] >> 2)) % 26) + (x)) % 26) + 'A')
	        key[k++] = Rad16((rb >> 4) & 0xF);
	        key[k++] = Rad16(rb & 0xF);
	        if (j & 1) {        key[k++] = '-';     }
        }
    }
    if (!binary) { 	key[--k] = 0;  }

}

void CRTP::Init()
{
  char md5key[16];
  sessionKeyGenerate(md5key, TRUE);
  memcpy((char *) &ssrc, md5key, sizeof ssrc);
  memcpy((char *) &timestamp, md5key + sizeof ssrc, sizeof timestamp);
  memcpy((char *) &seq, md5key + sizeof ssrc + sizeof timestamp, sizeof seq);
  memcpy((char *) &rtpdesrand, md5key + sizeof ssrc + sizeof timestamp + sizeof seq, sizeof rtpdesrand);

}


/*	RTPOUT	--	Convert a sound buffer into an RTP packet, given the
				SSRC, timestamp, and sequence number appropriate for the
				next packet sent to this connection.  */

LONG CRTP::rtp_out(soundbuf *sb_)
{
	soundbuf rp; 
	LONG pl = 0;

	rtp_hdr_t *rh = (rtp_hdr_t *) &rp.buffer.buffer_val;
	rh->version = RTP_VERSION;
	rh->p = 0;
	rh->x = 0;
	rh->cc = 0;
	rh->m = !!spurt;
	rh->seq = htons(seq);
	rh->ts = htonl(timestamp);
	rh->ssrc = htonl(ssrc);

	/* GSM */
//	rh[1] = 3;
			rh->pt = 3;

	bcopy(sb_->buffer.buffer_val + 2, ((char *) &rp.buffer.buffer_val) + 12,
				  (int) sb_->buffer.buffer_len - 2);
	pl = (sb_->buffer.buffer_len - 2) + 12;

	if (pl > 0) 
	{ bcopy((char *) &rp, (char *) sb_, (int) pl); }
	return pl;

}

/*	ISRTP  --  Determine if a packet is RTP or not.  If so, convert
			   in place into a sound buffer.  */

int CRTP::isrtp(unsigned char *pkt, int len)
{
	rtp_hdr_t *rh = (rtp_hdr_t *) pkt;
	

	/* Tear apart the header in a byte- and bit field-order
	   independent fashion. */

	if (
		rh->version == RTP_VERSION && /* Version ID correct */
		rh->pt < ELEMENTS(adt) &&	  /* Payload type credible */
		adt[rh->pt].sample_rate != 0 && /* Defined payload type */
									  /* Padding, if present, is plausible */
		(!rh->p || (pkt[len - 1] < (len - (12 + 4 * rh->cc))))
	   ) 
	{
		soundbuf sb_;
		unsigned char *payload;
		int lex, paylen;

							  /* Length of fixed header extension, if any */
		lex = rh->x ? (ntohs(*((short *) (pkt + 2 + 12 + 4 * rh->cc))) + 1) * 4 : 0;
		payload = pkt + (12 + 4 * rh->cc) + lex; /* Start of payload */
		paylen = len - ((12 + 4 * rh->cc) +	   /* Length of payload */
					lex + (rh->p ? pkt[len - 1] : 0));
		sb_.compression = fProtocol;
		sb_.buffer.buffer_len = 0;

#ifdef NEEDED
		/* Fake an RTP unique host name from the SSRC identifier. */

        sprintf(sb_.sendinghost, ".RTP:%02X%02X%02X%02X",
			pkt[8], pkt[9], pkt[10], pkt[11]);
#else
        strcpy(sb_.sendinghost, ".RTP");
#endif

#ifdef RTP_PACKET_DUMP
		xd(pkt, len, TRUE);
#endif		  				

				sb_.buffer.buffer_len = paylen + sizeof(short);
				bcopy(payload, sb_.buffer.buffer_val + 2, paylen);
				*((short *) sb_.buffer.buffer_val) =
					htons((short) ((((long) paylen) * 160) / 33));
				sb_.compression |= fCompGSM;

			if (sb_.buffer.buffer_len > 0) {
			bcopy(&sb_, pkt, (int) (((sizeof sb_ - BUFL)) + sb_.buffer.buffer_len));
#ifdef RTP_PACKET_DUMP			
			xd(&sb_, (int) ((sizeof sb_ - BUFL) + sb_.buffer.buffer_len), TRUE);
#endif			
		}  				
		return TRUE;
	}
	return FALSE;

}

/*	ISVALIDRTCPPACKET  --  Consistency check a packet to see if
						   is a compliant RTCP packet.	*/
int CRTP::isValidRTCPpacket(unsigned char *p, int len)
{
	unsigned char *end;

	if (((((p[0] >> 6) & 3) != RTP_VERSION) &&	   /* Version incorrect ? */
		((((p[0] >> 6) & 3) != 1))) ||			   /* Allow Speak Freely too */
		((p[0] & 0x20) != 0) || 				   /* Padding in first packet ? */
		((p[1] != RTCP_SR) && (p[1] != RTCP_RR))) { /* First item not SR or RR ? */
		return FALSE;
	}
	end = p + len;

	do {
		/* Advance to next subpacket */
		p += (ntohs(*((short *) (p + 2))) + 1) * 4;
	} while (p < end && (((p[0] >> 6) & 3) == RTP_VERSION));

	return p == end;

}

/*	ISRTCPBYEPACKET  --  Test if this RTCP packet contains a BYE.  */
int CRTP::isRTCPByepacket(unsigned char *p, int len)
{
	unsigned char *end;
	int sawbye = FALSE;

												   /* Version incorrect ? */
	if ((((p[0] >> 6) & 3) != RTP_VERSION && ((p[0] >> 6) & 3) != 1) ||
		((p[0] & 0x20) != 0) || 				   /* Padding in first packet ? */
		((p[1] != RTCP_SR) && (p[1] != RTCP_RR))) { /* First item not SR or RR ? */
		return FALSE;
	}
	end = p + len;

	do {
		if (p[1] == RTCP_BYE) {
			sawbye = TRUE;
		}
		/* Advance to next subpacket */
		p += (ntohs(*((short *) (p + 2))) + 1) * 4;
	} while (p < end && (((p[0] >> 6) & 3) == RTP_VERSION));

	return (p == end) && sawbye;
}



void CRTP::change_seq(short val)
{
	seq+=val;	
}

void CRTP::change_LONG(int id, LONG val)
{
	switch (id)
	{ case TIMESTAMP: timestamp+=val; break;
	}
}
