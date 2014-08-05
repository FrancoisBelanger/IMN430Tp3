//
//  main.cpp
//  imn430-tp3
//
//  Created by Steven Aubertin on 2014-07-16.
//

//#ifdef _WIN32
//#include <windows.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include "Utilities/GLUT.h"
//#endif

//#ifdef __APPLE__
#include <GLUT/glut.h>
//#endif

#include <iostream>

#include "Region.h"
#include "Edge.h"
#include "Vertex.h"
#include "ConvexHull.h"
#include "Camera.h"


using namespace std;
GLfloat faceColor[18] = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
    1, 1, 0,
    1, 0, 1,
    0, 1, 1
};

//Camera Variables
Camera gTheCam(0,0,20);

ConvexHull convexHull("ananas.txt");


//int main(int argc, char** argv)
//{
//    ConvexHull convexHull("ananas.txt");
//    convexHull.computeConvexHull();
//
//    glutInit(&argc, argv);
//
//    glutInitWindowSize(800, 600);
//    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
//    glutCreateWindow("IMN-430 tp 3");
//
//    glutDisplayFunc(display);
//    glutReshapeFunc(reshape);
//
//    glClearColor(0,0,0,1);
//
//    glutMainLoop();
//
//    return 0;
//}

//#define GLUT_DISABLE_ATEXIT_HACK
//
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//
//#ifdef __APPLE__
//#include <GLUT/glut.h>
//#else
//#include <windows.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include "Glut/GLUT.h"
//#endif


/* Variables globales pour contenir la dimension de la fenetre */
GLint windW, windH;

/* Variable globale indiquant si les axes doivent suivre le cube ou non */
GLboolean moveAxis = GL_FALSE;

/* Modes de transformations (commandes dans le menu) */
enum
{
    None,
    Cube,
    Cone,
    ModeTranslateX,
    ModeTranslateY,
    ModeTranslateZ,
    ModeRotateX,
    ModeRotateY,
    ModeRotateZ,
    ModeScaleX,
    ModeScaleY,
    ModeScaleZ,
    ModeScaleU,
    ModeShearX,
    ModeShearY,
    ModeShearZ,
    ActionReset,
    ActionQuit
};
int CurrentMode = None;    /* translation, rotation, changement d'echelle, ...*/
int SelectedObject = Cube; /* Cube ou Cone */


/* Etat de la souris */
int LeftButtonIsPressed = GL_FALSE;     /* Bouton gauche enfonce? */
int LastPosX, LastPosY;                 /* Derniere position de souris (en coordonnees pixel) */

GLfloat cubeModelviewMatrix[16];        /* Matrice de transformation du cube */
GLfloat coneModelviewMatrix[16];        /* Matrice de transformation du cone */

/*
 Fonction permettant d'initialiser la matrice de transformation
 du cone et du cube
 */
void initTransformations(void)
{
    //    glMatrixMode(GL_MODELVIEW);
    //    glLoadIdentity();
    //    glTranslated(0,0,-80);  /* les objets sont initialement centres en (0,0,-80) */
    //    glGetFloatv(GL_MODELVIEW_MATRIX,cubeModelviewMatrix);
    //    glGetFloatv(GL_MODELVIEW_MATRIX,coneModelviewMatrix);
}

/*
 Fonction permettant de dessiner un cube centre en (0,0,0)
 */
void DrawCube()
{
    const int M = -10;
    const int P =  10;
    
    /* Contour du cube (avant, haut, arriere, bas) */
    glBegin( GL_TRIANGLE_STRIP );
    {
        glColor3f(1,0,0);
        glVertex3f(M,M,P);
        glVertex3f(P,M,P);
        
        glColor3f(1,1,0);
        glVertex3f(M,P,P);
        glVertex3f(P,P,P);
        
        glColor3f(0,1,0);
        glVertex3f(M,P,M);
        glVertex3f(P,P,M);
        
        glColor3f(0,0,1);
        glVertex3f(M,M,M);
        glVertex3f(P,M,M);
        
        glColor3f(1,0,0);
        glVertex3f(M,M,P);
        glVertex3f(P,M,P);
    }
    glEnd();
    
    /* Face gauche */
    glBegin( GL_TRIANGLE_FAN );
    {
        glColor3f(0,1,1);
        glVertex3f(M,M,P);
        glVertex3f(M,P,P);
        
        glColor3f(1,0,1);
        glVertex3f(M,P,M);
        glVertex3f(M,M,M);
    }
    glEnd();
    
    /* Face Droite */
    glBegin( GL_TRIANGLE_FAN );
    {
        glColor3f(0,1,1);
        glVertex3f(P,M,P);
        glVertex3f(P,P,P);
        
        glColor3f(1,0,1);
        glVertex3f(P,P,M);
        glVertex3f(P,M,M);
    }
    glEnd();
}

