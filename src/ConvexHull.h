#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H

#include <vector>
#include <list>

#include "Vertex.h"
#include "vect.h"
#include <map>

class ConvexHull
{
	//Members
	std::vector<DCEL::Vertex*> pointList;
	std::vector<DCEL::Vertex> convexHullPoints;
	std::list<DCEL::Region*> Clist;

    static const int NB_POINTS_LIMIT = 2000;

    //OPTIMISATION: voir pour utiliser un set au lieu des list/vector
	std::map<DCEL::Vertex*, std::list<DCEL::Region*> > Fconflit;
	std::map<DCEL::Region*, std::list<DCEL::Vertex*> > Pconflit;

	//Init-Func
	void initializeConflictsGraph(); 
	void createFirstTetraedron(DCEL::Vertex* p1, DCEL::Vertex* p2, DCEL::Vertex* p3, DCEL::Vertex* p4);

	//Tool-Func
    void testPointForFace(DCEL::Region* face, std::vector<DCEL::Vertex*>& List);
	bool faceIsVisible(DCEL::Vertex*, DCEL::Region*);
    DCEL::Region* createAFace(DCEL::Vertex* p, DCEL::HalfEdge* e, DCEL::HalfEdge* ep2begin = nullptr, DCEL::HalfEdge* ep2end = nullptr);
	void findHorizon(DCEL::Vertex* p, std::vector<DCEL::Edge*>& hList);
	bool isCoplanar(const vect& ab, const vect& ac, const vect& ad);
    void fuseRegions(DCEL::Region* r1, DCEL::Region* r2);
    void setPrevNext(DCEL::HalfEdge* e1, DCEL::HalfEdge* e2, DCEL::HalfEdge* e3);
    void createEdge(DCEL::HalfEdge** e12, DCEL::HalfEdge** e21, DCEL::Vertex* p1, DCEL::Vertex* p2);



public :
	ConvexHull();
	ConvexHull(const char* filename); 
	~ConvexHull(); 

	bool loadFile(const char* filename); 
	void computeConvexHull();
	void display();
    std::vector<DCEL::Vertex*>::const_iterator getPointListIter() {return pointList.cbegin();}
    std::vector<DCEL::Vertex*>::const_iterator getPointListEnd() {return pointList.cend();}
    std::list<DCEL::Region*>::iterator getFaceListIter() {return Clist.begin();}
    std::list<DCEL::Region*>::iterator getFaceListEnd() {return Clist.end();}
};

#endif