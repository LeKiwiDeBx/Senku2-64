/*
 * Créé le 15-10-2016 11:26:32 CEST
 *
 * timer.c
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */

#include "timer.h"
#include <stdio.h>
#include <math.h>

static clock_t startClock ;
static clock_t stopClock ;
static time_t startTimer ;
static time_t stopTimer ;
static time_t currentTimer ;
static time_t oldCurrentTimer ;
static double elapseTimer ;
static int uniqInstance = 1 ;

int
timerStartClock( ) {
    return ( (signed int) (startClock = clock( )) != -1) ? 1 : 0 ;
}

int
timerStopClock( ) {
    return ( (signed int) (stopClock = clock( )) != -1) ? 1 : 0 ;
}

double
timerGetElapseClock( ) {
    return (stopClock >= startClock) ? (double) (stopClock - startClock) / CLOCKS_PER_SEC : -1 ;
}

int
timerSetStartTimer( ) {
    if (uniqInstance) {
        startTimer = time( NULL ) ;
        stopTimer = startTimer ;
        oldCurrentTimer = startTimer ;
        uniqInstance-- ;
        return 0 ;
    }
    else return -1 ;
}

int
timerSetStopTimer( ) {
    if (timerSetStartTimer( )) {
        stopTimer = time( NULL ) ;
        uniqInstance = 1 ;
        return 0 ;
    }
    else {
        stopTimer = 0 ;
        return -1 ;
    }
}

int
timerSetElapseTimer( ) {
    if (timerSetStartTimer( )) {
        currentTimer = time( NULL ) ;
        elapseTimer = difftime( currentTimer, oldCurrentTimer ) ;
        oldCurrentTimer = currentTimer ;
        return 0 ;
    }
    else {
        elapseTimer = 0.0 ;
        return -1 ;
    }
}

double
timerGetElapseTimer( ) {
    return (elapseTimer >= 0.0) ? elapseTimer : -1 ;
}

double
timerGetTotalTimer( ) {
    return (currentTimer > startTimer) ? difftime( currentTimer, startTimer ) : -1 ;
}

pfmtTime
timerGetMktime( double rawTime ) {
    /*
    fmtTime sfmtTime = {
                            0, //0 minute
                            0  //0 seconde
                        };
    pfmtTime pfm = &sfmtTime;
    double x,fp, ip ;
            if( rawTime ){
                x = rawTime / 60.0 ;
                fp = modf(x, &ip) ;
                fp *= 60 ;
            }
            else{
                ip = 0 ;
                fp = 0 ;
            }
    pfm->mkt_sec = fp ;
    pfm->mkt_min = ip ;
    return pfm ;
     */
    return NULL ;
}
