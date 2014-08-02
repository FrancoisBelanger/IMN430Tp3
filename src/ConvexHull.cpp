#include "ConvexHull.h"
#include "Region.h"

#include <algorithm>

#include <iostream>
using std::cout;
using std::endl;
using std::cin;

#include <map>
using std::map;

#include <fstream>
#include <string>
#include <sstream>
#include <cctype>


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
			while (ifs.good() && !ifs.eof() && counter != NB_POINTS_LIMIT)
			{
				++counter;
				DCEL::Vertex pt;
				ifs >> pt;
				pointList.push_back(pt);
			}

			ifs.close();
		}
	}

	return success;
}

bool ConvexHull::isCoplanar(const vect& ab, const vect& ac, const vect& ad){
	return abs(ab.cross(ac).dot(ad)) > std::numeric_limits<double>::epsilon();
}

//FIXME: a tester
void ConvexHull::initializeConflictsGraph()
{
    for(auto fIter = Clist.begin(); fIter != Clist.end(); ++fIter)
    {
        for(auto pIter = pointList.begin(); pIter != pointList.end(); ++pIter)
        {
            if ( faceIsVisible(&(*pIter), *fIter) )
            {
                Pconflit[*fIter].push_back(&(*pIter));
                Fconflit[&(*pIter)].push_back(*fIter);
            }
        }
    }
}


//Works only if a pt is out of the hull
bool ConvexHull::faceIsVisible(DCEL::Vertex* pt, DCEL::Region* r)
{
	DCEL::Vertex p1 = *(r->getBound()->getOrigin());
	DCEL::Vertex p2 = *(r->getBound()->getEnd());
	DCEL::Vertex p3 = *(r->getBound()->getNext()->getEnd());

	vect pt2face(*pt, p1);

	vect p12p2(p1, p2);
	vect p22p3(p2, p3);
	vect rNormal(p12p2.cross(p22p3));
	
	//If the normal is in the opposite direction of pt2centroid; 
	if (pt2face.dot(rNormal) < 0)
	{
		return true;
	}
	return false;
}

void ConvexHull::createFirstTetraedron(DCEL::Vertex* p1, DCEL::Vertex* p2, DCEL::Vertex* p3, DCEL::Vertex* p4)
{	
	//Creating region 1
	//Get a counter-clockwise orientation
	std::vector<DCEL::Vertex*> face1 = sortPointsCCw(p1, p2, p3);
	
	DCEL::HalfEdge* e12 = new DCEL::HalfEdge(face1[0]);
	DCEL::HalfEdge* e21 = new DCEL::HalfEdge(face1[1]);
	e12->setTwin(e21);

	DCEL::HalfEdge* e23 = new DCEL::HalfEdge(face1[1]);
	DCEL::HalfEdge* e32 = new DCEL::HalfEdge(face1[2]);
	e23->setTwin(e32);

	DCEL::HalfEdge* e31 = new DCEL::HalfEdge(face1[2]);
	DCEL::HalfEdge* e13 = new DCEL::HalfEdge(face1[0]);
	e31->setTwin(e13);

	e12->setNext(e23);
	e23->setNext(e31);
	e31->setNext(e12);

	DCEL::Region* r1 = new DCEL::Region(e12);

	e12->setRegion(r1);
	e23->setRegion(r1);
	e31->setRegion(r1);
	
	//Creating region 2
	//Because we use twin edges from the first region which is counter clockwise
	//we are sure reigion 2 will be oriented counter clockwise too. 

	DCEL::Region* r2 = new DCEL::Region(e21);

	DCEL::HalfEdge* e14 = new DCEL::HalfEdge(e21->getEnd());
	DCEL::HalfEdge* e41 = new DCEL::HalfEdge(p4);
	e14->setTwin(e41);

	DCEL::HalfEdge* e42 = new DCEL::HalfEdge(p4);
	DCEL::HalfEdge* e24 = new DCEL::HalfEdge(e21->getOrigin());
	e42->setTwin(e24);

	e21->setNext(e14);
	e14->setNext(e42);
	e42->setNext(e21);

	e21->setRegion(r2);
	e14->setRegion(r2);
	e42->setRegion(r2);

	//Creating region 3
	DCEL::Region* r3 = new DCEL::Region(e32);

	DCEL::HalfEdge* e43 = new DCEL::HalfEdge(p4);
	DCEL::HalfEdge* e34 = new DCEL::HalfEdge(e32->getOrigin());

	e32->setNext(e24);
	e24->setNext(e43);
	e43->setNext(e32);

	e32->setRegion(r3);
	e24->setRegion(r3);
	e43->setRegion(r3);

	//Creating region 4
	DCEL::Region* r4 = new DCEL::Region(e13);

	e13->setNext(e34);
	e34->setNext(e41);
	e41->setNext(e13);

	e13->setRegion(r4);
	e34->setRegion(r4);
	e41->setRegion(r4);

	//Including the created regions in Clist
	Clist.push_back(r1);
	Clist.push_back(r2);
	Clist.push_back(r3);
	Clist.push_back(r4);
}


