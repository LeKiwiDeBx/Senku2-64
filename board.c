/* board.c
 * Créé le 05-09-2016 14:01:44 CEST
 *
 */

/**
 * headers C
 */
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
/**
 * headers GTK/Glib
 */
#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>
/**
 * headers Senku project
 */
#include "score.h"
#include "board.h"
#include "matrix.h"
#include "timer.h"
#include "memento.h"

#ifndef _LINUX_
#define _LINUX_ /* execution de system clear */
#endif

/* MACROS PANGO*/
#define LABEL_COLOR_TITLE #516079   // #385998
#define LABEL_COLOR_TEXT #F1895C    // #FF6600
#define LABEL_COLOR_LOGO "gray"     //"#E0777D"
#define LABEL_COLOR_BG_LOGO "white" // #2D3047

// #385998
#define SENKU_PANGO_CONCAT_STR(color, type) "<span size=\"x-large\" weight=\"bold\" color=\"" #color "\">%" #type "</span>"
#define SENKU_PANGO_MARKUP_LABEL(color, type) SENKU_PANGO_CONCAT_STR(color, type)
#define SENKU_ABS(x) ((x)) ? (x) : (-x)

#define TITLE_MAIN "Senku (GTK+3.22) 2.2.0 Beta\n\n[°} LeKiwiDeBx\n\nCopyright (C) 2016-2024 GNU GPLv3."
#define TITLE_MENU "Shapes choice"
#define TIMER_DELAY 1000
// #define IMG_PEG_MOVE "image/circle_gold32.png"
#define IMG_PEG_MOVE "image/5AAAA4.png"
// #define IMG_PEG_SELECT "image/circle_gold_select32.png"
#define IMG_PEG_SELECT "image/F1895C.png"
// #define IMG_PEG_DELETE "image/circle_white32.png"
#define IMG_PEG_DELETE "image/516079.png"
#define IMG_PEG_DELETE_UNDO "image/circle_white_undo32.png"
#define IMG_PEG_UNDO "image/circle_gold_undo32.png"
// #define IMG_PEG_BOARD "image/marble_1.png"
#define IMG_PEG_BOARD "image/2E3244_32x32.png"
#define IMG_PEG_TRANSPARENT "image/transparent_32x32.png"
#define NO_MORE_MOVE "No more move!"
#define NO_ACTION_UNDO "Nothing to [Undo]"
#define ACTION_UNDO "[Undo] the previous move"
#define BLANK ""
#define BOX_SCORE_TITLE " Score "
#define APPLICATION_TITLE "Senku 2.2.0 Beta"
#define APPLICATION_SIZE_WIDTH 360
#define APPLICATION_SIZE_HEIGHT 340
#define APPLICATION_BORDER_WIDTH 10
#define MAX_LABEL 10 // nombre etiquette maximum coté droit
#define LABEL_BONUS_TEXT "Bonus"
#define LABEL_PEG_TEXT "Pegs"
#define LABEL_TIME_TEXT "Time"
#define LABEL_LOGO "[°} LeKiwiDeBx"
#define SENKU_THEME_PATH "theme/"
#define SENKU_THEME_FILE "senku.css"
#define IMG_BUTTON_NEW_GAME "image/img_button_new_game.png"

static void
_g_displayAllMatrix();

/*
static void
__displayHeader( ) ;
static void
__g_displayHeader( ) ;

static void
__displayMenu( ) ;

static void
__displayResult( int ) ;

static void
__displaySetCoordToSelect( int*, int* ) ;

static int
__displaySetCoordToMove( ) ;

static int
__getMenuChoice( ) ;

static int
__displayPlayAgain( ) ;

static void
__displayTimer( double, double ) ;

static void
__displayBonusTimeScore( ) ;
 */

Board onlyOneBoard;

// static

typedef struct s_coord
{
    gchar x;
    gchar y;
} Coord;
Coord eventCoord, *pEventCoord;

typedef enum e_actionSelect
{
    ACTION_SELECT_PEG,
    ACTION_SELECT_TAKE_NORTH,
    ACTION_SELECT_TAKE_EAST,
    ACTION_SELECT_TAKE_SOUTH,
    ACTION_SELECT_TAKE_WEST,
    ACTION_SELECT_UNSELECT_PEG = 10,
} actionSelect;

enum typeLabel
{
    LABEL_BONUS,
    LABEL_PEG,
    LABEL_TIME,
};

typedef struct s_dataName
{
    GtkWidget *pWidgetName;
    int rang;
} dataName;
dataName *pDataName;

static pMemento pMementoLastUndo = NULL; // retiens le dernier undo
static guint handleTimeout = 0;          // handle de la fonction g_timeout_add()

/* *****************************************************************************
 * And the Widget's land begin here...
 * ****************************************************************************/

/* application instance */
GtkApplication *app;
/* fenetre principal application */
GtkWidget *pWindowMain;
/* grille principale */
GtkWidget *pGridMain;
/* grille matrice qui contient le shape*/
GtkWidget *pGridMatrix;
/* boite qui contient les etiquettes indicatrices valeurs BONUS PEG TIME*/
GtkWidget *pVbox;
/* label pegs remaining value */
GtkWidget *plbTime = NULL;
GtkWidget *plbTimeValue = NULL;
GtkWidget *plbBonus = NULL;
GtkWidget *plbBonusValue = NULL;
GtkWidget *plbPegs = NULL;
GtkWidget *plbPegsValue = NULL;
GtkWidget *pfrComments = NULL;
GtkWidget *plbComments = NULL;
GtkWidget *pBoxMenu = NULL;
GtkWidget *pHbox = NULL;
GtkWidget *pButtonNewGame = NULL;
GtkWidget *pButtonUndo = NULL;
GtkWidget *pButtonRotateLeft = NULL;
GtkWidget *pButtonRotateRight = NULL;
GtkWidget *pButtonQuit = NULL;
GtkWidget *pButtonScore = NULL;
GtkWidget *pfrTitle = NULL;
GtkWidget *pBoxMenuOption = NULL;
GtkWidget *plbTitle = NULL;
GtkWidget *radio = NULL;
GtkWidget *pBoxMenuButton = NULL;
GtkWidget *pBtnMenuQuit = NULL;
GtkWidget *pBtnMenuPlay = NULL;
GtkWidget *pBoxScore = NULL;
GtkWidget *pGridScore = NULL;
GtkWidget *plbValues[MAX_LABEL];
GtkWidget *plbValuesValue[MAX_LABEL];
GtkWidget *pDialogBoxQuit = NULL;
GtkWidget *pWindowGetName = NULL;
GtkWidget *plbLogo = NULL;
/**
 * @brief Appel selection image avec un clic souris
 * @param pWidget boxEvent qui encapsule l'image
 * @param event
 * @param pData une structure qui renferme les coordonnées du click mouse (en unité de matrice)
 */
void OnSelect(GtkWidget *pWidget, GdkEvent *event, gpointer pData);

/**
 * @brief Appel par le bouton play du menu du choix de shape
 * @param GtkWidget le bouton play
 * @param pData un bouton radion (sert à recuperer le groupe du bouton radio)
 */
void OnPlay(GtkWidget *pWidget, gpointer pData);

/**
 * @brief Appel par le bouton rotate pour tourner la matrice
 * @param GtkWidget le bouton rotate
 * @param pData NULL
 */
void OnRotate(GtkWidget *pWidget, gpointer pData);

/**
 * @brief Appel pour fin d'application
 * @param pWidget appele par les boutons quit et fermeture fenetre
 * @param pData NULL
 */
void OnDestroy(GtkWidget *pWidget, gpointer pData);

/**
 * @brief Appel par le survol en entree du grid pour un peg donne
 * @param GtkWidget la matrix grid
 * @param pData les coordonnées du peg
 */
void OnEnter(GtkWidget *pWidget, GdkEvent *event, gpointer pData);

/**
 * @brief Appel par le survol en sortie du grid pour un peg donne
 * @param GtkWidget la matrix grid
 * @param pData les coordonnées du peg
 */
void OnLeave(GtkWidget *pWidget, GdkEvent *event, gpointer pData);

/**
 * @brief Appel pour defaire le dernier mouvement de pion
 * @param pWidget appele par le bouton Undo
 * @param pData NULL
 */
void OnUndo(GtkWidget *pWidget, gpointer pData);
/**
 * @brief Validation de la boite de nom
 * @param pWidget
 * @param pData ptr sur struct le nom et rang
 */
void OnSetName(GtkWidget *pWidget, dataName *pData);

/**
 * @brief appel par OnUndo pour retablir l'affichage des pions undo
 * @param pm les coordonnées des étapes du undo
 */
void _g_displayUndoMatrix(pMemento pm);

/**
 * @brief appel par OnNewGame pour commencer un nouveau jeu
 * @param pWidget appel du bouton New Game
 * @param pData NULL (pour le moment:))
 */
void OnNewGame(GtkWidget *pWidget, gpointer pData);

/**
 * @brief appel fermeture boite de dialogue pWindowGetName
 * @param pWidget pWindowGetName
 * @param pData rank position du joueur du topmost
 */
void OnDestroyGetName(GtkWidget *pWidget, gpointer pData);
/**
 * @ Not use (???)
 * @param pWidget
 * @param event
 * @param pData
 * @return
 */
gboolean
OnRadioToggled(GtkWidget *pWidget, GdkEvent *event, gpointer pData);

/**
 * @ determine quel bouton du groupe à option est utilisé
 * @param group groupe du bouton
 * @return int la position dans le groupe sinon 0
 */
