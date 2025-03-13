/******************************************************************************/
/* MAIN.c																	  */
/******************************************************************************/
#include "./main.h"
#include "../lib/libgraphique.h"
#define resolution 24

/******************************************************************************/
/* MAIN																		  */
/******************************************************************************/
int main(int argc, char **argv)
/* argc indique le nombre d'arguments,
   argv est un tableau de mots contenant ces arguments. */
	{
	Partie  p;	  // la structure contenant les données d'une partie 

	/* Chargement du plan à partir du fichier fourni en paramètre				 */
	printf("Chargement du plan...\n");
	p = charge_plan("data/niveau_3.txt");
	
	/* Si problème lors du chargement du plan...								  */
	if(p.plateau == NULL)
		return 1;
		
#if DEBUG==1 //Ceci est un drapeau à activer/désactiver dans main.h
	/* Affichage du plan lu													   */
	int debug_i,debug_j;
	printf("Affichage du plan...\n");
	for(debug_i=0;debug_i!=p.L;debug_i++)
		{
		for(debug_j=0;debug_j!=p.C;debug_j++)
			printf("%c",p.plateau[debug_i][debug_j]);
		printf("\n");
		}
#endif

/******************************************************************************/
/* A PARTIR D'ICI...C'EST A VOUS DE JOUER!									  */
/******************************************************************************/

	ouvrir_fenetre(p.C*resolution,(p.L+2)*resolution);
	p= debut_partie(p); 
	actualiser();
	
	/* BOUCLE DU JEU*/
	while (p.niveau!=0){
		p=deplacer_pacman(p); 
		for(int i=0;i<NBFANTOMES;i++){
			p=deplacer_fantome(p,i);
		}
		p = test_fin_partie(p);
		if(p.mode_pacgum != 0)
			p.mode_pacgum --;
		actualiser();
		attente(200); //on attend pour etre sure d'enregistrer le nombre d'input voulu par l'utilisateur
	}
	/******************************************************************************/
	/* FIN DE LA BOUCLE DU JEU                 									  */
	/******************************************************************************/
	actualiser();
	attendre_clic();
	fermer_fenetre();
	return 0;
}

/*******************************************************************************************************************/
/*                                     FONCTIONS QUI MODIFIE PARTIE p                                              */
/*******************************************************************************************************************/

/*Fonction qui initialise les variables necessaire au début de la partie*/
Partie debut_partie (Partie p){
	int i;
	for(i=0;i<NBFANTOMES;i++){
		p.fantome_initial[i].l = p.fantomes[i].l;
		p.fantome_initial[i].c = p.fantomes[i].c;
		p.plateau[p.fantomes[i].l][p.fantomes[i].c] = 'Q'; //on peut choisir si la position de départ des fantomes devient un point a manger ou non
	}

	lancement_partie(); //affichage du menu
	p.viepacman = 3;
	Point p_clic;
	int lancer_partie= 1;
	do{ // attend que le joueur clic sur un des 3 boutons 
		p_clic = attendre_clic();
		if (p_clic.x >16*resolution && p_clic.x <20*resolution && p_clic.y >26*resolution && p_clic.y< 27*resolution)
			p = choix_vie_pacman(p);
		else if(p_clic.x >1*resolution && p_clic.x <8.5*resolution && p_clic.y >26*resolution && p_clic.y< 27*resolution)
			afficher_resultat();
		else if (p_clic.x >6*resolution && p_clic.x <15*resolution && p_clic.y >12*resolution && p_clic.y< 14*resolution)
			lancer_partie= 0;
	}while (lancer_partie);
	Point p_debut={0,0};
	dessiner_rectangle(p_debut, 21*resolution, 27*resolution,noir);
	p.plateau[p.pacman.l][p.pacman.c] = ' ';
	p.score = 0;
	p.niveau = 2;
	p.mode_pacgum = 0;
	p.pacman_initial.c = p.pacman.c;
	p.pacman_initial.l = p.pacman.l;
	p = compter_nb_point(p);
	dessiner_plateau(p);
	return p; 
	
}

