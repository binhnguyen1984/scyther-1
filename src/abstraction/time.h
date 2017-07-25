/*
 * time.h
 *
 *  Created on: Jul 22, 2014
 *      Author: nguyen
 */

#ifndef TIME_H_
#define TIME_H_
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

long timevaldiff (struct timeval *starttime, struct timeval *finishtime);
#endif /* TIME_H_ */
