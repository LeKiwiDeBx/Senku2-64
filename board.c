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
 * translation
 */
#include <glib/gi18n.h>
#include <libintl.h>
#include <locale.h>

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
/* #define SENKU_PANGO_CONCAT_STR(color, type) "<span size=\"x-large\" weight=\"bold\" color=\"" #color "\">%" #type "</span>" */
/* #define SENKU_PANGO_MARKUP_LABEL(color, type) SENKU_PANGO_CONCAT_STR(color, type) */
/* #define SENKU_ABS(x) ((x)) ? (x) : (-x) */
/*  */
#define TITLE_MAIN _("Senku (GTK+3.22) 2.2.0 Beta\n\n[°} LeKiwiDeBx\n\nCopyright (C) 2016-2024 GNU GPLv3.")
#define TITLE_MENU _("Shapes choice")
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
#define NO_MORE_MOVE _("No more move!")
#define NO_ACTION_UNDO _("Nothing to [Undo]")
#define ACTION_UNDO _("[Undo] the previous move")
#define BLANK ""
#define BOX_SCORE_TITLE _("Scoring")
#define APPLICATION_TITLE _("Senku 2.2.0 Beta")
#define APPLICATION_SIZE_WIDTH 360
#define APPLICATION_SIZE_HEIGHT 340
#define APPLICATION_BORDER_WIDTH 10
#define MAX_LABEL 10 // nombre etiquette maximum coté droit
#define LABEL_BONUS_TEXT _("Bonus")
#define LABEL_PEG_TEXT _("Pegs")
#define LABEL_TIME_TEXT _("Time")
#define LABEL_LOGO _("[°} LeKiwiDeBx")
#define SENKU_THEME_PATH "theme/"
#define SENKU_THEME_FILE "senku.css"
#define IMG_BUTTON_NEW_GAME "image/img_button_new_game.png"

#define SENKU_LOCALE_FILE "fr_FR.UTF-8"
#define GETTEXT_DOMAIN_NAME "senku"
#define GETTEXT_DIR_NAME "locale"

static void
_g_displayAllMatrix();

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

static GTimer *timerSelection = NULL;
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
 * @brief appel par OnQueryTooltip pour afficher un message
 * @param pWidget
 * @param event   query-tooltip
 * @param pData
 */
void OnQueryTooltip(GtkWidget *self,
                    gint x,
                    gint y,
                    gboolean keyboard_mode,
                    GtkTooltip *tooltip,
                    gpointer user_data);

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
 * @brief Met à jour l'accessibilite des boutons Undo Turn left turn right...
 *
 * @param pArrayWidgets le tableau des boutons
 * @param size taille du tableau
 * @param state etat concernant l'accessibilite des boutons
 * @param sensitive si actif ou pas
 */
