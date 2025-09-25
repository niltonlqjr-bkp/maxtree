#include <vector>


template<typename T>
Edge<T>::Edge(uint64_t u, uint64_t v){
    this->v1 = u;
    this->v2 = v;
}

template<typename T>
Edge<T>::Edge(uint64_t u, uint64_t v, T w){
    this->v1 = u;
    this->v2 = v;
    this->w = w;
}


template<typename Tv, typename Ted>
Graph<Tv,Ted>::Graph(){
    
}

template<typename Tv, typename Ted>
void Graph<Tv,Ted>::add_vertex(Tv v){
    uint64_t pos = this->V.size();
    map_V_vector[v] = pos;
    this->V.push_back(v);

}

template<typename Tv, typename Ted>
void Graph<Tv,Ted>::add_edge(Edge<Ted> e){
    this->E = e;
}

template<typename Tv, typename Ted>
void Graph<Tv,Ted>::add_edge(Tv u, Tv v, Ted w){
    Edge<Ted> ne = Edge<Ted>(this->map_V_vector[u], this->map_V_vector[v]);
    this->E = ne;
}


