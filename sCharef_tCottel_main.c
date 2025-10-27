
/**COTTEL THIBAULT & CHAREF SOHEL - 3A AVM - TP INFOGRAPHIE 2023

Problemes actuels :
    - Ballon
        Rotation Pas fonctionnel

**/
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
///FONCTIONS///

/** surface de dessin**/
typedef struct Surface
{
    int width;    /// largeur
    int height;   /// hauteur
    int depth;    /// Valeur Max
    double *data[3]; ///< pixels | 0 = R | 1 = V | 2 = B
}SURFACE;

typedef struct Surface SURFACE;

typedef struct Color
{
    double r;
    double g;
    double b;
} Color;

Color rgb(int red, int green, int blue)
{
    Color curr;
    curr.r = (double)red;
    curr.g = (double)green;
    curr.b = (double)blue;
    return curr;
}

typedef struct Color COLOR;

// Structure pour stocker des coordonnees (x, y)
typedef struct Point {
    double x;
    double y;
} Point;


typedef struct Pile{
    Point *data;
    int size;
    int top;
}Pile;


/**Fonction qui remplit la valeur RGB pixel par pixel**/
int pixel_set(SURFACE *s, int x, int y, Color c)
{
    if (x < 0 || x >= s->width || y < 0 || y >= s->height){
        //printf("x = %d / y = %d\n", x, y);
        return 0;
    }
    s->data[0][(int)(x + y * s->width)] = c.r;
    s->data[1][(int)(x + y * s->width)] = c.g;
    s->data[2][(int)(x + y * s->width)] = c.b;
    return 1;
}

/**Fonction qui cree une pile et la retourne **/
Pile *creerPile(int size){
    Pile *p = (Pile*)malloc(sizeof(Pile));
    p->data = (Point*)malloc(size*sizeof(Point));
    p->size = size;
    p->top = -1;
    return p;
}

/**Fonction qui permet d'empiler un element dans une pile donnee en parametre**/
void empiler(Pile *p, int x, int y){
    if(p->top < p->size -1){
        p->top++;
        p->data[p->top].x = x;
        p->data[p->top].y = y;
        //printf("X %d Y %d \n", x,y);
    }
}

/**Fonction qui permet de depiler la derniere pile**/
int depiler(Pile *p, int *x, int *y){
    if(p->top >= 0){
        *x = p->data[p->top].x;
        *y = p->data[p->top].y;
        p->top--;
        return 1;
    }
    return 0;
}

/**Fonction qui libere la pile**/
void freePile(Pile *p){
    free(p->data);
    free(p);
}

/**Fonction qui permet le remplissage par propagation d'une surface**/
void remplissageProp(SURFACE *s, int x, int y, Color c){
    //Fonction qui permet de faire un remplissage par propagation d'une surface
    Pile *p = creerPile(s->width * s->height);
    empiler(p, x, y);
    while(p->top >= 0){
        int currX, currY;
        //printf("AVANT CurX %d CurY %d\n", currX, currY);
        depiler(p, &currX, &currY);
        //printf("APRES CurX %d CurY %d\n", currX, currY);
        //printf("%lf \n",  s->data[0][currX + currY *s->width]);
        if(currX >= 0 && currX < s->width && currY >= 0 && currY < s->height && s->data[0][currX + currY * s->width] == 256 && s->data[1][currX + currY * s->width] && s->data[2][currX + currY * s->width]){        //Ici on pourrait ajouter un truc pour le cas ou la zone a dessiner n'est pas blanche
                //printf("Dans le IF \n");
                pixel_set(s, currX, currY, c);
                //On ajoute les voisins a la pile
                empiler(p, currX + 1, currY);
                empiler(p, currX -1, currY);
                empiler(p, currX, currY +1);
                empiler(p, currX, currY -1);
           }
    }
    freePile(p);
}

/**Fonction qui permet le remplissage par propagation d'une surface en y ajoutant du bruit**/
void remplissagePropAvecBruit(SURFACE *s, int x, int y, Color c, double degradation) {       // + degradation eleve, + l'image sera degrade
    Pile *p = creerPile(s->width * s->height);
    empiler(p, x, y);
    srand(time(NULL));

    while (p->top >= 0) {
        int currX, currY;
        depiler(p, &currX, &currY);

        //Verifie que les coordonnees sont a l'interieur de la surface
        if (currX >= 0 && currX < s->width && currY >= 0 && currY < s->height) {

                if (s->data[0][currX + currY * s->width] == 256 && s->data[1][currX + currY * s->width] == 256 && s->data[2][currX + currY * s->width] == 256){
                Color noise;
                noise.r = c.r - degradation / 2.0 + (rand() % (int)degradation);
                noise.g = c.g - degradation / 2.0 + (rand() % (int)degradation);
                noise.b = c.b - degradation / 2.0 + (rand() % (int)degradation);
                pixel_set(s, currX, currY, noise);

                //Ajoute les voisins a la pile
                empiler(p, currX + 1, currY);
                empiler(p, currX - 1, currY);
                empiler(p, currX, currY + 1);
                empiler(p, currX, currY - 1);
            }
        }
    }
    freePile(p);
}




