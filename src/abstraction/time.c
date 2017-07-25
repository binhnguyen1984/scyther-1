/*
 * time.c

 *
 *  Created on: Jul 22, 2014
 *      Author: nguyen
 */
#include "time.h"
long
timevaldiff (struct timeval *starttime, struct timeval *finishtime)
{
  long msec;
  msec = (finishtime->tv_sec - starttime->tv_sec) * 1000;
  msec += (finishtime->tv_usec - starttime->tv_usec) / 1000;
  return msec;
}