//fonction pour choisir le nombre de vie souhaiter
Partie choix_vie_pacman( Partie p){
	affichage_choix_vie(p);
	int  clic_y, clic_x;
	do{ //boucle qui attend que le joueur choisisse le nbr de vie
		clic_y = 0;
		clic_x= 0; //permet de determiner le nbre de vie choisis
		Point p_clic;
		p_clic= attendre_clic();
		if (p_clic.y> 15*resolution  && p_clic.y < 16 *resolution){
			clic_y =1;
			if (p_clic.x > 1*resolution && p_clic.x < 2*resolution)
				clic_x = 1;
			else if (p_clic.x > 5*resolution && p_clic.x < 7*resolution)
				clic_x = 2;
			else if (p_clic.x> 10*resolution && p_clic.x < 13*resolution)
				clic_x = 3;
			else if (p_clic.x> 16*resolution && p_clic.x < 20*resolution)
				clic_x = 4;
			else
				clic_y =0;
		}
	}while( clic_y!= 1 && (clic_x >= 1 || clic_x<= 4));
	p.viepacman = clic_x; 
	lancement_partie();
	actualiser();
	return p;

}

/*Foncion qui compte le nombre de points qu il faut manger*/ 
Partie compter_nb_point(Partie p){
	p.nbpoint =0;
	for(int i=0;i!=p.L;i++)
		for(int j=0; j!=p.C ;j ++)
			if (p.plateau[i][j]== '.' || p.plateau[i][j]== 'B')
				p.nbpoint ++;
	return p; 
}

/*Fonction qui déplace pacman, à faire au début du "tour"*/
Partie deplacer_pacman(Partie p){
	int touche;
	int i = p.pacman.l;
	int j = p.pacman.c;
	int haut_bas=0, droite_gauche=0;
	char tab_direction[30];
	do{
		touche = attendre_touche();
	}while(touche != SDLK_UP && touche != SDLK_DOWN && touche != SDLK_LEFT && touche != SDLK_RIGHT); //empeche de se tromper de touche 
	/* test de la direction qu'on souhaite prendre*/
	if(touche == SDLK_UP){
		haut_bas = -1;
		sprintf(tab_direction, "data/pacman_haut.bmp") ;
	}else if(touche == SDLK_DOWN){
		haut_bas = 1;
		sprintf(tab_direction,"data/pacman_bas.bmp" );
	}else if( touche== SDLK_LEFT){
		droite_gauche = -1;
		sprintf(tab_direction,"data/pacman_gauche.bmp");
	}else if( touche== SDLK_RIGHT){
		droite_gauche = 1;
		sprintf(tab_direction,"data/pacman_droite.bmp");
	}
	/*supression ancien pacman*/
	p.plateau[i][j]=' ';
	Point p_suprimer_pacman = {p.pacman.c*resolution+4,p.pacman.l*resolution+4};
	dessiner_rectangle(p_suprimer_pacman, 20, 20,noir);
	/* teste de deplacement + score */ 
	if (p.pacman.c == 0  && droite_gauche == -1 ){
        p.pacman.c = 20; 
        if (p.plateau[i][20]=='.'){
			p.score += 10; 
			p.nbpoint-- ; 
		}
    }else if (p.pacman.c == 20  && droite_gauche == 1 ){
        p.pacman.c = 0; 
        if (p.plateau[i][0]=='.'){
			p.score += 10; 
			p.nbpoint-- ; 
		}
    }else if (p.pacman.l == 0  && haut_bas == -1 ){
        p.pacman.l = 26; 
        if (p.plateau[26][j]=='.'){
			p.score += 10; 
			p.nbpoint-- ; 
		}
    }else if (p.pacman.l == 26  && haut_bas == 1 ){
        p.pacman.l = 0; 
        if (p.plateau[0][j]=='.'){
			p.score += 10; 
			p.nbpoint-- ; 
		}
    }else if (p.plateau[i+haut_bas][j+droite_gauche]=='.'){
		p.score += 10; 
		p.nbpoint--; // retire un point a manger 
		p.pacman.l += haut_bas;
		p.pacman.c += droite_gauche;
	}else if (p.plateau[i+haut_bas][j+droite_gauche]==' ' || p.plateau[i+haut_bas][j+droite_gauche]=='F' || p.plateau[i+haut_bas][j+droite_gauche]=='Q' || p.plateau[i+haut_bas][j+droite_gauche]=='S'){
		p.pacman.l += haut_bas;
		p.pacman.c += droite_gauche;
	}else if (p.plateau[i+haut_bas][j+droite_gauche]=='B'){
		p.score += 10; 
		p.nbpoint-- ;
		p.pacman.l += haut_bas;
		p.pacman.c += droite_gauche;
		p.mode_pacgum = 10;
	}
	
	/*dessin du nouveau pacman + affichage du score */ 
	Point p_pos = {p.pacman.c*resolution+4 ,p.pacman.l*resolution+4};
	afficher_image(tab_direction, p_pos);
	afficher_score(p);
	return p;
}

