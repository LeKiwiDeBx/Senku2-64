/*
 * peg.h
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */

#ifndef _PEG_H_
#define _PEG_H_

#define MAX_STRUCT_PEG 5 // struct peg North/Est/South/West + celle qui est selectionnée

typedef struct s_Peg_Coord
{
    int row;
    int column;
} Peg_Coord;

typedef enum e_Peg_Direction
{
    CENTER,      /* 0 Peg central par rapport aux 4 directions */
    NORTH,       /* 1 direction où aller */
    EAST,        /* 2 					*/
    SOUTH,       /* 3 					*/
    WEST,        /* 4 					*/
    DEFAULT,     /* 5 emplacement par defaut */
    UNDO = 666,  /* 666 Devil Number =:> Peg à undo */
    NO_MOVE = 10 /* on reclique sur le meme Peg (GDK) */
} Peg_Direction;

typedef struct s_Peg
{
    Peg_Coord coord;
    Peg_Direction direction;
    int bydefault;
} Peg;

/**
 * @brief sert à memoriser les mouvements
 *
 */
typedef struct s_Peg_Memento
{
    Peg_Coord coordStart;
    Peg_Coord coordBetween;
    Peg_Coord coordEnd;
} Peg_Memento;

extern Peg tab_Peg[MAX_STRUCT_PEG];
void pegFlushPeg();
void pegFirstPeg(int, int);
int pegBuildPeg(int, int, Peg_Direction);
int pegReturnDefaultPeg();
#endif
