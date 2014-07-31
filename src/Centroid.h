#ifndef CENTROID_H
#define CENTROID_H

#include "Vertex.h"
#include <vector>

class Centroid
{
	DCEL::Vertex point; 
	int weight; 

public:
	Centroid(std::vector<DCEL::Vertex>* list = nullptr) : weight(0)
	{
		if (list)
		{
			for (int i = 0; i < list->size(); ++i)
			{
				++weight;
				point.x += list->operator[](i).x / list->size();
				point.y += list->operator[](i).y / list->size();
				point.z += list->operator[](i).z / list->size();
			}
		}
	}

	void update(const DCEL::Vertex& pt)
	{
		point.x *= weight;
		point.y *= weight;
		point.z *= weight;
		++weight;
		point.x = (point.x + pt.x) / weight;
		point.y = (point.y + pt.y) / weight;
		point.z = (point.z + pt.z) / weight;
	}

	const DCEL::Vertex& getPoint() const
	{
		return point; 
	}
};

#endif //CENTROID_H