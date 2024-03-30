/*
 * File:   memento.c
 * Author: Administrateur
 * implementation en C inspirée du pattern GOF memento
 * Created on 6 novembre 2016, 19:06
 */

#include "memento.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE_TAB(array) (sizeof(array) / sizeof(array[0]))

static Peg_Memento statePeg;
static pMemento pm;
static mementoArrayList mArrayList = {NULL}; // tableau de pointeur de memento
extern char (*pMatrixLoad)[VER_MAX];

/**
 * @brief re/initialise la mArrayList à NULL
 *
 */
void caretakerNew()
{
    unsigned int i;
    for (i = 0; i < SIZE_TAB(mArrayList); i++)
    {
        mArrayList[i] = NULL;
    }
}

/**
 * @brief ajoute un memento à la liste
 * @param m
 * @return void
 */
void caretakerAddMemento(pMemento pmArray)
{
    int i;
    mArrayList[NB_UNDO - 1] = NULL; // sentinelle
    for (i = 0; i < NB_UNDO - 1; i++)
    {
        if (mArrayList[i] == NULL)
        {
            mArrayList[i] = pmArray;
            break;
        }
        else if (i == NB_UNDO - 2)
        {
            // decalage vers la gauche du tableau --> FIFO
            memmove(mArrayList, mArrayList + 1, (NB_UNDO - 2) * sizeof(pMemento));
            mArrayList[NB_UNDO - 2] = pmArray;
            break;
        }
    }
}

/**
 * @brief renvoi le memento a un indice donné du UNDO
 * @param un undo cad un retour arriere immediat
 * @return un pointeur sur le memento
 */
pMemento
caretakerGetMemento(int undo)
{
    int j;
    if (undo)
    {
        if (!mArrayList[0])
            return NULL; // liste vide
        for (j = 1; j < NB_UNDO; j++)
        {
            if (mArrayList[j] == NULL)
            {
                if (memcpy(pm, mArrayList[j - 1], sizeof(memento)) != NULL)
                {
                    mArrayList[j - 1] = NULL;
                    return pm;
                }
            }
        }
    }
    return NULL;
}

/**
 * @brief Si la liste des mementos est vide
 * @return 1 vrai sinon faux
 */
int mementoIsEmpty()
{
    return (!mArrayList[0]) ? 1 : 0;
}

/**
 * @brief positionne la variable static de l'etat à sauver
 * @param state Peg à sauver
 */
void originatorSet(Peg_Memento state)
{
    statePeg = state;
}

/**
 * @brief demande à memento une reference sur le dernier sauvegardé
 * @return un pointeur sur le memento
 */
pMemento
originatorSaveToMemento()
{
    mvt mvtStart, mvtEnd, mvtBetween;
    mvtEnd.row = statePeg.coordEnd.row;
    mvtEnd.column = statePeg.coordEnd.column;
    mvtBetween.row = statePeg.coordBetween.row;
    mvtBetween.column = statePeg.coordBetween.column;
    mvtStart.row = statePeg.coordStart.row;
    mvtStart.column = statePeg.coordStart.column;
    return mementoNew(mvtStart, mvtBetween, mvtEnd);
}

/**
 * @brief appel mementoGetSaveState à l'indice caretakerGetMemento(int)et modifie la matrice
 * @param le memento à restaurer
 * @return succes ou pas
 */
int originatorRestoreFromMemento(pMemento pm)
{
    Peg_Memento state;
    if (pm != NULL)
    {
        state = mementoGetSaveState(pm);
        pMatrixLoad[state.coordStart.row][state.coordStart.column] = 1;
        pMatrixLoad[state.coordBetween.row][state.coordBetween.column] = 1;
        pMatrixLoad[state.coordEnd.row][state.coordEnd.column] = 0;
        return 1;
    }
    return 0;
}

/**
 * @brief calcul les mvt precedent
 * @return un memento que l'on vient de sauver
 */
Peg_Memento
mementoGetSaveState(pMemento pm)
{
    if (pm != NULL)
    {
        statePeg.coordStart.row = pm->mvtStart.row;
        statePeg.coordStart.column = pm->mvtStart.column;
        statePeg.coordBetween.row = pm->mvtBetween.row;
        statePeg.coordBetween.column = pm->mvtBetween.column;
        statePeg.coordEnd.row = pm->mvtEnd.row;
        statePeg.coordEnd.column = pm->mvtEnd.column;
        return statePeg;
    }
    else
        return statePeg;
}
/**
 * @brief Tranforme le tableau memento apres une rotation CLOCKWISE || COUNTERCLOCKWISE
 * @param
 * @return
 *
 */
void
mementoTransformRotate( int rotation ) 
{ 
    int COL_MAX = VER_MAX - 1 ;
    int ROW_MAX = HOR_MAX - 1;
    memento mementoTmp ;
    for(int i=0; i < NB_UNDO; i++)
    {
        if(mArrayList[i] != NULL){
            mementoTmp = *mArrayList[i];
            // x'= -y, y' = x
            mArrayList[i]->mvtStart.column = (rotation == THETA_CLOCKWISE)? COL_MAX - mementoTmp.mvtStart.row : mementoTmp.mvtStart.row ; //x'
            mArrayList[i]->mvtStart.row =   (rotation == THETA_CLOCKWISE)? mementoTmp.mvtStart.column : ROW_MAX - mementoTmp.mvtStart.column ; //y'
            mArrayList[i]->mvtBetween.column = (rotation == THETA_CLOCKWISE)? COL_MAX - mementoTmp.mvtBetween.row : mementoTmp.mvtBetween.row ; //x'
            mArrayList[i]->mvtBetween.row = (rotation == THETA_CLOCKWISE)? mementoTmp.mvtBetween.column : ROW_MAX - mementoTmp.mvtBetween.column ; //y'
            mArrayList[i]->mvtEnd.column = (rotation == THETA_CLOCKWISE)?  COL_MAX - mementoTmp.mvtEnd.row : mementoTmp.mvtEnd.row; //x'
            mArrayList[i]->mvtEnd.row =   (rotation == THETA_CLOCKWISE)? mementoTmp.mvtEnd.column : ROW_MAX - mementoTmp.mvtEnd.column ; //y'
        }
    }
}

/**
 * @brief le memento à créer
 * @param 1 mvt de depart
 * @param 2 mvt du milieu
 * @param 3 mvt de fin
 */
pMemento
mementoNew(mvt mvtStart, mvt mvtBetween, mvt mvtEnd)
{
    static int mvtId = 1;
    pm = (pMemento)malloc(sizeof(memento));
    if (pm != NULL)
    {
        pm->idRollback = mvtId;
        pm->mvtStart = mvtStart;
        pm->mvtBetween = mvtBetween;
        pm->mvtEnd = mvtEnd;
        mvtId++;
    }
    else
    {
        exit(EXIT_FAILURE);
    }
    return pm;
}
