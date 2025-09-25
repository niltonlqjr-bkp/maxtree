#include<cinttypes>
#include<unordered_map>
#include<vector>

#ifndef __GRAPH_H__
#define __GRAPH_H__

template <typename T>
class Edge{
    private:
        uint64_t v1;
        uint64_t v2;
    public:
        T w;

        Edge(uint64_t u, uint64_t v);
        Edge(uint64_t u, uint64_t v, T w);
    

};

template <typename Tv, typename Ted>
class Graph{
    private:
        std::vector<Tv> V;
        std::vector<Edge <Ted> > E;
        std::unordered_map<Tv, uint64_t> map_V_vector;
    public:
        //Graph(std::vector<Tv> v = std::vector<Tv>(), std::vector<Edge<Te> > = std::vector<Edge<Te> >());
        Graph();
        void add_vertex(Tv v);
        void add_edge(Edge<Ted> e);
        void add_edge(Tv u, Tv v, Ted w);
};

#include "graph.tpp"

#endif