int which_radio_is_selected(GSList *group);

/**
 * Affiche la matrice dans le pWindowMain
 * @param matrix
 */
void _g_displayMatrix(Matrix matrix);
/**
 * Met à jour l'affichage de la matrice apres une selection/prise
 * @param action type action et direction
 * @param x coord du clic ligne
 * @param y coord du clic colonne
 */
void _g_displayUpdateMatrix(actionSelect action, const int x, const int y);
/**
 * affichage du chronometre
 */
gboolean
_g_display_time(gpointer pData);
/**
 * @brief reecrit le texte du label
 * @param pWidget le label
 * @param pData   le texte à écrire
 */
void _g_labelSet(GtkWidget *pWidget, gpointer pData);
/**
 * @brief gestion de la variable booleene firsSelectPeg
 * @param action "get" ou "set" une valeur bool
 * @param value booelen à positionner
 * @return return la valeur de firstSelectPeg ou -1 si Pbleme
 */
gboolean
_firstSelectPeg(char *action, gboolean value);
/**
 * @brief affiche les scores
 * @param pointeur sur le tableau des scores
 * @param rang dans le top rated
 */
void _g_display_box_score(pScore ps, const int rank);
/**
 *@brief Dialog box Menu
 *       choix des shapes modale et sans decoration (style screen splash)
 *@param determine l'option par defaut
 */
void _g_display_box_menu(gpointer pData);
/**
 * @brief Construct a new g display box menu new game object
 *
 */
int _g_display_box_menu_new_game(gpointer pData);
/**
 * @brief efface l'affichage de la matrice
 */
void _g_erase_displayMatrix();
/**
 * @brief construit gridMatrix pour contenir la matrice
 */
void _g_new_GridMatrix();
/**
 * @brief demande le nom pour le score
 * @param rank rang ou est inserer le joueur
 */
void _g_display_get_name(int rank);
/**
 * @brief redessinne les peg et trou sans point rouge ni croix (signe du last undo)
 * @param pm le memento des images à redessiner sans les symboles du Undo
 */
void
    _setLastMementoUndoRedrawNormal(pMemento);
/**
 * @brief fermeture de BoxScore
 * @param pWidget le button close
 * @param pData le widget window pBoxScore
 */
void OnCloseBoxScore(GtkWidget *pWidget, gpointer pData);
/**
 * @brief affiche la fiche de score
 * @param pWidget le button score
 * @param pData NULL
 */
void OnDisplayScore(GtkWidget *pWidget, dataName *pData);

static void
activate(GtkApplication *app,
         gpointer user_data)
{
    // GtkWidget *window;
    /* creation fenetre Main */
    pWindowMain = gtk_application_window_new(app);
    gtk_window_set_position(GTK_WINDOW(pWindowMain), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(pWindowMain), APPLICATION_TITLE);

    // gtk_window_set_default_size(GTK_WINDOW(pWindowMain), APPLICATION_SIZE_WIDTH, APPLICATION_SIZE_HEIGHT);

    gtk_window_set_resizable(GTK_WINDOW(pWindowMain), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(pWindowMain), APPLICATION_BORDER_WIDTH);
    /* signal fermeture l'application sur fermeture fenetre principale */
    g_signal_connect(G_OBJECT(pWindowMain), "delete-event", G_CALLBACK(OnDestroy), NULL);
    // g_signal_connect(G_OBJECT(pWindowMain), "destroy", G_CALLBACK(OnDestroy), NULL);
    //  Creation de la grille principale container */
    pGridMain = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(pGridMain), FALSE);

    // attachement de la grille matrice a la grille principale
    pGridMatrix = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(pGridMain), pGridMatrix, 0, 0, 1, 1);

    gtk_widget_set_halign(GTK_WIDGET(pGridMatrix), GTK_ALIGN_CENTER);
    // attachement de la grille pricipale a la fenetre principale
    gtk_container_add(GTK_CONTAINER(pWindowMain), pGridMain);

    /* Button bottom  <New Game> <Undo> <RotLeft> <RotRight> <Quit> */
    pHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_top(GTK_WIDGET(pHbox), 0);
    gtk_widget_set_halign(GTK_WIDGET(pHbox), GTK_ALIGN_CENTER);

    pButtonNewGame = gtk_button_new();
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonNewGame), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonNewGame, FALSE, FALSE, 0);

    pButtonUndo = gtk_button_new();
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonUndo), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonUndo, FALSE, FALSE, 0);

    pButtonScore = gtk_button_new();
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonScore), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonScore, FALSE, FALSE, 0);

    pButtonRotateLeft = gtk_button_new();
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonRotateLeft), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonRotateLeft, FALSE, FALSE, 0);

    pButtonRotateRight = gtk_button_new();
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonRotateRight), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonRotateRight, FALSE, FALSE, 0);

    pButtonQuit = gtk_button_new();
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonQuit), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonQuit, FALSE, FALSE, 0);

    // attachement de la boite des boutons a la grille principale avec sibling pGridMatrix
    gtk_grid_attach_next_to(GTK_GRID(pGridMain), pHbox, pGridMatrix, GTK_POS_BOTTOM, 1, 1);

    /* frame Comments */
    pfrComments = gtk_frame_new(NULL);
    plbComments = gtk_label_new("Initializing ...");
    gtk_frame_set_shadow_type(GTK_FRAME(pfrComments), GTK_SHADOW_NONE);
    gtk_container_add(GTK_CONTAINER(pfrComments), plbComments);
    gtk_style_context_add_class(gtk_widget_get_style_context(pfrComments), "matrix-name-label");
    // attachement du frame comments a la grille principale avec sibling pHbox
    gtk_grid_attach_next_to(GTK_GRID(pGridMain), pfrComments, pHbox, GTK_POS_BOTTOM, 1, 2); // 1 a la place de 2

    /* frame Values */
    GtkWidget *pfrValues = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(pfrValues), GTK_SHADOW_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(pfrValues), 0);
    gchar *plbValuesTitle[] = {LABEL_BONUS_TEXT, LABEL_PEG_TEXT, LABEL_TIME_TEXT};
    pVbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_set_homogeneous(GTK_BOX(pVbox), TRUE);
    gtk_widget_set_halign(GTK_WIDGET(pVbox), GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(pfrValues), pVbox);
    // char *markup;
    for (int k = 0; k < 3; k++)
    {
        plbValues[k] = gtk_label_new(plbValuesTitle[k]);
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValues[k]), "value-title-label");
    }

    gtk_box_pack_start(GTK_BOX(pVbox), plbValues[0], FALSE, FALSE, 0);
    plbValuesValue[0] = gtk_label_new("0");
    gtk_box_pack_start(GTK_BOX(pVbox), plbValuesValue[0], FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(pVbox), plbValues[1], FALSE, FALSE, 0);
    plbValuesValue[1] = gtk_label_new("0");
    gtk_box_pack_start(GTK_BOX(pVbox), plbValuesValue[1], FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(pVbox), plbValues[2], FALSE, FALSE, 0);
    plbValuesValue[2] = gtk_label_new("0");
    gtk_box_pack_start(GTK_BOX(pVbox), plbValuesValue[2], FALSE, FALSE, 0);

    gtk_grid_attach_next_to(GTK_GRID(pGridMain), pfrValues, pfrComments, GTK_POS_BOTTOM, 1, 1); // 1 a la palce de 2

    /* _____________________________________________________*
     *                                                       *
     *                 CSS est la vie plus belle             *
     *                                                       *
     *______________________________________________________ */
    GtkCssProvider *pCssProvider = gtk_css_provider_new();
    const gchar *themePathFile = g_strdup_printf("%s%s", SENKU_THEME_PATH, SENKU_THEME_FILE);
    if (gtk_css_provider_load_from_path(pCssProvider, themePathFile, NULL))
        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                                  GTK_STYLE_PROVIDER(pCssProvider),
                                                  GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_widget_set_name(pWindowMain, "windowMain");
    gtk_widget_set_name(pGridMatrix, "gridMatrix");
    gtk_widget_set_name(pButtonNewGame, "buttonNewGame");
    gtk_widget_set_name(pButtonUndo, "buttonUndo");
    gtk_widget_set_name(pButtonScore, "buttonScore");
    gtk_widget_set_name(pButtonRotateLeft, "buttonRotateLeft");
    gtk_widget_set_name(pButtonRotateRight, "buttonRotateRight");
    gtk_widget_set_name(pButtonQuit, "buttonQuit");

    gtk_style_context_add_class(gtk_widget_get_style_context(pGridMatrix), "logo");

    /* les signaux des boutons */
    g_signal_connect(G_OBJECT(pButtonScore), "clicked", G_CALLBACK(OnDisplayScore), NULL);
    g_signal_connect(G_OBJECT(pButtonQuit), "clicked", G_CALLBACK(OnDestroy), pWindowMain);
    g_signal_connect(G_OBJECT(pButtonUndo), "clicked", G_CALLBACK(OnUndo), NULL);
    g_signal_connect(G_OBJECT(pButtonNewGame), "clicked", G_CALLBACK(OnNewGame), NULL);
    g_signal_connect(G_OBJECT(pButtonRotateLeft), "clicked", G_CALLBACK(OnRotate), GINT_TO_POINTER(THETA_COUNTER_CLOCKWISE));
    g_signal_connect(G_OBJECT(pButtonRotateRight), "clicked", G_CALLBACK(OnRotate), GINT_TO_POINTER(THETA_CLOCKWISE));

    onlyOneBoard.set = &currentMatrixOfBoard;
    scoreInit();
    gtk_widget_show_all(pWindowMain);
    _g_display_box_menu_new_game(NULL);
}

