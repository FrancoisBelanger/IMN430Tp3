#include "ConvexHull.h"
#include "Region.h"

#include <algorithm>

#include <iostream>
using std::cout;
using std::endl;
using std::cin;

#include <map>
using std::map;

#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
//#include <climits>

//using std::numeric_limits;


ConvexHull::ConvexHull()
{

}

ConvexHull::ConvexHull(const char* filename)
{
	if (!loadFile(filename))
		cout << "Unable to load file..." << endl;
}

ConvexHull::~ConvexHull(){}


bool ConvexHull::loadFile(const char* filename)
{
	bool success = false;

	if (filename)
	{
		pointList.clear();

		std::ifstream ifs(filename, std::ifstream::in);
		
		success = ifs.is_open(); 
		if (success)
		{
			int counter = 0; //limits the number of points considered
			while (ifs.good() && !ifs.eof() && (counter == -1 ||counter != NB_POINTS_LIMIT))
			{
				++counter;
				DCEL::Vertex pt;
				ifs >> pt;
				pointList.push_back(new DCEL::Vertex(pt));
			}

			ifs.close();
		}
	}

	return success;
}

bool ConvexHull::isCoplanar(const vect& ab, const vect& ac, const vect& ad){
	return abs(ab.cross(ac).dot(ad)) < std::numeric_limits<double>::epsilon();
}

void ConvexHull::testPointForFace(DCEL::Region* face, std::vector<DCEL::Vertex*>& pList, int r)
{
    for(int i = r; i < pList.size(); ++i)
    {
        if ( faceIsVisible(pList[i], face) )
        {
            Pconflit[face].push_back(pList[i]);
            Fconflit[pList[i]].push_back(face);
        }
    }
}

void ConvexHull::initializeConflictsGraph()
{
    for(auto fIter = Clist.begin(); fIter != Clist.end(); ++fIter)
    {
        testPointForFace(*fIter, pointList);
    }
}

//TO DO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void ConvexHull::fuseRegions(DCEL::Region* r1, DCEL::Region* r2)
{
    
}

//Works only if a pt is out of the hull
bool ConvexHull::faceIsVisible(DCEL::Vertex* pt, DCEL::Region* r)
{
	DCEL::Vertex p1 = *(r->getBound()->getOrigin());
	DCEL::Vertex p2 = *(r->getBound()->getEnd());
	DCEL::Vertex p3 = *(r->getBound()->getNext()->getEnd());

	vect pt2face(p1, *pt);

	vect v1(p1, p2);
	vect v2(p1, p3);
	vect rNormal(v1.cross(v2));
	
	//If the normal is in the opposite direction of pt2centroid; 
	if (pt2face.dot(rNormal) > 0)
	{
		return true;
	}
	return false;
}

void ConvexHull::setPrevNext(DCEL::HalfEdge* e1, DCEL::HalfEdge* e2, DCEL::HalfEdge* e3)
{
    e1->setNext(e2);
    e2->setNext(e3);
    e3->setNext(e1);
}
void ConvexHull::createEdge(DCEL::HalfEdge** e12, DCEL::HalfEdge** e21, DCEL::Vertex* p1, DCEL::Vertex* p2)
{
    *e12 = new DCEL::HalfEdge(p1);
    *e21 = new DCEL::HalfEdge(p2);
    
    (*e12)->setTwin(*e21);
    (*e21)->setTwin(*e12);
}


