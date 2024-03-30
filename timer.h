/*
 * timer.h
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include <time.h>
typedef struct s_fmtTime {
    double mkt_min;
    double mkt_sec;
} fmtTime;
 
typedef fmtTime *pfmtTime;

int timerStartClock();
int timerStopClock();
double timerGetElapseClock();
int timerSetStartTimer();
int timerSetStopTimer();
int timerSetElapseTimer();
double timerGetElapseTimer();
double timerGetTotalTimer();
pfmtTime timerGetMktime(double);
#endif