/*Fonction qui déplace le fantome numero  "n_f" */
Partie deplacer_fantome(Partie p,int n_f){
	if(!(p.mode_pacgum)){
		if(p.fantomes[n_f].l == 0 && p.fantomes[n_f].c == 0){
			p.fantomes[n_f].l = p.fantome_initial[n_f].l;
			p.fantomes[n_f].c = p.fantome_initial[n_f].c;
			if(p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] == '.'){
				p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] = 'Q';
			}else if(p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] == 'B'){
				p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] = 'S';
			}else{
				p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] = 'F';
			}
		}
		
		/*Suppression de l'ancien fantome graphiquement*/
		Point p_ancien = {p.fantomes[n_f].c*resolution+4,p.fantomes[n_f].l*resolution+4};
		dessiner_rectangle(p_ancien,20,20,noir);
		
		/*Suppression de l'ancien fatome dans le tableau et dessin de ce qu'il y avait en dessous*/
		if(p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] == 'Q'){ //Q si il y a un point en dessous
			p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] = '.';
			Point p_point = {p.fantomes[n_f].c*resolution+10,p.fantomes[n_f].l*resolution+10};
			dessiner_rectangle(p_point,4,4,beige);
		}else if(p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] == 'S'){ // S si il y a un bonbon endessous
			p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] = 'B';
			Point p_pacgum = {p.fantomes[n_f].c*resolution+7,p.fantomes[n_f].l*resolution+7};
			dessiner_rectangle(p_pacgum,10,10,beige);
		}else{
			p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] = ' ';
		}

		/*On compare la position du pac-man et du fantome et on déplace le fantome*/
		if(p.pacman.l > p.fantomes[n_f].l && p.plateau[p.fantomes[n_f].l+1][p.fantomes[n_f].c] != '*' && p.plateau[p.fantomes[n_f].l+1][p.fantomes[n_f].c] != '+' && !(p.mode_pacgum) && p.plateau[p.fantomes[n_f].l+1][p.fantomes[n_f].c] != 'Q' && p.plateau[p.fantomes[n_f].l+1][p.fantomes[n_f].c] != 'F' && p.plateau[p.fantomes[n_f].l+1][p.fantomes[n_f].c] != 'S'){
			p.fantomes[n_f].l += 1;
		}else if(p.pacman.l < p.fantomes[n_f].l && p.plateau[p.fantomes[n_f].l-1][p.fantomes[n_f].c] != '*' && p.plateau[p.fantomes[n_f].l-1][p.fantomes[n_f].c] != '+' && !(p.mode_pacgum) && p.plateau[p.fantomes[n_f].l-1][p.fantomes[n_f].c] != 'Q' && p.plateau[p.fantomes[n_f].l-1][p.fantomes[n_f].c] != 'F' && p.plateau[p.fantomes[n_f].l-1][p.fantomes[n_f].c] != 'S'){
			p.fantomes[n_f].l -= 1;
		}else if(p.pacman.c > p.fantomes[n_f].c && p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c+1] != '*' && p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c+1] != '+' && !(p.mode_pacgum) && p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c+1] != 'Q' && p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c+1] != 'F' && p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c+1] != 'S'){
			p.fantomes[n_f].c += 1;
		}else if(p.pacman.c < p.fantomes[n_f].c && p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c-1] != '*' && p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c-1] != '+' && !(p.mode_pacgum) && p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c-1] != 'Q' && p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c-1] != 'F' && p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c-1] != 'S'){
			p.fantomes[n_f].c -= 1;
		}
		
		if(p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] == '.'){
			p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] = 'Q';
		}else if(p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] == 'B'){
			p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] = 'S';
		}else{
			p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] = 'F';
		}
		
		/*dessin du nouveau fantome*/
		dessiner_fantome(p, n_f);
	}else{/* si on est en mode pacgum*/
		if(p.fantomes[n_f].l == p.pacman.l && p.fantomes[n_f].c == p.pacman.c){
			p.fantomes[n_f].l = 0;
			p.fantomes[n_f].c = 0;
			p.score += 200;
			afficher_score(p);
		}
		/*permet de cacher le fantome s'il a ete magner pendant le mode pacgum*/
		dessiner_fantome(p, n_f);
		Point p_cacher_fantome = {2,2};
		dessiner_rectangle(p_cacher_fantome,resolution-2,resolution-2,noir); 
	}
	
	actualiser();
	return p;
}