int boardInitNew()
{
    int status;
    app = gtk_application_new("lekiwidebx.dev.senku", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), 0, NULL);
    return status;
}

int boardInit() ////////  DO NOT USE OLD CODE DO NOT USE OLD CODE DO NOT USE OLD CODE  ////////////////////
{
    const char *lbNewGame = "New Game";
    const char *lbUndo = "Undo";
    const char *lbScore = "Score";
    const char *lbRotateLeft = "Rot L";
    const char *lbRotateRight = "Rot R";
    const char *lbQuit = "Quit";
    // init table des scores
    scoreInit();

    // do {
    //	#ifdef _LINUX_
    //		system("clear");
    //
    //     #endif
    /***********************************************************************
     * __displayHeader() ;
     * function console
     **********************************************************************/
    /**
     * CSS How-To
     * http://stackoverflow.com/questions/37609381/how-to-set-a-specific-css-class-to-a-widget-in-gtk3-c
     */
    //  Creation window main
    pWindowMain = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(pWindowMain), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(pWindowMain), APPLICATION_TITLE);
    gtk_window_set_default_size(GTK_WINDOW(pWindowMain), APPLICATION_SIZE_WIDTH, APPLICATION_SIZE_HEIGHT);
    gtk_window_set_resizable(GTK_WINDOW(pWindowMain), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(pWindowMain), APPLICATION_BORDER_WIDTH);

    /* signal fermeture l'application sur fermeture fenetre principale */
    g_signal_connect(G_OBJECT(pWindowMain), "delete-event", G_CALLBACK(OnDestroy), NULL);
    // Creation de la grille principale container */
    pGridMain = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(pGridMain), FALSE);
    //    Grille du Senku
    _g_new_GridMatrix();
    gtk_grid_set_column_spacing(GTK_GRID(pGridMatrix), 0);
    // Creation grille des valeurs à droite
    GtkWidget *pGridValues = gtk_grid_new();
    gtk_widget_set_margin_top(GTK_WIDGET(pGridValues), 75);
    gtk_widget_set_margin_start(GTK_WIDGET(pGridValues), 50);
    char *plbValuesTitle[] = {LABEL_BONUS_TEXT, LABEL_PEG_TEXT, LABEL_TIME_TEXT};
    int sizeValueArray = (int)(sizeof(plbValuesTitle) / sizeof(char *));
    if (sizeValueArray > MAX_LABEL)
        exit(0);
    int k = 0, i = 0;
    const char *textInit = " 0 ";
    const int valInit = 0;
    char *markup;
    for (k = 0; k < sizeValueArray; k++)
    {
        plbValues[k] = gtk_label_new(plbValuesTitle[k]);
        markup = g_markup_printf_escaped(SENKU_PANGO_MARKUP_LABEL(LABEL_COLOR_TITLE, s), plbValuesTitle[k]);
        gtk_label_set_markup(GTK_LABEL(plbValues[k]), markup);
        plbValuesValue[k] = gtk_label_new(textInit);
        markup = g_markup_printf_escaped(SENKU_PANGO_MARKUP_LABEL(LABEL_COLOR_TEXT, d), valInit);
        gtk_label_set_markup(GTK_LABEL(plbValuesValue[k]), markup);
        g_free(markup);

        gtk_grid_attach(GTK_GRID(pGridValues), plbValues[k], i++, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(pGridValues), plbValuesValue[k], i++, 1, 1, 1);
    }
    // LOGO
    // plbLogo = gtk_label_new(LABEL_LOGO);
    // markup = g_markup_printf_escaped("<span size=\"x-large\" weight=\"bold\" bgcolor=\"%s\" color=\"%s\">%s</span>", LABEL_COLOR_BG_LOGO, LABEL_COLOR_LOGO, LABEL_LOGO);
    // gtk_label_set_markup(GTK_LABEL(plbLogo), markup);
    // g_free(markup);
    // gtk_grid_attach(GTK_GRID(pGridValues), plbLogo, 1, i++, 1, 1);
    // gtk_container_add(GTK_CONTAINER(pGridMain), pGridValues);
    gtk_container_add(GTK_CONTAINER(pWindowMain), pGridMain);
    /* pour pouvoir ajouter en dessous la zone des commentaires*/
    gtk_orientable_set_orientation(GTK_ORIENTABLE(pGridMain), GTK_ORIENTATION_VERTICAL); // GTK_ORIENTATION_VERTICAL
    pfrComments = gtk_frame_new(NULL);

    gtk_frame_set_label(GTK_FRAME(pfrComments), "");
    plbComments = gtk_label_new(NULL);
    gtk_container_add(GTK_CONTAINER(pfrComments), plbComments);
    gtk_widget_set_margin_top(GTK_WIDGET(pfrComments), 20);
    gtk_container_add(GTK_CONTAINER(pGridMain), pGridValues);
    // gtk_container_add(GTK_CONTAINER(pfrComments), pGridValues);
    gtk_widget_set_halign(GTK_WIDGET(plbComments), GTK_ALIGN_START); // GTK_ALIGN_START
    /* ajoute sur deux cellules horizontale du GridMain (soit la totalité) */
    gtk_grid_attach(GTK_GRID(pGridMain), pfrComments, 0, 1, 2, 1);

    /* Button bottom  <New Game> <Undo> <RotLeft> <RotRight> <Quit> */
    pHbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); // GTK_ORIENTATION_HORIZONTAL
    gtk_widget_set_margin_top(GTK_WIDGET(pHbox), 15);
    /* on ajoute à partir de la fin */
    gtk_widget_set_halign(GTK_WIDGET(pHbox), GTK_ALIGN_CENTER); // GTK_ALIGN_CENTER
    pButtonNewGame = gtk_button_new_with_label(lbNewGame);
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonNewGame, FALSE, FALSE, 15);
    pButtonUndo = gtk_button_new_with_label(lbUndo);
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonUndo, FALSE, FALSE, 15);
    /* bouton Score caler à droite */
    pButtonScore = gtk_button_new_with_label(lbScore);
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonScore, FALSE, FALSE, 15);
    /* bouton Rotate Left caler à droite */
    pButtonRotateLeft = gtk_button_new_with_label(lbRotateLeft);
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonRotateLeft, FALSE, FALSE, 15);
    /* bouton Rotate Right caler à droite */
    pButtonRotateRight = gtk_button_new_with_label(lbRotateRight);
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonRotateRight, FALSE, FALSE, 15);
    /* bouton Quit caler à droite */
    pButtonQuit = gtk_button_new_with_label(lbQuit);
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonQuit, FALSE, FALSE, 15);
    /* on ajoute les boutons */
    gtk_grid_attach(GTK_GRID(pGridMain), pHbox, 0, 3, 1, 3); // 0 ,3 ,1 ,3

    /* les signaux des boutons */
    g_signal_connect(G_OBJECT(pButtonScore), "clicked", G_CALLBACK(OnDisplayScore), NULL);
    g_signal_connect(G_OBJECT(pButtonQuit), "clicked", G_CALLBACK(OnDestroy), pWindowMain);
    g_signal_connect(G_OBJECT(pButtonUndo), "clicked", G_CALLBACK(OnUndo), NULL);
    g_signal_connect(G_OBJECT(pButtonNewGame), "clicked", G_CALLBACK(OnNewGame), NULL);
    g_signal_connect(G_OBJECT(pButtonRotateLeft), "clicked", G_CALLBACK(OnRotate), GINT_TO_POINTER(THETA_COUNTER_CLOCKWISE));
    g_signal_connect(G_OBJECT(pButtonRotateRight), "clicked", G_CALLBACK(OnRotate), GINT_TO_POINTER(THETA_CLOCKWISE));
    _g_display_box_menu(NULL);
    onlyOneBoard.set = &currentMatrixOfBoard;
    // on lance la boucle infernale
    pMementoLastUndo = malloc(1 * sizeof(memento));
    if (pMementoLastUndo == NULL)
        exit(EXIT_FAILURE);
    gtk_main();
    EXIT_SUCCESS;
    /***************************************************************************
     * __displayMenu()  ;
     * function console
     **************************************************************************/
    /*********************************************************************
     * __getMenuChoice() ;
     * function console
     * while (!matrixLoad( num = __getMenuChoice( ) )) ;
     */

    /**
     * adresse de la matrice courante (globale)
     * @return
     */
    /*
     * onlyOneBoard.set = &currentMatrixOfBoard ;
     */
    /* TODO: code a adapter en GTK
     * boardPlay( ) ;
     * scoreNew( ) ;
     * */

    //            } while (__displayPlayAgain( )) ;

    return 1;
}

/**
 * @brief display box menu new game
 *
 * @param pData
 */