void _g_set_state_flags_buttons(GtkWidget *pArrayWidgets[], const int size, const int state, const gboolean sensitive);
/**
 * @brief gestion de la variable booleene firsSelectPeg
 * @param action "get" ou "set" une valeur boolean
 * @param value booleen à positionner
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
    /* creation fenetre Main */
    pWindowMain = gtk_application_window_new(app);
    // gtk_window_set_position(GTK_WINDOW(pWindowMain), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(pWindowMain), APPLICATION_TITLE);
    gtk_window_set_resizable(GTK_WINDOW(pWindowMain), FALSE);

    /* signal fermeture l'application sur fermeture fenetre principale */
    g_signal_connect(G_OBJECT(pWindowMain), "delete-event", G_CALLBACK(OnDestroy), NULL);

    /*  Creation de la grille principale container */
    pGridMain = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(pGridMain), FALSE);

    /* attachement de la grille matrice a la grille principale */
    pGridMatrix = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(pGridMain), pGridMatrix, 0, 0, 1, 1);

    gtk_widget_set_halign(GTK_WIDGET(pGridMatrix), GTK_ALIGN_CENTER);
    /* attachement de la grille pricipale a la fenetre principale */
    gtk_container_add(GTK_CONTAINER(pWindowMain), pGridMain);
    // //DEBUG GTK4 nouvel equivalent
    //  gtk_window_set_child() , gtk_box_append , gtk_scrolled_window_set_child , gtk_info_bar_add_child , etc.

    /* Button bottom  <New Game> <Undo> <RotLeft> <RotRight> <Quit> */
    pHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_top(GTK_WIDGET(pHbox), 0);
    gtk_widget_set_halign(GTK_WIDGET(pHbox), GTK_ALIGN_CENTER);

    pButtonNewGame = gtk_button_new();
    gtk_widget_set_has_tooltip(pButtonNewGame, TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonNewGame), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonNewGame, FALSE, FALSE, 0);

    pButtonUndo = gtk_button_new();
    gtk_widget_set_has_tooltip(pButtonUndo, TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonUndo), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonUndo, FALSE, FALSE, 0);

    pButtonScore = gtk_button_new();
    gtk_widget_set_has_tooltip(pButtonScore, TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonScore), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonScore, FALSE, FALSE, 0);

    pButtonRotateLeft = gtk_button_new();
    gtk_widget_set_has_tooltip(pButtonRotateLeft, TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonRotateLeft), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonRotateLeft, FALSE, FALSE, 0);

    pButtonRotateRight = gtk_button_new();
    gtk_widget_set_has_tooltip(pButtonRotateRight, TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonRotateRight), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonRotateRight, FALSE, FALSE, 0);

    pButtonQuit = gtk_button_new();
    gtk_widget_set_has_tooltip(pButtonQuit, TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(pButtonQuit), "image-button");
    gtk_box_pack_start(GTK_BOX(pHbox), pButtonQuit, FALSE, FALSE, 0);

    // attachement de la boite des boutons
    gtk_grid_attach_next_to(GTK_GRID(pGridMain), pHbox, pGridMatrix, GTK_POS_BOTTOM, 1, 1);

    /* frame Comments */
    pfrComments = gtk_frame_new(NULL);
    plbComments = gtk_label_new(_("Initializing ..."));
    gtk_frame_set_shadow_type(GTK_FRAME(pfrComments), GTK_SHADOW_NONE);
    gtk_container_add(GTK_CONTAINER(pfrComments), plbComments);
    gtk_style_context_add_class(gtk_widget_get_style_context(pfrComments), "matrix-name-label");
    /* attachement du frame comments */
    gtk_grid_attach_next_to(GTK_GRID(pGridMain), pfrComments, pHbox, GTK_POS_BOTTOM, 1, 2);

    /* frame Values */
    GtkWidget *pfrValues = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(pfrValues), GTK_SHADOW_NONE);
    gchar *plbValuesTitle[] = {LABEL_BONUS_TEXT, LABEL_PEG_TEXT, LABEL_TIME_TEXT};
    pVbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(pVbox), TRUE);
    gtk_widget_set_halign(GTK_WIDGET(pVbox), GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(pfrValues), pVbox);

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
    for (int k = 0; k < 3; k++)
    {
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[k]), "value-values-label");
    }

    gtk_grid_attach_next_to(GTK_GRID(pGridMain), pfrValues, pfrComments, GTK_POS_BOTTOM, 1, 1);

    /* _____________________________________________________ *
     *                                                       *
     *                 CSS est la vie plus belle             *
     *                 =========================             *
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
    g_signal_connect(G_OBJECT(pButtonNewGame), "clicked", G_CALLBACK(OnNewGame), NULL);
    g_signal_connect(G_OBJECT(pButtonNewGame), "query-tooltip", G_CALLBACK(OnQueryTooltip), GINT_TO_POINTER(1));

    g_signal_connect(G_OBJECT(pButtonUndo), "clicked", G_CALLBACK(OnUndo), NULL);
    g_signal_connect(G_OBJECT(pButtonUndo), "query-tooltip", G_CALLBACK(OnQueryTooltip), GINT_TO_POINTER(2));

    g_signal_connect(G_OBJECT(pButtonScore), "clicked", G_CALLBACK(OnDisplayScore), NULL);
    g_signal_connect(G_OBJECT(pButtonScore), "query-tooltip", G_CALLBACK(OnQueryTooltip), GINT_TO_POINTER(3));

    g_signal_connect(G_OBJECT(pButtonRotateLeft), "clicked", G_CALLBACK(OnRotate), GINT_TO_POINTER(THETA_COUNTER_CLOCKWISE));
    g_signal_connect(G_OBJECT(pButtonRotateLeft), "query-tooltip", G_CALLBACK(OnQueryTooltip), GINT_TO_POINTER(4));

    g_signal_connect(G_OBJECT(pButtonRotateRight), "clicked", G_CALLBACK(OnRotate), GINT_TO_POINTER(THETA_CLOCKWISE));
    g_signal_connect(G_OBJECT(pButtonRotateRight), "query-tooltip", G_CALLBACK(OnQueryTooltip), GINT_TO_POINTER(5));

    g_signal_connect(G_OBJECT(pButtonQuit), "clicked", G_CALLBACK(OnDestroy), pWindowMain);
    g_signal_connect(G_OBJECT(pButtonQuit), "query-tooltip", G_CALLBACK(OnQueryTooltip), GINT_TO_POINTER(6));

    onlyOneBoard.set = &currentMatrixOfBoard;
    scoreInit();
    gtk_widget_show_all(pWindowMain);
    _g_display_box_menu_new_game(NULL);
}

void OnQueryTooltip(GtkWidget *self,
                    gint x,
                    gint y,
                    gboolean keyboard_mode,
                    GtkTooltip *tooltip,
                    gpointer user_data)
{
    const gchar *tooltipLabel[] = {N_("New Game"), N_("Undo"), N_("Score"), N_("Rotate Left"), N_("Rotate Right"), N_("Quit")}; // "Score", "Rotate Left", "Rotate Right", "Quit"};
    gtk_widget_set_tooltip_text(self, _(tooltipLabel[GPOINTER_TO_INT(user_data) - 1]));
}

int boardInitNew()
{
    int status;
    app = gtk_application_new("lekiwidebx.dev.senku", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    setlocale(LC_ALL, "");
    textdomain(GETTEXT_DOMAIN_NAME);
    bind_textdomain_codeset(GETTEXT_DOMAIN_NAME, "UTF-8");
    bindtextdomain(GETTEXT_DOMAIN_NAME, GETTEXT_DIR_NAME);

    status = g_application_run(G_APPLICATION(app), 0, NULL);
    return status;
}

/**
 * @brief display box menu new game
 *
 * @param pData
 */