//TO FINISH!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
DCEL::Region* ConvexHull::createAFace(DCEL::Vertex* p1, DCEL::Vertex* p2, DCEL::Vertex* p3)
{
	std::vector<DCEL::Vertex*> points;
	points.push_back(p1);
	points.push_back(p2);
	points.push_back(p3);

	for (int i = 0; i < points.size(); ++i)
	{
		if (points[i]->getEdge() == nullptr)
		{
			DCEL::HalfEdge* edge = new DCEL::HalfEdge(points[i]);
			points[i]->edge = edge; 
		}
	}

	//TO FINISH...
	return new DCEL::Region();
}


std::vector<DCEL::Vertex*> ConvexHull::sortPointsCCw(DCEL::Vertex* p1, DCEL::Vertex* p2, DCEL::Vertex* p3)
{ 
	std::vector<DCEL::Vertex*> sortedPoints; 

	sortedPoints.push_back(p1);
	sortedPoints.push_back(p2);
	sortedPoints.push_back(p3);

	//after only one iteration, the points will be sorted in x,
	//because there's only 3 points
	for (int i = 0; i < sortedPoints.size() -1; ++i)
	{
		if (sortedPoints[i]->x < sortedPoints[i + 1]->x) 
		{
			DCEL::Vertex* temp = sortedPoints[i];
			sortedPoints[i] = sortedPoints[i + 1];
			sortedPoints[i + 1] = temp; 
		}
	}

	//if the two last points are equal in x
	if (sortedPoints[1]->x == sortedPoints[2]->x &&
		sortedPoints[1]->y < sortedPoints[2]->y)
	{
		DCEL::Vertex* temp = sortedPoints[1];
		sortedPoints[1] = sortedPoints[2];
		sortedPoints[2] = temp;
	}

	//if the two first points are equal in x
	if (sortedPoints[0]->x == sortedPoints[1]->x &&
		sortedPoints[0]->y > sortedPoints[1]->y)
	{
		DCEL::Vertex* temp = sortedPoints[1];
		sortedPoints[1] = sortedPoints[2];
		sortedPoints[2] = temp;
	}

	return sortedPoints; 
}

std::vector<DCEL::Edge*> ConvexHull::findHorizon(DCEL::Vertex* p)
{
	//TO DO....
	return std::vector<DCEL::Edge*>(); 
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

			auto ab = vect(*aItt, *bItt);
			auto ac = vect(*aItt, *cItt);

			for (; dItt != pointList.end() && isCoplanar(ab, ac, vect(*aItt, *dItt)); ++dItt);
			if (dItt == pointList.end())
			{
				cout << "All points are coplanar. The convex hull can be computed with a 2D algorithm. Program is closing..." << endl;
				return;
			}
			else
			{
				//STEP 2
				//Compute the convexHull made by the 4 points( C <- CH(p1,p2,p3,p4))
				createFirstTetraedron(&*aItt, &*bItt, &*cItt, &*dItt);
				centroid.update(*aItt);
				centroid.update(*bItt);
				centroid.update(*cItt);
				centroid.update(*dItt);

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
				if (Fconflit[&pointList[r]].size() > 0)
				{
					//7. Add pr to C
					//8. retirer les faces dans Fconflit(pr) de C

					//9. Creer une liste ordonnee L formant l'horizon de pr
					std::vector<DCEL::Edge*> prHorizon = findHorizon(&pointList[r]);
					
					//10. Pour chaque arete e de L 
					for (int i = 0; i < prHorizon.size(); ++i)
					{
						//11. Relier e a pr en creant une face triangulaire f et l'ajouter a C
						DCEL::Region* regionF = createAFace(prHorizon[i]->getOrigin(), prHorizon[i]->getEnd(), &pointList[r]);
						Clist.push_back(regionF);

						//12. si f est coplanaire avec la face voisine f^t le long de e,
							//13.combiner f et f^t en une seule face dont la liste des conflits est la meme que pour f^t

						//14. sinon 
						//15. creer un sommet pour f dans G
						//16. P(e) <- Pconflit(f1) U Pconflit(f2) ou f1 et f2 sont les 2 faces incidentes a e dans CH(P_r-1)
						std::vector<DCEL::Vertex*> P;

						//17.
						for (int i = 0; i < P.size(); ++i)
						{
							//18. si f est visible depuis p, ajouter (p,f) dans G
							if (faceIsVisible(P[i], regionF))
								Fconflit[P[i]].push_back(regionF); 
						}

					}

					//19. Suprimer le sommet associe � pr de G //Fconflit.erase(&pointList[r]);
					//20. Suprimer les sommets correspondants aux faces dans Fconflit(pr) de G ainsi que leurs aretes incidentes
				}
			}
		}
	}
	else
		cout << "Not enough points to create a convex hull, program is closing..." << endl;
}

void ConvexHull::display()
{
	//TODO : Using OPENGL and following the created DCEL...
    
}