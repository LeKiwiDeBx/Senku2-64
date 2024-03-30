/*
 *  * File:   memento.h
 * Author: Administrateur
 * Created on 6 novembre 2016, 19:06
 */

#ifndef MEMENTO_H
#define MEMENTO_H

#include "peg.h"
#include "matrix.h"

#define NB_UNDO 11 // nombre max de undo géré:
// NB_UNDO-1-1 : niveau de depilage
// NB_UNDO-1 = NULL: sentinelle

typedef struct s_mvt
{
    int row;
    int column;
} mvt;

typedef struct s_memento
{
    int idRollback; // identifiant du mouvement
    mvt mvtStart;   // coord depart du mouvement
    mvt mvtBetween; // coord intermediaire du mouvement
    mvt mvtEnd;     // coord arrivée du mouvement
} memento;

typedef memento *pMemento;

typedef pMemento mementoArrayList[NB_UNDO]; // typedef pMemento mementoArrayList[NB_UNDO]<br/>tableau de pointeur de memento

pMemento mementoNew(mvt, mvt, mvt);           // ecrit la structure
Peg_Memento mementoGetSaveState(pMemento pm); // calcul les mvt precedent

pMemento originatorSaveToMemento(); // appel mementoNew qui ecrit la struct
void originatorSet(Peg_Memento pegMemento);
int originatorRestoreFromMemento(pMemento pm); // appel mementoGetSaveState à l'indice caretakerGetMemento(int)
// et modifie la matrice
void caretakerNew();                   // initialise le caretaker
void caretakerAddMemento(pMemento pm); // ajoute un pointeur dans mementoArrayList
pMemento caretakerGetMemento(int);     // retourne pointeur memento à l'indice donné
// faudra gérer la Liste en detruisant juste après le numero UNDO?
int mementoIsEmpty(); // retourne 1 si vide sinon 0

/////////// DEBUG //////////////////
void mementoTransformRotate();
#endif /* MEMENTO_H */