int _g_display_box_menu_new_game(gpointer pData)
{
    GtkWidget *pRadio = gtk_radio_button_new_with_label(NULL, _("English"));
    GtkWidget *pRadio_1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pRadio), _("German"));
    GtkWidget *pRadio_2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pRadio), _("Diamond"));
    GtkWidget *pDialogMenu = gtk_dialog_new_with_buttons(
        _("Senku Shapes"),
        GTK_WINDOW(pWindowMain),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        _("New Game"), GTK_RESPONSE_OK,
        _("Cancel"), GTK_RESPONSE_CANCEL,
        _("Quit"), GTK_RESPONSE_REJECT,
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

void OnDestroy(GtkWidget *pWidget, gpointer pData)
{
    pDialogBoxQuit = gtk_message_dialog_new(GTK_WINDOW(pWindowMain), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, _("Do you Really wish to Quit Senku ?"));
    gtk_window_set_title(GTK_WINDOW(pDialogBoxQuit), _("Confirm QUIT"));
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

void OnNewGame(GtkWidget *pWidget, gpointer pData)
{
    gint response = _g_display_box_menu_new_game(NULL);
    if (response != GTK_RESPONSE_CANCEL)
    {
        gtk_label_set_label(GTK_LABEL(plbComments), _(currentMatrixOfBoard.name));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbComments), "matrix-name-label");
        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", 0));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_PEG]), "value-values-label");
        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_BONUS]), g_strdup_printf("%4.0lf", 0.0));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_BONUS]), "value-values-label");
        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_TIME]), g_strdup_printf("%4d", 0));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_TIME]), "value-values-label");
        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", matrixCountRemainPeg()));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_PEG]), "value-values-label");
        scoreResetBonusTimeScore();
        scoreSetRemainingPeg(matrixCountRemainPeg());
        printf("\nDEBUG :: 1 BonusTimeScore = %f\n", scoreGetBonusTimeScore());
        // timerSetStopTimer();
        // timerStopClock();
        // timerSetStartTimer();
        // timerStartClock();
        if (timerSelection != NULL)
            timerSelection = NULL;
        printf("\nDEBUG :: 2 BonusTimeScore = %f\n", scoreGetBonusTimeScore());
        caretakerNew();
        _g_displayAllMatrix();
    }
}

