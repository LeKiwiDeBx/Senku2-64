/*
 * Créé le 20-09-2016 15:47:19 CEST
 */
/*
 * matrix.c
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memcpy() */
#include "matrix.h"
#include "peg.h"
#include "memento.h"
/**
 * headers GTK/Glib
 */
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>

#include <glib/gi18n.h>
// #include <glib/gi18n-lib.h>
#include <libintl.h>
#include <locale.h>

/* Matrix matrixEnglish = {  // TEST carre gauche
//	0  1  2  3  4  5  6  7  8  9  10
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, //0
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},	//1
	{-1,-1,-1,-1, 0, 1, 0,-1,-1,-1,-1},	//2
	{-1,-1,-1,-1, 0, 1, 0,-1,-1,-1,-1},	//3
    {-1,-1, 1, 0, 1, 0, 1, 0, 0,-1,-1},	//4
    {-1,-1, 1, 0, 1, 1, 1, 0, 0,-1,-1},	//5
    {-1,-1, 0, 0, 1, 1, 0, 0, 0,-1,-1},	//6
    {-1,-1,-1,-1, 0, 1, 0,-1,-1,-1,-1},	//7
	{-1,-1,-1,-1, 0, 0, 0,-1,-1,-1,-1},	//8
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},	//9
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}	//10
} ;
 */
/* Matrix matrixEnglish = {  // TEST TEST TEST GAGNANT
//	0  1  2  3  4  5  6  7  8  9  10
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, //0
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},	//1
	{-1,-1,-1,-1, 0, 1, 0,-1,-1,-1,-1},	//2
	{-1,-1,-1,-1, 0, 1, 0,-1,-1,-1,-1},	//3
   {-1,-1, 0, 0, 1, 0, 1, 0, 0,-1,-1},	//4
   {-1,-1, 0, 0, 0, 1, 1, 0, 0,-1,-1},	//5
   {-1,-1, 0, 0, 0, 1, 0, 0, 0,-1,-1},	//6
   {-1,-1,-1,-1, 0, 1, 0,-1,-1,-1,-1},	//7
	{-1,-1,-1,-1, 0, 0, 0,-1,-1,-1,-1},	//8
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},	//9
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}	//10
} ; */
/**
 * @brief schema du shape dit Anglais
 *         matrice carrée 11 x 11
 * @Matrix type de tableau 2D de integer (int)
 */
Matrix matrixEnglish = {
    // 0  1  2  3  4  5  6  7  8  9  10
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 0
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 1
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 2
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 3
    {-1, -1, 1, 1, 1, 1, 1, 1, 1, -1, -1},        // 4
    {-1, -1, 1, 1, 1, 0, 1, 1, 1, -1, -1},        // 5
    {-1, -1, 1, 1, 1, 1, 1, 1, 1, -1, -1},        // 6
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 7
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 8
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 9
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}  // 10
};

Matrix matrixGerman = {
    // 0  1  2  3  4  5  6  7  8  9  10
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 0
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 1
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 2
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 3
    {-1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1},          // 4
    {-1, 1, 1, 1, 1, 0, 1, 1, 1, 1, -1},          // 5
    {-1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1},          // 6
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 7
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 8
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 9
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}  // 10
};

Matrix matrixDiamond = {
    // 0  1  2  3  4  5  6  7  8  9  10
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 0
    {-1, -1, -1, -1, -1, 1, -1, -1, -1, -1, -1},  // 1
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 2
    {-1, -1, -1, 1, 1, 1, 1, 1, -1, -1, -1},      // 3
    {-1, -1, 1, 1, 1, 1, 1, 1, 1, -1, -1},        // 4
    {-1, 1, 1, 1, 1, 0, 1, 1, 1, 1, -1},          // 5
    {-1, -1, 1, 1, 1, 1, 1, 1, 1, -1, -1},        // 6
    {-1, -1, -1, 1, 1, 1, 1, 1, -1, -1, -1},      // 7
    {-1, -1, -1, -1, 1, 1, 1, -1, -1, -1, -1},    // 8
    {-1, -1, -1, -1, -1, 1, -1, -1, -1, -1, -1},  // 9
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}  // 10
};

