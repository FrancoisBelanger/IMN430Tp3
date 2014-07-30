//
//  main.cpp
//  imn430-tp3
//
//  Created by Steven Aubertin on 2014-07-16.
//

#include "ConvexHull.h"


int main(int argc, const char * argv[]){
    
	ConvexHull convexHull("ananas.txt");
	convexHull.computeConvexHull(); 
    
    return 0;
}