/// @brief
/// @param pWidget
/// @param pData
void OnRotate(GtkWidget *pWidget, gpointer pData)
{
    matrixRotate((theta)pData); /* THETA_COUNTER_CLOCKWISE || THETA_CLOCKWISE */
    _g_displayAllMatrix();
}

void _g_displayAllMatrix()
{
    GtkWidget *imgPegStatus;

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
    GtkWidget *pArrayWidgets[] = {pButtonRotateLeft, pButtonRotateRight, pButtonUndo};
    gint sizeArrayWidgets = (gint)sizeof(pArrayWidgets) / sizeof(GtkWidget *);

    static gdouble elapsed_old = 0.0;
    gdouble elapsed = 0.0;
    gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", matrixCountRemainPeg()));
    gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_PEG]), "value-values-label");
    //    g_print( "\nDEBUG :: Coord Old X:%d Y:%d", pOld.x, pOld.y ) ;
    //    g_print( "\nDEBUG :: Coord New X:%d Y:%d", p->x, p->y ) ;

    // g_printf("\n\nDEBUG 0:: debut Onselect----------------------------------\n");
    if (!timerSelection)
    {
        timerSelection = g_timer_new();
    }
    else if (g_timer_is_active(timerSelection))
    {
        g_timer_stop(timerSelection);
        g_timer_continue(timerSelection);
        elapsed = g_timer_elapsed(timerSelection, NULL);
        if (elapsed > elapsed_old && _firstSelectPeg("get", TRUE))
        {
            g_printf("\nDEBUG :: Elapse Wait %.1fs\n", elapsed - elapsed_old);
            scoreSetCalculateBonusElapseTimer(elapsed - elapsed_old, SCORE_TIMER_WAIT_PEG);
        }
        else
        {
            g_printf("\nDEBUG :: Elapse Hold %.1fs\n", elapsed - elapsed_old);
            scoreSetCalculateBonusElapseTimer(elapsed - elapsed_old, SCORE_TIMER_HOLD_PEG);
        }
        elapsed_old = elapsed;
    }
    else
    {
        g_timer_start(timerSelection);
    }

    if (pOld.x == p->x && pOld.y == p->y) // si même coordonnees on reclique le meme peg
    {
        // g_printf("\nDEBUG 0a:: coordonnees identiques");

        pOld.x = 0;
        pOld.y = 0;
        if (!_firstSelectPeg("get", TRUE))
        {
            _firstSelectPeg("set", TRUE);
            _g_displayUpdateMatrix(ACTION_SELECT_UNSELECT_PEG, p->x, p->y);
            _g_set_state_flags_buttons(pArrayWidgets, sizeArrayWidgets, GTK_STATE_FLAG_NORMAL, TRUE);
            return;
        }
    }
    else
    {
        // g_printf("\nDEBUG 0b:: coordonnees differentes");
    }
    if (matrixCanMovePeg())
    {
        // g_printf("\nDEBUG 1:: debut des prises sont possibles");

        if (_firstSelectPeg("get", FALSE))
        { // premier clic de selection

            g_print("\nDEBUG 2:: si premiere selection clic");

            // timerStartClock();
            // g_printf("\nDEBUG :: Elapse Prise %f s\n", g_timer_elapsed(timerSelection, NULL));

            _g_set_state_flags_buttons(pArrayWidgets, sizeArrayWidgets, GTK_STATE_FLAG_INSENSITIVE, TRUE);
            if (matrixSelectPeg(p->x, p->y))
            {
                // g_print("\nDEBUG 3:: si premiere selection clic est selectionnable pour prise");

                _firstSelectPeg("set", FALSE);

                _g_displayUpdateMatrix(ACTION_SELECT_PEG, p->x, p->y);
                if ((pOld.x || pOld.y) && (pMatrixLoad[pOld.x][pOld.y] == 1) && (pOld.x != p->x || pOld.y != p->y))
                { /* unselect si l'ancien si existe */

                    // g_print("\nDEBUG 4:: deselecte l'ancienne selection  si un l'ancienne existe");

                    _g_displayUpdateMatrix(ACTION_SELECT_UNSELECT_PEG, pOld.x, pOld.y);
                }
                pOld.x = p->x;
                pOld.y = p->y;
            }
        }
        else
        { // seconde selection cliquée *******************************************************

            // g_print("\nDEBUG 5:: c'est la seconde selection clic (pour terminer une prise) ");

            // scoreSetCalculateBonusElapseTimer(timerGetElapseClock() * 1000);

            if (matrixSelectPeg(pOld.x, pOld.y))
            { // si prise possible

                // g_print("\nDEBUG 6:: à la seconde selection avec la premiere selection qui est une prise possible");

                int deltaX = 0, deltaY = 0, sumDelta = 0;
                deltaX = pOld.x - p->x;
                deltaY = pOld.y - p->y;
                sumDelta = deltaX + deltaY;
                _firstSelectPeg("set", TRUE);
                _g_set_state_flags_buttons(pArrayWidgets, sizeArrayWidgets, GTK_STATE_FLAG_NORMAL, TRUE);
                //                g_print( "\nDEBUG :: deltaX: %d deltaY: %d sumDelta: %d", deltaX, deltaY, sumDelta ) ;
                //                g_print( "\nDEBUG :: pOldX: %d pOldY: %d px: %d py: %d", pOld.x, pOld.y, p->x, p->y ) ;
                if (deltaX != deltaY && (sumDelta == 2 || sumDelta == -2))
                {
                    // g_print("\nDEBUG 7:: on determine le sens de la prise Nord Sud Est Ouest");

                    if (deltaConstantXY == abs(deltaX) && (deltaY == 0))
                        action = (deltaX > 0) ? ACTION_SELECT_TAKE_NORTH : ACTION_SELECT_TAKE_SOUTH;
                    else if ((deltaConstantXY == abs(deltaY)) && (deltaX == 0)) // bug deltaX !=0
                        action = (deltaY > 0) ? ACTION_SELECT_TAKE_WEST : ACTION_SELECT_TAKE_EAST;
                    if (matrixUpdate(action))
                    {
                        g_print("\nDEBUG 8:: on modifie la matrice sur une action Nord Sud Est Ouest");

                        _g_displayUpdateMatrix(action, p->x, p->y);
                        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", matrixCountRemainPeg()));
                        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_PEG]), "value-values-label");
                        pOld.x = p->x;
                        pOld.y = p->y;
                        if (matrixSelectPeg(pOld.x, pOld.y))
                        {
                            // g_print("\nDEBUG 9:: on trouve que la nouvelle pos peut faire une prise successive");

                            _firstSelectPeg("set", FALSE);
                            _g_displayUpdateMatrix(ACTION_SELECT_PEG, pOld.x, pOld.y);
                            _g_set_state_flags_buttons(pArrayWidgets, sizeArrayWidgets, GTK_STATE_FLAG_INSENSITIVE, TRUE);
                        }
                        // g_timer_stop(timerSelection);
                        // g_printf("\nDEBUG :: Elapse Apres prise %f s\n", g_timer_elapsed(timerSelection, NULL));
                        // timerStopClock();
                        // g_printf("\nDEBUG :: Elapse after Stop %f s", timerGetElapseClock());
                        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_BONUS]), g_strdup_printf("%4.0lf", scoreGetBonusTimeScore()));
                        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_BONUS]), "value-values-label");
                    }
                    else if (matrixSelectPeg(p->x, p->y))
                    { // clique changement d'avis avec prise

                        /* g_print("\nDEBUG 10: clique mais pas prise avec depart prec \
                                  \n -bon ecart(3 pegs succ) jeu changement d'avis mais avec prise \
                                  \n -nouvelle pos action NSEO"); */

                        _firstSelectPeg("set", FALSE);
                        _g_displayUpdateMatrix(ACTION_SELECT_UNSELECT_PEG, pOld.x, pOld.y);
                        _g_displayUpdateMatrix(ACTION_SELECT_PEG, p->x, p->y);
                        pOld.x = p->x;
                        pOld.y = p->y;
                        _g_set_state_flags_buttons(pArrayWidgets, sizeArrayWidgets, GTK_STATE_FLAG_INSENSITIVE, TRUE);
                    }
                    else
                    { // changement d'avis sans prise (ie: erreur de second clique)

                        // g_print("\nDEBUG 11: changement d'avis sans prise possible\n-(second clique bon ecart (3 pegs succ) mais prise impossible)");

                        _g_displayUpdateMatrix(ACTION_SELECT_UNSELECT_PEG, pOld.x, pOld.y);
                        if (!_firstSelectPeg("get", TRUE))
                        {                                  // DEBUG CRITIQUE
                            _firstSelectPeg("set", FALSE); // DEBUG CRITIQUE
                            pOld.x = p->x;                 // DEBUG CRITIQUE
                            pOld.y = p->y;                 // DEBUG CRITIQUE
                        } // DEBUG CRITIQUE
                        _g_set_state_flags_buttons(pArrayWidgets, sizeArrayWidgets, GTK_STATE_FLAG_INSENSITIVE, TRUE);
                    }
                    if (!matrixCanMovePeg()) // si le jeu est termine
                    {

                        // g_print("\nDEBUG 12: plus rien ne prends, EOG end of game");

                        remainingPeg = matrixCountRemainPeg();
                        scoreSetRemainingPeg(remainingPeg);
                        gtk_style_context_add_class(gtk_widget_get_style_context(plbComments), "matrix-name-label");
                        gtk_widget_set_state_flags(pButtonUndo, GTK_STATE_FLAG_INSENSITIVE, TRUE);
                        g_timer_destroy(timerSelection);
                        if (handleTimeout)
                        {
                            g_source_remove(handleTimeout);
                            g_timeout_add(TIMER_DELAY, _g_display_time, GINT_TO_POINTER(FALSE));
                        }
                        if ((rank = scoreNew()))
                            _g_display_get_name(rank);
                        return;
                    }
                }
                else if (sumDelta == 0 && (deltaX != -deltaY)) // pas de prise possible ??? prise diagonale??? test impossible ??
                {
                    // g_print("\nDEBUG 13: ***recov sans prise possible (second clique test du delta)"); // on reclic sur le meme que le premier

                    if (matrixSelectPeg(p->x, p->y))
                    {
                        // g_print("\nDEBUG 14: ***recov mais le peg est selectionnable");

                        // en excluant la cdtions particuliere sumdelta==0
                        _g_displayUpdateMatrix(ACTION_SELECT_PEG, p->x, p->y); // pour une autre raison (pions coins opposes d'un carre)
                    }
                }
                else
                { // ni prise ni meme peg de depart
                    // g_print("\nDEBUG :: change selection de depart si prise possible\n");

                    // g_print("\nDEBUG 15: sans prise effective ni meme peg du depart (second clique test du delta)");

                    _firstSelectPeg("set", FALSE);
                    _g_set_state_flags_buttons(pArrayWidgets, sizeArrayWidgets, GTK_STATE_FLAG_INSENSITIVE, TRUE);
                    if (matrixSelectPeg(p->x, p->y))
                    { // si une prise possible

                        // g_print("\nDEBUG 16: change selection du depart si une prise est possible ");

                        _g_displayUpdateMatrix(ACTION_SELECT_UNSELECT_PEG, pOld.x, pOld.y);
                        _g_displayUpdateMatrix(ACTION_SELECT_PEG, p->x, p->y);
                        pOld.x = p->x;
                        pOld.y = p->y;
                    }
                    else
                    {
                        // g_print("\nDEBUG 17: la selection ne fait pas de prise possible");
                        _firstSelectPeg("set", TRUE); // DEBUG CRITIQUE
                        _g_displayUpdateMatrix(ACTION_SELECT_UNSELECT_PEG, pOld.x, pOld.y);
                        _g_set_state_flags_buttons(pArrayWidgets, sizeArrayWidgets, GTK_STATE_FLAG_NORMAL, TRUE);
                    }
                }
            }
            else
            {
                // g_print("\nDEBUG 18:: ***recov? seconde selection TEST :/ impossible! Où suis-je?");
            }
        }
        if (_firstSelectPeg("get", TRUE) && !matrixSelectPeg(p->x, p->y))
        {
            // g_print("\n\nDEBUG 19a:: premier pas de selection possible");
            _g_set_state_flags_buttons(pArrayWidgets, sizeArrayWidgets, GTK_STATE_FLAG_NORMAL, TRUE);
        }

        // g_print("\n\nDEBUG 19:: fin du Onselect --------------------------------");

        gtk_widget_show_all(GTK_WIDGET(pGridMain));
    }
}