Matrix matrixCopy;
char (*pMatrixLoad)[VER_MAX] = NULL;
matrixOfBoard currentMatrixOfBoard;

////////////////////////// DEBUG /////////////////////////
static void
    __displayMatrix(Matrix);

static char *__selectItem(int);

/*
static void
__displayLoadChoice(const char *) ;

static void
__displayPegFromTo(int, int, int, Peg_Direction, int);
 */

static int
__getCoordPegWhereWeGo(int);

/*
 *
 * @brief: Charge la matrice
 * @param int choice numéro choisit de selection du shape
 * @return 1 si Ok, 0 si Nok
 *
 */
int matrixLoad(int choice)
{
    char *nameShape[] = {N_("Unknown"), N_("English"), N_("German"), N_("Diamond")};
    Matrix *matrixType[] = {(Matrix *)NULL, (Matrix *)matrixEnglish, (Matrix *)matrixGerman, (Matrix *)matrixDiamond};
    if (choice >= 0 && choice <= 4)
    {
        switch (choice)
        {
        case 1:
        case 2:
        case 3:
            currentMatrixOfBoard.pShape = matrixType[choice];
            break;
        case 4:
            //			printf("\n Thank you, Good bye! ;)" );
            exit(EXIT_SUCCESS);
            break;
        default:
            return 0;
        }
        currentMatrixOfBoard.name = nameShape[choice];
        //		__displayLoadChoice(currentMatrixOfBoard.name) ;
        currentMatrixOfBoard.id = choice;
        memcpy(matrixCopy, currentMatrixOfBoard.pShape, HOR_MAX * VER_MAX * sizeof(char));
        pMatrixLoad = matrixCopy;
        currentMatrixOfBoard.nbMaxPeg = matrixCountRemainPeg();
        //		__displayMatrix(matrixCopy) ;
        return 1;
    }
    return 0;
}

/*
 * test si coord est un Peg selectionnable pour prendre un pion
 * */
int matrixSelectPeg(int row, int column)
{
    Peg_Direction direction;
    int nbMove = 0; // nombre de mouvement possible
    int row_arrival, column_arrival;
    int PegGoTo = 0; // indice du trou tab_Peg[] ou aller
    pegFlushPeg();
    if (pMatrixLoad[row][column] == 1)
    {
        for (direction = NORTH; direction <= WEST; direction++)
        {                      // determine pour les 4 directions
            row_arrival = row; // par defaut
            column_arrival = column;
            switch (direction)
            {
            case NORTH: // conditions pour prendre un peg
                if (pMatrixLoad[row - 1][column] == 1 && pMatrixLoad[row - 2][column] == 0)
                    row_arrival = row - 2;
                break;
            case SOUTH:
                if (pMatrixLoad[row + 1][column] == 1 && pMatrixLoad[row + 2][column] == 0)
                    row_arrival = row + 2;
                break;
            case WEST:
                if (pMatrixLoad[row][column - 1] == 1 && pMatrixLoad[row][column - 2] == 0)
                    column_arrival = column - 2;
                break;
            case EAST:
                if (pMatrixLoad[row][column + 1] == 1 && pMatrixLoad[row][column + 2] == 0)
                    column_arrival = column + 2;
                break;
            case DEFAULT:
            case CENTER:
            case UNDO:
            default:;
                break;
            }
            if (row_arrival != row || column_arrival != column)
            {
                nbMove++;
                PegGoTo = pegBuildPeg(row_arrival, column_arrival, direction);
                //				__displayPegFromTo(PegGoTo, row, column, direction, nbMove);
            }
            else
            {
                pegFirstPeg(row, column);
            }
        }
    }
    return nbMove;
}

