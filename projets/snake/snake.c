/* Snake - compatible C90 / ancien Dev-C++ sous Windows */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>
#include <windows.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_LEN (WIDTH * HEIGHT)
#define NB_POMMES 3
#define SAVE_FILE "snake_save.txt"
#define MANUEL 0
#define ORDI_LENT 1
#define ORDI_RAPIDE 2

static const int DX[5] = {0,-1,1,0,0};
static const int DY[5] = {0,0,0,-1,1};
int x,y, direction, taille, score, mode, fini, pommesMangees;
int sonActif = 1; /* M active/desactive les sons */
int musiqueActive = 1; /* N active/desactive la musique */
/* PlaySound charge dynamiquement : aucun -lwinmm necessaire. */
typedef BOOL (WINAPI *FonctionPlaySound)(LPCSTR, HMODULE, DWORD);
HMODULE bibliothequeSon = NULL;
FonctionPlaySound jouerSonWindows = NULL;
char cheminMusique[MAX_PATH];
#define SND_ASYNC 0x0001
#define SND_LOOP 0x0008
#define SND_FILENAME 0x00020000
#define SND_NODEFAULT 0x0002
void initialiserMusique(void) {
    char *fin;
    DWORD n;
    bibliothequeSon = LoadLibraryA("winmm.dll");
    if (!bibliothequeSon) return;
    jouerSonWindows = (FonctionPlaySound)GetProcAddress(bibliothequeSon,"PlaySoundA");
    n = GetModuleFileNameA(NULL, cheminMusique, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) { jouerSonWindows = NULL; return; }
    fin = strrchr(cheminMusique, '\\');
    if (fin) *(fin+1) = '\0';
    else cheminMusique[0] = '\0';
    if (strlen(cheminMusique) + strlen("musique_snake.wav") >= MAX_PATH) {
        jouerSonWindows = NULL; return;
    }
    strcat(cheminMusique,"musique_snake.wav");
}
void demarrerMusique(void) {
    if (jouerSonWindows && sonActif && musiqueActive)
        jouerSonWindows(cheminMusique,NULL,SND_ASYNC|SND_LOOP|SND_FILENAME|SND_NODEFAULT);
}
void arreterMusique(void) {
    if (jouerSonWindows) jouerSonWindows(NULL,NULL,0);
}

void sonPomme(void) { if(sonActif) Beep(950, 35); }
void sonGrandir(void) { if(sonActif) { Beep(1100, 45); Beep(1450, 65); } }
void sonDebut(void) { if(sonActif) { Beep(650, 60); Beep(900, 60); } }
void sonFin(void) { if(sonActif) { Beep(500, 100); Beep(330, 150); } }
int corpsX[MAX_LEN], corpsY[MAX_LEN];
int pommeX[NB_POMMES], pommeY[NB_POMMES], pommePrise[NB_POMMES];
char message[80];
int messageDuree;