/* detecte si le pacman est manger et si cela signifie la fin de la partie ou de la perte d'une vie
permet detecter la fin d'un niveau ou la fin de la partie */
Partie test_fin_partie( Partie p){
	//test si la position du pacman est egal a d'un fantome 
	for(int i=0; i<NBFANTOMES; i++){
		if( p.fantomes[i].c == p.pacman.c && p.fantomes[i].l == p.pacman.l && !(p.mode_pacgum)){
			if (p.viepacman!=0){
				p = pacman_manger(p);
			}else{//si on a plus de vie
				fin_partie();
				enregistrer_score(p);
				p.niveau= 0; 
			}
		}
	}if (p.nbpoint ==0){  //s'il y a plus de point a manger
		p.niveau --;
		
		if (p.niveau ==0){ //si tous les niveaux ont ete fait 
			affichage_victoire(p);
			enregistrer_score(p);
		}else{ 
			attendre_clic();
			p = changer_niveau(p);
			dessiner_plateau(p);
		}
	}
	return p;
}

/*renitialise le pacmanet lesfantomes lorsque le pacman est manger
se lance uniquement quand "test_fin_partie" l'appel */
Partie pacman_manger (Partie p){
	for(int j=0; j<NBFANTOMES; j++){
		/*On retire le fantome*/
		Point p_ancien = {p.fantomes[j].c*resolution+4,p.fantomes[j].l*resolution+4};
		dessiner_rectangle(p_ancien,20,20,noir);

		/*On met un point, une pacgum ou rien a l'ancienne place du fantome*/
		if(p.plateau[p.fantomes[j].l][p.fantomes[j].c] == 'Q'){
			p.plateau[p.fantomes[j].l][p.fantomes[j].c] = '.';
			Point p_point = {p.fantomes[j].c*resolution+10,p.fantomes[j].l*resolution+10};
			dessiner_rectangle(p_point,4,4,beige);
		}else if(p.plateau[p.fantomes[j].l][p.fantomes[j].c] == 'S'){
			p.plateau[p.fantomes[j].l][p.fantomes[j].c] = 'B';
			Point p_pacgum = {p.fantomes[j].c*resolution+7,p.fantomes[j].l*resolution+7};
			dessiner_rectangle(p_pacgum,10,10,beige);
		}else{
			p.plateau[p.fantomes[j].l][p.fantomes[j].c] = ' ';
		}
		
		//on modifie la position du fantome
		p.fantomes[j].l = p.fantome_initial[j].l;
		p.fantomes[j].c = p.fantome_initial[j].c;

		if(p.plateau[p.fantomes[j].l][p.fantomes[j].c] == '.'){
			p.plateau[p.fantomes[j].l][p.fantomes[j].c] = 'Q'; //Q s'ily a un point en dessous
		}else if(p.plateau[p.fantomes[j].l][p.fantomes[j].c] == 'B'){
			p.plateau[p.fantomes[j].l][p.fantomes[j].c] = 'S'; // S s'il y a un bonbon en desous
		}else{
			p.plateau[p.fantomes[j].l][p.fantomes[j].c] = 'F'; // F s'il y a rien en dessous
		}
		
		dessiner_fantome(p,j); 
	}
	// on modifie la vie du pacman
	p.viepacman --; 
	int numero_vie_pacman = p.viepacman *20;
	Point p_suprimer_vie= { 15*resolution + numero_vie_pacman, 27*resolution};
	dessiner_rectangle(p_suprimer_vie, 20, 20, noir);

	// on renitialise la position du pacman et surprime l'ancienne
	Point p_suprimer_pacman = {p.pacman.c*resolution+4,p.pacman.l*resolution+4};
	dessiner_rectangle(p_suprimer_pacman, 20, 20,noir);
	p.pacman.l = p.pacman_initial.l;
	p.pacman.c = p.pacman_initial.c;
	Point p_pos = {p.pacman.c*resolution+4 ,p.pacman.l*resolution+4};
	afficher_image("data/pacman_droite.bmp", p_pos);
	actualiser();
	attente(500);
	return p;
}

