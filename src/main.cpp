//
//  main.cpp
//  imn430-tp3
//
//  Created by Steven Aubertin on 2014-07-16.
//

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Utilities/GLUT.h"
#endif

//#ifdef __APPLE__
#include <GLUT/glut.h>
//#endif

#include "ConvexHull.h"


int main(int argc, const char * argv[]){
    
	ConvexHull convexHull("ananas.txt");
	convexHull.computeConvexHull(); 


//    glutInit(&argc,argv);
//	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_ALPHA|GLUT_DEPTH);
//	glutInitWindowSize(gs32_WindowWidth,gs32_WindowHeight);
//	glutCreateWindow("Tp5");
//    
//	glutIdleFunc( IdleCallback );
//	glutDisplayFunc( IdleCallback );
//    
//	InitScene();
//	glutMainLoop();
//    
    return 0;
}

