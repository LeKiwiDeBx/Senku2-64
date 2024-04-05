/*
 * Créé le 17-10-2016 19:48:05 CEST
 * score.c
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <libintl.h>
#include <locale.h>

// #include <gmessages.h> /* memcpy */
#include "score.h"

#define PRN printf("\n")
#define UNKNOWN       _("No name")

typedef enum e_typeSort
{
    PEG,
    TIME,
    SCORE
} typeSort;

static tabScore tabSortScore;
static score cursorCurrentScore;
static pScore cursorScore;
static score inputScore;
static double bonusTimeScore = 0;
static int remainingPeg = 0;

static void __addBegin();
static int __addEnd();
static void __addInside(int);
static void __updateCursorScore(const score *);
static double __calculateScore(const int, const double);
static int __insertRecord(score *);
static void __setRemainingPeg(int);
// static void 	__setNamePlayer() ;
static void __setBonusElapseTime(int);
static void __setIdScore(int);
static void __setScoreGame(int);
// static void       __clean(const char *buffer, FILE *fp) ;
// static void		__displaySortScore(int ) ;
// static void		__displaySetNamePlayer() ;
// static void		__displaySetCalculateBonusElapseTimer(double bonus) ;

int scoreNew()
{
    static int id = 0, ret = 0;
    inputScore.idScore = ++id;
    ret = __insertRecord(&inputScore);
    //	__displaySortScore(ret) ; //version terminal
    return ret;
}

/*
static void
__displaySortScore(int topScore){
    int i;
    if(topScore)
        printf("\nCongratulations! You are in TOP %d SCORE %s with a score %.f\n",SCORE_BEST_OF,cursorScore->namePlayer, cursorScore->scoreGame);
    else
        printf("\nCongratulations! Your score is %.f\n",cursorScore->scoreGame);
    PRN ;
        printf("%*s\t%-*s\t%*s\t%*s\n",5,"ORDER",12,"   PLAYER   ",3,"PEG",5,"SCORE");
        for (i = 0; i < SCORE_BEST_OF; i++)
        {
    printf("%d\t%-12s\t%-3d\t%.f\n",i+1,tabSortScore[i].namePlayer, tabSortScore[i].remainingPeg, tabSortScore[i].scoreGame);
        }
}
 */
static int
__insertRecord(score *inputScore)
{
    int i, scoreGame;
    __updateCursorScore(inputScore);
    __setRemainingPeg(remainingPeg);
    __setBonusElapseTime(bonusTimeScore);
    scoreGame = __calculateScore(cursorScore->remainingPeg, cursorScore->bonusElapseTime);
    __setScoreGame(scoreGame);
    __setIdScore(inputScore->idScore);
    if (cursorScore->scoreGame > tabSortScore[0].scoreGame)
    {
        //		__setNamePlayer(); version terminal
        __addBegin();
        return 1;
    }
    else if (cursorScore->scoreGame < tabSortScore[SCORE_BEST_OF - 2].scoreGame &&
             cursorScore->scoreGame > tabSortScore[SCORE_BEST_OF - 1].scoreGame)
    {
        //		__setNamePlayer(); version terminal
        return SCORE_BEST_OF;
    }
    else
    {
        for (i = 0; i < SCORE_BEST_OF - 1; i++)
        {
            if (cursorScore->scoreGame <= tabSortScore[i].scoreGame &&
                cursorScore->scoreGame > tabSortScore[i + 1].scoreGame)
            {
                //				 __setNamePlayer(); version terminal
                __addInside(i + 1);
                return (i + 2);
            }
        }
    }
    return 0;
}

void scoreSetCalculateBonusElapseTimer(double elapseTimer)
{
    /* cast sauvage !!!! :(*/
    int i = (int)elapseTimer;
    double bonus = 0;
    if (i >= MIN_SEC_BONUS && i <= MAX_SEC_BONUS)
    {
        // bonus = 5*(9-i) ;
        bonus = (MAX_SEC_BONUS - elapseTimer);
        bonusTimeScore += bonus;
        //		__displaySetCalculateBonusElapseTimer(bonus) ;
    }
}

/*
static void
__displaySetCalculateBonusElapseTimer(double bonus){
    printf("\n Yep! Bonus: + %d points!\n", (int)bonus) ;
}
 */

double
scoreGetBonusTimeScore()
{
    return bonusTimeScore;
}

void scoreResetBonusTimeScore()
{
    bonusTimeScore = 0;
}

void scoreSetRemainingPeg(int number)
{
    remainingPeg = number;
}

tabScore *
scoreGetSortScore(int rank)
{
    return (!rank) ? (tabScore *)&tabSortScore[0] : (tabScore *)&tabSortScore[rank - 1];
}

static void
__setIdScore(int idScore)
{
    if (idScore)
        cursorScore->idScore = idScore;
}

static void
__setBonusElapseTime(int bonus)
{
    cursorScore->bonusElapseTime = bonus;
}

static void
__setRemainingPeg(int remainPeg)
{
    if (remainPeg)
        cursorScore->remainingPeg = remainPeg;
}