/**Fonction qui cree une surface avec longueur et largeur donnes en parametre**/
void surface(SURFACE *s, int width, int height)
{
    s->data[0] = (double *)malloc(width * height * sizeof(double));
    s->data[1] = (double *)malloc(width * height * sizeof(double));
    s->data[2] = (double *)malloc(width * height * sizeof(double));
    if (s->data != NULL)
    {
        s->width = width;
        s->height = height;
        s->depth = 255;
    }
}



 /**Fonction qui libere le contenu d'une surface**/
void free_surface(SURFACE *s)
{
    free(s->data[0]);
    free(s->data[1]);
    free(s->data[2]);
}


/**Fonction qui remplit une surface en couleur RGB***/
void fill(SURFACE *s, double valueR, double valueG, double valueB)
{
    for (int i = 0; i < s->width * s->height; ++i)
    {
        s->data[0][i] = valueR;
        s->data[1][i] = valueG;
        s->data[2][i] = valueB;
    }
}


/**Fonction qui ecrit une surface s sur un fichier f**/
int pgm_write(SURFACE *s, FILE *f)
{
    int max = s->depth;
    int count = fprintf(f, "P3\n#_png_write\n%d %d\n%d\n", s->width, s->height, max);
    int cr = s->width;
    for (int i = 0; i < s->width * s->height; ++i)
    {
        count += fprintf(f, "%d ", (int)(s->data[0][i]));
        count += fprintf(f, "%d ", (int)(s->data[1][i]));
        count += fprintf(f, "%d ", (int)(s->data[2][i]));
        //clamp((int)(s->data[i] * max), 0, max));
        if (--cr)
            count += fprintf(f, " ");
        else
        {
            cr = s->width;
            count += fprintf(f, "\n");
        }
    }
    return count;
}

/**Fonction qui trace un rectangle de couleur**/
int rectangle(SURFACE *s,int Posx, int Posy, int lengthx, int lengthy, Color c){
    int initPose = (Posy*s->width)+Posx;
    int endPose = ((Posy+lengthy)*s->width) + Posx + lengthx;
    int i,j;
    //Ajout d'une securit  en cas de OOB
    for(j=initPose;(j<endPose);j+=s->width){
        for(i=0;(i<lengthx && j+i<(s->height*s->width));i++){
            s->data[0][j+i] = c.r ;
            s->data[1][j+i] = c.g ;
            s->data[2][j+i] = c.b ;
        }
    }
    return 1;
}



/**Fonction qui applique un filtre au triangle**/
int filtre_rectangle(SURFACE *s,int Posx, int Posy, int lengthx, int lengthy, Color c){
    int initPose = (Posy*s->width)+Posx;
    int endPose = ((Posy+lengthy)*s->width) + Posx + lengthx;
    int i,j, x, bruit = 10;
    for(j=initPose;j<endPose;j+=s->width){
        for(i=0;(i<lengthx && j+i<(s->height*s->width));i++){
            if(c.r>bruit) x = rand() % bruit; // Bruit
            else x = 0;
            s->data[0][j+i] = (s->data[0][j+i]+c.r)/2 - x;
            if(c.g>bruit) x = rand() % bruit; // Bruit
            else x = 0;
            s->data[1][j+i] = (s->data[1][j+i]+c.g)/2 - x;
            if(c.b>bruit) x = rand() % bruit; // Bruit
            else x = 0;
            s->data[2][j+i] = (s->data[2][j+i]+c.b)/2 - x;
        }
    }
    return 1;
}



/**Fonction qui applique un degrade sur la surface donnee en parametre**/
void degrade(SURFACE *surface, int x0, int x1, int y, Color start, Color finish)
{
    for (int x = x0; x <= x1; ++x)
    {

        double normalized_position = (double)(x - x0) / (double)(x1 - x0);
        Color current = rgb(start.r + normalized_position * (finish.r - start.r), start.g + normalized_position * (finish.g - start.g), start.b + normalized_position * (finish.b - start.b));
        pixel_set(surface, x, y, current);
    }
}

