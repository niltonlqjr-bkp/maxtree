#include <numeric>
#include <source_location>

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vips/vips8>
#include <vector>
#include <algorithm>
#include <thread>
#include <queue>
#include <limits>
#include <mutex>
#include <condition_variable>
#include <queue>

#include "maxtree_node.hpp"
#include "maxtree.hpp"
#include "heap.hpp"
#include "utils.hpp"
#include "bag_of_task.hpp"
#include "graph.hpp"

bool verbose;
int connectivity;

using namespace vips;

class task{
    private:
        std::vector<int> pixels_index;
        std::unordered_map<int, bool> visited;
    public:
        int parent_thread;
        int size;
        double gval;
        uint64_t ini_idx;

    task(uint64_t ini_idx, double gval = 0, int parent_thread = 0){
        this->gval = gval;
        this->ini_idx = ini_idx;
        this->parent_thread = parent_thread;
        this->size = 0;
    }

    void visit(int idx){
        this->visited.at(idx) = true;
    }

    bool visited_at(int idx){
        return this->visited.at(idx);
    }

    friend std::ostream& operator<<(std::ostream& os, const task& t){
        os << "parent thread: " << t.parent_thread << " ";
        os << "size: " << t.size << " ";
        os << "threshold: " << t.gval << " ";
        os << "task pixels: ";
        for(auto p: t.pixels_index){
            os << p << " ";
        }
        return os;
    }
   
    int weight(){
        return this->size;
    }

    void add_pixel(int p_idx){
        this->size += 1;
        this->pixels_index.push_back(p_idx);
        this->visited.emplace(p_idx,false);
    }

    std::vector<int> get_all_pixels_ids(){
        return this->pixels_index;
    }

    /*  std::unordered_map<int, bool> get_visited(){
        return this->visited;
    } */
    int get_task_pixel(int idx){
        int ret = this->pixels_index.at(idx);
        return ret;
    }

    bool operator<(const task &r){
        return this->size < r.size;
    }

    bool operator>(const task &r){
        return this->size > r.size;
    }

    bool operator==(const task &r){
        return this->size == r.size;
    }

};


/* Graph<double, double> build_adjacent_graph(maxtree *m){
    m->all_thresholds();
} */

int delta;

component *grow_region(maxtree *m, uint64_t idx_ini, uint64_t component_idx){
    std::queue<maxtree_node *>q;
    component *ret = new component(component_idx);
    
    double region_gval = m->at_pos(idx_ini)->gval;
    
    //std::cout << "ini:" << idx_ini <<" gval:" << region_gval<< "\n";


    if(!(m->at_pos(idx_ini)->visited)){
        m->at_pos(idx_ini)->visited = true;
        
        q.push(m->at_pos(idx_ini));
    }
    
    while(!q.empty()){
        maxtree_node *p = q.front();
        q.pop();
        // std::cout << "p->gval:" << p->gval << " " << region_gval << "\n";
        if((p->gval >= region_gval - delta) && (p->gval <= region_gval + delta)){
            ret->insert_pixel(p->idx);
            auto nb = m->get_neighbours(p->idx, connectivity);
            for(auto n: nb){
                if(!n->visited){
                    if((n->gval >= region_gval - delta) && (n->gval <= region_gval + delta)){
                        n->visited = true;
                        q.push(n);
                    }
                }
            }
        }

    }
    //std::cout << "\n";
    return ret;
}


/*
Ideia geral:
    Duas regioes r e s são do mesmo componente se o tom de cinza de ambos é memso
    e existe caminho de r ate s e todos os tons de cinza no caminho sao
    menores ou iguais ao tom de cinza de r
*/
maxtree *maxtree_main(VImage *in, int nth = 1){
    maxtree *m = new maxtree(in->height(), in->width());
    m->fill_from_VImage(*in, in->height(), in->height());
    uint64_t tpixel=0;
    component *c;
    int id = 0;
    for(int i=0; i<m->h; i++){
        for(int j=0; j<m->w;j++){
            if(!m->at_pos(i,j)->visited){
                
                c = grow_region(m, m->index_of(i,j), id);
                m->insert_component(*c, m->at_pos(i,j)->gval);
                if(c->get_pixels_index().size() >= 100){
                    std::cout << "id:" << id << " number of pixels:" << c->get_pixels_index().size() << "\n";
                }
                tpixel += c->get_pixels_index().size();
                id++;
                
                delete c;
            }
        }
    }
    std::cout << "total pixels:" << tpixel << "\n";
    return m;
}

int main(int argc, char **argv){
    VImage *in;
    maxtree *t;

    if(argc < 3){
        std::cout << "usage:\n" << argv[0] << " <image file> <config file>\n";
        std::cout << "example:\n" << argv[0] << " input.png configs/config_test.txt\n";
        return 1;
    }
    if (VIPS_INIT(argv[0])) 
        vips_error_exit (NULL);

    in = new VImage(VImage::new_from_file(argv[1],NULL));

    std::unordered_map<std::string, std::string> *configs;

    configs = parse_config(argv[2]);

    int h,w,nth;
    verbose = false;
    connectivity = 4;
    nth = std::stoi(configs->at("threads"));
    auto conf_verbose = configs->find("verbose");
    if(conf_verbose != configs->end()){
        if(conf_verbose->second == "true"){
            verbose = true;
        }
    }

    auto conf_delta = configs->find("delta");
    if(conf_delta != configs->end()){
        delta = std::stoi(conf_delta->second);
    }


    if(argc == 4) nth = std::stoi(argv[3]);
    
    print_unordered_map(configs);
    h=in->height();
    w=in->width();
    if(verbose){
        print_VImage_band(in);
    }
    
    std::cout<<"+++++++++++++"<< __LINE__ <<"++++++++++++\n";
    
    t=maxtree_main(in,nth);

    if(verbose){    
        std::cout<<"+++++++++++++"<< __LINE__ <<"++++++++++++\n";

        for(auto thold: t->all_thresholds()){
            std::vector<component> comps = t->components_at(thold);
            std::cout << "threshold: " << thold << "\n";
            for(int i=0;i<comps.size(); i++){
                std::cout << comps[i].to_string() << "\n===================\n";
            }
        }
    }
    if(verbose){ 
        print_VImage_band(in);
        std::cout << "=====================labels=====================\n";
        std::cout << t->to_string(LABEL) << "\n================================\n";
        std::cout << "=====================parents=====================\n";
        std::cout << t->to_string();
    }


    vips_shutdown();
    return 0;
}


