/*
 * board.h
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */

#ifndef _BOARD_H_
#define _BOARD_H_
#include "matrix.h"

typedef struct s_Board
{
    matrixOfBoard *set;
} Board;

int boardInitNew();
int boardPlay();
#endif