/**Fonction qui permet de tracer un cercle**/
void cercle(SURFACE *s, int posX,int posY,int ray,Color c){
    double i;
    for(i = 1/(2*M_PI*ray);i<(2*M_PI);i=i+1/(2*M_PI*ray)){
        int x = (int)(posX + ray * cos(i));
        int y = (int)(posY + ray * sin(i));
        pixel_set(s, x, y, c);
    }
}

void demi_cercle(SURFACE *s, int posX, int posY, int ray, Color c) {
    double i;
    for (i = 0; i <= M_PI; i += 1.0 / (2*M_PI*ray)) {
        int x = (int)(posX + ray * cos(i));
        int y = (int)(posY + ray * sin(i));
        pixel_set(s, x, y, c);
    }
}

/**Fonction qui permet de tracer un cercle colore**/
void cercle_color(SURFACE *s, int x, int y, int ray,Color c){
    do{
        cercle(s, x, y, ray, c);
        ray-=1;
    }while(ray>0);
}

/**Fonction qui permet de tracer une courbe de Bezier de degre 4**/
void courb(SURFACE *s, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, Color c) {
    double t;
    for (t = 0; t <= 1; t += 0.001) {
        double un_moins_t = 1 - t;
        double un_moins_t_cube = un_moins_t * un_moins_t * un_moins_t;
        double t_cube = t * t * t;

        int qx = (int)((x0 * un_moins_t_cube) +
                      (x1 * 3 * t * un_moins_t * un_moins_t) +
                      (x2 * 3 * t * t * un_moins_t) +
                      (x3 * t_cube));

        int qy = (int)((y0 * un_moins_t_cube) +
                      (y1 * 3 * t * un_moins_t * un_moins_t) +
                      (y2 * 3 * t * t * un_moins_t) +
                      (y3 * t_cube));


        qx = (qx < 0) ? 0 : ((qx >= s->width) ? s->width - 1 : qx);             //Verifie que qx et qy sont dans l'image
        qy = (qy < 0) ? 0 : ((qy >= s->height) ? s->height - 1 : qy);

        //printf("%d %d\n", qx, qy);
        pixel_set(s, qx, qy, c);
    }
}

/**Fonction qui permet de tracer une ligne d'un point A=(x1,y1) a un point B=(x2,y2)**/
void ligne(SURFACE *s, int x1, int y1, int x2, int y2, Color c) {
    double x, y = 0.0;
    double a, b;

    if (x1 == x2) {
        // Gere le cas o� x1 est �gal � x2 (division par z�ro)
        for (int y = y1; y <= y2; y++) {
            pixel_set(s, x1, y, c);
        }
    } else {
        if (x1 < x2) {
            int dx = x2 - x1;
            int dy = y2 - y1;
            a = (double)(dy) / dx;
            b = y1 - a * x1;

            for (x = x1; x <= x2; x += 0.1) {
                y = a * x + b;
                pixel_set(s, (int)x, (int)y, c);
            }
        } else {
            int dx = x1 - x2;
            int dy = y1 - y2;
            a = (double)(dy) / dx;
            b = y1 - a * x1;

            for (x = x2; x <= x1; x += 0.1) {
                y = a * x + b;
                pixel_set(s, (int)x, (int)y, c);
            }
        }
    }
}



void draw_line_point(SURFACE *s, Point p1, Point p2, Color color_line) {
    int x1 = p1.x;
    int y1 = p1.y;
    int x2 = p2.x;
    int y2 = p2.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int x = x1;
    int y = y1;

    int step_x = (x1 < x2) ? 1 : -1;
    int step_y = (y1 < y2) ? 1 : -1;

    int e;

    if (dx >= dy) {  // case where the slope is less than or equal to 45 degrees
        e = dx / 2;
        while (x != x2) {
            int pos = y * s->width + x;
            s->data[0][pos] = color_line.r;
            s->data[1][pos] = color_line.g;
            s->data[2][pos] = color_line.b;
            e -= dy;
            if (e < 0) {
                y += step_y;
                e += dx;
            }
            x += step_x;
        }
    } else {  // case where the slope is greater than 45 degrees
        e = dy / 2;
        while (y != y2) {
            int pos = y * s->width + x;
            s->data[0][pos] = color_line.r;
            s->data[1][pos] = color_line.g;
            s->data[2][pos] = color_line.b;
            e -= dx;
            if (e < 0) {
                x += step_x;
                e += dy;
            }
            y += step_y;
        }
    }

    int pos = y2 * s->width + x2;
    s->data[0][pos] = color_line.r;
    s->data[1][pos] = color_line.g;
    s->data[2][pos] = color_line.b;
}