int matrixUpdate(Peg_Direction where)
{
    int i;
    int coefRow = 0, coefColumn = 0;
    int row, column;
    Peg_Memento pegMemento;

    if (where == DEFAULT)
    {
        i = pegReturnDefaultPeg();
        where = tab_Peg[i].direction;
    }
    else if (where == UNDO)
    {
        //        __displayMatrix(pMatrixLoad);
        return 0;
    }
    else
    {
        i = __getCoordPegWhereWeGo(where);
        if (!i)
        {
            //			printf("Is not a valid direction, do NOTHING ! :((\n");
            //			__displayMatrix(pMatrixLoad);
            return 0;
        }
    }
    row = tab_Peg[i].coord.row;
    column = tab_Peg[i].coord.column;
    switch (where)
    {
    case NORTH:
        coefRow = 1;
        coefColumn = 0; // coefficient d'effacement
        break;
    case SOUTH:
        coefRow = -1;
        coefColumn = 0;
        break;
    case WEST:
        coefRow = 0;
        coefColumn = 1;
        break;
    case EAST:
        coefRow = 0;
        coefColumn = -1;
        break;
    case NO_MOVE:
        row = 0;
        column = 0;
        coefRow = 0;
        coefColumn = 0;
        return 0;
    default:
        return 0;
    }
    pMatrixLoad[row][column] = 1;
    pMatrixLoad[row + coefRow][column + coefColumn] = 0;         // erase
    pMatrixLoad[row + 2 * coefRow][column + 2 * coefColumn] = 0; //
    // mecanisme memento UNDO:: memorisation
    pegMemento.coordStart.row = row + 2 * coefRow;
    pegMemento.coordStart.column = column + 2 * coefColumn;
    pegMemento.coordBetween.row = row + coefRow;
    pegMemento.coordBetween.column = column + coefColumn;
    pegMemento.coordEnd.row = row;
    pegMemento.coordEnd.column = column;
    originatorSet(pegMemento);
    caretakerAddMemento(originatorSaveToMemento());
    // fin memento UNDO
    //	__displayMatrix(pMatrixLoad);
    return 1;
}

int matrixCanMovePeg()
{
    Peg_Direction direction;
    int row, column;
    for (row = 0; row < HOR_MAX; row++)
    {
        for (column = 0; column < VER_MAX; column++)
        {
            if (pMatrixLoad[row][column] == 1)
            {
                for (direction = NORTH; direction <= WEST; direction++)
                {
                    switch (direction)
                    {
                    case NORTH: // conditions pour prendre un peg
                        if (pMatrixLoad[row - 1][column] == 1 && pMatrixLoad[row - 2][column] == 0)
                            return 1;
                        break;
                    case SOUTH:
                        if (pMatrixLoad[row + 1][column] == 1 && pMatrixLoad[row + 2][column] == 0)
                            return 1;
                        break;
                    case WEST:
                        if (pMatrixLoad[row][column - 1] == 1 && pMatrixLoad[row][column - 2] == 0)
                            return 1;
                        break;
                    case EAST:
                        if (pMatrixLoad[row][column + 1] == 1 && pMatrixLoad[row][column + 2] == 0)
                            return 1;
                        break;
                    case DEFAULT:
                    case CENTER:
                    case UNDO:
                    default:;
                        break;
                    }
                }
            }
        } // for column
    }     // for row
    return 0;
}

int matrixCountRemainPeg()
{
    int row, column, count = 0;
    for (row = 0; row < HOR_MAX; row++)
    {
        for (column = 0; column < VER_MAX; column++)
        {
            if (pMatrixLoad[row][column] == 1)
                count++;
        }
    }
    return count;
}