void ConvexHull::createFirstTetraedron(DCEL::Vertex* p1, DCEL::Vertex* p2, DCEL::Vertex* p3, DCEL::Vertex* p4)
{
    DCEL::Region* r1; //p1 p2 p3
    DCEL::Region* r2; //p1 p2 p4
    DCEL::Region* r3; //p2 p3 p4
    DCEL::Region* r4; //p3 p1 p4
    
    //edge
    DCEL::HalfEdge* e12;
    DCEL::HalfEdge* e23;
    DCEL::HalfEdge* e31;
    DCEL::HalfEdge* e24;
    DCEL::HalfEdge* e41;
    DCEL::HalfEdge* e34;
    //twin
    DCEL::HalfEdge* e21;
    DCEL::HalfEdge* e32;
    DCEL::HalfEdge* e13;
    DCEL::HalfEdge* e42;
    DCEL::HalfEdge* e14;
    DCEL::HalfEdge* e43;
    
    createEdge(&e12, &e21, p1, p2);
    createEdge(&e23, &e32, p2, p3);
    createEdge(&e31, &e13, p3, p1);
    createEdge(&e24, &e42, p2, p4);
    createEdge(&e14, &e41, p1, p4);
    createEdge(&e34, &e43, p3, p4);
    
    //face 1 (p1, p2, p3)
    vect v1(*p1, *p2);
    vect v2(*p1, *p3);
    vect v3(*p1, *p4);
    
    if(v1.cross(v2).dot(v3) < 0)
    {
        setPrevNext(e12, e23, e31);
        r1 = new DCEL::Region(e12);
        e12->setRegion(r1);
        e23->setRegion(r1);
        e31->setRegion(r1);
        
    }
    else
    {
        setPrevNext(e21, e13, e32);
        r1 = new DCEL::Region(e21);
        e21->setRegion(r1);
        e13->setRegion(r1);
        e32->setRegion(r1);
    }
    
    //face 2 (p1, p2, p4)
    v1 = vect(*p1, *p2);
    v2 = vect(*p1, *p4);
    v3 = vect(*p1, *p3);
    
    if(v1.cross(v2).dot(v3) < 0)
    {
        setPrevNext(e12, e24, e41);
        r2 = new DCEL::Region(e12);
        e12->setRegion(r2);
        e24->setRegion(r2);
        e41->setRegion(r2);
    }
    else
    {
        setPrevNext(e21, e14, e42);
        r2 = new DCEL::Region(e21);
        e21->setRegion(r2);
        e14->setRegion(r2);
        e42->setRegion(r2);
    }
    
    
    //face 3 (p2, p3, p4)
    v1 = vect(*p2, *p3);
    v2 = vect(*p2, *p4);
    v3 = vect(*p2, *p4);
    
    if(v1.cross(v2).dot(v3) < 0)
    {
        setPrevNext(e32, e24, e43);
        r3 = new DCEL::Region(e32);
        e32->setRegion(r3);
        e24->setRegion(r3);
        e43->setRegion(r3);
    }
    else
    {
        setPrevNext(e23, e34, e42);
        r3 = new DCEL::Region(e23);
        e23->setRegion(r3);
        e34->setRegion(r3);
        e42->setRegion(r3);
    }
    
    //face 4 (p3, p1, p4)
    v1 = vect(*p3, *p1);
    v2 = vect(*p3, *p4);
    v3 = vect(*p3, *p2);
    
    if(v1.cross(v2).dot(v3) < 0)
    {
        setPrevNext(e31, e14, e43);
        r4 = new DCEL::Region(e31);
        e31->setRegion(r4);
        e14->setRegion(r4);
        e43->setRegion(r4);

    }
    else
    {
        setPrevNext(e13, e34, e41);
        r4 = new DCEL::Region(e13);
        e13->setRegion(r4);
        e34->setRegion(r4);
        e41->setRegion(r4);
    }
    
    //Including the created regions in Clist
    Clist.push_back(r1);
    Clist.push_back(r2);
    Clist.push_back(r3);
    Clist.push_back(r4);
}

//The edge e must be an horizon edge.
//The point p is a point being added to the hull
//ep2begin is an edge already created by a previous face. When it's the first face of a polygon to be created,
//it's obvioulsy null.
// TO TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
DCEL::Region* ConvexHull::createAFace(DCEL::Vertex* p, DCEL::HalfEdge* e, DCEL::HalfEdge* ep2Begin, DCEL::HalfEdge* ep2End)
{
	DCEL::Region* regionF = new DCEL::Region(e);
	e->setRegion(regionF);
    
	DCEL::HalfEdge* eEnd2p = new DCEL::HalfEdge(e->getEnd());
	eEnd2p->setRegion(regionF);
    
	if (ep2End == nullptr)
        ep2End = new DCEL::HalfEdge(p);
    
	eEnd2p->setTwin(ep2End); //will be ep2begin of the next face of the polygon
    
	if (ep2Begin == nullptr)
        ep2Begin = new DCEL::HalfEdge(p);
	ep2Begin->setRegion(regionF);
    
	e->setNext(eEnd2p);
	eEnd2p->setNext(ep2Begin);
	ep2Begin->setNext(e);
    
	return regionF; 
}