/**Fonction qui permet de tracer des lignes de maniere aleatoire**/
void lignes_al(SURFACE *s, int n, Color c){
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;


    int const MIN = 1, MAX  = 4;                        //1=OUEST , 2=SUD , 3=EST , 4=NORD


    for (int i = 0; i<n; i++){
            int nombre_un, nombre_deux;
            do{
                nombre_un = (rand() % (MAX + 1 - MIN)) + MIN;
                nombre_deux = (rand() % (MAX + 1 - MIN)) + MIN;
            }while(nombre_un == nombre_deux);

            if(nombre_un == 1){
            x1 = 0;
            y1 = rand() % (s->height+1);
            }
            if(nombre_un == 2){
                x1 = rand() % (s->width+1);
                y1 = 1000;
            }
            if(nombre_un == 3){
                x1 = 1000;
                y1 = rand() % (s->height+1);
            }
            if(nombre_un == 4){
                x1 = rand() % (s->width+1);
                y1 = 0;
            }
            if(nombre_deux == 1){
                x2 = 0;
                y2 = rand() % (s->height+1);
                ligne(s, x1, y1, x2, y2,c);
            }
            if(nombre_deux == 2){
                x2 = rand() % (s->width+1);
                y2 = 1000;
                ligne(s, x1, y1, x2, y2, c);
            }
            if(nombre_deux == 3){
                x2 = 1000;
                y2 = rand() % (s->height+1);
                ligne(s, x1, y1, x2, y2, c);
            }
            if(nombre_deux == 4){
                x2 = rand() % (s->width+1);
                y2 = 0;
            }
            printf("x1 %d / y1 %d / \n x2 %d /  y2 %d\n",x1,  y1,x2,  y2);
            printf("nbr1 %d \n nbr2 %d\n", nombre_un, nombre_deux);
        }
}

/**Fonction qui affiche les points d'intersection entre 2 droites**/
void intersection(Point p1,Point p2, Point p3, Point p4){     //tab pour valeurs de x et y
    double ad1 = (p2.y-p1.y)/(p2.x-p1.x);
    double bd1 = p1.y -ad1*p1.x;
    double cd1 = -(ad1*p1.x + bd1*p1.y);

    double ad2 = (p4.y-p3.y)/(p4.x-p3.x);
    double bd2 = p3.y-ad2*p3.x;
    double cd2 = -(ad2*p3.x + bd2*p3.y);

/*
    double droite1 = ad1*p1.x + bd1*p1.y + cd1;
    double droite2 = ad2*p3.x + bd2*p3.y + cd2;
*/
    Point I = {};
    I.x = (bd1*cd2 - cd1*bd2);
    I.y = (cd1*ad2 - ad1*cd2);

    printf("Ix = %lf / Iy = %lf \n", I.x, I.y);
    //printf("bd1 = %lf,cd2 = %lf,cd1 = %lf,bd2 = %lf\n", bd1, cd2, cd1, bd2);


    /*
    f2 = a*x2+b*y2+c2;
    if((y2-y1)/(x2-x1) == ){

    }
    */
    //I = (a1,b1,c1)*(a2,b2,c2) = (X2,Y2,Z2)
}

/**Fonction qui permet de tracer un triangle**/

void triangle(SURFACE *s, Color c){
    ligne(s, 20, 0, 80, 0, c);      //ok
    ligne(s, 20, 0, 50, 20, c);
    ligne(s, 50,20, 80,0,c);        //ok
}


