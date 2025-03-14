/******************************************************************************/
/* CHARGEMENT.h															   */
/******************************************************************************/
#ifndef CHARGEMENT_H
#define CHARGEMENT_H

#include <stdio.h>
#include <stdlib.h>

#define NBFANTOMES  4 // nombres de fantômes dans les plateaux chargés

// STRUCTURES
/* Structure Pos: permet de stocker un couple ligne/colonne				   */
typedef struct pos {
	int	 l;
	int	 c;
	} Pos;

/* Structure Partie:  permet de stocker les paramètres d'une partie		   */
typedef struct partie {
	char ** plateau; // le tableau de caractères contenant le plateau de jeu
	int	 L; // le nb de lignes du plateau de jeu
	int	 C; // le nb de colonne du plateau de jeu
	Pos	 pacman; // la position de Pacman dans le plateau
	Pos  pacman_initial; // on enregistre sa position initial 
	Pos	 fantomes[NBFANTOMES]; // les positions de chaque fantôme
	Pos  fantome_initial[NBFANTOMES]; // on enregistre leur position initial 
	int	 nbbonus; // le nombre de bonus restants à manger
	int  nbpoint; // le nombre de point qu'il reste a manger
	int  score;    // le score du joueur 
	int  viepacman; //nombre de vie de pacman
	int  niveau; // niveau jouer et permet la fin de la partie
	int  mode_pacgum; //si une pacgum à été manger récement
	} Partie;




// PROTOTYPES
/* charge_plan : lit un fichier contenant un plateau de jeu et le charge
					en mémoire, dans le champ 'plateau' d'une Partie */
Partie charge_plan(char *fichier);

#endif
