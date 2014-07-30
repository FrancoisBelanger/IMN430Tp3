//
//  vect.h
//  imn430-tp3
//
//  Created by Steven Aubertin on 2014-07-16.
//

#ifndef imn430_tp3_vect_h
#define imn430_tp3_vect_h

#include <iostream>
using std::istream;
using std::ostream;

#include "Vertex.h"

//template<class T>
class vect : public DCEL::Vertex/*<T>*/{
public:
    //---- Ctor
    vect(const int& x, const int& y, const int& z)
        : Vertex(x, y, z){
    }
    vect(const Vertex& a, const Vertex& b)
        : Vertex(b.x - a.x, b.y - a.y, b.z - a.z){
    }

    //---- Public methods
    vect cross(const vect& b)const{
        return vect(
            (this->y * b.z) - (this->z * b.y),
            (this->x * b.z) - (this->z * b.x),
            (this->x * b.y) - (this->y * b.x)
        );
    }
    
    int dot(const vect& v)const{
        return (this->x * v.x + this->y * v.y + this->z * v.z);
    }
};

#endif