/**Fonction qui permet de tracer un ballon (le ballon peut varier en taille)**/
void ballon(SURFACE *s, int centreX, int centreY ,int rayon, int rotation, Color contour, Color c1, Color c2, int nbrParties){
    int i, x, y, count,iteration = (rayon*2)/(nbrParties-2),rotX,rotY;
    SURFACE calque;
    Color saveColor, lastColor;
    surface(&calque,rayon*2,rayon*2);
    fill(&calque,256,256,256);
    //Cercle
    cercle(&calque,(int)rayon,(int)rayon,rayon,contour);
    //Interieur Balon
    for(i=0;i<=rayon*2;i+=iteration){
        if(nbrParties%2 == 0 && i == rayon) ligne(&calque,rayon,0,rayon,rayon*2,contour);
        else courb(&calque,rayon,0,i,0,i,rayon*2,rayon,rayon*2,contour);
    }
    //Remplissage du ballon avec une technique similaire au crossing number
    count = 0;
    for(i=0;i<rayon*2;++i){
        saveColor.r = calque.data[0][calque.width*rayon+i];
        saveColor.g = calque.data[1][calque.width*rayon+i];
        saveColor.b = calque.data[2][calque.width*rayon+i];
        if(saveColor.r != contour.r && saveColor.g != contour.g && saveColor.b != contour.b){
            if(count == 0){
                remplissageProp(&calque,i,rayon,c1);
                lastColor = c1;
                ++count;
            }
            else if(count%2 != 0 && saveColor.r != lastColor.r && saveColor.g != lastColor.g && saveColor.b != lastColor.b){
                remplissageProp(&calque,i,rayon,c2);
                lastColor = c2;
                ++count;
            }
            else if(count%2 == 0 && saveColor.r != lastColor.r && saveColor.g != lastColor.g && saveColor.b != lastColor.b){
                remplissageProp(&calque,i,rayon,c1);
                lastColor = c1;
                ++count;
            }
        }
    }
    //Copie sur calque
    i = 0;
    // x,y sont les pixels sur la surface final, i represente le pixel du calque
    for(y=centreY-rayon;y<(centreY+rayon);y++){
        for(x=centreX-rayon;x<(centreX+rayon);x++){
            //Si le pixel selectionner est bien une couleur
            if(calque.data[0][i]!=256 && calque.data[1][i]!=256 && calque.data[2][i]!=256){
                //Nous sauvegardons la valeur de couleur du pixel du calque
                saveColor.r = calque.data[0][i];
                saveColor.g = calque.data[1][i];
                saveColor.b = calque.data[2][i];
                //Si il n'y a pas de rotation a faire, nous appliquons simplement les pixels normalement
                if(rotation%360 == 0) pixel_set(s,x,y,saveColor);
                else{
                    //Si il y a rotation a faire, nous calculons la position qu'il aura dans la surfance finale et l'appliquons
                    rotX = round((x-centreX)*cos((double)(rotation*2*M_PI)/360)+(y-centreY)*sin((double)(rotation*2*M_PI)/360))+centreX;
                    rotY = round(-(x-centreX)*sin((double)(rotation*2*M_PI)/360)+(y-centreY)*cos((double)(rotation*2*M_PI)/360))+centreY;
                    pixel_set(s,rotX,rotY,saveColor);
                }
            }
            //On change de pixel dans le calque
            i++;
        }
    }
    free_surface(&calque);
}

/**Fonction qui permet de tracer un ballon simple**/
void ballon2(SURFACE *s, Color Contour, Color c1, Color c2, Color c3, Color c4, Color interieur1, Color interieur2,int posX,int posY){
    cercle(s, posX, posY, 20, Contour);
    cercle(s, posX+5, posY-10, 4, Contour);
    courb(s, posX-19, posY+6, posX-11, posY-5, posX-11, posY-5, posX+1, posY-8, c1);
    courb(s, posX-11, posY+16, posX-5, posY+2, posX-5, posY+2, posX+2, posY-7, c1);
    remplissageProp(s, posX-11, posY-2,c1);
    courb(s, posX+2, posY+18, posX+2, posY+4, posX+2, posY+4, posX+6, posY-6, c2);
    courb(s, posX+13, posY+14, posX+11, posY+2, posX+11, posY+2, posX+7, posY-6, c2);
    remplissageProp(s, posX+7, posY+8, c2);
    courb(s, posX-16, posY-11, posX-7, posY-13, posX-7, posY-13, posX+1, posY-13, c3);
    courb(s, posX-6, posY-19, posX+1,posY-17, posX+1, posY-17, posX+3, posY-14, c3);
    remplissageProp(s, posX-7, posY-16, c3);
    courb(s, posX+7, posY-15, posX+7, posY-16, posX+7, posY-16, posX+8, posY-18, c4);
    courb(s, posX+9, posY-11, posX+12, posY-10, posX+12, posY-10, posX+15, posY-13, c4);
    remplissageProp(s, posX+10, posY-14, c4);
    remplissageProp(s, posX-5, posY-11, interieur1);
    remplissageProp(s, posX+4, posY-17, interieur1);
    remplissageProp(s, posX+13, posY-4, interieur1);
    remplissageProp(s, posX-4, posY+7, interieur1);
    remplissageProp(s, posX+5, posY-10, interieur2);
}


