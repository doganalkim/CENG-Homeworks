#include "BinaryHeap.h"

BinaryHeap::BinaryHeap()
{
    // TODO: or not
    
    struct HeapElement newItem;
    elements.push_back(newItem);
    
}


bool BinaryHeap::Add(int uniqueId, double weight)
{
    // TODO:
    int hole = (int)elements.size();
    for(int i = hole-1 ; i > 0 ; i--){
        if(elements[i].uniqueId == uniqueId )
            return false;
    }
    struct HeapElement newItem;
    newItem.uniqueId = uniqueId;
    newItem.weight = weight;
    elements.push_back(newItem);
    elements[0] = newItem;
    for(; weight < elements[hole/2].weight ; hole/=2){
        elements[hole]=elements[hole/2];

    }
    elements[hole] = newItem;
    return true;
    
}

bool BinaryHeap::PopHeap(int& outUniqueId, double& outWeight)
{
    // TODO:
   
    int size=elements.size()-1;
    if(size>0){
        outUniqueId = elements[1].uniqueId;
        outWeight = elements[1].weight;
        elements[1]=elements[size];
        elements.pop_back();
        int hole=1;
        int child=hole*2;
        struct HeapElement temp = elements[hole];
        for(; hole*2 <= size-1 ; hole = child){
            child=hole*2;
            
            if(child != size-1 && elements[ child + 1 ].weight < elements[child].weight)
                child++;
            
            if(elements[child].weight < temp.weight)
                elements[hole] = elements[child];
            else
                break;
            
        }
        elements[hole] = temp;
        return true;
    }
    return false;
}

bool BinaryHeap::ChangePriority(int uniqueId, double newWeight)
{
    // TODO:
    bool result=false;
    int size=(int)elements.size()-1;
    for(int i=size; i>0 ; i--){
        if(elements[i].uniqueId == uniqueId){
            elements[i].weight = newWeight;
            result = true;
        }
    }
    
    if(!result) 
        return result;
    
    int hole;
    for(int j= size/2 ; j>0 ; j--){
        hole=j;
        int child=hole*2;
        struct HeapElement temp = elements[hole];
        for(; hole*2 <= size ; hole = child){
            child=hole*2;
            
            if(child != size && elements[ child + 1 ].weight < elements[child].weight)
                child++;
            
            if(elements[child].weight < temp.weight)
                elements[hole] = elements[child];
            else
                break;
            
        }
        elements[hole] = temp;
    }
    return result;
    
}

int BinaryHeap::HeapSize() const
{
    // TODO:
    return (int)elements.size()-1;
}