void ConvexHull::findHorizon(DCEL::Vertex* p, std::vector<DCEL::Edge*>& hList)
{
//    DCEL::Edge* firstHorizon = nullptr;
//    std::list<DCEL::Region*> conflicts = Fconflit[p];
//    
//    for(auto face = conflicts.begin(); face != conflicts.end() && firstHorizon == nullptr; ++face)
//    {
//        DCEL::Edge* first = (*face)->getBound();
//        DCEL::Edge* bound = (*face)->getBound();
//        do
//        {
//            auto found = std::find(conflicts.begin(), conflicts.end(), bound->getTwin()->getRegion());
//            
//            if(found == conflicts.end())
//            {
//                firstHorizon = bound;
//                break;
//            }
//            
//            bound = bound->getNext();
//            
//        }while(first != bound);
//    }
//    
//    DCEL::Edge* nextHorn = firstHorizon;
//    
//    do
//    {
//        auto found = std::find(conflicts.begin(), conflicts.end(), nextHorn->getTwin()->getRegion());
//        
//        if(found == conflicts.end())
//        {
//             hList.push_back(nextHorn);
//            nextHorn = nextHorn->getNext();
//        }
//        else
//        {
//            nextHorn = nextHorn->getNext()->getTwin();
//        }
//        
//    } while(nextHorn != firstHorizon);
    std::vector<DCEL::Edge*> horizon;

	for (auto i = Fconflit[p].begin(); i != Fconflit[p].end(); ++i)
	{
        DCEL::HalfEdge* firstEdge = (*i)->getBound();
        DCEL::HalfEdge* treatenEdge = firstEdge->getNext();
        
		bool backOnFirstEdge = false;
		while (!backOnFirstEdge)
		{
			//If the treaten edge's twin points to an invisible face,
			//the treaten edge is on the horizon line
			if (std::find(Fconflit[p].begin(), Fconflit[p].end(), treatenEdge->getTwin()->getRegion()) == Fconflit[p].end())
				horizon.push_back(treatenEdge);
            
			//If the treaten edge is the first one of the face,
			//All the face's edges have been treated
			if (treatenEdge == firstEdge)
				backOnFirstEdge = true;
            
			treatenEdge = treatenEdge->getNext();
		}
	}
    
	//Organize horizon edges
//	std::vector<DCEL::Edge*> organizedHorizon;
//	organizedHorizon.push_back(horizon[0]);
    hList.push_back(horizon[0]);
    
	while (hList.size() != horizon.size())
	{
		int initialSize = hList.size();
		for (std::vector<DCEL::Edge*>::iterator it = horizon.begin(); it != horizon.end(); ++it)
		{
			if ((*it)->getOrigin() == hList.back()->getEnd())
			{
				hList.push_back(*it);
				break;
			}
		}
        
		//if the vector hasn't increased, there's a discontinuity in the horizon line founded
		//and everything is going to collapse, causing apocalypse in the algorithm :(
		assert(hList.size() != initialSize);
	}
    
//	return organizedHorizon;
}

