#include "MeshGraph.h"
#include "BinaryHeap.h"

// For printing
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>

MeshGraph::MeshGraph(const std::vector<Double3>& vertexPositions,
                     const std::vector<IdPair>& edges)
{
    // TODO:
    for(int i = 0;  i< (int)vertexPositions.size();i++){
        Vertex ver;
        ver.id = i;
        ver.position3D = vertexPositions[i];
        vertices.push_back(ver);
    }
    
    for(int i = 0; i<TotalVertexCount();i++){
        std::list<Vertex*> temp;
        for(int j = 0 ; j< (int)edges.size();j++){
            if(edges[j].vertexId0 == vertices[i].id){
                Vertex *ptr;
                for(int k=0;k<TotalVertexCount();k++){
                    if(vertices[k].id==edges[j].vertexId1){
                        ptr=&vertices[k];
                    }
                }
                temp.push_back(ptr);
            }
            if(edges[j].vertexId1 == vertices[i].id){
                Vertex *ptr;
                for(int k=0;k<TotalVertexCount();k++){
                    if(vertices[k].id==edges[j].vertexId0){
                        ptr=&vertices[k];
                    }
                }
                temp.push_back(ptr);
            }
        }
        adjList.push_back(temp);
    }
}

double MeshGraph::AverageDistanceBetweenVertices() const
{
    // TODO:
    double sum=0;
    int counter=0;
    std::list<Vertex*>::const_iterator it; 
    for(int i = 0; i<(int)adjList.size(); i++){
        Vertex f =vertices[i] ;
        for(it=adjList[i].begin() ; it != adjList[i].end(); it++){
            sum = sum + f.position3D.Distance(f.position3D, (*it)->position3D);
            counter++;
        }
    }
    return sum/counter;
    
}

double MeshGraph::AverageEdgePerVertex() const
{
    // TODO:
    /*
    int counter=0;
    std::list<Vertex*>::const_iterator it; 
    for(int i = 0; i<(int)adjList.size(); i++){;
        for(it=adjList[i].begin() ; it != adjList[i].end(); it++){
            counter++;
        }
    }
    */
    return (double)TotalEdgeCount()/TotalVertexCount();
    
}

int MeshGraph::TotalVertexCount() const
{
    // TODO:
    return vertices.size();
}

int MeshGraph::TotalEdgeCount() const
{
    // TODO:
    int counter=0;
    std::list<Vertex*>::const_iterator it; 
    for(int i = 0; i<(int)adjList.size(); i++){;
        for(it=adjList[i].begin() ; it != adjList[i].end(); it++){
            counter++;
        }
    }
    return counter/2;
}

int MeshGraph::VertexEdgeCount(int vertexId) const
{
    // TODO:
    int counter=0;
    if(0>vertexId || vertexId >= TotalVertexCount())
        return -1;
    for(int i= 0 ; i< TotalVertexCount() ; i++){
        if(vertices[i].id == vertexId){
            std::list<Vertex*>::const_iterator it;
            for(it=adjList[i].begin() ; it != adjList[i].end();it++){
                counter++;
            }
        }
    }
    return counter;
}

void MeshGraph::ImmediateNeighbours(std::vector<int>& outVertexIds,
                                    int vertexId) const
{
    // TODO:
    outVertexIds.resize(0,-1);
    if(0<=vertexId && vertexId<TotalVertexCount()){
        std::list<Vertex*>::const_iterator it;
        for(it=adjList[vertexId].begin(); it!=adjList[vertexId].end(); it++){
            outVertexIds.push_back((*it)->id);
        }
    }
}

