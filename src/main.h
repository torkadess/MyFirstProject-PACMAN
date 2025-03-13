/******************************************************************************/
/* MAIN.h																	 */
/******************************************************************************/
#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include "./partie.h" // on a besoin de connaître la structure Partie

#define DEBUG 0


#endif
/******************************************************************************/
/* PROTOTYPE																  */
/******************************************************************************/

Partie debut_partie(Partie p);
Partie choix_vie_pacman( Partie p);
Partie compter_nb_point (Partie p);
Partie deplacer_pacman(Partie p);
Partie deplacer_fantome(Partie p, int n_f);
Partie test_fin_partie( Partie p);
Partie pacman_manger(Partie p);
Partie changer_niveau(Partie p);
Partie enregistrer_score(Partie p);

void lancement_partie();
void affichage_choix_vie(Partie p);
void dessiner_plateau(Partie p);
void dessiner_fantome( Partie p, int n_f);
void afficher_score(Partie p);
void fin_partie();
void affichage_victoire(Partie p);

void afficher_resultat();
