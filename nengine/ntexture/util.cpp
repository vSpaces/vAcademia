/******************************************************************************
 * Fast DXT - a realtime DXT compression tool
 *
 * Author : Luc Renambot
 *
 * Copyright (C) 2007 Electronic Visualization Laboratory,
 * University of Illinois at Chicago
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either Version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License along
 * with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *****************************************************************************/

#include "util.h"


#if defined(WIN32)
#include <io.h>
LARGE_INTEGER perf_freq;
LARGE_INTEGER perf_start;
HANDLE win_err; // stderr in a console
#elif defined(__APPLE__)
#include <mach/mach_time.h>
static double perf_conversion = 0.0;
static uint64_t perf_start;
#else
struct timeval tv_start;
#endif

#if !defined(WIN32)
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <fcntl.h>


void aInitialize()
{
#if defined(WIN32)
	QueryPerformanceCounter(&perf_start);
	QueryPerformanceFrequency(&perf_freq);
	AllocConsole();
	win_err =  GetStdHandle(STD_ERROR_HANDLE);
#elif defined(__APPLE__)
	if( perf_conversion == 0.0 )
	{
		mach_timebase_info_data_t info;
		kern_return_t err = mach_timebase_info( &info );

		//Convert the timebase into seconds
		if( err == 0  )
			perf_conversion = 1e-9 * (double) info.numer / (double) info.denom;
	}
		// Start of time
	perf_start = mach_absolute_time();

		// Initialize the random generator
	srand(getpid());
#else
		// Start of time
	gettimeofday(&tv_start,0);
		// Initialize the random generator
	srand(getpid());
#endif
}

double aTime()
// return time since start of process in seconds
{
#if defined(WIN32)
    LARGE_INTEGER perf_counter;
#else
    struct timeval tv;
#endif

#if defined(WIN32)
        // Windows: get performance counter and subtract starting mark
	QueryPerformanceCounter(&perf_counter);
	return (double)(perf_counter.QuadPart - perf_start.QuadPart) / (double)perf_freq.QuadPart;
#elif defined(__APPLE__)
    uint64_t difference = mach_absolute_time() - perf_start;
    return perf_conversion * (double) difference;
#else
        // UNIX: gettimeofday
	gettimeofday(&tv,0);
	return (double)(tv.tv_sec - tv_start.tv_sec) + (double)(tv.tv_usec - tv_start.tv_usec) / 1000000.0;
#endif
}

void* aAlloc(size_t const n)
{
	void* result;
#if defined(WIN32)
	result = LocalAlloc(0,n);
#else
	result = malloc(n);
#endif
	// Filling with zeros
	memset(result, 0, n);

	return result;
}

void aFree(void* const p)
{
#if defined(WIN32)
	LocalFree(p);
#else
	if (p)
        free(p);
    else
        aError("Alloc> Trying to free a NULL pointer\n");
#endif
}

#if defined(WIN32)
float
drand48(void)
{
	return (((float) rand()) / RAND_MAX);
}
#endif


void *aligned_malloc(size_t size, size_t align_size) {

  char *ptr,*ptr2,*aligned_ptr;
  int align_mask = (int)align_size - 1;

  ptr=(char *)malloc(size + align_size + sizeof(int));
  if(ptr==NULL) return(NULL);

  ptr2 = ptr + sizeof(int);
  aligned_ptr = ptr2 + (align_size - ((size_t)ptr2 & align_mask));


  ptr2 = aligned_ptr - sizeof(int);
  *((int *)ptr2)=(int)(aligned_ptr - ptr);

  return(aligned_ptr);
}

void aligned_free(void *ptr)
{
	int *ptr2=(int *)ptr - 1;
	ptr = (char*)ptr - *ptr2;
	free(ptr);
}