void MeshGraph::PaintInBetweenVertex(std::vector<Color>& outputColorAllVertex,
                                     int vertexIdFrom, int vertexIdTo,
                                     const Color& color) const
{
    // TODO:
    struct Color black;
    black.r=0;
    black.g=0;
    black.b=0;
    if(vertexIdFrom<0 || vertexIdFrom >= TotalVertexCount() || vertexIdTo<0 || vertexIdTo >= TotalVertexCount()){
        outputColorAllVertex.resize(0,black);
        return;
    }
    outputColorAllVertex.resize(vertices.size(),black);
    BinaryHeap pq;
    int var = TotalVertexCount();
    double *distance = new double [var];
    std::vector<int> previous;
    for(int i=0; i<TotalVertexCount(); i++){
        distance[i]=pow(10,300);
        previous.push_back(-1);
    }
    distance[vertexIdFrom]=0;
    for(int i=0;i<TotalVertexCount();i++){
        pq.Add(vertices[i].id,pow(10,300));
    }
    std::list<Vertex*>::const_iterator itt;
    for(itt=adjList[vertexIdFrom].begin(); itt!=adjList[vertexIdFrom].end(); itt++){
        pq.ChangePriority((*itt)->id,vertices[vertexIdFrom].position3D.Distance(vertices[vertexIdFrom].position3D,(*itt)->position3D));
    }
    pq.ChangePriority(vertexIdFrom,0);
    
    while(pq.HeapSize()>1){
        int vertex_id;
        double vertex_weight;
        pq.PopHeap(vertex_id,vertex_weight);
        //std::cout<<"vertex id: "<<vertex_id<<std::endl;
        std::list<Vertex*>::const_iterator it;
        for(it=adjList[vertex_id].begin(); it!=adjList[vertex_id].end(); it++ ){
            double new_distance = distance[vertex_id]+vertices[vertex_id].position3D.Distance(vertices[vertex_id].position3D,(*it)->position3D);
            if(new_distance < distance[(*it)->id]){
                distance[(*it)->id] = new_distance;
                previous[(*it)->id] = vertex_id;
                pq.ChangePriority((*it)->id,new_distance);
            }
        }
    }
    /*
    while(vertexIdTo!=vertexIdFrom){
        outputColorAllVertex[vertexIdTo]=color;
        //std::cout<< "id: " << vertexIdTo<< std::endl;
        vertexIdTo=previous[vertexIdTo];
    }*/
    bool res=true;
    while(vertexIdTo!=vertexIdFrom){
        if(vertexIdTo==-1){
            res=false;
            break;
        }
        outputColorAllVertex[vertexIdTo]=color;
        vertexIdTo=previous[vertexIdTo];
    }
    if(!res){
        /*
        for(int y = 0; y<vertices.size();y++){
            outputColorAllVertex[y]=black;
        }
        */
        outputColorAllVertex.assign(vertices.size(),black);
    }
    else{
        outputColorAllVertex[vertexIdFrom]=color;
    }
    //outputColorAllVertex[vertexIdFrom]=color;
    delete [] distance;
    
}

void MeshGraph::PaintInRangeGeodesic(std::vector<Color>& outputColorAllVertex,
                                    int vertexId, const Color& color,
                                    int maxDepth, FilterType type,
                                    double alpha) const
{
    // TODO:
    //std::cout<<"vertexId:"<<vertexId<<" dpth"<<maxDepth<<std::endl;
    if(vertexId>-1 && vertexId<TotalVertexCount() && maxDepth>=0){
        struct Color black;
        std::vector<bool> mark;
        std::vector<double> distance;
        //std::vector<int> cur,prev;
        for(int i = 0; i< TotalVertexCount();i++){
            mark.push_back(false);
            distance.push_back(0);
        }
        mark[vertexId]=true;
        black.r=0;
        black.g=0;
        black.b=0;
        outputColorAllVertex.resize(vertices.size(),black);
        BinaryHeap pq,aux;
        int counter=0;
        pq.Add(vertexId,0);
        counter++;
        while(maxDepth>-1){
            while(pq.HeapSize()>0){
                int vertex_id;
                double vertex_weight;
                double dist;
                pq.PopHeap(vertex_id,vertex_weight);
                dist=distance[vertex_id];
                mark[vertex_id]=true;
                if(type==FILTER_GAUSSIAN){
                    double val=std::exp(-dist*dist/alpha/alpha);
                    struct Color tmp;
                    tmp.r=color.r*val;
                    tmp.g=color.g*val;
                    tmp.b=color.b*val;
                    outputColorAllVertex[vertex_id]=tmp;
                }
                else if(type==FILTER_BOX){
                    int val=(dist >= -alpha && dist<= alpha)?1:0;
                    struct Color tmp;
                    tmp.r=color.r*val;
                    tmp.g=color.g*val;
                    tmp.b=color.b*val;
                    outputColorAllVertex[vertex_id]=tmp;
                }
                std::list<Vertex*>::const_iterator it;
                std::vector<int> id_order;
                for(it=adjList[vertex_id].begin();it!=adjList[vertex_id].end();it++){
                    if(id_order.empty()){
                        id_order.push_back((*it)->id);
                    }
                    else{
                        int i = 0;
                        int size=id_order.size();
                        while(i<size && (*it)->id > id_order[i]){
                            i++;
                        }
                        id_order.insert(id_order.begin()+i,(*it)->id);
                        //std::cout<<"*_*"<<(*it)->id<<std::endl;
                    }
                }
                
                for(int i = 0;i<(int)id_order.size();i++){
                    if(!mark[id_order[i]]){
                        //std::cout<<"*-*"<<id_order[i]<<std::endl;
                        distance[id_order[i]]=dist+vertices[vertex_id].position3D.Distance(vertices[vertex_id].position3D,vertices[id_order[i]].position3D);
                        aux.Add(id_order[i],counter);
                        counter++;
                        mark[id_order[i]]=true;
                    }
                }
                //std::cout<<std::endl;
                
            }
            while(aux.HeapSize()>0){
                int n;
                double d;
                aux.PopHeap(n,d);
                //std::cout<<"id: "<< n<<" weight: "<< d<<" distance: "<< distance[n]<<std::endl;
                pq.Add(n,d);
            }
            //std::cout<<std::endl;
            maxDepth--;
        }
    }
    
}