// permet de passer au niveau suivant
Partie changer_niveau(Partie p){
	// on fait une sauvegarde de certains parametre pour les remettre apres lechargement du nouveau niveau
	Point p_noir={0,0};
	dessiner_rectangle ( p_noir, 21*resolution, 27*resolution,noir);
	int vie = p.viepacman;
	int score =p.score;
	int niveau =p.niveau;
	char tab[100];
	sprintf (tab,"data/niveau_%d.txt", p.niveau);
	p= charge_plan(tab);
	p.viepacman= vie;
	p.score = score;
	p.mode_pacgum = 0;
	p.niveau = niveau;
	for( int n_f =0; n_f < NBFANTOMES; n_f ++){
		p.fantome_initial[n_f].c = p.fantomes[n_f].c;
		p.fantome_initial[n_f].l = p.fantomes[n_f].l;
		p.plateau[p.fantomes[n_f].l][p.fantomes[n_f].c] = '.';
	}
	p.pacman_initial.c = p.pacman.c ;
	p.pacman_initial.l = p.pacman.l ;
	p= compter_nb_point(p);
	return p;
}

//permet d'enregistrer le score
Partie  enregistrer_score( Partie p){
	FILE* fichier = NULL; // on initialisele pointeur en NULL pour eviter les bugs
	fichier = fopen("data/score.txt", "a"); 
	
	if (fichier != NULL){  //si le fichier existe bien 
		char tab_pseudo[10];
		scanf("%s", tab_pseudo);
		Point p_pseudo= {6 *resolution, 20*resolution};
		afficher_texte(tab_pseudo,25,p_pseudo, blanc);
        fprintf(fichier, "%d  %s \n", p.score, tab_pseudo);
    }
    fclose(fichier);
	return p;
}

/*******************************************************************************************************************/
/*                                     FONCTIONS D'AFFICHAGE                                                       */
/*******************************************************************************************************************/
// interface du menu
void lancement_partie(){
	Point p_noir={0,0};
	dessiner_rectangle ( p_noir, 21*resolution, 28*resolution,noir);
	Point p_lancer_partie ={ 6*resolution, 12*resolution};
	afficher_image ("data/lancer_partie.bmp", p_lancer_partie);
	char tab_choisir_vie[100];
	sprintf(tab_choisir_vie,"choisir sa vie");
	Point p ={ 16*resolution, 26*resolution};
	afficher_texte(tab_choisir_vie, 15, p,white);
	char tab_resultat[50];
	p.x = 1*resolution;
	sprintf(tab_resultat, "afficher tout les scores");
	afficher_texte(tab_resultat, 15, p, white);
	actualiser();
}