void DrawConvexHull()
{
    auto f = convexHull.getFaceListIter();

    int cpt = 0;
    glBegin(GL_TRIANGLES);
    for(; f != convexHull.getFaceListEnd(); ++f)
    {
        
        glColor3f(faceColor[cpt],faceColor[cpt+1],faceColor[cpt+2]);
//        glColor3f(rand()/(float)RAND_MAX, rand()/(float)RAND_MAX, rand()/(float)RAND_MAX);
        
        DCEL::HalfEdge* edge = (*f)->getBound();
        glVertex3f(edge->getOrigin()->x, edge->getOrigin()->y, edge->getOrigin()->z);
        edge = edge->getNext();
        glVertex3f(edge->getOrigin()->x, edge->getOrigin()->y, edge->getOrigin()->z);
        edge = edge->getNext();
        glVertex3f(edge->getOrigin()->x, edge->getOrigin()->y, edge->getOrigin()->z);
        
        cpt = (cpt+3)%18;
    }
    glEnd();
}

void DrawPoints()
{
    glColor3f(1, 0, 0);
    
    glBegin(GL_POINTS);
    for(auto p = convexHull.getPointListIter();p != convexHull.getPointListEnd();++p)
    {
        glVertex3f((*p)->x, (*p)->y, (*p)->z);
    }
    glEnd();
}

/*
 Fonction permettant de dessiner les axes du cube
 */
void DrawAxis()
{
    
    /* AJOUTER DU CODE ICI!
     
     (Il faut dessiner la pointe des axes avec des
     tetrahedres. Ne pas oublier de bien specifier les couleurs,
     les translations et les rotations. Le tetrahedre
     original pointe dans la direction des X positifs.) */
    
}


/*
 Fonction de selection d'un item dans le menu (standard glut)
 */
void MenuSelection(int value)
{
    
    switch(value){
            
        case Cube:
            /* AJOUTER DU CODE ICI!
             l'object selectionne est le cube
             */
            break;
            
        case Cone:
            /* AJOUTER DU CODE ICI!
             l'object selectionne est le cube
             */
            break;
            
        case ActionReset:
            
            /* AJOUTER DU CODE ICI!
             
             Il faut ici re-initialiser les coordonnees des
             transformations affines. */
            break;
        case ActionQuit:
            exit(0);
            break;
    }
    
    CurrentMode = value;
}



/*
 Fonction de redimensionnement de la fenetre (standard glut)
 */
void Reshape(int width, int height)
{
    windW = (GLint)width;
    windH = (GLint)height;
}


/*
 Fonction de rafraichissement de la fenetre (standard glut)
 */
void Display(void)
{
    gTheCam.Refresh();
    /* Configurer le "viewport" OpenGL.
     Les dimensions de la fenetre sont contenues dans windW et windH */
    glViewport(0, 0, windW, windH);
    
    /* Configurer une camera tres simple (projection perspective) */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //    gluPerspective(60, (GLdouble)windW / (GLdouble)windH, -1, 300);
    glOrtho(-200, 200, -200, 200, -3000, 3000);
    
    /* AJOUTER DU CODE ICI!
     Il faut ici afficher le cube, le cone et les axes avec LES BONNES
     TRANSFORMATIONS AFFINES.  */
    
    /* Selectionner la pile de transformations attachee au modele (MODELVIEW) */
    glMatrixMode(GL_MODELVIEW);
    /* Effacer l'ecran */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /* Afficher le cube */
	//DrawPoints();
    DrawConvexHull();
    
    /* Afficher le cone de couleur blanche */
    
    /* Afficher les axes du systeme centres sur l'objet selectionne */
    glFinish();
    glutSwapBuffers();
}

void Idle()
{
    glutPostRedisplay();
}

/*
 Fonction appelee lorsqu'un bouton de la souris est presse (standard glut)
 */
void MouseButton(int aButton, int aState, int aX, int aY)
{
    Camera::MouseEventArgs Args;
    
    switch(aButton)
    {
        case GLUT_LEFT_BUTTON :     Args.mButton = Camera::Mouse_Left;  break;
        case GLUT_RIGHT_BUTTON :    Args.mButton = Camera::Mouse_Right; break;
        case GLUT_MIDDLE_BUTTON :   Args.mButton = Camera::Mouse_Middle;break;
        default :                   Args.mButton = Camera::Mouse_None;  break;
    }
    
    switch(aState)
    {
        case GLUT_UP :      Args.mState = Camera::State_Up;     break;
        case GLUT_DOWN :    Args.mState = Camera::State_Down;   break;
        default :           Args.mState = Camera::State_None;   break;
            
    }
    
    Args.mCoordX = aX;
    Args.mCoordY = aY;
    
    gTheCam.UpdateControls(Args);
    
}

void MouseMove(int aX, int aY)
{
    Camera::MouseEventArgs Args;
    
    Args.mCoordX = aX;
    Args.mCoordY = aY;
    
    Args.mButton = Camera::Mouse_None;
    Args.mState = Camera::State_None;
    
    gTheCam.UpdateControls(Args);
}