void MeshGraph::PaintInRangeEuclidian(std::vector<Color>& outputColorAllVertex,
                                      int vertexId, const Color& color,
                                      int maxDepth, FilterType type,
                                      double alpha) const
{
    // TODO:
    if(vertexId>-1 && vertexId<TotalVertexCount()){
        struct Color black;
        std::vector<bool> mark;
        for(int i = 0; i< TotalVertexCount();i++){
            mark.push_back(false);
        }
        mark[vertexId]=true;
        black.r=0;
        black.g=0;
        black.b=0;
        outputColorAllVertex.resize(vertices.size(),black);
        BinaryHeap pq,aux;
        int counter=0;
        pq.Add(vertexId,0);
        counter++;
        while(maxDepth>-1){
            while(pq.HeapSize()>0){
                int vertex_id;
                double vertex_weight;
                double dist;
                pq.PopHeap(vertex_id,vertex_weight);
                dist=vertices[vertexId].position3D.Distance(vertices[vertexId].position3D,vertices[vertex_id].position3D);
                mark[vertex_id]=true;
                if(type==FILTER_GAUSSIAN){
                    double val=std::exp(-dist*dist/alpha/alpha);
                    struct Color tmp;
                    tmp.r=color.r*val;
                    tmp.g=color.g*val;
                    tmp.b=color.b*val;
                    outputColorAllVertex[vertex_id]=tmp;
                }
                else if(type==FILTER_BOX){
                    int val=(dist >= -alpha && dist<= alpha)?1:0;
                    struct Color tmp;
                    tmp.r=color.r*val;
                    tmp.g=color.g*val;
                    tmp.b=color.b*val;
                    outputColorAllVertex[vertex_id]=tmp;
                }
                std::list<Vertex*>::const_iterator it;
                std::vector<int> id_order;
                for(it=adjList[vertex_id].begin();it!=adjList[vertex_id].end();it++){
                    if(id_order.empty()){
                        id_order.push_back((*it)->id);
                    }
                    else{
                        int i = 0;
                        int size=id_order.size();
                        while(i<size && (*it)->id > id_order[i]){
                            i++;
                        }
                        id_order.insert(id_order.begin()+i,(*it)->id);
                        //std::cout<<"*_*"<<(*it)->id<<std::endl;
                    }
                }
                for(int i = 0;i<(int)id_order.size();i++){
                    if(!mark[id_order[i]]){
                        //std::cout<<"*-*"<<id_order[i]<<std::endl;
                        aux.Add(id_order[i],counter);
                        counter++;
                        mark[id_order[i]]=true;
                    }
                }
            }
            while(aux.HeapSize()>0){
                int n;
                double d;
                aux.PopHeap(n,d);
                //std::cout<<"id: "<< n<<" prev: "<< d<<" distance: "<< distance[n]<<std::endl;
                pq.Add(n,d);
            }
            //std::cout<<std::endl;
            maxDepth--;
        }
    }
}

void MeshGraph::WriteColorToFile(const std::vector<Color>& colors,
                                 const std::string& fileName)
{
    // IMPLEMENTED
    std::stringstream s;
    for(int i = 0; i < static_cast<int>(colors.size()); i++)
    {
        int r = static_cast<int>(colors[i].r);
        int g = static_cast<int>(colors[i].g);
        int b = static_cast<int>(colors[i].b);

        s << r << ", " << g << ", " << b << "\n";
    }
    std::ofstream f(fileName.c_str());
    f << s.str();
}

void MeshGraph::PrintColorToStdOut(const std::vector<Color>& colors)
{
    // IMPLEMENTED
    for(int i = 0; i < static_cast<int>(colors.size()); i++)
    {
        std::cout << static_cast<int>(colors[i].r) << ", "
                  << static_cast<int>(colors[i].g) << ", "
                  << static_cast<int>(colors[i].b) << "\n";
    }
}