void _g_set_state_flags_buttons(GtkWidget *pArrayWidgets[], const int size, const int state, const gboolean sensitive)
{
    for (int i = 0; i < size; i++)
    {
        gtk_widget_set_state_flags(pArrayWidgets[i], state, sensitive);
    }
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
        // timerStartClock();

        gtk_label_set_label(GTK_LABEL(plbValuesValue[LABEL_PEG]), g_strdup_printf("%3d", matrixCountRemainPeg()));
        gtk_style_context_add_class(gtk_widget_get_style_context(plbValuesValue[LABEL_PEG]), "value-values-label");

        gtk_label_set_label(GTK_LABEL(plbComments), _(currentMatrixOfBoard.name));
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
    GtkWidget *pEntryName;
    GtkWidget *pLabelName;
    GtkWidget *pLabelMessage;
    GtkWidget *pButtonOk;
    GtkWidget *pGridGetName;
    const char *labelNom = "";
    char *labelMessage = _("\nCongratulations, ");
    const char *labelInsideEntry = _("Username");
    score *findRecord = (score *)scoreGetSortScore(rank);
    const char *title = _("Saving score");
    const char *msgAdd;
    if (findRecord->remainingPeg == 1)
        msgAdd = _("\n\t\tYeaah!  You're a real SENKU");
    else if (findRecord->remainingPeg == 2)
        msgAdd = _("\n\t\tThe victory is imminent!");
    else
        msgAdd = _("\n\t\tNobody is perfect :)");
    const char *topmost = g_strdup_printf(_("you're number %d in topmost!\n%s"), rank, msgAdd);
    pLabelName = gtk_label_new(labelNom);
    labelMessage = g_strconcat(labelMessage, topmost, NULL);
    pLabelMessage = gtk_label_new(labelMessage);
    pWindowGetName = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(pWindowGetName, "windowGetName");
    gtk_window_set_resizable(GTK_WINDOW(pWindowGetName), FALSE);
    gtk_window_set_deletable(GTK_WINDOW(pWindowGetName), FALSE);
    gtk_window_set_title(GTK_WINDOW(pWindowGetName), title);
    pGridGetName = gtk_grid_new();
    pButtonOk = gtk_button_new_with_label(_("Register"));
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
    scoreSetNameMatrix(g_strdup_printf("%10s", _(currentMatrixOfBoard.name)), rank);
    // pScore resultScore = (score *)malloc(SCORE_BEST_OF * sizeof(score));
    pScore resultScore = g_malloc(SCORE_BEST_OF * sizeof(score));
    if (resultScore)
    {
        resultScore = (pScore)scoreGetSortScore(0); /*arg: (int) NULL */
        _g_display_box_score(resultScore, rank);
        // g_free(resultScore);
    }
    else
    {
        // g_free(resultScore);
    }
    gtk_widget_destroy(pWindowGetName);
}