/*
 Fonction appelee a chaque fois qu'une touche du clavier est enfoncee
 */
void keyboard( unsigned char key, int x, int y )
{
    //FIXME: inutile
    //GLfloat buf[16];
    
    switch(key)
    {
        case 'p':/* Afficher la matrice de transformation de l'objet selectionne. */
        case 'P':
            if(SelectedObject == Cube)
            {
                printf("%f %f %f %f\n",cubeModelviewMatrix[0],cubeModelviewMatrix[4],cubeModelviewMatrix[8],cubeModelviewMatrix[12]);
                printf("%f %f %f %f\n",cubeModelviewMatrix[1],cubeModelviewMatrix[5],cubeModelviewMatrix[9],cubeModelviewMatrix[13]);
                printf("%f %f %f %f\n",cubeModelviewMatrix[2],cubeModelviewMatrix[6],cubeModelviewMatrix[10],cubeModelviewMatrix[14]);
                printf("%f %f %f %f\n",cubeModelviewMatrix[3],cubeModelviewMatrix[7],cubeModelviewMatrix[11],cubeModelviewMatrix[15]);
            }
            else
            {
                printf("%f %f %f %f\n",coneModelviewMatrix[0],coneModelviewMatrix[4],coneModelviewMatrix[8],coneModelviewMatrix[12]);
                printf("%f %f %f %f\n",coneModelviewMatrix[1],coneModelviewMatrix[5],coneModelviewMatrix[9],coneModelviewMatrix[13]);
                printf("%f %f %f %f\n",coneModelviewMatrix[2],coneModelviewMatrix[6],coneModelviewMatrix[10],coneModelviewMatrix[14]);
                printf("%f %f %f %f\n",coneModelviewMatrix[3],coneModelviewMatrix[7],coneModelviewMatrix[11],coneModelviewMatrix[15]);
            }
            break;
            
        case 'q':/* quit*/
        case 'Q':
            exit(0);
            break;
    }
    
    glutPostRedisplay(); /* appel la fonction 'display'*/
}

int main(int argc, char **argv)
{
    srand (1);
    convexHull.computeConvexHull();
    
    glutInit(&argc, argv);
	
	/* Variable locale pour la configuration de la fenetre graphique OpenGL */
    int MenuId;
    
    /* DEBUT: Initialisation standard de la fenetre graphique OpenGL */
    windW = 800;
    windH = 600;
    glutInitWindowPosition(0, 0);
    glutInitWindowSize( windW, windH);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    if (glutCreateWindow("IMN428") == GL_FALSE)
        exit(1);
    
    /* Configuration de la couleur de fond (noir) */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    /* Activer le Z-buffer */
    glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );
    
    /*  Initialiser a zero les transformations de base */
    initTransformations();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /* Creation d'un menu GLUT */
    MenuId = glutCreateMenu(MenuSelection);
    glutSetMenu( MenuId );
    glutAddMenuEntry( "Move Cube", Cube );
    glutAddMenuEntry( "Move Cone", Cone );
    glutAddMenuEntry( "Translation X", ModeTranslateX );
    glutAddMenuEntry( "Translation Y", ModeTranslateY );
    glutAddMenuEntry( "Translation Z", ModeTranslateZ );
    glutAddMenuEntry( "Rotation X", ModeRotateX );
    glutAddMenuEntry( "Rotation Y", ModeRotateY );
    glutAddMenuEntry( "Rotation Z", ModeRotateZ );
    glutAddMenuEntry( "Changement d'echelle X", ModeScaleX );
    glutAddMenuEntry( "Changement d'echelle Y", ModeScaleY );
    glutAddMenuEntry( "Changement d'echelle Z", ModeScaleZ );
    glutAddMenuEntry( "Changement d'echelle uniforme", ModeScaleU );
    glutAddMenuEntry( "Cisaillement d'echelle X", ModeShearX );
    glutAddMenuEntry( "Cisaillement d'echelle Y", ModeShearY  );
    glutAddMenuEntry( "Cisaillement d'echelle Z", ModeShearZ  );
    glutAddMenuEntry( "Retour a l'origine", ActionReset );
    glutAddMenuEntry( "Quitter", ActionQuit );
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    /* Fonctions de rappel (callback) pour glut */
    glutMouseFunc(MouseButton);     /* Fonctions appelee lorsqu'un bouton de la souris est enfonce */
    glutMotionFunc(MouseMove);      /* Fonctions appelee lorsque la souris bouge et qu'un de ses boutons est enfonce */
    glutReshapeFunc(Reshape);       /* Fonctions appelee lorsque la taille de la fenetre est modifiee */
    glutDisplayFunc(Display);       /* Fonctions appelee lorsque la scene doit etre affichee. */
    glutIdleFunc(Idle);
    glutKeyboardFunc( keyboard );   /* Fonctions appelee lorsqu'une touche du clavier est enfoncee. */
    glutMainLoop();
    
    return( 0 );
}