int _g_display_box_menu_new_game(gpointer pData)
{
    GtkWidget *pRadio = gtk_radio_button_new_with_label(NULL, "English");
    GtkWidget *pRadio_1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pRadio), "German");
    GtkWidget *pRadio_2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pRadio), "Diamond");
    GtkWidget *pDialogMenu = gtk_dialog_new_with_buttons(
        "Senku Shapes",
        GTK_WINDOW(pWindowMain),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "New Game", GTK_RESPONSE_OK,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Quit", GTK_RESPONSE_REJECT,
        NULL);
    gtk_window_set_position(GTK_WINDOW(pDialogMenu), GTK_WIN_POS_CENTER);
    gtk_window_set_transient_for(GTK_WINDOW(pDialogMenu), GTK_WINDOW(pWindowMain));
    gtk_widget_set_name(pDialogMenu, "dialogMenu");
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(pDialogMenu));

    gtk_container_add(GTK_CONTAINER(content_area), pRadio);
    gtk_container_add(GTK_CONTAINER(content_area), pRadio_1);
    gtk_container_add(GTK_CONTAINER(content_area), pRadio_2);

    gtk_widget_show_all(pDialogMenu);
    gint result = gtk_dialog_run(GTK_DIALOG(pDialogMenu));
    gint newGame = 0;
    switch (result)
    {
    case GTK_RESPONSE_OK:
        int radio_selected = which_radio_is_selected(gtk_radio_button_get_group(GTK_RADIO_BUTTON(pRadio)));
        OnPlay(NULL, GINT_TO_POINTER(radio_selected));
        newGame = radio_selected;
        gtk_widget_destroy(GTK_WIDGET(pDialogMenu));
        break;
    case GTK_RESPONSE_REJECT:
        g_application_quit(G_APPLICATION(app));
        newGame = GTK_RESPONSE_REJECT;
        break;
    case GTK_RESPONSE_CANCEL:
        gtk_widget_destroy(GTK_WIDGET(pDialogMenu));
        newGame = GTK_RESPONSE_CANCEL;
        break;
    default:
        gtk_widget_destroy(GTK_WIDGET(pDialogMenu));
    }
    return newGame;
}
/**
 *@brief Dialog box Menu
 *       choix des shapes modale et sans decoration (style screen splash)
 *@param determine l'option par defaut
 */
void _g_display_box_menu(gpointer pData)
{
    gint optK = (GPOINTER_TO_INT(pData)) ? GPOINTER_TO_INT(pData) : 0;
    char *shapeName[] = {"Shape English", "Shape German", "Shape Diamond"};
    const int sizeShapeName = (int)(sizeof(shapeName) / sizeof(shapeName[0]));
    const int boxMenuWidth = 360;
    const int boxMenuHeight = 340;
    const int boxMenuOptionSpacing = 20;
    const int boxMenuOptionPadding = 25;
    const int boxMenuButtonSpacing = 20;
    const gchar *labelQuit = "Quit";
    const gchar *labelPlay = "Play";
    pBoxMenu = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(pBoxMenu), TITLE_MENU);
    gtk_window_set_modal(GTK_WINDOW(pBoxMenu), TRUE);
    gtk_window_set_position(GTK_WINDOW(pBoxMenu), GTK_WIN_POS_CENTER);
    gtk_window_set_decorated(GTK_WINDOW(pBoxMenu), TRUE);
    gtk_window_set_deletable(GTK_WINDOW(pBoxMenu), FALSE);
    /* rend la fenetre de choix dependante de la fenetre principale */
    gtk_window_set_transient_for(GTK_WINDOW(pBoxMenu), GTK_WINDOW(pWindowMain));
    gtk_window_resize(GTK_WINDOW(pBoxMenu), boxMenuWidth, boxMenuHeight);
    gtk_container_set_border_width(GTK_CONTAINER(pBoxMenu), APPLICATION_BORDER_WIDTH);

    // options
    pBoxMenuOption = gtk_box_new(GTK_ORIENTATION_VERTICAL, boxMenuOptionSpacing);
    gtk_box_set_homogeneous(GTK_BOX(pBoxMenuOption), FALSE);
    plbTitle = gtk_label_new(TITLE_MAIN);
    gtk_box_pack_start(GTK_BOX(pBoxMenuOption), plbTitle, TRUE, FALSE, boxMenuOptionPadding);

    GtkWidget *pRadio[sizeShapeName];

    pRadio[0] = gtk_radio_button_new_with_label(NULL, shapeName[0]);
    gtk_box_pack_start(GTK_BOX(pBoxMenuOption), pRadio[0], FALSE, FALSE, 0);

    pRadio[1] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pRadio[0]), shapeName[1]);
    gtk_box_pack_start(GTK_BOX(pBoxMenuOption), pRadio[1], FALSE, FALSE, 0);

    pRadio[2] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pRadio[1]), shapeName[2]);
    gtk_box_pack_start(GTK_BOX(pBoxMenuOption), pRadio[2], FALSE, FALSE, 0);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pRadio[optK]), TRUE);

    // boutons <Quit> et <Play> ben oui au moins :)) */
    pBoxMenuButton = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, boxMenuButtonSpacing);
    pBtnMenuQuit = gtk_button_new_with_label(labelQuit);
    pBtnMenuPlay = gtk_button_new_with_label(labelPlay);
    /* on ajoute les boutons */
    gtk_box_pack_start(GTK_BOX(pBoxMenuButton), pBtnMenuPlay, TRUE, TRUE, boxMenuButtonSpacing);
    gtk_box_pack_start(GTK_BOX(pBoxMenuButton), pBtnMenuQuit, FALSE, FALSE, boxMenuButtonSpacing);
    /* on ajoute box des boutons à la box des menu*/
    gtk_box_pack_start(GTK_BOX(pBoxMenuOption), pBoxMenuButton, TRUE, FALSE, boxMenuOptionSpacing);
    /* on ajoute les options */
    gtk_container_add(GTK_CONTAINER(pBoxMenu), pBoxMenuOption);
    // les signaux
    g_signal_connect(G_OBJECT(pBtnMenuQuit), "clicked", G_CALLBACK(OnDestroy), pBoxMenu);
    g_signal_connect(G_OBJECT(pBtnMenuPlay), "clicked", G_CALLBACK(OnPlay), pRadio[0]); // radio a la place de pRadio[0]
    // on se la montre...
    gtk_widget_show_all(pBoxMenu);
}
/*
int
boardPlay( ) {
    int row = 0, column = 0, nbMove = 0, remainingPeg = 0 ;
            int *pRow = &row, *pColumn = &column ;
            int canDisplayBonusTimeScore = 0 ;
            double elapseTimer, totalTimer ;
            timerSetStartTimer( ) ;
            timerSetElapseTimer( ) ;
            scoreResetBonusTimeScore( ) ;
            caretakerNew( ) ;
    while (matrixCanMovePeg( )) {
        timerSetElapseTimer( ) ;
//        __displaySetCoordToSelect( pRow, pColumn ) ;
        if (*pRow == -1 || *pColumn == -1) {// Appel de UNDO pour dernier mouvement
            timerSetElapseTimer( ) ;
            if (originatorRestoreFromMemento( caretakerGetMemento( 1 ) )) {
//                printf( "You ask [UNDO] the last move!\n" ) ;
                matrixUpdate( UNDO ) ;
            }
//            else printf( "There is no action to [UNDO] :( \n" ) ;
            canDisplayBonusTimeScore = 0 ;
            }
        else {// selection normale
            timerSetElapseTimer( ) ;
                    nbMove = matrixSelectPeg( *pRow, *pColumn ) ;
            if (nbMove) {
                if (matrixUpdate( __displaySetCoordToMove( ) ))
                    canDisplayBonusTimeScore = 1 ;
                }
        }
        elapseTimer = timerGetElapseTimer( ) ;
        totalTimer = timerGetTotalTimer( ) ;
//        __displayTimer( elapseTimer, totalTimer ) ;
        if (canDisplayBonusTimeScore) {
            scoreSetCalculateBonusElapseTimer( elapseTimer ) ;
            __displayBonusTimeScore( ) ;
        }
        canDisplayBonusTimeScore = 0 ;
    }// while()
    timerSetStopTimer( ) ;
    remainingPeg = matrixCountRemainPeg( ) ;
    scoreSetRemainingPeg( remainingPeg ) ;
    __displayResult( remainingPeg ) ;
    return 1 ;
}
 */

/*
void
__displayHeader( ) {

    printf( "\t       __         __ ___ _      ___\n" ) ;
            printf( "\t      / /  ___   / //_(_) | /| / (_)\n" ) ;
            printf( "\t     / /__/ -_) / ,< / /| |/ |/ / /\n" ) ;
            printf( "\t    /____/\\__/ /_/|_/_/ |__/|__/_/   présente\n" ) ;
            printf( "\t\n" ) ;
            printf( "\t       _____            __\n" ) ;
            printf( "\t      / ___/___  ____  / /____  __\n" ) ;
            printf( "\t      \\__ \\/ _ \\/ __ \\/ //_/ / / /\n" ) ;
            printf( "\t     ___/ /  __/ / / / ,< / /_/ /\n" ) ;
            printf( "\t    /____/\\___/_/ /_/_/|_|\\__,_/     (c) 2016\n" ) ;
            printf( "\t\n" ) ;
            printf( "!==  Senku ver Beta 1.3   	    (c) 2016   Le KiWi  ==!\n\n" ) ;
            printf( "\n" ) ;

}
 */

/*
void
__displayMenu( ) {

    printf( "!==\t\t\t\t       \t\t\t\t==!\n" ) ;
            printf( "!==\t\t\t\tM E N U\t\t\t\t==!\n" ) ;
            printf( "!==\t\t\t\t       \t\t\t\t==!\n" ) ;
            printf( "!==\t\t\t# [1] Shape English       \t\t==!\n" ) ;
            printf( "!==\t\t\t# [2] Shape German        \t\t==!\n" ) ;
            printf( "!==\t\t\t# [3] Shape Diamond       \t\t==!\n" ) ;
            printf( "!==\t\t\t# [4] Quit                \t\t==!\n" ) ;
            printf( "!==\t\t\t\t       \t\t\t\t==!\n" ) ;
}
 */

