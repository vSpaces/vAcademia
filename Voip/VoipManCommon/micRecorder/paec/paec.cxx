/*
 * paec.cxx
 *
 * Open Phone Abstraction Library (OPAL)
 * Formally known as the Open H323 project.
 *
 * Copyright (c) 2001 Post Increment
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open Phone Abstraction Library.
 *
 * The author of this code is Damien Sandras
 *
 * rewritten amd made generic ptlib by Simon Horne
 *
 * Contributor(s): Miguel Rodriguez Perez.
 *
 * $Log: paec.cxx,v $
 * Revision 1.1  2009/10/22 07:44:08  Vladimir
 * no message
 *
 * Revision 1.1.2.1  2009/10/20 14:00:25  Vladimir
 * no message
 *
 * Revision 1.3  2009/01/22 13:33:06  Alex
 * no message
 *
 * Revision 1.2  2008/12/05 14:08:37  Percei
 * no message
 *
 * Revision 1.7  2007/03/19 07:06:51  shorne
 * correctly set the receive timestamp if buffer runs dry
 *
 * Revision 1.6  2007/03/18 06:23:59  shorne
 * Added check to ensure AEC buffer is not empty
 *
 * Revision 1.5  2007/03/15 21:36:41  shorne
 * fix for buffer underrun
 *
 * Revision 1.4  2007/03/06 00:22:00  shorne
 * Changed to a buffering type arrangement
 *
 * Revision 1.3  2007/02/18 18:39:28  shorne
 * Added PWaitAndSignal
 *
 * Revision 1.2  2006/06/20 09:23:56  csoutheren
 * Applied patch 1465192
 * Fix pwlib make files, and config for unix
 *
 * Revision 1.1  2006/02/26 09:19:17  shorne
 * AEC moved to seperate library
 *
 * Revision 1.1  2006/01/26 08:05:03  shorne
 * Added AEC support
 *
 *
 */

#include "StdAfx.h"
#include "paec.h"

extern "C" {
#include "speex_echo.h"
#include "speex_preprocess.h"
}

///////////////////////////////////////////////////////////////////////////////

PAec::PAec(int _clock, int _sampletime)
  : clockrate(_clock), sampleTime(_sampletime)
{

  echoState = NULL;
  preprocessState = NULL;

  e_buf = NULL;
  echo_buf = NULL;
  ref_buf = NULL;
  noise = NULL;

  receiveReady = FALSE;

  bufferTime = sampleTime*2;  // Indicating it takes sampletime to play and sampletime to record
  minbuffer = bufferTime - sampleTime;
  maxbuffer = bufferTime + 2*sampleTime;  // Indicating that 
  //lastTimeStamp = PTimer::Tick();

  silenceBuffer = MP_NEW_ARR( BYTE, 480);
  memset( silenceBuffer, 0, 480);
  //PTRACE(3, "AEC\tcreated AEC " << clockrate << " hz " << " buffer Size " << bufferTime << " ms." );
}


PAec::~PAec()
{
  //PWaitAndSignal m(readwritemute);

  if (echoState) {
    speex_echo_state_destroy(echoState);
    echoState = NULL;
  }
  
  if (preprocessState) {
    speex_preprocess_state_destroy(preprocessState);
    preprocessState = NULL;
  }

  if (ref_buf)
  {
    MP_DELETE_ARR(ref_buf);
  }
  if (e_buf)
  {
    MP_DELETE_ARR(e_buf);
  }
  if (echo_buf)
  {
    MP_DELETE_ARR(echo_buf);
  }
  if (noise)
  {
    MP_DELETE_ARR(noise);
  }

  MP_DELETE_ARR(silenceBuffer);
  //echo_chan->Close();
  //MP_DELETE(echo_chan);

}


void PAec::Receive(BYTE * buffer, unsigned & length)
{
  //PWaitAndSignal m(readwritemute);

  if (length == 0)
	  return;

  /* Buffer has run dry adjust the timestamp to now */
  //if (echo_chan->GetSize() == 0) 
	//  lastTimeStamp = PTimer::Tick();

  /* Write to the soundcard, and write the frame to the PQueueChannel */
  //if ( echo_chan->Write( silenceBuffer, length))
  //if (echo_chan->Write(buffer, length))
	//  rectime.Enqueue(new PTimeInterval(PTimer::Tick()));	  

  if (!receiveReady)
     receiveReady = TRUE;

}

