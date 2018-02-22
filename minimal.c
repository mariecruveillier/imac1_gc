#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>

/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 400;
static unsigned int WINDOW_HEIGHT = 400;

/* Nombre de bits par pixel de la fenêtre */
static const unsigned int BIT_PER_PIXEL = 32;

/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

typedef struct Point{
    float x, y;// Position 2D du point
    unsigned char r,g,b; // Couleur du point
    struct Point* next;
} Point, *PointList;

typedef struct Primitive{
	GLenum primitiveType;
	PointList points;
	struct Primitive* next;
}Primitive, *PrimitiveList;


Point* allocPoint(float x, float y, unsigned char r, unsigned char g, unsigned char b){
   Point* tmp = (Point*) malloc (sizeof(Point));
    if(tmp!=NULL){
        tmp->x=x;
        tmp->y=y;
        tmp->r = r;
        tmp->g = g;
        tmp->b = b;
        tmp->next = NULL;
    }
    return tmp;
}

void addPointToList(Point* point, PointList* list){
   /*allocPoint(point->x, point->y, point->r, point->g, point->b);*/

    if(*list == NULL){
        *list = point;
    }else {
	addPointToList(point,&(*list)->next);
    }
}

void drawPoints(PointList list){
	while(list->next != NULL){
			glColor3ub(list->r,list->g,list->b);
	                glVertex2f(list->x,list->y);
			list = list->next;
	}
}

void deletePoints(PointList* list){
	while(*list){
		PointList *tmp = &(*list)->next;		
		free(list);
		list = tmp;
	}		
}

void addPrimitive(Primitive* primitive, PrimitiveList* list){	
    	primitive->next = *list;
        *list = primitive;
}

Primitive* allocPrimitive(GLenum primitiveType){
	Primitive* tmp = (Primitive*) malloc(sizeof(primitiveType));
	if(tmp!=NULL){
		tmp->points = NULL;
		tmp->next = NULL;
		tmp->primitiveType = primitiveType;
		return tmp;
	}
	return 0;
}


void drawPrimitives(PrimitiveList list){
	while(list){
		glBegin(list->primitiveType);
                        glVertex2f(list->points->x, list->points->y);
               		drawPoints(list->points);
		glEnd();
		list = list->next;
	}
}

void deletePrimitive(PrimitiveList* list){
	while(*list){
		PrimitiveList* tmp= &(*list)->next;
		deletePoints(&(*list)->points);
		free(*list);
		*list = *tmp;
	}
}

void scene() {
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1., -1., -1., 1.);
    glColor3ub(255, 255, 255);
}

int main(int argc, char** argv) {

    /* Initialisation de la SDL */
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

    /* Ouverture d'une fenêtre et création d'un contexte OpenGL */
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_GL_DOUBLEBUFFER  |SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }

    /* Titre de la fenêtre */
    SDL_WM_SetCaption("Noowl powa :D", NULL);
    
    /* Placer ici le code de dessin */
    glClear(GL_COLOR_BUFFER_BIT);
    
    PrimitiveList premiere = NULL;
    addPrimitive(allocPrimitive(GL_POINTS), &premiere);

    /* Boucle d'affichage */
    int loop = 1;
    while(loop) {

        /* Récupération du temps au début de la boucle */
        Uint32 startTime = SDL_GetTicks();
        
        /* Echange du front et du back buffer : mise à jour de la fenêtre */
        SDL_GL_SwapBuffers();
        
        /* Boucle traitant les evenements */
        SDL_Event e;
        while(SDL_PollEvent(&e)) {

            /* L'utilisateur ferme la fenêtre : */
            if(e.type == SDL_QUIT) {
                loop = 0;
                break;
            }
            

            /* Quelques exemples de traitement d'evenements : */
            switch(e.type) {

                /* Clic souris */
                case SDL_MOUSEBUTTONUP:
                    printf("clic en (%d, %d)\n", e.button.x, e.button.y);
        			/*glClearColor((float)(e.button.x%WINDOW_WIDTH)/WINDOW_WIDTH, (float)(e.button.y%WINDOW_HEIGHT)/WINDOW_HEIGHT, 0, 1);*/
                    glBegin(GL_POINTS);
                        glVertex2f(-1 + 2. * e.button.x/ WINDOW_WIDTH, -(-1 +2. * e.button.y / WINDOW_HEIGHT));
                    glEnd();
                    break;

                /* Touche clavier */
                case SDL_KEYDOWN:
                    printf("touche pressée (code = %d)\n", e.key.keysym.sym);
                    if(e.key.keysym.sym == 113){
                    	loop = 0;
                    }
		    if(e.key.keysym.sym == 113)
		    	addPrimitive(allocPrimitive(GL_POINTS), &premiere);
			loop = 0;
                    }
		    if(e.key.keysym.sym == 108){
		    	addPrimitive(allocPrimitive(GL_LINES), &premiere);
			loop = 0;
                    }
		    if(e.key.keysym.sym == 116){
		    	addPrimitive(allocPrimitive(GL_TRIANGLES), &premiere);
			loop = 0;
                    }
                    break;
                case SDL_MOUSEMOTION:
                	/*glClearColor((float)(e.button.x%WINDOW_WIDTH)/WINDOW_WIDTH, (float)(e.button.y%WINDOW_HEIGHT)/WINDOW_HEIGHT, 0, 1);*/
                    break;

                case SDL_VIDEORESIZE:
                	WINDOW_WIDTH = e.resize.w;
                	WINDOW_HEIGHT = e.resize.h;
                	scene();
                	break;

                default:
                    break;
            }
        }

        /* Calcul du temps écoulé */
        Uint32 elapsedTime = SDL_GetTicks() - startTime;

        /* Si trop peu de temps s'est écoulé, on met en pause le programme */
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }
    DeletePrimitives(&primitives);
    /* Liberation des ressources associées à la SDL */ 
    SDL_Quit();

    return EXIT_SUCCESS;
}