// affichage du menu du choix de la vie
void affichage_choix_vie(Partie p){
	Point p_debut={0,0};
	dessiner_rectangle(p_debut, 21*resolution, 27*resolution,noir);
	Point p_texte={4.5*resolution,8*resolution};
	char tab[100];
	sprintf (tab, "Choisissez le nombre de vie ");
	afficher_texte(tab,20, p_texte, blanc); 
	p_texte.y+= resolution;
	char tab2[100];
	sprintf (tab2, "que vous souhaitez avoir:");
	afficher_texte(tab2,20, p_texte, blanc); 
	Point p_vie ={1*resolution, 15*resolution};
	for(int a=1; a<=4; a++){ 
		for( int b=1; b<= a; b++){
			afficher_image("data/pacman_droite.bmp",p_vie);
			p_vie.x += resolution;
		}
	p_vie.x +=3*resolution;
	}
	actualiser();
}

/*Fonction qui dessine le plateau de jeu au départ de la partie*/
void dessiner_plateau(Partie p){
	int i,j;
	/*On regarde les cases une par une*/
	for(i=0;i!=p.L;i++){
		for(j=0;j!=p.C;j++){
			/*On dessine les murs*/
			if(p.plateau[i][j] == '*'){
				/*On redéfinie les 4 coins de la case concernée*/
				Point p_HG = {j*resolution,i*resolution};
				Point p_HD = {j*resolution+resolution,i*resolution};
				Point p_BG = {j*resolution,i*resolution+resolution};
				Point p_BD = {j*resolution+resolution,i*resolution+resolution};
				
				/*On regarde ou il n'y a pas de mur voisin et on dessine une ligne*/
				if(i<p.L-1 && p.plateau[i+1][j] != '*') dessiner_ligne(p_BG,p_BD,blue);
				
				if(i!=0 && p.plateau[i-1][j] != '*') dessiner_ligne(p_HG,p_HD,blue);
				
				if(p.plateau[i][j+1] != '*') dessiner_ligne(p_HD,p_BD,blue);
				
				if(p.plateau[i][j-1] != '*') dessiner_ligne(p_BG,p_HG,blue);
			}
			/*On dessine les points*/
			else if(p.plateau[i][j] == '.'){
				Point p_point = {j*resolution+10,i*resolution+10};
				dessiner_rectangle(p_point,4,4,beige);
			}
			/*On dessine les pacgums*/
			else if(p.plateau[i][j] == 'B'){
				Point p_pacgum = {j*resolution+7,i*resolution+7};
				dessiner_rectangle(p_pacgum,10,10,beige);
			}
			/*On dessine la barriere*/
			else if(p.plateau[i][j] == '+'){
				Point p_gauche = {j*resolution,i*resolution};
				Point p_droite = {j*resolution+resolution,i*resolution};
				dessiner_ligne(p_gauche,p_droite,beige);
			}
			/*On dessine le pacman*/
			char tab_direction[30];
			sprintf(tab_direction,"data/pacman_droite.bmp") ;
			Point p_pos = {p.pacman.c*resolution+4 ,p.pacman.l*resolution+4};
			afficher_image(tab_direction, p_pos);
		}
	}
	/*On dessine les fantomes*/
	for(i=0;i<NBFANTOMES;i++){
		dessiner_fantome(p, i);
	}
	/*on dessine les vies et le score de depart */ 
	int numero_coeur = 0;
	for(int i=0;i<p.viepacman; i++){
		Point p_viepacman = { 15*resolution + numero_coeur, 27*resolution};
		afficher_image("data/pacman_droite.bmp",p_viepacman);
		numero_coeur += 20;
	}
	afficher_score(p);
	actualiser();
}

