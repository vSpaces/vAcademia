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

#ifndef WIN32
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#endif 


#if  (WIN32 &&TRACKD_DLL_EXPORTS) /* CAVE */
#define TRACKD_EXPORT_API _declspec(dllexport)
#else
#define TRACKD_EXPORT_API
#endif



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Attaches a tracker pointer to the shared memory specified by shmKey
   and returns that pointer as a void*. It returns NULL if it fails. 
*/
TRACKD_EXPORT_API void* trackdInitTrackerReader(int);

/* Returns the number sensors the tracker daemon is storing */
TRACKD_EXPORT_API int trackdGetNumberOfSensors(void* tracker);

/* These 3 functions return 1 if successful, 0 if they fail */

/* Gets the position values for sensor[id] */
TRACKD_EXPORT_API int trackdGetPosition(void* tracker, int id, float* pos);

/* Gets the euler angle values for sensor[id] */
TRACKD_EXPORT_API int trackdGetEulerAngles(void* tracker, int id, float* orn);

/* Gets ths transformation matrix for sensor[id] */ 
TRACKD_EXPORT_API  int trackdGetMatrix(void* tracker, int id, float mat[4][4]);


/* Attaches a controller pointer to the shared memory specified by shmKey
   and returns that pointer as a void*. It returns NULL if it fails. 
*/
TRACKD_EXPORT_API void* trackdInitControllerReader(int);

/* Returns the number of valuators the tracker daemon is storing */
TRACKD_EXPORT_API int trackdGetNumberOfValuators(void* controller);

/* Returns the number of buttons the tracker daemon is storing */
TRACKD_EXPORT_API int trackdGetNumberOfButtons(void* controller);



/*Both these functions return 0 when id >= numButtons or numValutors */

/* Returns value of valuator[id] from the tracker daemon. */
TRACKD_EXPORT_API float trackdGetValuator(void* controller, int id);

/* Returns the value of button[id] from the tracker daemon. */
TRACKD_EXPORT_API int trackdGetButton(void* controller, int id);

/* Returns the current version of the trackdAPI */
TRACKD_EXPORT_API const char* getTrackdAPIVersion();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TRACKD_API_H */