/*
 * rotation de la matrice
 * switch row and column VER_MAX = 3 and HOR_MAX = 3
 * A B C     A D G
 * D E F     B E H
 * G H I     C F I
 * reverse each element of column by row
 * A D G     G D A
 * B E H     H E B
 * C F I     I F C
 * */

/// @brief
/// @param where
/// @return
int matrixRotate(theta where)
{
    char elmtemp;
    const int COL_MAX = VER_MAX - 1; // 2
    const int ROW_MAX = HOR_MAX - 1; // 2

    for (int row = 0; row < ROW_MAX / 2; row++)
    {
        for (int column = row; column < COL_MAX - row; column++)
        {
            elmtemp = pMatrixLoad[row][column];
            pMatrixLoad[row][column] = (where == THETA_CLOCKWISE) ? pMatrixLoad[COL_MAX - column][row] : pMatrixLoad[column][ROW_MAX - row];

            if (where == THETA_CLOCKWISE)
                pMatrixLoad[COL_MAX - column][row] = pMatrixLoad[ROW_MAX - row][COL_MAX - column];
            else
                pMatrixLoad[column][ROW_MAX - row] = pMatrixLoad[ROW_MAX - row][COL_MAX - column];

            pMatrixLoad[ROW_MAX - row][COL_MAX - column] = (where == THETA_CLOCKWISE) ? pMatrixLoad[column][ROW_MAX - row] : pMatrixLoad[COL_MAX - column][row];
            if (where == THETA_CLOCKWISE)
                pMatrixLoad[column][ROW_MAX - row] = elmtemp;
            else
                pMatrixLoad[COL_MAX - column][row] = elmtemp;
        }
    }
    mementoTransformRotate(where); //maj du memento apres rotation
    ////////////////////////DEBUG//////////////////////////
    __displayMatrix(pMatrixLoad);
    ////////////////////////END OF DEBUG//////////////////////////
    return 0;
}

/*
void
__displayPegFromTo(int whereToGo,int row, int column, Peg_Direction direction, int nbMove){
    char *sDir[] = {"","[N|n]ORTH","[E|e]AST","[S|s]OUTH","[W|w]EST"} ;
    sDir[0] = (nbMove == 1)? "by default, type Enter":"";
    printf("|----------------------------------------------------|\n") ;
    printf("| From Peg [row: %d column: %d] direction: %s\n",row,column,sDir[direction]);
    printf("| => Go to [row: %d column: %d] %s\n",tab_Peg[whereToGo].coord.row,tab_Peg[whereToGo].coord.column, sDir[0]);
    printf("|----------------------------------------------------|\n") ;
}
 */

int __getCoordPegWhereWeGo(int where)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        if (tab_Peg[i].direction == (unsigned int)where)
            return i;
    }
    return 0;
}

/*
void
__displayLoadChoice(const char * shapeName){
//	printf("Loading %s...\n", shapeName);
    g_print("DEBUG :: Loading %s...\n", shapeName);
}
 */
////////////////////////////////////////  DEBUG  CONSOLE ////////////////////////////////////////

void __displayMatrix(Matrix matrix)
{
    int row, column;
    char *sign;
    printf("\n%2s", " ");
    for (column = 0; column < VER_MAX; column++)
    {
        printf("%- 3d|", column);
    }
    printf("\n");
    for (row = 0; row < HOR_MAX; row++)
    {
        printf("%2d", row);
        for (column = 0; column < VER_MAX; column++)
        {
            sign = (char *)__selectItem(matrix[row][column]);
            printf("%2s|", sign);
        }
        printf("\n");
    }
}

char *__selectItem(int item)
{
    char *ret;
    static int num = 1;
    char buff[255];
    switch (item)
    {
    case -1:
        ret = "   ";
        break;
    case 0:
        ret = " . ";
        break;
    case 1:
        sprintf(buff, "<%2d>", num++);
        ret = buff;
        ret = " O ";
        break;
    case 2:
        ret = "<0>";
        break;
    default:
        break;
    }
    return ret;
}
