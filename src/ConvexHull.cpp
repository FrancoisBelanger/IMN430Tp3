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

		while (ifs.good() && !ifs.eof())
		{
			DCEL::Vertex pt;
			ifs >> pt;
			pointList.push_back(pt);
		}

		ifs.close();

		success = true;
	}

	return success;
}

bool ConvexHull::isCoplanar(const vect& ab, const vect& ac, const vect& ad){
	return abs(ab.cross(ac).dot(ad)) > 0;
}

void ConvexHull::initializeConflictsGraph()
{
	//TODO... 
}

bool ConvexHull::faceIsVisible(DCEL::Vertex*, DCEL::Region*)
{
	//TODO...
	return false;
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
	return &DCEL::Region(); 
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
			auto bItt = ++aItt;
			auto cItt = ++bItt;
			auto dItt = ++cItt;

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
				Clist.push_back(createAFace(&*aItt, &*bItt, &*cItt));
				Clist.push_back(createAFace(&*aItt, &*bItt, &*dItt));
				Clist.push_back(createAFace(&*aItt, &*cItt, &*dItt));
				Clist.push_back(createAFace(&*bItt, &*cItt, &*dItt));

				//TO FIX .... !!!!!!!
				//pointList.erase(aItt);
				//pointList.erase(bItt);
				//pointList.erase(cItt);
				//pointList.erase(dItt);
			}

			//STEP 3
			//Shuffle rests of the points
			//TO put back as "std::random_shuffle(pointList.begin(), pointList.end()); once the fix at step 2 will be done !!!
			std::random_shuffle(pointList.begin()+4, pointList.end());

			//STEP 4
			//Initalize the conflits Graph<aNpoint, aCface>
			initializeConflictsGraph();

			//STEP 5
			//IF THE POINTS GET ERASED, R MUST BE STARTED AT 0 !!!!!!!!!!
			for (int r = 4; r < pointList.size(); ++r)
			{
				if (Fconflit[&pointList[r]].size() > 0)
				{
					//7. Add pr to C??? C contains faces...
					//8. retirer les faces dans Fconflit(pr) de C

					//9. Creer une liste ordonnee L formant l'horizon de pr
					std::vector<DCEL::Edge*> prHorizon = findHorizon(&pointList[r]);
					
					//10. Pour chaque arete e de L 
					for (int i = 0; i < prHorizon.size(); ++i)
					{
						//11. Relier e a pr en creant une face triangulaire f et l'ajouter a C
						DCEL::Region* regionF = createAFace(prHorizon[i]->getOrigin(), prHorizon[i]->getEnd(), &pointList[r]);
						Clist.push_back(regionF);

						//12. si f est complanaire avec la face voisine f^t le long de e,
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

					//19. Suprimer le sommet associe à pr de G //Fconflit.erase(&pointList[r]);
					//20. Suprimer les sommets correspondants aux faces dans Fconflit(pr) de G ainsi que leurs aretes incidentes
				}
			}
		}
	}
	else
		cout << "Not enough points to create a convex hull, program is closing..." << endl;
}