/*
int
__getMenuChoice( ) {
    int num = 0, c ;
            printf( "Enter a number between 1 and 4\n" ) ;
            printf( "Type your choice: " ) ;
    if (!scanf( " %2d[1-4]", &num )) {
        while (((c = getchar( )) != '\n') && c != EOF) ;
        }
    scanf( "%*[^\n]" ) ;
            getchar( ) ; // enleve '\n' restant

    return num ;
}
 */

/*
void
__displaySetCoordToSelect( int *numX, int *numY ) {
    int c ;
            printf( "\nSelect a peg's row and column number format like Xrow Ycol or -1 -1 to UNDO: " ) ;
    while (scanf( " %2i %2i", numX, numY ) != 2 || *numX > 9 || *numY > 9) {

        while (((c = getchar( )) != '\n') && c != EOF) ;
 *numX = 0 ;
 *numY = 0 ;
                printf( "Erreur de saisie ! try again\n" ) ;
                printf( "Select a peg's row and column number format like Xrow Ycol: " ) ;
        }
    scanf( "%*[^\n]" ) ;
            getchar( ) ; // enleve '\n' restant
}
 */
/**
 *
 * @return retourne le texte de la direction
 */
/*
int
__displaySetCoordToMove( ) {
    char *sDir[] = {"", "NORTH", "EAST", "SOUTH", "WEST", "the first BY DEFAULT", "unknown!"} ;
    char dir ;
            int i = 0 ;
            printf( "\nType the first letter for the direction: " ) ;
    if (!scanf( "%1c", &dir )) {
        int c ;
                printf( "Erreur de saisie !\n" ) ;
        while (((c = getchar( )) != '\n') && c != EOF) ; //vide buffer
                exit( EXIT_FAILURE ) ;
        }
    switch (toupper( dir )) {
    case 'N':
        i = 1 ;
        break ;
    case 'E':
        i = 2 ;
        break ;
    case 'S':
        i = 3 ;
        break ;
    case 'W':
        i = 4 ;
        break ;
    case '\n':
        i = 5 ;
        break ;
    default:
        i = 6 ;
    }
    printf( "You choose direction %s\n", sDir[i] ) ;
    return i ;
}
 */

/*
void
__displayTimer( double elapseTimer, double totalTimer ) {
    printf( "  _\n" ) ;
            printf( " \\ /  Elapsed Time of Reflexion: %2.f sec.\n", elapseTimer ) ;
            printf( " /_\\  Total Time: %2.fmin %02.fsec\n", timerGetMktime( totalTimer )->mkt_min, timerGetMktime( totalTimer )->mkt_sec ) ;
}
 */

/*
void
__displayBonusTimeScore( ) {
    printf( "\n" ) ;
            printf( "  ~~~\n" ) ;
            printf( " |@ @| Extra Bonus Time: %d\n", scoreGetBonusTimeScore( ) ) ;
            printf( "  (o)\n" ) ;
}
 */

/*
void
__displayResult( int remainingPegs ) {
    printf( "|-------------------------------------------------|\n" ) ;
    if (remainingPegs > 2)
            printf( "  /!\\ NO MORE MOVE Try again... /!\\\n" ) ;
    else if (remainingPegs == 2)
            printf( "  /!\\ NO MORE MOVE The victory is imminent! /!\\\n" ) ;

    else
        printf( "  Oo. Oh Yeaah! You WIN, you are a real Senku .oO\n" ) ;
            printf( "  Remaining Pegs: %d\n", remainingPegs ) ;
            printf( "|-------------------------------------------------|\n" ) ;
    }
 */

/*
int
__displayPlayAgain( ) {
    char buffer[] = {'\0'} ;
    printf( "\n|-------------------------------------------------|\n" ) ;
            printf( "| Play again [Yes|No]? : " ) ;
    if (!scanf( "%s", buffer )) {
        int c ;
                printf( "Erreur de saisie !  Press Enter\n" ) ;
        while (((c = getchar( )) != '\n') && c != EOF) ;
        }
    getchar( ) ;
    return ( !strncmp( buffer, "Y", 1 ) || !strncmp( buffer, "y", 1 )) ? 1 : 0 ;
}
 */

void OnDestroy(GtkWidget *pWidget, gpointer pData)
{
    pDialogBoxQuit = gtk_message_dialog_new(GTK_WINDOW(pWindowMain), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, "Do you Really wish to Quit Senku ?");
    gtk_window_set_title(GTK_WINDOW(pDialogBoxQuit), "Confirm QUIT");
    gint result = gtk_dialog_run((GTK_DIALOG(pDialogBoxQuit)));

    switch (result)
    {
    case GTK_RESPONSE_OK:
        g_application_quit(G_APPLICATION(app));
        break;
    default:
        gtk_widget_destroy(pDialogBoxQuit);
        gtk_widget_show_all(pWindowMain);
    }
}

void OnUndo(GtkWidget *pWidget, gpointer pData)
{
    char *msg;
    pMemento pMementoUndo = (memento *)malloc(1 * sizeof(memento));
    if (pMementoLastUndo->mvtEnd.row != 0 && pMementoLastUndo != NULL)
    {
        //        g_print("\nDEBUG pMementoLastUndo end.row %d end.column %d",pMementoLastUndo->mvtEnd.row,pMementoLastUndo->mvtEnd.column) ;
    }
    if ((pMementoUndo = caretakerGetMemento(1)))
    {
        msg = ACTION_UNDO;
        gtk_label_set_label(GTK_LABEL(plbComments), ACTION_UNDO);
        gtk_style_context_add_class(gtk_widget_get_style_context(plbComments), "matrix-name-label");
        if (!strcmp(msg, NO_ACTION_UNDO))
        {
            //            g_print("\nDEBUG Last Undo memento = NULL") ;
            //            pMementoLastUndo = NULL ;
        }
        else if (pMementoUndo != NULL && !strcmp(msg, ACTION_UNDO))
        {
            originatorRestoreFromMemento(pMementoUndo);
            _g_displayUndoMatrix(pMementoUndo);
            gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", matrixCountRemainPeg()));
        }
    }

    else
    {
        msg = NO_ACTION_UNDO;
        gtk_label_set_label(GTK_LABEL(plbComments), NO_ACTION_UNDO);
        gtk_style_context_add_class(gtk_widget_get_style_context(plbComments), "matrix-name-label");
        gtk_widget_set_state_flags(pButtonUndo, GTK_STATE_FLAG_INSENSITIVE, TRUE);
    }
}

void _setLastMementoUndoRedrawNormal(pMemento pm)
{
    int coefRow = 0, coefColumn = 0, x = 0, y = 0;
    if (pm != NULL && pm->mvtEnd.row != 0)
    {
        x = pm->mvtEnd.row;
        y = pm->mvtEnd.column;
        coefRow = pm->mvtBetween.row - pm->mvtStart.row;
        coefColumn = pm->mvtBetween.column - pm->mvtStart.column;
        GtkWidget *imgPegDelete = gtk_image_new_from_file(IMG_PEG_DELETE);
        GtkWidget *imgPegMove_2 = gtk_image_new_from_file(IMG_PEG_MOVE);
        GtkWidget *imgPegMove_3 = gtk_image_new_from_file(IMG_PEG_MOVE);
        if (pMementoLastUndo != NULL)
        {
            gtk_widget_destroy(gtk_grid_get_child_at(GTK_GRID(pGridMatrix), pMementoLastUndo->mvtEnd.column, pMementoLastUndo->mvtEnd.row));
            gtk_widget_destroy(gtk_grid_get_child_at(GTK_GRID(pGridMatrix), pMementoLastUndo->mvtBetween.row, pMementoLastUndo->mvtBetween.row));
            gtk_widget_destroy(gtk_grid_get_child_at(GTK_GRID(pGridMatrix), pMementoLastUndo->mvtStart.column, pMementoLastUndo->mvtStart.row));
            gtk_grid_attach(GTK_GRID(pGridMatrix), imgPegDelete, pMementoLastUndo->mvtEnd.column, pMementoLastUndo->mvtEnd.row, 1, 1);
            gtk_grid_attach(GTK_GRID(pGridMatrix), imgPegMove_2, pMementoLastUndo->mvtBetween.row, pMementoLastUndo->mvtBetween.row, 1, 1);
            gtk_grid_attach(GTK_GRID(pGridMatrix), imgPegMove_3, pMementoLastUndo->mvtStart.column, pMementoLastUndo->mvtStart.row, 1, 1);
        }
    }
}

void OnNewGame(GtkWidget *pWidget, gpointer pData)
{
    gint response = _g_display_box_menu_new_game(NULL);
    if (response != GTK_RESPONSE_CANCEL)
    {
        scoreResetBonusTimeScore();
        gtk_label_set_label(GTK_LABEL(plbComments), currentMatrixOfBoard.name);
        gtk_style_context_add_class(gtk_widget_get_style_context(plbComments), "matrix-name-label");

        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", 0));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_PEG]), "value-values-label");

        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_BONUS]), g_strdup_printf("%4.0lf", 0.0));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_BONUS]), "value-values-label");

        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_TIME]), g_strdup_printf("%4d", 0));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_TIME]), "value-values-label");

        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", matrixCountRemainPeg()));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_PEG]), "value-values-label");
        caretakerNew();
        _g_displayAllMatrix();
    }
}

/// @brief
/// @param pWidget
/// @param pData
void OnRotate(GtkWidget *pWidget, gpointer pData)
{
    // THETA_COUNTER_CLOCKWISE || THETA_CLOCKWISE
    matrixRotate((theta)pData);
    _g_displayAllMatrix();
}

