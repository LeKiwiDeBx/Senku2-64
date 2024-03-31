/*
 * senku  ### Jeu de pions solitaire ###
 * main.c
 * Le 03/09/2016
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *       __         __ ___ _      ___
 *      / /  ___   / //_(_) | /| / (_)
 *     / /__/ -_) / ,< / /| |/ |/ / /
 *    /____/\__/ /_/|_/_/ |__/|__/_/   [°} présente
 *
 *       _____            __
 *      / ___/___  ____  / /____  __
 *      \__ \/ _ \/ __ \/ //_/ / / /
 *     ___/ /  __/ / / / ,< / /_/ /
 *    /____/\___/_/ /_/_/|_|\__,_/     (c) 2016 - 2024
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "board.h"

/*
 * ==========================================================================
 * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 *                  And the Widget's land begin here... [°}
 * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 * ==========================================================================
 */
int main(int argc, char *argv[])
{
    // gtk_init(&argc, &argv);
    if (boardInitNew()) //boardInit()
        exit(EXIT_SUCCESS);
    //            g_print( "\nGood bye! Thanks for playing this game ;)\n" ) ;
    else
        exit(EXIT_FAILURE); // « Something is rotten in the state of Denmark »
    //            g_print( "\nSorry but Init game Senku FAILURE :( \n" ) ;
    return 0;
}
