/*
 * Créé le 06-09-2016 11:57:48 CEST
 * peg.c
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "peg.h"

Peg tab_Peg[MAX_STRUCT_PEG] = {
    {
        {0, 0}, UNDO, 0
    },
    {
        {0, 0}, UNDO, 0
    },
    {
        {0, 0}, UNDO, 0
    },
    {
        {0, 0}, UNDO, 0
    },
    {
        {0, 0}, UNDO, 0
    }
} ;

void
pegFlushPeg( ) {
    int i ;
    for (i = 0 ; i < MAX_STRUCT_PEG ; i++) {
        tab_Peg[i].coord.row = 0 ;
        tab_Peg[i].coord.column = 0 ;
        tab_Peg[i].direction = UNDO ;
        tab_Peg[i].bydefault = 0 ;
    }
}

void
pegFirstPeg( int row, int column ) {
    if (tab_Peg[0].direction == UNDO) {
        tab_Peg[0].direction = CENTER ;
        tab_Peg[0].coord.row = row ;
        tab_Peg[0].coord.column = column ;
    }
}

int
pegBuildPeg( int row, int column, Peg_Direction direction ) {
    int i ;
    for (i = 1 ; i < MAX_STRUCT_PEG ; i++) {
        if (tab_Peg[i].direction == UNDO) {
            tab_Peg[i].bydefault = (i == 1) ? 1 : 0 ;
            tab_Peg[i].direction = direction ;
            tab_Peg[i].coord.row = row ;
            tab_Peg[i].coord.column = column ;
            return i ;
        }
    }
    return 0 ;
}

int
pegReturnDefaultPeg( ) {
    int i ;
    for (i = 0 ; i < MAX_STRUCT_PEG ; i++) {
        if (tab_Peg[i].bydefault) return i ;
    }
    return 0 ;
}