//dessin des fantomes
void dessiner_fantome(Partie p, int n_f){
	Point p_new = {p.fantomes[n_f].c*resolution+4,p.fantomes[n_f].l*resolution+4};
	if(!(p.mode_pacgum)){ //en mode normal 
		char tab_fantome[100];
		sprintf(tab_fantome, "data/fantomes_%d.bmp", n_f);
		afficher_image(tab_fantome, p_new);
	}
	else //en mode pacgum
		afficher_image("data/fantomes_mode_pacgum.bmp", p_new);
}

//affichage du socre 
void afficher_score(Partie p ){
	Point p_score={ 5, 27 *resolution};
	dessiner_rectangle(p_score, 200, 40, noir);
	char tab_score[100];
	sprintf(tab_score, "score : %d", p.score);
	afficher_texte( tab_score, 18, p_score, jaune);
}

// affichage de la fin de la partie
void fin_partie(){
	Point p_gameover={ 4.5*resolution, 10* resolution};
	dessiner_rectangle(p_gameover,12*resolution, 7* resolution,noir);
	Point p_game={5*resolution, 10.25*resolution};
	char tab_game[10];
	sprintf(tab_game, "GAME");
	afficher_texte( tab_game, 70, p_game, blanc);
	Point p_over={8*resolution, 13.5*resolution};
	char tab_over[10];
	sprintf(tab_over, "OVER");
	afficher_texte( tab_over, 70, p_over, blanc);
	// on recupèrele pseudo du joueur dans la console 
	Point p_pseudo= { 1*resolution, 20*resolution};
	dessiner_rectangle(p_pseudo, 300, 25, noir);
	char tab_pseudo[20];
	sprintf (tab_pseudo, "pseudo :");
	afficher_texte(tab_pseudo, 25, p_pseudo, blanc);
	actualiser();
}

// affichage graphique de la victoire
void affichage_victoire(Partie p){
	Point p_youwin={ 4.5*resolution, 10* resolution};
	dessiner_rectangle(p_youwin,12*resolution, 7* resolution,noir);
	Point p_you={5*resolution, 10.25*resolution};
	char tab_you[10];
	sprintf(tab_you, "YOU");
	afficher_texte( tab_you, 70, p_you, blanc);
	Point p_win={9*resolution, 13.5*resolution};
	char tab_win[10];
	sprintf(tab_win, "WIN");
	afficher_texte( tab_win, 70, p_win, blanc);
	afficher_score(p);
	// on recupèrele pseudo du joueur dans la console 
	Point p_pseudo= { 1*resolution, 20*resolution};
	dessiner_rectangle(p_pseudo, 300, 25, noir);
	char tab_pseudo[20];
	sprintf (tab_pseudo, "pseudo :");
	afficher_texte(tab_pseudo, 25, p_pseudo, blanc);
	actualiser();
}

// afficheles different resultats des joueurs 
void afficher_resultat(){ 
	Point p_noir={0,0};
	dessiner_rectangle(p_noir, 21*resolution, 28*resolution,noir);
	char tab_nom[20];
	int score;
	Point p={1*resolution, 2*resolution};
	char tab_afficher[100];
	FILE* fichier = NULL; // le mttre a NULL permet d'eviter les bugs 
	fichier = fopen("data/score.txt", "r+");
	for(int a =0; a < 32; a++){
		fscanf(fichier,"%d %s\n ",&score, tab_nom);
		sprintf(tab_afficher, "%s: %d", tab_nom, score);
		afficher_texte(tab_afficher, 15, p, yellow);
		actualiser();
		if (p.y >= 26*resolution){
			p.x = 12*resolution;
			p.y = 2*resolution;
		}else
			p.y+= 40;
	}
	attendre_clic();
	fclose(fichier);
	lancement_partie();
	
}