/**Fonction qui permet de dessiner un navire**/
void navire(SURFACE *s,int x, int y, Color contour, Color coque, Color voile){
    rectangle(s, x,y, 5, 50, contour);
    rectangle(s, x-20, y+40, 55, 5, contour);
    courb(s, x-20,y+40, x-12,y+24,x-6,y+12,x,y,contour);
    courb(s, x+5,y, x+14,y+12, x+25,y+27,x+34,y+40,contour);
    courb(s, x-20,y+50,x-20,y+50, x-10,y+60,x-10,y+60, contour);
    courb(s, x+20,y+60, x+20,y+60, x+30,y+50, x+34,y+50,contour);
    ligne(s, x-10,y+60,x+20,y+60,contour);
    ligne(s, x-20,y+50,x+33,y+50,contour);
    remplissageProp(s, x-10,y+30,voile);
    remplissageProp(s, x+10,y+30,voile);
    remplissageProp(s, x,y+55,coque);

}

/**Fonction qui permet de tracer un drapeau**/
void drapeau(SURFACE *s,int x, int y, int hauteurPoteau, Color colorPoteau, Color couleurDrapeau){
    int hautPoteau = y-hauteurPoteau, largeurDrapeau = 30, largeurPoteau = 5;
    rectangle(s,x,hautPoteau,largeurPoteau,hauteurPoteau,colorPoteau);
    courb(s,x+largeurPoteau,hautPoteau,x+largeurPoteau+10,hautPoteau+10,x+largeurPoteau+20,hautPoteau-20,x+largeurPoteau+40,hautPoteau,couleurDrapeau);
    rectangle(s,x+largeurPoteau+40,hautPoteau,1,largeurDrapeau,couleurDrapeau);
    hautPoteau += largeurDrapeau;
    courb(s,x+largeurPoteau,hautPoteau,x+largeurPoteau+10,hautPoteau+10,x+largeurPoteau+20,hautPoteau-20,x+largeurPoteau+40,hautPoteau,couleurDrapeau);
    remplissageProp(s,x+largeurPoteau+1,hautPoteau-2,couleurDrapeau);
}

/**Fonction qui permet de tracer des vagues de maniere aleatoire dans la mer**/
void vagues(SURFACE *s, int x, int y, int longeurVague, Color couleurVagues){
    courb(s,x,y,x+((double)longeurVague/3),y+10,x+((double)2*longeurVague/3),y-10,x+longeurVague,y,couleurVagues);
}

void parasol(SURFACE *s, Color ColorMat, Color ColorToile){
    ligne(s, 340,305, 390, 250, ColorMat);
    ligne(s, 345,308, 395, 253, ColorMat);
    ligne(s, 340,304,345,307,ColorMat);
    ligne(s, 389,251,394,254,ColorMat);
    //Correction due a la droite qui n'est pas droite
    pixel_set(s, 350,294,ColorMat);
    pixel_set(s, 360,283,ColorMat);
    pixel_set(s, 370,272,ColorMat);
    pixel_set(s, 380,261,ColorMat);

    pixel_set(s, 385,264,ColorMat);
    pixel_set(s, 375,275,ColorMat);
    pixel_set(s, 365,286,ColorMat);
    pixel_set(s, 355,297,ColorMat);
    //Remplissage Mat
    remplissageProp(s, 365,280,ColorMat);

    ligne(s, 360,225, 427, 280, ColorToile);
    courb(s, 360,225,406,228,420,238,  426,279,ColorToile);
    remplissageProp(s, 400,245,ColorToile);
    courb(s, 360,225,361,231,361,231,369,232,ColorToile);
    remplissageProp(s, 362,228,ColorToile);
    courb(s, 369,232,368,237,368,237, 376,239,ColorToile);
    remplissageProp(s, 370,235,ColorToile);
    courb(s, 376,239,377,245,377,245, 385,245,ColorToile);
    remplissageProp(s, 379,242,ColorToile);
    courb(s, 385,245,384,249,384,249, 389,251,ColorToile);
    remplissageProp(s, 386,248,ColorToile);
    courb(s, 395,253,395,258,395,258,400,258,ColorToile);
    remplissageProp(s, 396,256,ColorToile);
    courb(s, 400,258,401,264,401,264,408,264,ColorToile);
    remplissageProp(s, 402,261,ColorToile);
    courb(s, 408,264,408,269,408,269,415,270,ColorToile);
    remplissageProp(s, 410,267,ColorToile);
    courb(s, 416,270,416,277,416,277,425,279,ColorToile);
    remplissageProp(s, 419,275,ColorToile);
}