void OnDisplayScore(GtkWidget *pWidget, dataName *pData)
{
    // pScore resultScore = (score *)malloc(SCORE_BEST_OF * sizeof(score));
    pScore resultScore = g_malloc(SCORE_BEST_OF * sizeof(score));
    if (resultScore)
    {
        resultScore = (pScore)scoreGetSortScore(0); /* arg:(int) NULL */
        _g_display_box_score(resultScore, 0);
        // g_free(resultScore);
    }
    else
    {
        // g_free(resultScore);}
    }
}

    void OnDestroyGetName(GtkWidget * pWidget, gpointer pData)
    {
    }

    void _g_display_box_score(pScore ps, const int rank)
    {
        int i, k;
        char *scoreTitle[] = {N_("RANK"), N_("PLAYER"), N_("PEG"), N_("SCORE"), N_("SHAPE")};

        GtkWidget *pButtonOk = NULL;
        GtkWidget *plbScoreOrder = NULL;
        GtkWidget *plbScorePlayer = NULL;
        GtkWidget *plbScorePeg = NULL;
        GtkWidget *plbScoreScore = NULL;
        GtkWidget *plbScoreRank = NULL;
        GtkWidget *lbScore[] = {plbScoreOrder, plbScorePlayer, plbScorePeg, plbScoreScore, plbScoreRank};
        gint sizeArray = (int)(sizeof(lbScore) / sizeof(GtkWidget *));
        pBoxScore = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_widget_set_name(GTK_WIDGET(pBoxScore), "boxScore");
        gtk_window_set_title(GTK_WINDOW(pBoxScore), BOX_SCORE_TITLE);
        gtk_window_set_modal(GTK_WINDOW(pBoxScore), TRUE);
        gtk_window_set_position(GTK_WINDOW(pBoxScore), GTK_WIN_POS_CENTER_ALWAYS);
        gtk_window_set_deletable(GTK_WINDOW(pBoxScore), FALSE);
        gtk_window_set_transient_for(GTK_WINDOW(pBoxScore), GTK_WINDOW(pWindowMain));
        gtk_window_set_resizable(GTK_WINDOW(pBoxScore), FALSE);
        /*  Grille du Score  */
        pGridScore = gtk_grid_new();
        gtk_container_add(GTK_CONTAINER(pBoxScore), pGridScore);

        for (k = 0; k < sizeArray; k++)
        {
            lbScore[k] = gtk_label_new(_(scoreTitle[k]));
        }
        gtk_grid_attach(GTK_GRID(pGridScore), lbScore[0], 0, 0, 1, 1);
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
            lbScore[4] = gtk_label_new(_(ps->nameMatrix));
            gtk_grid_attach(GTK_GRID(pGridScore), lbScore[0], 0, i, 1, 1);
            for (k = 0; k < sizeArray - 1; k++)
                gtk_grid_attach_next_to(GTK_GRID(pGridScore), lbScore[k + 1], lbScore[k], GTK_POS_RIGHT, 1, 1);
            ps++;
        }
        pButtonOk = gtk_button_new_with_label(_("Close"));
        gtk_grid_attach_next_to(GTK_GRID(pGridScore), pButtonOk, lbScore[sizeArray / 2 - 1], GTK_POS_BOTTOM, 3, 1);
        g_signal_connect(G_OBJECT(pButtonOk), "clicked", G_CALLBACK(OnCloseBoxScore), NULL);
        gtk_widget_show_all(pBoxScore);
    }

    void OnCloseBoxScore(GtkWidget * pWidget, gpointer pData)
    {
        gtk_widget_destroy(pBoxScore);
    }