void _g_displayAllMatrix()
{
    GtkWidget *imgPegStatus = NULL;

    for (int row = 0; row < HOR_MAX; row++)
    {
        for (int column = 0; column < VER_MAX; column++)
        {
            imgPegStatus = gtk_grid_get_child_at(GTK_GRID(pGridMatrix), column, row);
            if (pMatrixLoad[row][column] == 1)
            {
                gtk_style_context_remove_class(gtk_widget_get_style_context(imgPegStatus), "peg-delete");
                gtk_style_context_add_class(gtk_widget_get_style_context(imgPegStatus), "peg-move");
            }
            else if (pMatrixLoad[row][column] == 0)
            {
                gtk_style_context_remove_class(gtk_widget_get_style_context(imgPegStatus), "peg-move");
                gtk_style_context_add_class(gtk_widget_get_style_context(imgPegStatus), "peg-delete");
            }
            else
            {
                gtk_style_context_add_class(gtk_widget_get_style_context(imgPegStatus), "peg-board");
            }
        }
    }
    gtk_widget_show_all(GTK_WIDGET(pGridMatrix));
}

void _g_new_GridMatrix()
{
    pGridMatrix = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(pGridMain), pGridMatrix, 0, 0, 1, 1);
    gtk_widget_set_halign(GTK_WIDGET(pGridMatrix), GTK_ALIGN_CENTER);
    gtk_widget_set_name(pGridMatrix, "gridMatrix");
}

void _g_displayUndoMatrix(pMemento pm)
{
    int coefRow = 0, coefColumn = 0, x = 0, y = 0;
    if (pm)
    {
        matrixUpdate(UNDO);
        _firstSelectPeg("set", TRUE);
        x = pm->mvtEnd.row;
        y = pm->mvtEnd.column;
        coefRow = pm->mvtBetween.row - pm->mvtStart.row;
        coefColumn = pm->mvtBetween.column - pm->mvtStart.column;

        GtkWidget *imgPegDeleteUndo = gtk_grid_get_child_at(GTK_GRID(pGridMatrix), y, x);
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPegDeleteUndo), "peg-move");
        gtk_style_context_add_class(gtk_widget_get_style_context(imgPegDeleteUndo), "peg-delete");

        GtkWidget *imgPegMove_1 = gtk_grid_get_child_at(GTK_GRID(pGridMatrix), y - 1 * coefColumn, x - 1 * coefRow);
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPegMove_1), "peg-delete");
        gtk_style_context_add_class(gtk_widget_get_style_context(imgPegMove_1), "peg-move");

        GtkWidget *imgPegUndo = gtk_grid_get_child_at(GTK_GRID(pGridMatrix), y - 2 * coefColumn, x - 2 * coefRow);
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPegUndo), "peg-delete");
        gtk_style_context_add_class(gtk_widget_get_style_context(imgPegUndo), "peg-move");

        gtk_widget_show_all(GTK_WIDGET(pGridMatrix));
    }
}

gboolean
_firstSelectPeg(char *action, gboolean value)
{
    static gboolean firstSelectPeg = TRUE;
    if (!strcmp(action, "get"))
    {
        return firstSelectPeg;
    }
    else if (!strcmp(action, "set"))
    {
        firstSelectPeg = value;
        return firstSelectPeg;
    }
    return -1;
}

void OnSelect(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
    static Coord pOld = {0, 0};
    const int deltaConstantXY = 2;
    int remainingPeg = 0;
    int rank = 0;
    actionSelect action = 0;
    size_t size = sizeof(Coord);
    Coord *p = (Coord *)g_malloc(size);
    p = (Coord *)pData;

    gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", matrixCountRemainPeg()));
    gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_PEG]), "value-values-label");
    //    g_print( "\nDEBUG :: Coord Old X:%d Y:%d", pOld.x, pOld.y ) ;
    //    g_print( "\nDEBUG :: Coord New X:%d Y:%d", p->x, p->y ) ;
    if (matrixCanMovePeg())
    {
        if (_firstSelectPeg("get", FALSE))
        { // premier clic de selection
            //            g_print("\nDEBUG :: premier selection clic") ;
            gtk_widget_set_state_flags(pButtonRotateLeft, GTK_STATE_FLAG_INSENSITIVE, TRUE);
            gtk_widget_set_state_flags(pButtonRotateRight, GTK_STATE_FLAG_INSENSITIVE, TRUE);
            timerStartClock();
            if (matrixSelectPeg(p->x, p->y))
            {
                _firstSelectPeg("set", FALSE);
                _g_displayUpdateMatrix(ACTION_SELECT_PEG, p->x, p->y);
                if ((pOld.x || pOld.y) && (pMatrixLoad[pOld.x][pOld.y] == 1))
                { /* unselect si l'ancien si existe */
                    _g_displayUpdateMatrix(ACTION_SELECT_UNSELECT_PEG, pOld.x, pOld.y);
                }
                pOld.x = p->x;
                pOld.y = p->y;
            }
        }
        else
        { // seconde selection cliquée
            //            g_print("\nDEBUG :: seconde selection clic") ;
            timerStopClock();
            //            g_printf( "\nDEBUG :: Elapse %f", timerGetElapseClock( ) * 1000 ) ;
            if (matrixSelectPeg(pOld.x, pOld.y))
            { // si prise possible
                int deltaX = 0, deltaY = 0, sumDelta = 0;
                deltaX = pOld.x - p->x;
                deltaY = pOld.y - p->y;
                sumDelta = deltaX + deltaY;
                _firstSelectPeg("set", TRUE);
                gtk_widget_set_state_flags(pButtonRotateLeft, GTK_STATE_FLAG_NORMAL, TRUE);
                gtk_widget_set_state_flags(pButtonRotateRight, GTK_STATE_FLAG_NORMAL, TRUE);

                //                g_print( "\nDEBUG :: deltaX: %d deltaY: %d sumDelta: %d", deltaX, deltaY, sumDelta ) ;
                //                g_print( "\nDEBUG :: pOldX: %d pOldY: %d px: %d py: %d", pOld.x, pOld.y, p->x, p->y ) ;
                if (deltaX != deltaY && (sumDelta == 2 || sumDelta == -2))
                {
                    if (deltaConstantXY == abs(deltaX) && (deltaY == 0))
                        action = (deltaX > 0) ? ACTION_SELECT_TAKE_NORTH : ACTION_SELECT_TAKE_SOUTH;
                    else if ((deltaConstantXY == abs(deltaY)) && (deltaX == 0)) // bug deltaX !=0
                        action = (deltaY > 0) ? ACTION_SELECT_TAKE_WEST : ACTION_SELECT_TAKE_EAST;
                    if (matrixUpdate(action))
                    {
                        _g_displayUpdateMatrix(action, p->x, p->y);

                        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", matrixCountRemainPeg()));
                        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_PEG]), "value-values-label");

                        gtk_widget_set_state_flags(pButtonUndo, GTK_STATE_FLAG_NORMAL, TRUE);
                        pOld.x = p->x;
                        pOld.y = p->y;
                        if (matrixSelectPeg(pOld.x, pOld.y))
                        {
                            _firstSelectPeg("set", FALSE);
                            _g_displayUpdateMatrix(ACTION_SELECT_PEG, pOld.x, pOld.y);
                        }
                        scoreSetCalculateBonusElapseTimer(timerGetElapseClock() * 1000);
                        // printf("%.0f\n", scoreGetBonusTimeScore()); //DEBUG

                        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_BONUS]), g_strdup_printf("%4.0lf", scoreGetBonusTimeScore()));
                        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_BONUS]), "value-values-label");
                    }
                    else if (matrixSelectPeg(p->x, p->y))
                    { // clique changement d'avis avec prise
                        _firstSelectPeg("set", FALSE);
                        _g_displayUpdateMatrix(ACTION_SELECT_UNSELECT_PEG, pOld.x, pOld.y);
                        _g_displayUpdateMatrix(ACTION_SELECT_PEG, p->x, p->y);

                        pOld.x = p->x;
                        pOld.y = p->y;
                    }
                    else
                    { // changement d'avis sans prise (ie: erreur de second clique)
                        _firstSelectPeg("set", FALSE);
                    }
                    if (!matrixCanMovePeg())
                    {
                        remainingPeg = matrixCountRemainPeg();
                        scoreSetRemainingPeg(remainingPeg);
                        gtk_style_context_add_class(gtk_widget_get_style_context(plbComments), "matrix-name-label");
                        gtk_widget_set_state_flags(pButtonUndo, GTK_STATE_FLAG_INSENSITIVE, TRUE);
                        timerStopClock();
                        if (handleTimeout)
                        {
                            g_source_remove(handleTimeout);
                            g_timeout_add(TIMER_DELAY, _g_display_time, GINT_TO_POINTER(FALSE));
                        }
                        if ((rank = scoreNew()))
                            _g_display_get_name(rank);
                    }
                }
                else if (sumDelta == 0 && (deltaX != -deltaY))
                { // on reclic sur le meme que le premier
                    if (matrixSelectPeg(p->x, p->y))
                    {                                                          // en excluant la cdtions particuliere sumdelta==0
                        _g_displayUpdateMatrix(ACTION_SELECT_PEG, p->x, p->y); // pour une autre raison (pions coins opposes d'un carre)
                    }
                }
                else
                { // ni prise ni meme peg de depart
                    //                    g_print( "\nDEBUG :: change selection de depart si prise possible\n" ) ;
                    _firstSelectPeg("set", FALSE);
                    if (matrixSelectPeg(p->x, p->y))
                    { // si une prise possible
                        _g_displayUpdateMatrix(ACTION_SELECT_UNSELECT_PEG, pOld.x, pOld.y);
                        _g_displayUpdateMatrix(ACTION_SELECT_PEG, p->x, p->y);
                        gtk_widget_set_state_flags(pButtonRotateLeft, GTK_STATE_FLAG_INSENSITIVE, TRUE);
                        gtk_widget_set_state_flags(pButtonRotateRight, GTK_STATE_FLAG_INSENSITIVE, TRUE);
                        pOld.x = p->x;
                        pOld.y = p->y;
                    }
                }
            }
            //        else
            //            g_print( "\nDEBUG :: prise impossible " ) ;
        }
        gtk_widget_show_all(GTK_WIDGET(pGridMain));
    }
}