void filet_volley(SURFACE *s,Point p1, Point p2, Color colorPoto, Color colorFilet){
    rectangle(s, 48, 401, 2, 98, colorPoto);
    rectangle(s, 151,349, 2, 98, colorPoto);


    //Lignes en diagonale
    p1.x = 50;
    p1.y = 400;
    p2.x = 150;
    p2.y = 350;

    for(int j = 0;j<=10;++j){
        draw_line_point(s, p1,p2,colorFilet);
        p1.y += 5;
        p2.y += 5;
    }

    //lignes en verticale
    int posX = 50;
    int y1 = 400;
    int y2 = 450;

    for(int k=0; k<10; k+=1){
        posX += 5;
        y1 -= 2;
        y2 -= 2;
        ligne(s, posX, y1, posX,y2, colorFilet);
        posX += 5;
        y1 -= 3;
        y2 -= 3;
        ligne(s, posX, y1, posX,y2, colorFilet);
    }
}

void ecume(SURFACE *s, Color Couleur){
    courb(s, 0, 180, 120, 195, 150, 250, 210, 280, Couleur);
    courb(s, 210, 280, 330, 297, 412, 305, 500, 270, Couleur);

    courb(s, 0,186,10, 193, 19, 188, 25,192, Couleur);
    courb(s, 30,193,75,220,129,242, 182,281, Couleur);
    courb(s, 182,281, 217,294,286,293,345,298, Couleur);
    courb(s, 350,303, 404,307, 466,286,500,275, Couleur);
    remplissageProp(s, 10,185,Couleur);
    remplissageProp(s, 134,240,Couleur);
    remplissageProp(s, 391,297,Couleur);
}


void nuage(SURFACE *s, int xSet, int ySet, int length, Color colorLineNuage,Color colorInsideNuage, int isBehind){
    SURFACE calque;
    Color saveColor,currentColor;
    int i = 0,x,y, canBeDrawn;
    surface(&calque,length,50);
    fill(&calque,256,256,256);
    // Dessin des courbes du nuage
        //Longeur
    courb(&calque,length/5,10,length/5,0,2*length/5,0,2*length/5,10,colorLineNuage);
    courb(&calque,2*length/5,10,2*length/5,0,3*length/5,0,3*length/5,10,colorLineNuage);
    courb(&calque,3*length/5,10,3*length/5,0,4*length/5,0,4*length/5,10,colorLineNuage);
        //Largeur
    courb(&calque,4*length/5,10,length,10,length,25,4*length/5,25,colorLineNuage);
    courb(&calque,4*length/5,25,length,25,length,40,4*length/5,40,colorLineNuage);
        //Longeur
    courb(&calque,4*length/5,40,4*length/5,50,3*length/5,50,3*length/5,40,colorLineNuage);
    courb(&calque,3*length/5,40,3*length/5,50,2*length/5,50,2*length/5,40,colorLineNuage);
    courb(&calque,2*length/5,40,2*length/5,50,length/5,50,length/5,40,colorLineNuage);
        //Largeur
    courb(&calque,length/5,40,0,40,0,25,length/5,25,colorLineNuage);
    courb(&calque,length/5,25,0,25,0,10,length/5,10,colorLineNuage);
    //Remplissage nuage
    remplissageProp(&calque,length/2,20,colorInsideNuage);
    i = 0;
    for(y=ySet;y<ySet+calque.height;y++){
        for(x=xSet;x<xSet+calque.width;x++){
            //Regarde si la case peut etre colorier selon si on veut le nuages en arriere plan ou non et si la case fait parti du nuage
            canBeDrawn = 0;
            if(isBehind == 1){
                //Si il est en arriere plan, nous regardons si la case à colorier n'est pas encore prise avant d'appliquer le calque
                currentColor.r = s->data[0][y*s->width+x];
                currentColor.g = s->data[1][y*s->width+x];
                currentColor.b = s->data[2][y*s->width+x];
                if(calque.data[0][i]!=256 && calque.data[1][i]!=256 && calque.data[2][i]!=256
               && currentColor.r == 256 && currentColor.g == 256 && currentColor.b == 256){
                    canBeDrawn = 1;
                }
            }
            else if(calque.data[0][i]!=256 && calque.data[1][i]!=256 && calque.data[2][i]!=256){
                //Sinon nous pouvons directement colorier le calque en premier plan tant que c'est bien une bonne couleur
                canBeDrawn = 1;
            }

            if(canBeDrawn== 1){
                //Si on autorise le dessin, on dessine sur la surface
                saveColor.r = calque.data[0][i];
                saveColor.g = calque.data[1][i];
                saveColor.b = calque.data[2][i];
                pixel_set(s,x,y,saveColor);
            }
            i++;
        }
    }
    free_surface(&calque);
}