static void
__setScoreGame(int scoreGame)
{
    if (scoreGame)
        cursorScore->scoreGame = scoreGame;
}

/*
static void
__setNamePlayer(){
    char chaine[MAX_CAR_NAME] = "\0";
//	__displaySetNamePlayer() ;
//	fgets(chaine, sizeof(chaine), stdin);
//	__clean(chaine, stdin);
    strncpy(cursorScore->namePlayer, chaine, MAX_CAR_NAME);
}
 */

void scoreSetNamePlayer(const char *sName, int rank)
{
    // tabSortScore[rank-1].namePlayer car le nom est cette fois-ci
    // demandé après inscription dans le top 10
    if (sName)
        strncpy(tabSortScore[rank - 1].namePlayer, sName, MAX_CAR_NAME);
    else
        strncpy(tabSortScore[rank - 1].namePlayer, UNKNOWN, MAX_CAR_NAME);
}

void scoreSetNameMatrix(const char *sName, int rank)
{
    if (sName)
        strncpy(tabSortScore[rank - 1].nameMatrix, sName, MAX_CAR_NAME);
    else
        strncpy(tabSortScore[rank - 1].nameMatrix, UNKNOWN, MAX_CAR_NAME);
}

/*
static void
__displaySetNamePlayer(){
    printf("\nWho is the happiest person in this world?\nYour name, please: ");
}
 */

/*
static void
__clean(const char *buffer, FILE *fp){
    char *p = strchr(buffer,'\n');
    if (p != NULL)
 *p = 0;
    else{
        int c;
        while ((c = fgetc(fp)) != '\n' && c != EOF);
    }
}
 */

static double
__calculateScore(const int remainPeg, const double timeBonus)
{
    const int pegFloorToCalc = 7, pointFactor = 1000;
    if (remainPeg < pegFloorToCalc)
        return ((double)pointFactor * ((pegFloorToCalc - 1) - remainPeg) + timeBonus);
    else
        return timeBonus;
}

void scoreInit()
{
    int i;
    const char *unknown = _("Unknown");
    char buffer[MAX_CAR_NAME];
    for (i = 0; i < SCORE_BEST_OF; i++)
    {
        tabSortScore[i].idScore = 0;
        strncpy(tabSortScore[i].namePlayer, unknown, sizeof(buffer));
        strncpy(tabSortScore[i].nameMatrix, unknown, sizeof(buffer));
        tabSortScore[i].remainingPeg = 0;
        tabSortScore[i].scoreGame = 0;
        tabSortScore[i].bonusElapseTime = 0;
    }
    cursorScore = memcpy(&cursorCurrentScore, &tabSortScore[0], sizeof(tabSortScore[0]));
}

/*
 *
 * name: __addBegin
 * @param
 * @return
 * Insere un enregistrement pointé en debut de tableau temporaire,
 * recopie le tableau trié moins l'enregistrement de fin à la suite du tab temporaire
 * recopie tout le tableau temp dans le tableau trié
 */
static void
__addBegin()
{
    tabScore tabTempScore;
    memcpy(tabTempScore, cursorScore, sizeof(score));
    memcpy(tabTempScore + 1, tabSortScore, (SCORE_BEST_OF - 1) * sizeof(score));
    memcpy(tabSortScore, tabTempScore, sizeof(tabTempScore));
    ;
}

/*
 *
 * name: __addEnd
 * @param
 * @return la position d'insertion en fin de liste
 * Insere un enregistrement au premier endroit de libre avant la fin
 * sinon on ecrase le dernier enregistrement
 */
static int
__addEnd()
{
    int i = 0;
    while (tabSortScore[i].remainingPeg != 0 && i < SCORE_BEST_OF - 1)
        i++;
    if (i < SCORE_BEST_OF)
        memcpy(&tabSortScore[i], cursorScore, sizeof(score));
    return i;
    ;
}

/*
 *
 * name: __addInside
 * @param
 * @return
 * Insere dans le tableau des scores un enregistrement d'indice posInsert
 * sauvegarde le debut du tableau avant la position d'insertion du record
 * insere le record
 * recopie la fin du tableau
 */
static void
__addInside(int posInsert)
{
    tabScore tabTempScore;
    if (posInsert < SCORE_BEST_OF && posInsert > 0)
    {
        memcpy(tabTempScore, tabSortScore, posInsert * sizeof(score));
        memcpy(tabTempScore + posInsert, cursorScore, sizeof(score));
        memcpy(tabTempScore + posInsert + 1, tabSortScore + posInsert, (SCORE_BEST_OF - posInsert - 1) * sizeof(score));
        memcpy(tabSortScore, tabTempScore, sizeof(tabTempScore));
    };
}

/*
 *
 * name: __updateCursorScore
 * @param  inputScore pointeur sur le record à inserer
 * @return
 * met à jour le curseur (data et adresse) sur le record courant
 */
static void
__updateCursorScore(const score *inputScore)
{
    cursorScore = memcpy(&cursorCurrentScore, inputScore, sizeof(score));
}
