#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#define DEJAVU 1
#define PASVU 0
#define VRAI 1
#define FAUX 0
#include <conio.h>
#include <windows.h>

// c'est un tableau de couleur pour le design
int colors_tab[] = {FOREGROUND_BLUE|FOREGROUND_INTENSITY,
                    FOREGROUND_GREEN|FOREGROUND_INTENSITY,
                    FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
                    FOREGROUND_RED|FOREGROUND_INTENSITY,
                    FOREGROUND_BLUE|FOREGROUND_GREEN,
                    FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY,
                    FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY,
                    FOREGROUND_RED,
                    FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY};

struct parcelle{
    int mine;        //1 s'il ya une mine, 0 sinon
    int status;      //DEJAVU si deja vu, PASVU sinon
};
typedef struct parcelle parcelle;


typedef struct str_champs{
    int nb_erreurs_restants;
    int nb_lignes;              //nombre de lignes
    int nb_colonnes;            //nombre de colonnes
    int nb_mines;               //nombre de mines
    parcelle *tab;      // pointeur vers un tableau de nb_lignes*nb_colonnes parcelles
}str_champs;
typedef str_champs *p_champs;



//fonction qui permet de placer les mines aléatoirement
void mine_aleatoire(p_champs champs){
    int i, nb_mines=champs->nb_mines, taille=champs->nb_colonnes*champs->nb_lignes;
    srand((unsigned int) time(NULL)); //initialisation de rand()
    while(nb_mines>0){
        i=rand()%(taille);
        if(champs->tab[i].mine==0){
            champs->tab[i].mine=1;
            nb_mines--;
        }
    }

}
// (1) la fonction init qui construit et retourne un champs de taille nbLignes sur nbColonnes contenant nb_mines
p_champs init(int nb_lignes, int nb_colonnes, int nb_mines){

    int i,taille=nb_colonnes*nb_lignes;
    p_champs champs=(p_champs)malloc(sizeof(str_champs));

    champs->nb_colonnes=nb_colonnes;
    champs->nb_lignes=nb_lignes;
    champs->nb_mines=nb_mines;
    champs->tab= malloc(taille*sizeof(parcelle));

    for (i = 0; i < taille; i++) {
        champs->tab[i].status=PASVU;
        champs->tab[i].mine=0;
    }

   mine_aleatoire(champs)

    return champs;
}

// (4) la fonction qui retourne l'indice du tableau pour une parcelle dont les coordonnées sont (i,j)


int indice(p_champs C, int i, int j) {
    if (i == 0)
        return j;
    else
        return i * C->nb_colonnes + j;
}



// les fonctions qui retournent les numéros de ligne et de colonnes correspondant à l'element d'indice k
int getX(p_champs C, int k){
     return (k % C->nb_colonnes);
}

int getY(p_champs C, int k){
     return (int) (k / C->nb_colonnes);
}

// (2) la fonction qui retourne le nombre de mines contenues dans les parcelles voisines de la parcelle(i,j).
int nb_mines_voisins(p_champs C, int i, int j){
     int a,b,nb_mines = 0;

    for (a= i - 1; a <= i+ 1; a++)
        for (b = j - 1; b<= j+ 1; b++) {
            if (a>= 0 && b>= 0 && a < C->nb_colonnes && b< C->nb_lignes)
                if ((indice(C, b, a)!= indice(C, j, i)) && (C->tab[indice(C, b,a )].mine == 1))
                    nb_mines++;

        }
    return nb_mines;
}

// la fonction qui retourne le nombre de parcelles non minées
int parNonMineRes(p_champs C) {
    int i,inc = 0, taille = C->nb_colonnes * C->nb_lignes;
    for (i = 0; i < taille; i++) {
        if ((C->tab[i].status == PASVU) && (C->tab[i].mine == 0))
            inc++;
    }
    return inc;
}


void color(int flags) {
   HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
   SetConsoleTextAttribute(H, (WORD)flags);
}

void reset_color(void) { color(FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED); }