///-----------------------------------------------MAIN-----------------------------------------------------------///

int main()
{
    int width = 500, height = 500,i,nbrVagues;
    srand(time(0));
    /**Inititaliser seed de la fonction random afin d'etre sur de ne pas avoir les memes variables en relancant**/
    /**-------------------------Initialisation surface-------------------------**/
    SURFACE s = {0};
    surface(&s,width,height);

    Color Blanc = {255,255,255};
    Color Rouge = {255, 0, 0};
    Color Vert = {0, 255, 0};
    Color VertFonce = {0, 125, 0};
    Color Bleu = {0, 0, 255};
    //Color BleuFonce = {0, 0, 175};
    Color Jaune = {255, 217, 102};
    Color Noir = {0, 0, 0};
    Color GrisClair = {240,240,240};
    Color Gris = {192,192,192};
    Color GrisFonce = {115,115,115};
    Color Sable = {224, 205,169};
    Color Mer = {126,195,214};
    Color Ciel = {3,89,174};
    Color Marron = {63,34,4};
    Color Violet = {199,21,133};

    /** ---------------Tests des fonctions fill et rectangle -----------------**/

    // Rempli en Blanc
    fill(&s,256,256,256);

    /**---------------Dessin plage------------------**/

    Point p1;
    Point p2;

    /*Objets*/


    parasol(&s, Marron, Violet);
    filtre_rectangle(&s, 238,300,100, 40, VertFonce);
    ballon(&s,430,430,40,90,Noir,Blanc,Rouge,7);
    ballon(&s,70,290,15,0,Noir,Blanc,Bleu, 7);
    ballon2(&s, Noir, Bleu, Jaune, Rouge, Vert, GrisClair, Gris, 360,375);
    cercle_color(&s, 500, 0, 80, Jaune);

    parasol(&s, Marron, Violet);
    ligne(&s, 0,105, 234, 105, Ciel);
    ligne(&s, 276, 105, 500, 105, Ciel);
    navire(&s, 250, 50, Noir, Rouge, GrisClair);
    drapeau(&s,25,325,200,GrisFonce,VertFonce);
    //navire(&s, 100, 100, Noir, Vert, GrisClair);
    ecume(&s, Blanc);

    pixel_set(&s,25,183,GrisFonce );
    for (int correc = 25;correc<=29;correc++){
        pixel_set(&s, correc, 184, GrisFonce);
    }
    pixel_set(&s, 349,294,Marron);
    pixel_set(&s, 350,294,Marron);

    nuage(&s,50,35,70,Noir,Blanc,0);
    nuage(&s,150,15,150,Noir,Blanc,1);
    nuage(&s,300,40,100,Noir,Blanc,0);
    /*Remplissage*/

    for(i=351;i<=356;++i){pixel_set(&s, i, 294,Marron);}
    remplissageProp(&s, 1,1,Mer);
    remplissageProp(&s, 1, 106, Ciel);
    remplissageProp(&s, 390,270, Ciel);
    //remplissageProp(&s, 1,499,Sable);
    remplissagePropAvecBruit(&s, 1,409,Sable, 30);

    nbrVagues = 12;
    for(i=0;i<nbrVagues;++i) vagues(&s,(rand()%(s.width-100)+80),(rand()%(197-122))+122,(rand()%20)+20,Blanc);        //Generation de vagues dans la mer

    filet_volley(&s, p1, p2, GrisClair, Blanc);


    /**-------------TP INTERSECTION-------------**/
    /*
    Point p1 = {-1, -1};
    Point p2 = {1, 1};
    Point p3 = {-1, 1};
    Point p4 = {1, -1};
    intersection(p1, p2, p3, p4);

    courb(&s, 0,0,10,50,40,80,0,100,Jaune);
    ligne(&s, 50,50, 100, 50,Rouge);
    rectangle(&s, 0, 0, 100, 20, Magenta);
    triangle(&s, Cyan);
    remplissageProp(&s, 0, 70, Bleu);
    */


    /**------------------Ecriture---------------------**/
    FILE *output = fopen("beachDrawing.ppm", "w");
    if (output == NULL)
    {
        printf("Erreur d'ouverture du fichier de sortie\n");
        return 1;
    }
    pgm_write(&s, output);
    fclose(output);
    // nettoyage
    free_surface(&s);
    return 0;
}
