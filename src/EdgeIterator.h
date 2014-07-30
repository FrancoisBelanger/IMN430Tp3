//
//  EdgeIterator.h
//  IMN430_tp1
//
//  Created by Steven Aubertin on 2014-05-13.
//

#ifndef IMN430_tp1_EdgeIterator_h
#define IMN430_tp1_EdgeIterator_h

#include "Edge.h"

namespace DCEL {
    /*
        Base Iterator Class for edges in a DCEL
     */
    class EdgeIterator{
    public:
        //---- Predicates
        inline bool hasNext()const{
            return next != nullptr;
        }
        inline bool isStart(const Edge* const edge)const{
            return edge == start;
        }
        
        //---- Virtual Methods
        inline virtual Edge* getNext() = 0;
        
        //---- Operators
        EdgeIterator& operator ++(){
            this->getNext();
            return *this;
        }
        const bool operator ==(const EdgeIterator& ie)const{
            return &ie == this;
        }
        bool operator ==(const EdgeIterator& ie){
            return &ie == this;
        }
        const bool operator !=(const EdgeIterator& ie)const{
            return !(operator==(ie));
        }
        bool operator !=(const EdgeIterator& ie){
            return !(operator==(ie));
        }
        Edge* operator*()const{
            return this->next;
        }
        
    protected:
        //---- Constructors
        /*
         Desactivate constructions for not childs
         */
        EdgeIterator(Edge* startEdge = nullptr)
            : start(startEdge), next(startEdge){
        }
        EdgeIterator(const EdgeIterator& ei)
            : start(ei.start), next(ei.next){
        }
        
        //---- Members
        Edge* start;
        Edge* next;
    };
}

#endif