// (3) la fonction affichage
void affichage(p_champs C){

    int i, j, z;
    int ind;
    printf("\n   ");
    for(i = 0; i < C->nb_colonnes; ++i)
        printf(" %2d ", i);

    printf("\n   +");

    for(i = 0; i < C->nb_colonnes; ++i)
        printf("---+");
    putchar('\n');


    for(i = 0; i < C->nb_lignes; ++i) {
        printf("%2d ", i);
        for(j=0;j<C->nb_colonnes;++j){
              ind =indice(C,i,j);
            if(C->tab[ind].status==0){
                color(FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE);
                printf("| X ");
                reset_color();
            }else if(C->tab[ind].mine==1){
                printf("| M ");

            }else{
                color(colors_tab[C->tab[ind].mine+2]);
                printf("| %d ",nb_mines_voisins(C,j,i));
            }

        }
        putchar('\n');
        printf("   +");

    for(z = 0; z < C->nb_colonnes; ++z)

        printf("---+");
        putchar('\n');
    }

    printf("\n\n");
    printf("Il reste %d parcelle(s) non minee(s) a trouver\n",parNonMineRes(C));
    printf("Tu as encore droit a %d erreur(s)\n\n\n",C->nb_erreurs_restants);
    printf("choisissez ce que vous voulez faire\n\n"
		 		"Q ou q pour quitter\n"
		 		"S ou s pour sauvegarder\n"
		 		"C ou c coordonnees du point:\n");
}



// (5) la fonction qui permet de sauvegarder l'état du jeu dans un fichier
int sauvegarder(char* fichier , p_champs C){
	/*  ************************************************************************************************************************************************
	 * FORMAT DE NOTRE FICHIER
	 * nb_erreurs_restants
	 * nb_lignes
	 * nb_colonnes
	 * tab[i] sous la forme <mine status> avec mine qui indique si la parcelle est une mine et status qui indique si elle a deja ete vue
	 * 
	 ************************************************************************************************************************************************   */
 	int eval = FAUX ;
	FILE* file = fopen(fichier ,"w+") ;
	if (file != NULL){
		eval = VRAI ;
		fprintf(file,"%d ",C->nb_erreurs_restants) ;
		fprintf(file,"%d ",C->nb_lignes) ;
		fprintf(file,"%d ",C->nb_colonnes) ;
		fprintf(file,"%d\n",C->nb_mines) ;
		int i ;
		for (i=0 ; i < C->nb_lignes*C->nb_colonnes ; i++){
			fprintf(file,"%d %d\n",C->tab[i].mine,C->tab[i].status) ;
		}
		fclose(file) ;
	}
	return eval ;

}

// (6) la fonction qui lit l'état du demineur à partir d'un fichier
p_champs lire(char* fichier){

  p_champs C =(p_champs)malloc(sizeof ( str_champs));
  FILE* file = fopen(fichier ,"r") ;
	if (file != NULL){
		fscanf(file,"%d ",&C->nb_erreurs_restants) ;
		fscanf(file,"%d ",&C->nb_lignes) ;
		fscanf(file,"%d ",&C->nb_colonnes) ;
		fscanf(file,"%d\n",&C->nb_mines) ;
		int i,taille=C->nb_lignes*C->nb_colonnes;
        C->tab=(parcelle*) malloc(taille*sizeof(parcelle));
		for (i=0 ; i < C->nb_lignes*C->nb_colonnes ; i++){
			fscanf(file,"%d %d\n",&C->tab[i].mine,&C->tab[i].status) ;
		}
		}else{
		    printf("\n\n erreur de lecture\n");
		}
		fclose(file) ;

	return C ;


}



//(7) la fonction qui permet au joueur de jouer un coup sur la parcelle de position i,j
int joueur(p_champs C, int i, int j){
     int ind = indice(C, j, i) , res ;
    if (!(i >= 0 && j>= 0 && i< C->nb_colonnes && j < C->nb_lignes))
        res = -1;
    if (C->tab[ind].status == PASVU) {
        C->tab[ind].status = DEJAVU;
        if (C->tab[ind].mine == 0) {
            res = 0;
        } else  res = 1;
    } else  res = 2;
    return res ;
}



// procédure pour quitter du jeu
void abandonner(void) {
    printf("\n\n");
    printf("merci d'avoir jouer.\n\n");
    printf("taper exit pour quitter definitivement\n\n");
    getchar();
    exit(EXIT_SUCCESS);
}


//procedure pour quitter apres la sauvegarde
void quitter(p_champs C){
	int ok=0;
	char*res=malloc(1)
	char* fichier = malloc(50);
		printf("Nom du fichier de sauvegarde : ") ;
		scanf("%s",fichier) ;
		 if (fopen(fichier, "r") == NULL) {
                    sauvegarder(fichier, C);
                    ok = 1;

                } else {
                	printf("Ce fichier existe!!!! voulez-vous l'ecraser? O/N \n");
                	scanf("%s",res);
                	if(res[0]=='O'||res[0]=='o'){
                		sauvegarder(fichier,C);
                        ok=1;

                	}else{
                	    exit(EXIT_SUCCESS);
                	}

	}
	if(ok)
	printf("sauvegarde reussie\n");
	abandonner();

}

