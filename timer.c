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
#include <glib.h>
#include <glib/gprintf.h>

static clock_t startClock;
static clock_t stopClock;
static time_t startTimer;
static time_t stopTimer;
static time_t currentTimer;
static time_t oldCurrentTimer;
static double elapseTimer;
static int uniqInstance = 1;

int timerStartClock()
{
    // g_printf("\nDEBUG :: timerStartClock startClock = %ld\n", clock( ));
    return ((startClock = clock()) != -1) ? 1 : 0;
}

int timerStopClock()
{
    stopClock = clock();
    int h, m, s, ms;
    char time_str[100];
    h = m = s = ms = 0;
    ms = (double)(stopClock - startClock) / CLOCKS_PER_SEC * 1000; // promote the fractional part to milliseconds
    h = ms / 3600000;
    ms -= (h * 3600000);
    m = ms / 60000;
    ms -= (m * 60000);
    s = ms / 1000;
    ms -= (s * 1000);

    g_printf("\nDEBUG :: timerStopClock startClock = %ld\n", startClock);
    g_printf("\nDEBUG :: timerStopClock stopClock = %ld\n", stopClock);
    // g_printf("\nDEBUG :: CLOCKS_PER_SEC = %ld\n", CLOCKS_PER_SEC);
    sprintf(time_str, "%02i:%02i:%02i.%03i", h, m, s, ms);
    g_printf("\nDEBUG :: Elapse = %s", time_str );
    g_printf("\nDEBUG :: Fin Elapse"  );
    
    return (stopClock != -1) ? 1 : 0;
}

double
timerGetElapseClock()
{
    return (stopClock >= startClock) ? (double)(stopClock - startClock) / CLOCKS_PER_SEC : -1; // CLOCKS_PER_SEC
}

int timerSetStartTimer()
{
    if (uniqInstance)
    {
        startTimer = time(NULL);
        stopTimer = startTimer;
        oldCurrentTimer = startTimer;
        uniqInstance--;
        return 0;
    }
    else
        return -1;
}

int timerSetStopTimer()
{
    if (timerSetStartTimer())
    {
        stopTimer = time(NULL);
        uniqInstance = 1;
        return 0;
    }
    else
    {
        stopTimer = 0;
        return -1;
    }
}

int timerSetElapseTimer()
{
    if (timerSetStartTimer())
    {
        currentTimer = time(NULL);
        elapseTimer = difftime(currentTimer, oldCurrentTimer);
        oldCurrentTimer = currentTimer;
        return 0;
    }
    else
    {
        elapseTimer = 0.0;
        return -1;
    }
}

double
timerGetElapseTimer()
{
    return (elapseTimer >= 0.0) ? elapseTimer : -1;
}

double
timerGetTotalTimer()
{
    return (currentTimer > startTimer) ? difftime(currentTimer, startTimer) : -1;
}

pfmtTime
timerGetMktime(double rawTime)
{
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
    return NULL;
}