void PAec::Send(BYTE * buffer, unsigned & length)
{
  //PWaitAndSignal m(readwritemute);

  // Audio Recording to send 
// Inialise the Echo Canceller
  if (echoState == NULL) {
    echoState = speex_echo_state_init(length/sizeof(short), 32*length);
	echo_buf = (spx_int16_t *)MP_NEW_ARR( byte,length);
	noise = (spx_int16_t *)MP_NEW_ARR(byte,(length/sizeof(short)+1)*sizeof(float));
    e_buf = (spx_int16_t *)MP_NEW_ARR(byte,length);
    ref_buf = (spx_int16_t *)MP_NEW_ARR(byte,length);
    preprocessState = speex_preprocess_state_init(length/sizeof(short), clockrate);
	int k=1;
    speex_preprocess_ctl(preprocessState, SPEEX_PREPROCESS_SET_DENOISE, &k);
    speex_preprocess_ctl(preprocessState, SPEEX_PREPROCESS_SET_DEREVERB, &k);
	receiveReady = TRUE; // добавлено Vladimir
  }

  if (!receiveReady)
	  return;

 /* if (echo_chan->GetSize() == 0) 
	  return;

  memcpy((spx_int16_t*)ref_buf, buffer, length);
	    
  // Read from the PQueueChannel a reference echo frame
  PTimeInterval rec = lastTimeStamp;
  PInt64 time =  (PTimer::Tick()).GetMilliSeconds();
  PInt64 diff =  time - rec.GetMilliSeconds();

  if (diff < minbuffer) {
	  PTRACE(6,"AEC\tBuffer Time too short ignoring " << diff << " ms ");
	  return;
  }

  if (diff > maxbuffer) {
    do { 
      if (rectime.GetSize() == 0)      
		return;
      PTimeInterval *prec = (PTimeInterval *) rectime.GetAt( 0); 		  
      if ( prec == NULL)
         return;      
      rec = *( prec);
	  rectime.Dequeue();	  	  
	  MP_DELETE(prec);
	    	 
	  if  (time > rec.GetMilliSeconds()) {
	    PTRACE(6,"AEC\tBuffer Time too large " << diff << " ms  Max:" << maxbuffer << " Ignoring Packet!");
	    diff = time - rec.GetMilliSeconds();
		if (!echo_chan->Read((short *)echo_buf, length)) {
			speex_preprocess(preprocessState, (spx_int16_t*)ref_buf, NULL);
			memcpy(buffer, (spx_int16_t*)ref_buf, length);
			PTRACE(3,"AEC\tExiting Buffer Read error.");
			return;
		} 
	 } else return;
   } while (diff > maxbuffer);
  } else {
     if (!echo_chan->Read((short *)echo_buf, length)) {
        speex_preprocess(preprocessState, (spx_int16_t*)ref_buf, NULL);
        memcpy(buffer, (spx_int16_t*)ref_buf, length);
		PTRACE(6,"AEC\tExiting Buffer Read error.");
	    return;
     } 
	 if (rectime.GetSize() == 0)     
		 return;

     PTimeInterval *prec = (PTimeInterval *) rectime.GetAt( 0);      	 
	 if ( prec == NULL)
         return;     
	 rec = *( prec);
	 rectime.Dequeue();	 	 
	 MP_DELETE(prec);
  } 

  PTRACE(6,"AEC\tBuffer Time difference " << diff << " ms "); 

  lastTimeStamp = rec;

  // Cancel the echo in this frame 
  speex_echo_cancel(echoState, (short *)ref_buf, (short *)echo_buf, (short *)e_buf, (float *)noise);

  // Suppress the noise & reverb
  speex_preprocess(preprocessState, (spx_int16_t*)e_buf, (float *)noise);

  lastTimeStamp = rec;

  // Use the result of the echo cancelation as capture frame 
  memcpy(buffer, e_buf, length);*/


  memcpy((spx_int16_t*)ref_buf, buffer, length);
  // Suppress the noise & reverb
  speex_preprocess(preprocessState, (spx_int16_t*)ref_buf, NULL);
  memcpy(buffer, ref_buf, length);
}