void curseur(int cx,int cy) {
    COORD c;
    c.X=(SHORT)cx; c.Y=(SHORT)cy;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);
}
void cacherCurseur(void) {
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize=1; ci.bVisible=FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&ci);
}
int occupe(int px,int py,int nombrePommes) {
    int i;
    if(px==x && py==y) return 1;
    for(i=0;i<taille;i++) if(corpsX[i]==px && corpsY[i]==py) return 1;
    for(i=0;i<nombrePommes;i++) if(pommeX[i]==px && pommeY[i]==py) return 1;
    return 0;
}
int creerPommes(void) {
    int i,px,py,essais;
    if(WIDTH*HEIGHT-taille-1<NB_POMMES) return 0;
    for(i=0;i<NB_POMMES;i++) {
        essais=0;
        do {
            px=rand()%WIDTH; py=rand()%HEIGHT;
            essais++;
            if(essais>10000) return 0;
        } while(occupe(px,py,i));
        pommeX[i]=px; pommeY[i]=py; pommePrise[i]=0;
    }
    pommesMangees=0;
    return 1;
}
void nouveauJeu(void) {
    fini=0; score=0; taille=0;
    x=WIDTH/2; y=HEIGHT/2; direction=0;
    message[0]='\0'; messageDuree=0;
    if(!creerPommes()) fini=1;
}
void dessiner(void) {
    char grille[HEIGHT][WIDTH];
    int i,j;
    const char *nom;
    memset(grille,' ',sizeof(grille));
    for(i=0;i<taille;i++) {
        if(corpsX[i]>=0 && corpsX[i]<WIDTH && corpsY[i]>=0 && corpsY[i]<HEIGHT)
            grille[corpsY[i]][corpsX[i]]='o';
    }
    for(i=0;i<NB_POMMES;i++)
        if(!pommePrise[i]) grille[pommeY[i]][pommeX[i]]='*';
    if(x>=0 && x<WIDTH && y>=0 && y<HEIGHT) grille[y][x]='O';
    curseur(0,0);
    for(j=0;j<WIDTH+2;j++) putchar('#');
    putchar('\n');
    for(i=0;i<HEIGHT;i++) {
        putchar('#');
        for(j=0;j<WIDTH;j++) putchar(grille[i][j]);
        puts("#");
    }
    for(j=0;j<WIDTH+2;j++) putchar('#');
    putchar('\n');
    nom=mode==MANUEL ? "Manuel" : mode==ORDI_LENT ? "Ordinateur lent" : "Ordinateur rapide";
    printf("Score : %-6d Taille : %-4d Pommes restantes : %d     \n",score,taille+1,NB_POMMES-pommesMangees);
    printf("Mode : %-20s                      \n",nom);
    puts("W A S D : diriger | P : sauvegarder | X : quitter    ");
    printf("M : sons ON/OFF | N : musique ON/OFF | Musique : %s      \n",(sonActif && musiqueActive) ? "ON " : "OFF");
    printf("%-65s\n",messageDuree>0 ? message : "");
    fflush(stdout);
}
int sauvegarder(void) {
    FILE *f;
    int i;
    f=fopen(SAVE_FILE,"w");
    if(f==NULL) return 0;
    fprintf(f,"%d %d %d %d %d %d %d\n",mode,score,x,y,direction,taille,pommesMangees);
    for(i=0;i<taille;i++) fprintf(f,"%d %d\n",corpsX[i],corpsY[i]);
    for(i=0;i<NB_POMMES;i++) fprintf(f,"%d %d %d\n",pommeX[i],pommeY[i],pommePrise[i]);
    if(fclose(f)!=0) return 0;
    return 1;
}
int charger(void) {
    FILE *f;
    int i,j,compte=0;
    f=fopen(SAVE_FILE,"r");
    if(f==NULL) return 0;
    if(fscanf(f,"%d %d %d %d %d %d %d",&mode,&score,&x,&y,&direction,&taille,&pommesMangees)!=7) {
        fclose(f); return 0;
    }
    if(mode<0 || mode>2 || score<0 || direction<0 || direction>4 ||
       taille<0 || taille>=MAX_LEN || x<0 || x>=WIDTH || y<0 || y>=HEIGHT ||
       pommesMangees<0 || pommesMangees>NB_POMMES) {
        fclose(f); return 0;
    }
    for(i=0;i<taille;i++) {
        if(fscanf(f,"%d %d",&corpsX[i],&corpsY[i])!=2 ||
           corpsX[i]<0 || corpsX[i]>=WIDTH || corpsY[i]<0 || corpsY[i]>=HEIGHT) {
            fclose(f); return 0;
        }
        if(corpsX[i]==x && corpsY[i]==y) { fclose(f); return 0; }
        for(j=0;j<i;j++) if(corpsX[i]==corpsX[j] && corpsY[i]==corpsY[j]) {
            fclose(f); return 0;
        }
    }
    for(i=0;i<NB_POMMES;i++) {
        if(fscanf(f,"%d %d %d",&pommeX[i],&pommeY[i],&pommePrise[i])!=3 ||
           pommeX[i]<0 || pommeX[i]>=WIDTH || pommeY[i]<0 || pommeY[i]>=HEIGHT ||
           (pommePrise[i]!=0 && pommePrise[i]!=1)) {
            fclose(f); return 0;
        }
        if(pommePrise[i]) compte++;
        else {
            if(pommeX[i]==x && pommeY[i]==y) { fclose(f); return 0; }
            for(j=0;j<taille;j++) if(pommeX[i]==corpsX[j] && pommeY[i]==corpsY[j]) {
                fclose(f); return 0;
            }
            for(j=0;j<i;j++) if(!pommePrise[j] && pommeX[i]==pommeX[j] && pommeY[i]==pommeY[j]) {
                fclose(f); return 0;
            }
        }
    }
    fclose(f);
    if(compte!=pommesMangees || compte==NB_POMMES) return 0;
    fini=0; message[0]='\0'; messageDuree=0;
    return 1;
}
int oppose(int d) {
    if(d==1) return 2;
    if(d==2) return 1;
    if(d==3) return 4;
    if(d==4) return 3;
    return 0;
}
void lireClavier(void) {
    int c,nouvelle;
    if(!_kbhit()) return;
    c=_getch();
    if(c==0 || c==224) { _getch(); return; }
    c=tolower((unsigned char)c);
    if(c=='x') { fini=1; return; }
    if(c=='m') {
        sonActif=!sonActif;
        if(sonActif) demarrerMusique(); else arreterMusique();
        strcpy(message,sonActif ? "Son active" : "Son coupe"); messageDuree=30; return;
    }
    if(c=='n') {
        musiqueActive=!musiqueActive;
        if(musiqueActive) demarrerMusique(); else arreterMusique();
        strcpy(message,musiqueActive ? "Musique activee" : "Musique coupee"); messageDuree=30; return;
    }
    if(c=='p') {
        strcpy(message,sauvegarder() ? "Partie sauvegardee !" : "Erreur de sauvegarde.");
        messageDuree=30;
        return;
    }
    if(mode!=MANUEL) return;
    nouvelle=0;
    if(c=='a') nouvelle=1;
    if(c=='d') nouvelle=2;
    if(c=='w') nouvelle=3;
    if(c=='s') nouvelle=4;
    if(nouvelle && (taille==0 || nouvelle!=oppose(direction))) direction=nouvelle;
}
int surCorps(int px,int py,int ignorerDernier) {
    int i,limite=taille-(ignorerDernier ? 1 : 0);
    for(i=0;i<limite;i++) if(corpsX[i]==px && corpsY[i]==py) return 1;
    return 0;
}
/* Surface accessible en evitant les cases occupees par le corps. */
int surfaceLibre(int sx,int sy) {
    unsigned char vu[HEIGHT][WIDTH];
    int file[MAX_LEN],debut=0,fin=0,aire=0;
    int i,d,px,py,nx,ny,caseCourante;
    if(sx<0 || sx>=WIDTH || sy<0 || sy>=HEIGHT) return 0;
    memset(vu,0,sizeof(vu));
    for(i=0;i<taille;i++) vu[corpsY[i]][corpsX[i]]=1;
    if(vu[sy][sx]) return 0;
    vu[sy][sx]=1;
    file[fin++]=sy*WIDTH+sx;
    while(debut<fin) {
        caseCourante=file[debut++];
        px=caseCourante%WIDTH; py=caseCourante/WIDTH;
        aire++;
        for(d=1;d<=4;d++) {
            nx=px+DX[d]; ny=py+DY[d];
            if(nx<0 || nx>=WIDTH || ny<0 || ny>=HEIGHT || vu[ny][nx]) continue;
            vu[ny][nx]=1;
            file[fin++]=ny*WIDTH+nx;
        }
    }
    return aire;
}
int distancePomme(int px,int py) {
    int i,d,meilleure=100000;
    for(i=0;i<NB_POMMES;i++) {
        if(pommePrise[i]) continue;
        d=abs(pommeX[i]-px)+abs(pommeY[i]-py);
        if(d<meilleure) meilleure=d;
    }
    return meilleure==100000 ? 0 : meilleure;
}
int choixOrdi(void) {
    int d,nx,ny,cout,meilleur=100000000,choix=0,aire;
    for(d=1;d<=4;d++) {
        if(direction!=0 && taille>0 && d==oppose(direction)) continue;
        nx=x+DX[d]; ny=y+DY[d];
        if(nx<0 || nx>=WIDTH || ny<0 || ny>=HEIGHT) continue;
        if(surCorps(nx,ny,1)) continue;
        aire=surfaceLibre(nx,ny);
        cout=distancePomme(nx,ny)*10;
        if(d!=direction) cout++;
        if(aire<taille+2) cout+=10000;
        if(cout<meilleur) { meilleur=cout; choix=d; }
    }
    return choix;
}
void avancer(void) {
    int nx,ny,i,dernierX,dernierY,grandit=0;
    if(direction==0) return;
    nx=x+DX[direction]; ny=y+DY[direction];
    if(nx<0 || nx>=WIDTH || ny<0 || ny>=HEIGHT) { fini=1; return; }
    for(i=0;i<NB_POMMES;i++)
        if(!pommePrise[i] && pommeX[i]==nx && pommeY[i]==ny && pommesMangees==NB_POMMES-1)
            grandit=1;
    /* La derniere case se libere, sauf lors d'une croissance. */
    if(surCorps(nx,ny,!grandit)) { fini=1; return; }
    dernierX=taille>0 ? corpsX[taille-1] : x;
    dernierY=taille>0 ? corpsY[taille-1] : y;
    for(i=taille-1;i>0;i--) {
        corpsX[i]=corpsX[i-1]; corpsY[i]=corpsY[i-1];
    }
    if(taille>0) { corpsX[0]=x; corpsY[0]=y; }
    x=nx; y=ny;
    for(i=0;i<NB_POMMES;i++) {
        if(!pommePrise[i] && pommeX[i]==x && pommeY[i]==y) {
            pommePrise[i]=1; pommesMangees++; score+=10; sonPomme();
        }
    }
    if(pommesMangees==NB_POMMES) {
        sonGrandir();
        if(taille<MAX_LEN-1) {
            corpsX[taille]=dernierX;
            corpsY[taille]=dernierY;
            taille++;
        }
        if(!creerPommes()) {
            strcpy(message,"Bravo ! Plateau rempli.");
            messageDuree=100;
            fini=1;
        }
    }
}
int delai(void) {
    if(mode==ORDI_LENT) return 150;
    if(mode==ORDI_RAPIDE) return 30;
    return 100;
}
void jouer(int reprise) {
    int choix;
    system("cls");
    if(reprise) {
        if(!charger()) {
            puts("Aucune sauvegarde valide trouvee.");
            puts("Appuyez sur une touche...");
            _getch(); return;
        }
        dessiner();
        puts("Partie chargee. Appuyez sur une touche pour continuer...");
        _getch(); system("cls");
    } else nouveauJeu();
    sonDebut();
    demarrerMusique();
    while(!fini) {
        dessiner();
        lireClavier();
        if(fini) break;
        if(mode!=MANUEL) {
            choix=choixOrdi();
            if(choix==0) { fini=1; break; }
            direction=choix;
        }
        avancer();
        if(messageDuree>0) messageDuree--;
        Sleep((DWORD)delai());
    }
    arreterMusique();
    sonFin();
    dessiner();
    printf("\n*** FIN DE PARTIE *** Score final : %d\n",score);
    puts("Appuyez sur une touche pour revenir au menu...");
    while(_kbhit()) _getch();
    _getch();
}
int menu(void) {
    system("cls");
    puts("========== SNAKE ==========");
    puts(" 1. Nouvelle partie (manuel)");
    puts(" 2. Reprendre la sauvegarde");
    puts(" 3. Ordinateur - lent");
    puts(" 4. Ordinateur - rapide");
    puts(" 5. Quitter");
    puts(" Pendant le jeu : M = sons ON/OFF, N = musique ON/OFF");
    printf("\nVotre choix : ");
    return _getch();
}
int main(void) {
    int actif=1;
    srand((unsigned int)time(NULL));
    cacherCurseur();
    initialiserMusique();
    while(actif) {
        switch(menu()) {
            case '1': mode=MANUEL; jouer(0); break;
            case '2': jouer(1); break;
            case '3': mode=ORDI_LENT; jouer(0); break;
            case '4': mode=ORDI_RAPIDE; jouer(0); break;
            case '5': actif=0; break;
            default: break;
        }
    }
    return 0;
}