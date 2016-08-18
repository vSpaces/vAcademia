/*
  Copyright 2005, VRCO, Inc.
  ALL RIGHTS RESERVED
  
  UNPUBLISHED -- Rights reserved under the copyright laws of the United
  States.   Use of a copyright notice is precautionary only and does not
  imply publication or disclosure.
  
  THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY
  INFORMATION OF VRCO, INC. ANY DUPLICATION, MODIFICATION,
  DISTRIBUTION, OR DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY
  PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF VRCO, INC.
  
  trackdAPI.h
  
  Version 2.2

*/

#ifndef TRACKD_API_H
#define TRACKD_API_H


#include <string>

/*#if  (WIN32 &&TRACKD_DLL_EXPORTS) 
#define TRACKD_EXPORT_API _declspec(dllexport)
#else
#define TRACKD_EXPORT_API
#endif*/

#define TRACKD_EXPORT_API _declspec(dllexport)


class TRACKD_EXPORT_API TrackdError  {
  
 public:
  
  TrackdError( const char* );
  TrackdError( const std::string& );
  
  ~TrackdError();
  
  const char* what() const;
  
 private:
  
  std::string m_error;
};


class TRACKD_EXPORT_API TrackerReader {
  
 public:
  
  /*  The constructor attaches a pointer to the tracker shared 
      memory segment. To create a tracker reader the decimal 
      value of the shared memory key must be provided.
  */
  TrackerReader(int)
    throw( TrackdError ) ;
  
  /* Detaches from the tracker shared memory segment */
  virtual ~TrackerReader();
  
  /*  Returns the number of sensors the tracker daemon is storing */
  int trackdGetNumberOfSensors(void);
  
  /* Gets the three float positions of sensor 'id' where id ranges
     from 0 to (number of sensors - 1)
  */
  void trackdGetPosition(int id, float* pos)
    throw ( TrackdError );
  
  /* Gets the three float euler orientations of sensor 'id' where id 
     ranges from 0 to (number of sensors - 1)
  */
  void trackdGetEulerAngles(int id, float* orn)
    throw ( TrackdError );
  
  /* Gets the transformations matrix of sensor 'id' where id 
     ranges from 0 to (number of sensors - 1)
  */
  void trackdGetMatrix(int id, float mat[4][4])
    throw ( TrackdError );
  
  /* Returns the current version of the trackdAPI */
  const char* getTrackdAPIVersion();
  
 private:
  
  TrackerReader(){ }
  
void* m_tracker;
  static const char* s_version;
#ifdef WIN32
  struct CShmh* m_CShmh1;
#endif
  
};

class TRACKD_EXPORT_API ControllerReader {
  
 public:
  
  /* The constructor attaches a pointer to the controller 
     shared memory segment. To create a tracker reader the 
     decimal value of the shared memory key must be provided.
  */
  ControllerReader(int)
    throw ( TrackdError );
  
  /* Detaches from the controller shared memory segment */
  virtual ~ControllerReader();
  
  /* Returns the number of valuators the tracker daemon is storing */
  int trackdGetNumberOfValuators();
  
  /* Returns the number of buttons the tracker daemon is storing */
  int trackdGetNumberOfButtons();
  
  /* Returns value of valuator[id] from the tracker daemon. */
  float trackdGetValuator(int id)
    throw ( TrackdError );
  
  /* Returns the value of button[id] from the tracker daemon. */
  int trackdGetButton(int id)
    throw ( TrackdError );
  
  /* Returns the current version of the trackdAPI */
  const char* getTrackdAPIVersion();
  
 private:
  
  ControllerReader(){ assert( false ); }
  void* m_controller; 
  static const char* s_version;
#ifdef WIN32   
  struct CShmh* m_CShmh1;
#endif
  
};



#endif /* TRACKD_API_H */