void ConvexHull::computeConvexHull()
{
	if (pointList.size() >= 4)
	{
		//STEP 1
		//find 4 points not coplanar
		if (Clist.size() < 4)
		{
			auto aItt = pointList.begin();
			auto bItt = aItt+1;
			auto cItt = bItt+1;
			auto dItt = cItt+1;
            
			auto ab = vect(**aItt, **bItt);
			auto ac = vect(**aItt, **cItt);
            
			for (; dItt != pointList.end(); ++dItt)
            {
                if(!isCoplanar(ab, ac, vect(**aItt, **dItt)))
                    break;
            }
            
			if (dItt == pointList.end())
			{
				cout << "All points are coplanar. The convex hull can be computed with a 2D algorithm. Program is closing..." << endl;
				return;
			}
			else
			{
				//STEP 2
				//Compute the convexHull made by the 4 points( C <- CH(p1,p2,p3,p4))
				createFirstTetraedron(*aItt, *bItt, *cItt, *dItt);
                
				pointList.erase(dItt);
				pointList.erase(cItt);
				pointList.erase(bItt);
				pointList.erase(aItt);
			}
            
			//STEP 3
			//Shuffle rests of the points
			std::random_shuffle(pointList.begin(), pointList.end());
            
			//STEP 4
			//Initalize the conflits Graph<aNpoint, aCface>
			initializeConflictsGraph();
            
			//STEP 5
			for (int r = 0; r < pointList.size(); ++r)
			{
				if (Fconflit.find(pointList[r]) != Fconflit.end() && Fconflit[pointList[r]].size() > 0)
				{
					//7. Add pr to C (done by the following steps)
                    //8. Retirer les faces dans Fconflit(pr) de C;
                    for(auto i = Fconflit[pointList[r]].begin(); i != Fconflit[pointList[r]].end(); ++i)
                    {
                        Clist.remove(*i);
                    }
                    
					//9. Creer une liste ordonnee L formant l'horizon de pr
					std::vector<DCEL::Edge*> prHorizon;
                    findHorizon(pointList[r], prHorizon);
                   
					//10. Pour chaque arete e de L
                    DCEL::Edge* lastFaceSecondEdge = nullptr;
                    DCEL::Edge* sharedEdge = nullptr;
					for (int e = 0; e < prHorizon.size(); ++e)
					{
						//11. Relier e a pr en creant une face triangulaire f et l'ajouter a C
                        DCEL::Vertex* vertex = pointList[r];
//                        DCEL::Edge *e1 = nullptr, *t1 = nullptr;
//                        DCEL::Region* f = new DCEL::Region(prHorizon[e]);
//
//                        if(sharedEdge == nullptr)
//                        {
//                            createEdge(&e1, &t1, prHorizon[e]->getEnd(), vertex);
//                            sharedEdge = t1;
//                        }
//                        
//                        DCEL::Edge *e2 = nullptr, *t2 = nullptr;
//                        
//                        if(e==0)
//                        {
//                            createEdge(&e2, &t2, vertex, prHorizon[e]->getOrigin());
//                            setPrevNext(prHorizon[e], e1, e2);
//                            e1->setRegion(f);
//                            e2->setRegion(f);
//                        }
//                        else if(e < prHorizon.size()-1)
//                        {
//                            createEdge(&e2, &t2, prHorizon[e]->getEnd(), vertex);
//                            setPrevNext(prHorizon[e], e2, sharedEdge);
//                            sharedEdge->setRegion(f);
//                            e2->setRegion(f);
//                            sharedEdge = t2;
//                        }
//                        else
//                        {
//                            setPrevNext(prHorizon[e], lastFaceSecondEdge, sharedEdge);
//                            lastFaceSecondEdge->setRegion(f);
//                            
//                        }
//                        
//                        if(lastFaceSecondEdge == nullptr)
//                        {
//                            lastFaceSecondEdge = t2;
//                        }
//                        prHorizon[e]->setRegion(f);
//                        Clist.push_back(f);
                        
                        DCEL::Region* regionF = nullptr;
                        
                        if (e == 0)
                            regionF = createAFace(vertex, prHorizon[e]);
                        else if (e == prHorizon.size()-1)
                            regionF = createAFace(vertex, prHorizon[e], prHorizon[e - 1]->getNext()->getTwin(), prHorizon[0]->getPrev());
                        else
                            regionF = createAFace(vertex, prHorizon[e], prHorizon[e - 1]->getNext()->getTwin());
                        
                        prHorizon[e]->setRegion(regionF);
                        Clist.push_back(regionF);

                        //19. Suprimer le sommet associe ˆ pr de G
                        for(auto iterF = Fconflit[pointList[r]].begin(); iterF != Fconflit[pointList[r]].end(); ++iterF)
                        {
                            for(auto iterP = Pconflit[*iterF].begin() ; iterP != Pconflit[*iterF].end(); ++iterP)
                            {
                                if(*iterP != pointList[r])
                                {
                                    Fconflit[*iterP].remove(*iterF);
                                }
                            }
                            //delete (*iterF);
                            //*iterF = nullptr;
                        }
                        
                        for (auto i = Fconflit[pointList[r]].begin(); i != Fconflit[pointList[r]].end(); ++i)
                        {
                            delete (*i);
                            *i = nullptr;
                        }
                        Fconflit[pointList[r]].clear();
                        Fconflit.erase(pointList[r]);
                        
                        //12. si f est coplanaire avec la face voisine f' le long de e alors
//                        if(isCoplanar(vect(*prHorizon[e]->getOrigin(), *pointList[r]), vect(*prHorizon[e]->getOrigin(), *prHorizon[e]->getEnd()), vect(*prHorizon[e]->getOrigin(), *prHorizon[e]->getTwin()->getNext()->getOrigin())))
//                        {
//                            //TODO: fusionner le shit
//                        }
//                        else //14.
//                        {
//                            //TODO: optimize like the algo in the slides.
//                            testPointForFace(f, pointList, r+1);
                            testPointForFace(regionF, pointList, r+1);
//                        }
                        
                    }
//                        //11. Relier e a pr en creant une face triangulaire f et l'ajouter a C
//						DCEL::Region* regionF = nullptr;
//                        
//						/// TO TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//						if (e == 0)
//							regionF = createAFace(pointList[r], prHorizon[e]);
//						else if (e == prHorizon.size()-1)
//							regionF = createAFace(pointList[r], prHorizon[e], prHorizon[e - 1]->getNext()->getTwin()), prHorizon[0]->getPrev();
//						else
//							regionF = createAFace(pointList[r], prHorizon[e], prHorizon[e - 1]->getNext()->getTwin());
//                        
//						Clist.push_back(regionF);
//                        
//						//12. si f est coplanaire avec la face voisine f^t le long de e,
//                        
//						vect vf1(*regionF->getBound()->getOrigin(), *regionF->getBound()->getEnd());
//						vect vf2(*regionF->getBound()->getEnd(), *regionF->getBound()->getNext()->getEnd());
//						vect normalf(vf1.cross(vf2));
//                        
//						DCEL::Region* regionFT = prHorizon[e]->getTwin()->getRegion();
//						vect vft1(*regionFT->getBound()->getOrigin(), *regionFT->getBound()->getEnd());
//						vect vft2(*regionFT->getBound()->getEnd(), *regionFT->getBound()->getNext()->getEnd());
//						vect normalft(vft1.cross(vft2));
//                        
//						if (normalf == normalft)
//						{
//							//13.combiner f et f^t en une seule face dont la liste des conflits est la meme que pour f^t
//							fuseRegions(regionF, regionFT);
//						}
//						else
//						{
//							//TO COMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//							//15. creer un sommet pour f dans G
//							//16. P(e) <- Pconflit(f1) U Pconflit(f2) ou f1 et f2 sont les 2 faces incidentes a e dans CH(P_r-1)
//							std::vector<DCEL::Vertex*> P;
//                        
//							//17.
//							for (int i = 0; i < P.size(); ++i)
//							{
//								//18. si f est visible depuis p, ajouter (p,f) dans G
//								if (faceIsVisible(P[i], regionF))
//									Fconflit[P[i]].push_back(regionF);
//							}
//						}
//					}
//
//					//8. retirer les faces dans Fconflit(pr) de C
//					for (int i = Fconflit[&pointList[r]].size() - 1; i >= 0; --i)
//					{
//						delete Fconflit[&pointList[r]][i];
//						Fconflit[&pointList[r]][i] = nullptr;
//					}
//					Fconflit[&pointList[r]].clear();
//                    
//					//19. Suprimer le sommet associe ˆ pr de G
//                    for(auto iter = Fconflit[pointList[r]].begin(); iter != Fconflit[pointList[r]].end(); ++iter)
//                    {
//                        Pconflit[*iter].remove(pointList[r]);
//                    }
//					Fconflit.erase(pointList[r]);
//                    
//
//					//TO COMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					//20. Suprimer les sommets correspondants aux faces dans Fconflit(pr) de G ainsi que leurs aretes incidentes
				}
			}
		}
	}
//	else
//		cout << "Not enough points to create a convex hull, program is closing..." << endl;
}

void ConvexHull::display()
{
	//TODO : Using OPENGL and following the created DCEL...
    
}