void _g_labelSet(GtkWidget *pWidget, gpointer pData)
{
    char *markup = g_markup_printf_escaped(SENKU_PANGO_MARKUP_LABEL(LABEL_COLOR_TEXT, d), GPOINTER_TO_INT(pData));
    gtk_label_set_markup(GTK_LABEL(pWidget), markup);
    gtk_label_set_use_markup(GTK_LABEL(pWidget), TRUE);
    g_free(markup);
}

void _g_displayUpdateMatrix(actionSelect action, const int x, const int y)
{
    gint coefRow = 0, coefColumn = 0; // coefficient d'effacement

    GtkWidget *imgPeg = gtk_grid_get_child_at(GTK_GRID(pGridMatrix), y, x);
    // GList *listClass = gtk_style_context_list_classes(gtk_widget_get_style_context(imgPeg));
    // GList *l;
    // for (l = listClass; l != NULL; l = l->next)
    // {
    //     g_printf("Class : %s X : %d Y : %d\n", (char *)l->data, x, y);
    //     ;
    // }
    // g_list_free(listClass);

    switch (action)
    {
    case ACTION_SELECT_PEG:
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-hover");
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-delete");
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-move");
        gtk_style_context_add_class(gtk_widget_get_style_context(imgPeg), "peg-select");
        break;
    case ACTION_SELECT_UNSELECT_PEG:
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-hover");
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-delete");
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-select");
        gtk_style_context_add_class(gtk_widget_get_style_context(imgPeg), "peg-move");
        break;
    case ACTION_SELECT_TAKE_NORTH:
        coefRow = -1;
        coefColumn = 0;
        break;
    case ACTION_SELECT_TAKE_EAST:
        coefRow = 0;
        coefColumn = 1;
        break;
    case ACTION_SELECT_TAKE_SOUTH:
        coefRow = 1;
        coefColumn = 0;
        break;
    case ACTION_SELECT_TAKE_WEST:
        coefRow = 0;
        coefColumn = -1;
        break;
    default:
        break;
    }
    if (action != ACTION_SELECT_PEG && action != ACTION_SELECT_UNSELECT_PEG && action)
    {
        imgPeg = gtk_grid_get_child_at(GTK_GRID(pGridMatrix), y, x);
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-hover");
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-delete");
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-select");
        gtk_style_context_add_class(gtk_widget_get_style_context(imgPeg), "peg-move");

        imgPeg = gtk_grid_get_child_at(GTK_GRID(pGridMatrix), y - 1 * coefColumn, x - 1 * coefRow);
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-hover");
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-select");
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-move");
        gtk_style_context_add_class(gtk_widget_get_style_context(imgPeg), "peg-delete");

        imgPeg = gtk_grid_get_child_at(GTK_GRID(pGridMatrix), y - 2 * coefColumn, x - 2 * coefRow);
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-hover");
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-select");
        gtk_style_context_remove_class(gtk_widget_get_style_context(imgPeg), "peg-move");
        gtk_style_context_add_class(gtk_widget_get_style_context(imgPeg), "peg-delete");
    }
}

void OnPlay(GtkWidget *pWidget, gpointer pData)
{ // equivalent while (!matrixLoad( num = __getMenuChoice( ) )) ;
    gtk_style_context_remove_class(gtk_widget_get_style_context(pGridMatrix), "logo");
    gtk_widget_set_state_flags(pButtonUndo, GTK_STATE_FLAG_INSENSITIVE, TRUE);

    if (matrixLoad(GPOINTER_TO_INT(pData)))
    {
        _g_displayMatrix(pMatrixLoad);
        onlyOneBoard.set = &currentMatrixOfBoard;
        caretakerNew(); // pattern memento du mecanisme pour le Undo
        scoreResetBonusTimeScore();
        timerStartClock();

        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", matrixCountRemainPeg()));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_PEG]), "value-values-label");

        gtk_label_set_label(GTK_LABEL(plbComments), currentMatrixOfBoard.name);
        gtk_style_context_add_class(gtk_widget_get_style_context(plbComments), "matrix-name-label");
        if (handleTimeout)
        {
            g_source_remove(handleTimeout);
            g_timeout_add(TIMER_DELAY, _g_display_time, GINT_TO_POINTER(FALSE));
            gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_TIME]), g_strdup_printf("%4d", 0));
            gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_TIME]), "value-values-label");
        }
        handleTimeout = g_timeout_add(TIMER_DELAY, _g_display_time, GINT_TO_POINTER(TRUE));
        gtk_widget_show_all(pWindowMain);
    }
}

gboolean
_g_display_time(gpointer pData)
{
    static int i = 1;
    gboolean start = GPOINTER_TO_INT(pData);
    if (start)
    {
        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_TIME]), g_strdup_printf("%4d", i++));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_TIME]), "value-values-label");
    }
    else
        i = 1;
    return start;
}

int OnRadioToggled(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
    return 0;
}

int which_radio_is_selected(GSList *group)
{
    GSList *Group = group;
    int i = 0, j = 0;
    j = g_slist_length(group);
    for (; group != NULL; group = group->next)
    {
        GtkRadioButton *radio = group->data;
        i = g_slist_position(Group, group);
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio)))
        {
            return (j - i);
        }
    }
    return 0;
}

void _g_erase_displayMatrix()
{
    gtk_widget_destroy(GTK_WIDGET(pGridMatrix));
}

void _g_displayMatrix(Matrix matrix)
{
    int i, k;
    GtkWidget *imgPeg = NULL;
    GtkWidget *pMatrix_event[HOR_MAX][VER_MAX];
    pEventCoord = (Coord *)malloc(HOR_MAX * VER_MAX * sizeof(Coord));
    if (pEventCoord)
        ;
    // pEventCoord = &eventCoord;
    else
    {
        //        g_print( "\nDEBUG :: fonction: _g_displayMatrix allocation failure" ) ;
        exit(EXIT_FAILURE);
    }
    // g_print("\nDEBUG :: fonction: _g_displayMatrix [ok]\n");
    for (k = 0; k < HOR_MAX; k++)
    {
        for (i = 0; i < VER_MAX; i++)
        {
            if (gtk_grid_get_child_at(GTK_GRID(pGridMatrix), i, k) != NULL)
                gtk_widget_destroy(GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(pGridMatrix), i, k)));
            imgPeg = gtk_image_new_from_file(IMG_PEG_TRANSPARENT);
            switch (matrix[k][i])
            {
            case -1:
                gtk_style_context_add_class(gtk_widget_get_style_context(imgPeg), "peg-board");
                break;
            case 0:
                gtk_style_context_add_class(gtk_widget_get_style_context(imgPeg), "peg-delete");
                break;
            case 1:
                gtk_style_context_add_class(gtk_widget_get_style_context(imgPeg), "peg-move");
                break;
            default:;
            }
            pMatrix_event[k][i] = gtk_event_box_new();
            gtk_grid_attach(GTK_GRID(pGridMatrix), pMatrix_event[k][i], i, k, 1, 1);
            gtk_grid_attach(GTK_GRID(pGridMatrix), GTK_WIDGET(imgPeg), i, k, 1, 1);
            pEventCoord->x = k;
            pEventCoord->y = i;
            g_signal_connect(G_OBJECT(pMatrix_event[k][i]), "button_press_event", G_CALLBACK(OnSelect), (gpointer)pEventCoord);
            // g_signal_connect(G_OBJECT(pMatrix_event[k][i]), "enter_notify_event", G_CALLBACK(OnEnter), (gpointer)pEventCoord);
            // g_signal_connect(G_OBJECT(pMatrix_event[k][i]), "leave_notify_event", G_CALLBACK(OnLeave), (gpointer)pEventCoord);
            pEventCoord++;
        }
    }
}