//la fonction qui permet de choisir les coordonnées pour jouer
void coord(p_champs C){
    int x,y,z;

        printf("donner x: \n");
        scanf("%d",&x);
        printf("donner y: \n");
        scanf("%d",&y);
    z  =joueur(C,x,y);
    if(z==1) C->nb_erreurs_restants--;
}

//  la fonction qui determine la fin du jeu
int fin(p_champs C) {
    int i;
    for (i = 0; i < C->nb_lignes * C->nb_colonnes; i++) {
        if (C->tab[i].mine == 0 && C->tab[i].status == PASVU) {
            return 0;
        }
    }
    return 1;
}


// (3) la fonction affichage des mines cachés
void affichage_cache(p_champs C){

    int i, j, z;
    int ind;
    printf("\n   ");
    for(i = 0; i < C->nb_colonnes; ++i)
        printf(" %2d ", i);

    printf("\n   +");

    for(i = 0; i < C->nb_colonnes; ++i)
        printf("---+");
    putchar('\n');


    for(i = 0; i < C->nb_lignes; ++i) {
        printf("%2d ", i);
        for(j=0;j<C->nb_colonnes;++j){
              ind =indice(C,i,j);
            if(C->tab[ind].mine==1){
                color(FOREGROUND_INTENSITY|FOREGROUND_RED);
                 printf("| M ");
                reset_color();
            }else {
                color(FOREGROUND_INTENSITY|BACKGROUND_BLUE);
                printf("| X ");
                reset_color();
            }
        }
        putchar('\n');
        printf("   +");

    for(z = 0; z < C->nb_colonnes; ++z)

        printf("---+");
        putchar('\n');
    }

}


//pocedure du jeu
void menu(p_champs C){
 char *tmp=malloc(1);
 while (C->nb_erreurs_restants >= 0 && !fin(C)) {
        int x, y;
        system("cls");
            affichage(C);
            scanf("%s",tmp);
            if(tmp[0]=='Q' || tmp[0]=='q')
                abandonner();
             else if(tmp[0]=='S'||tmp[0]=='s')
                quitter(C);
            else if(tmp[0]=='C'||tmp[0]=='c')
                coord(C);
            else if(tmp[0]=='a'||tmp[0]=='A')
                affichage_cache(C);
                delay(3000);
  }
}

//la fonction qui détermine si le joueur a perdu
void perdu(p_champs C){
    if(C->nb_erreurs_restants<0)
    printf("Desole!!! vous avez perdu\n\n");

}


//la fonction qui détermine si le joueur a gagné{}
void gagne(p_champs C){
    if(C->nb_erreurs_restants>=0 && parNonMineRes==0)
    printf("Bravo!!! vous avez gagne\n\n");
}


// fonction qui verifie si le nombre de mines est inferieur a la taille des cases
int verifmine(int a, int b, int c){
    if(c>a*b) return 0;
    else return 1;

}
//
int main(int argc, char *argv[]){
//system ("color F0");
    char tmp;
    int l, c, m, e, ok=0;
     p_champs C= (p_champs) malloc(l*c*sizeof(str_champs));
    srand((unsigned int) time(NULL));
    if (argc==3){
        if (strcmp(argv[1],"-f")==0){
             C = lire(argv[2]);
        }
        else {
                printf("erreur de saisie!!! Veuillez la corriger svp!");
                getch();
                exit(1);
            }
    }else if (argc==4){
        l=atoi(argv[1]);
        c=atoi(argv[2]);
        m=atoi(argv[3]);
        if(verifmine(l,c,m)==1) {
            C=init(l,c,m);
            C->nb_erreurs_restants=0;
        }else{
            printf("entrer un nombre de mines valide svp!!!\n");
        }

    }else if (argc==5){
        l=atoi(argv[1]);
        c=atoi(argv[2]);
        m=atoi(argv[3]);
        e=atoi(argv[4]);
        if(verifmine(l,c,m)==1) {
            C=init(l,c,m);
            C->nb_erreurs_restants=e;
        }else{
            printf("entrer un nombre de mines valide svp!!!\n");
        }
    }
  while (C->nb_erreurs_restants >= 0 && !fin(C)) {
         menu(C);
        perdu(C);
        gagne(C);
    return (EXIT_SUCCESS);
 }

}

