//
//  DCEL.h
//  IMN430-TP1
//
//  Created by Steven Aubertin on 2014-05-11.
//  Copyright (c) 2014 VISU_TEAM. All rights reserved.
//

#ifndef IMN430_TP1_DCEL_h
#define IMN430_TP1_DCEL_h

/*
    Master include for DCEL data structure
 */

#include "Edge.h"
#include "Region.h"
#include "Vertex.h"

/*
    Traversing the faces of a planar subdivision from all edges around a given vertex
 
    auto v1 = shared_ptr<Vertex>(new Vertex(10, 0));
    auto v2 = shared_ptr<Vertex>(new Vertex(0, 10));
    auto v3 = shared_ptr<Vertex>(new Vertex(10, 20));
 
    auto e11 = shared_ptr<HalfEdge>(new HalfEdge(v1.get()));
    auto e12 = shared_ptr<HalfEdge>(new HalfEdge(v2.get()));//twin
    v1->edge = e11.get();
    v2->edge = e12.get();
    e11->setTwin(e12.get());
 
    auto e21 = shared_ptr<HalfEdge>(new HalfEdge(v2.get()));
    auto e22 = shared_ptr<HalfEdge>(new HalfEdge(v3.get()));//twin
    v2->edge = e21.get();
    v3->edge = e22.get();
    e21->setTwin(e22.get());
 
    auto e31 = shared_ptr<HalfEdge>(new HalfEdge(v3.get()));
    auto e32 = shared_ptr<HalfEdge>(new HalfEdge(v1.get()));//twin
    v3->edge = e31.get();
    v1->edge = e32.get();
    e31->setTwin(e32.get());
 
    auto triangle = shared_ptr<Region>(new Region(e11.get()));
    e11->setNext(e21.get());
    e21->setNext(e31.get());
 
    int i = 0;
    for(auto edge = triangle->begin(); edge.hasNext(); ++edge){
        cout << "edge "<< i++ <<" " << (*edge)->getOrigin()<< ", " << (*edge)->getTwin()->getOrigin() << endl;
    }
    cout << endl;
 */

#endif