void _g_display_get_name(int rank)
{
    GtkWidget *pEntryName = NULL;
    GtkWidget *pLabelName = NULL;
    GtkWidget *pLabelMessage = NULL;
    GtkWidget *pButtonOk = NULL;
    GtkWidget *pGridGetName = NULL;
    const char *labelNom = "";
    char *labelMessage = "\nCongratulations, ";
    const char *labelInsideEntry = "Username";
    score *findRecord = (score *)scoreGetSortScore(rank);
    const char *title = "Saving score";
    const char *msgAdd;
    if (findRecord->remainingPeg == 1)
        msgAdd = "\n\t\tYeaah!  You're a real SENKU";
    else if (findRecord->remainingPeg == 2)
        msgAdd = "\n\t\tThe victory is imminent!";
    else
        msgAdd = "\n\t\tNobody is perfect :)";
    const char *topmost = g_strdup_printf("you're number %d in topmost!\n%s", rank, msgAdd);
    pLabelName = gtk_label_new(labelNom);
    labelMessage = g_strconcat(labelMessage, topmost, NULL);
    pLabelMessage = gtk_label_new(labelMessage);
    pWindowGetName = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(pWindowGetName, "windowGetName");
    gtk_window_set_resizable(GTK_WINDOW(pWindowGetName), FALSE);
    gtk_window_set_deletable(GTK_WINDOW(pWindowGetName), FALSE);
    gtk_window_set_title(GTK_WINDOW(pWindowGetName), title);
    pGridGetName = gtk_grid_new();
    pButtonOk = gtk_button_new_with_label("Register");
    gtk_widget_set_can_default(GTK_WIDGET(pButtonOk), TRUE);
    gtk_window_set_default(GTK_WINDOW(pWindowGetName), GTK_WIDGET(pButtonOk));
    pEntryName = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(pEntryName), ""); // labelInsideEntry
    gtk_entry_set_placeholder_text(GTK_ENTRY(pEntryName), labelInsideEntry);
    gtk_entry_set_max_length(GTK_ENTRY(pEntryName), MAX_CAR_NAME - 1);
    gtk_entry_set_input_purpose(GTK_ENTRY(pEntryName), GTK_INPUT_PURPOSE_NAME);
    gtk_entry_set_activates_default(GTK_ENTRY(pEntryName), TRUE);
    gtk_grid_attach(GTK_GRID(pGridGetName), pLabelName, 0, 0, 1, 1);
    gtk_grid_attach_next_to(GTK_GRID(pGridGetName), pLabelMessage, pLabelName, GTK_POS_TOP, 3, 1);
    gtk_grid_attach_next_to(GTK_GRID(pGridGetName), pEntryName, pLabelName, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to(GTK_GRID(pGridGetName), pButtonOk, NULL, GTK_POS_BOTTOM, 3, 1); // pEntryName
    pDataName = (dataName *)malloc(1 * (sizeof(dataName)));
    if (pDataName)
    {
        pDataName->pWidgetName = pEntryName;
        pDataName->rang = rank;
        g_signal_connect(G_OBJECT(pButtonOk), "clicked", G_CALLBACK(OnSetName), pDataName);
    }
    else
        exit(EXIT_FAILURE);
    g_signal_connect(G_OBJECT(pWindowGetName), "destroy", G_CALLBACK(OnDestroyGetName), GINT_TO_POINTER(rank));
    gtk_container_add(GTK_CONTAINER(pWindowGetName), pGridGetName);
    gtk_window_set_position(GTK_WINDOW(pWindowGetName), GTK_WIN_POS_CENTER_ALWAYS);

    gtk_widget_show_all(pWindowGetName);
    gtk_widget_grab_focus(pButtonOk);
}

void OnSetName(GtkWidget *pWidget, dataName *pData)
{
    int rank = pData->rang;
    const gchar *sName = gtk_entry_get_text(GTK_ENTRY(pData->pWidgetName));
    scoreSetNamePlayer(sName, rank);
    ;
    scoreSetNameMatrix(g_strdup_printf("%10s", currentMatrixOfBoard.name), rank);
    pScore resultScore = (score *)malloc(SCORE_BEST_OF * sizeof(score));
    if (resultScore)
    {
        resultScore = (pScore)scoreGetSortScore(0); /*arg: (int) NULL */
        _g_display_box_score(resultScore, rank);
    }
    else
        g_free(resultScore);
    gtk_widget_destroy(pWindowGetName);
}

void OnDisplayScore(GtkWidget *pWidget, dataName *pData)
{
    pScore resultScore = (score *)malloc(SCORE_BEST_OF * sizeof(score));
    if (resultScore)
        resultScore = (pScore)scoreGetSortScore(0); /* arg:(int) NULL */
    _g_display_box_score(resultScore, 0);
    // g_free(resultScore);
}

void OnDestroyGetName(GtkWidget *pWidget, gpointer pData)
{
}

void OnEnter(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
    Coord *p = pData;
    GtkWidget *pPeg = gtk_grid_get_child_at(GTK_GRID(pGridMatrix), p->y, p->x);
    // g_printf("pPeg-=>%s x:%d y:%d\n",gtk_widget_get_name(pPeg),  p->x, p->y);    //, gtk_widget_get_name(pPeg), p->x, p->y);
    // gtk_widget_set_visible(pPeg, FALSE)   ;
    gtk_image_set_from_file(GTK_IMAGE(pPeg), IMG_PEG_TRANSPARENT);
    if (pMatrixLoad[p->y][p->x] == 1)
    {
        gtk_style_context_add_class(gtk_widget_get_style_context(pPeg), "peg-hover");
    }
    else if (pMatrixLoad[p->y][p->x] == 0)
    {
        gtk_style_context_add_class(gtk_widget_get_style_context(pPeg), "peg-delete");
    }
}

void OnLeave(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
    Coord *p = pData;
    GtkWidget *pPeg = gtk_grid_get_child_at(GTK_GRID(pGridMatrix), p->y, p->x);
    // printf("pPeg: %s x: %d y: %d\n", gtk_widget_get_name(pPeg), p->x, p->y);
    gtk_image_set_from_file(GTK_IMAGE(pPeg), IMG_PEG_TRANSPARENT);
    gtk_style_context_remove_class(gtk_widget_get_style_context(pPeg), "peg-hover");
    if (pMatrixLoad[p->y][p->x] == 1)
    {
        gtk_style_context_add_class(gtk_widget_get_style_context(pPeg), "peg-move");
    }
    else if (pMatrixLoad[p->y][p->x] == 0)
    {
        gtk_style_context_add_class(gtk_widget_get_style_context(pPeg), "peg-delete");
    }
    else
    {
        gtk_style_context_add_class(gtk_widget_get_style_context(pPeg), "peg-board");
    }
}

void _g_display_box_score(pScore ps, const int rank)
{
    int i, k;
    char *scoreTitle[] = {"RANK", "PLAYER", "PEG", "SCORE", "SHAPE"};
    int sizeArray = 0;

    GtkWidget *pButtonOk = NULL;
    GtkWidget *plbScoreOrder = NULL;
    GtkWidget *plbScorePlayer = NULL;
    GtkWidget *plbScorePeg = NULL;
    GtkWidget *plbScoreScore = NULL;
    GtkWidget *plbScoreRank = NULL;
    GtkWidget *lbScore[] = {plbScoreOrder, plbScorePlayer, plbScorePeg, plbScoreScore, plbScoreRank};
    sizeArray = (int)(sizeof(lbScore) / sizeof(GtkWidget *));
    pBoxScore = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(GTK_WIDGET(pBoxScore), "boxScore");
    gtk_window_set_title(GTK_WINDOW(pBoxScore), BOX_SCORE_TITLE);
    gtk_window_set_modal(GTK_WINDOW(pBoxScore), TRUE);
    gtk_window_set_position(GTK_WINDOW(pBoxScore), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_deletable(GTK_WINDOW(pBoxScore), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(pBoxScore), GTK_WINDOW(pWindowMain));
    gtk_window_set_resizable(GTK_WINDOW(pBoxScore), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(pBoxScore), APPLICATION_BORDER_WIDTH);
    /**
     * Grille du Score
     */
    pGridScore = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(pBoxScore), pGridScore);

    for (k = 0; k < sizeArray; k++)
    {
        lbScore[k] = gtk_label_new(scoreTitle[k]);
    }
    gtk_grid_attach(GTK_GRID(pGridScore), lbScore[0], 0, 0, 1, 1); // plbScoreOrder
    for (k = 0; k < sizeArray - 1; k++)
        gtk_grid_attach_next_to(GTK_GRID(pGridScore), lbScore[k + 1], lbScore[k], GTK_POS_RIGHT, 1, 1);
    for (i = 1; i <= SCORE_BEST_OF; i++)
    {
        lbScore[0] = gtk_label_new(g_strdup_printf("%d", i));
        lbScore[1] = gtk_label_new(ps->namePlayer);
        lbScore[2] = gtk_label_new(g_strdup_printf("%d", ps->remainingPeg));
        lbScore[3] = gtk_label_new(g_strdup_printf("%.f", ps->scoreGame));
        for (int j = 0; j < 4; j++)
        {
            (i == rank) ? gtk_style_context_add_class(gtk_widget_get_style_context(lbScore[j]), "rank")
                        : gtk_style_context_remove_class(gtk_widget_get_style_context(lbScore[j]), "rank");
        }
        lbScore[4] = gtk_label_new(ps->nameMatrix);
        gtk_grid_attach(GTK_GRID(pGridScore), lbScore[0], 0, i, 1, 1);
        for (k = 0; k < sizeArray - 1; k++)
            gtk_grid_attach_next_to(GTK_GRID(pGridScore), lbScore[k + 1], lbScore[k], GTK_POS_RIGHT, 1, 1);
        ps++;
    }
    pButtonOk = gtk_button_new_with_label("Close");
    gtk_grid_attach_next_to(GTK_GRID(pGridScore), pButtonOk, lbScore[sizeArray / 2 - 1], GTK_POS_BOTTOM, 3, 1);
    g_signal_connect(G_OBJECT(pButtonOk), "clicked", G_CALLBACK(OnCloseBoxScore), NULL);
    gtk_widget_show_all(pBoxScore);
}

void OnCloseBoxScore(GtkWidget *pWidget, gpointer pData)
{
    gtk_widget_destroy(pBoxScore);
}
