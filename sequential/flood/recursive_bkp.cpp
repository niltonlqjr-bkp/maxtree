#include <vips/vips8>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
#include <string>
#include <ostream>
#include <deque>

#include "maxtree_node.hpp"
#include "maxtree.hpp"
#include "utils.hpp"

#define INQUEUE -2

bool verbose;


using namespace vips;
template<typename T, typename cmp>
class custom_priority_queue : public std::priority_queue<T, std::vector<T>, cmp>
{
  public:

        bool remove(const T& value) {
            //auto it= std::find(this->c.begin(), this->c.end(), value);
            auto it = this->c.begin(); 
            while(it != this->c.end()){
                if(*it == value){
                    break;
                }
                it++;
            }

            if (it == this->c.end()) {
                return false;
            }else if (it == this->c.begin()) {
                // deque the top element
                this->pop();
            }else{
                // remove element and re-heap
                this->c.erase(it);
                std::make_heap(this->c.begin(), this->c.end(), this->comp);
            }
            return true;
        }
};

void print_deque(std::deque<maxtree_node*> d){
    for(auto x: d){
        std::cout << *x;
    }
    std::cout << "\n";
}

void print_hq(std::vector<std::deque<maxtree_node*>> *hqueue){
    std::cout <<"===========QUEUE=============\n";
    for(auto d: *hqueue){
        if(!d.empty()){
            print_deque(d);
        }
    }
    std::cout<<"\n";
    std::cout <<"============================\n";
}


std::tuple<uint64_t, uint64_t> lin_col(int64_t index, int64_t h, int64_t w){
    return std::make_tuple(index/w, index%w);
}

std::vector<maxtree_node*> get_neighbours(maxtree_node *pixel, 
                                          std::vector<maxtree_node *> *t,
                                          uint64_t h, uint64_t w){
    std::vector<maxtree_node*> v;
    uint64_t idx, pl, pc;
    std::tie(pl, pc) = lin_col(pixel->idx, h, w);
    if(pl >= 1){
        idx = index_of(pl-1, pc, h, w);
        v.push_back(t->at(idx));
    }
    if(pl < (unsigned int)h - 1){
        idx = index_of(pl+1, pc, h, w);
        v.push_back(t->at(idx));
    }
    if(pc >= 1){
        idx = index_of(pl, pc-1, h, w);
        v.push_back(t->at(idx));
    }
    if(pc < (unsigned int)w - 1){
        idx = index_of(pl, pc+1, h, w);
        v.push_back(t->at(idx));
    }
    return v;
}



int flood(int lambda, maxtree_node *r, 
          std::vector<std::deque<maxtree_node*>> *hqueue,
          std::vector<maxtree_node*> *data,
          std::vector<maxtree_node*> *levroot,
          int h, int w){
    
    while(!hqueue->at(lambda).empty()){
        maxtree_node* p = hqueue->at(lambda).front();
        hqueue->at(lambda).pop_front();
        p->parent = r->idx;
        auto N = get_neighbours(p,data, h, w);
        for(auto n: N){
            if(n->parent == -1){
                int l = n->gval;
                if(levroot->at(l) == NULL){
                    levroot->at(l) = n;
                }
                hqueue->at(l).push_back(n);
                n->parent = INQUEUE;
                while(l>lambda){
                    l=flood(l,levroot->at(l),hqueue,data,levroot,h,w);
                }
            }
        }
        
    }
    levroot->at(lambda) = NULL;
    int lpar = lambda-1;
    while(lpar >= 0 && levroot->at(lpar) == NULL){
        lpar--;
    }
    if(lpar != -1){
        r->parent = levroot->at(lpar)->idx;
    }
    
    return lpar;
}

std::vector<maxtree_node*> *compute_maxtree(VImage *in, int band = 0, int gl = 256){
    std::vector<maxtree_node*> *data;
    data = new std::vector<maxtree_node*>;
    int h=in->height();
    int w=in->width();
    int lmin;
    
    std::vector<maxtree_node*> *levroot = new std::vector<maxtree_node*>(gl, NULL);
    std::vector<std::deque<maxtree_node*>> *hqueue = 
        new std::vector<std::deque<maxtree_node*>>(gl);
    maxtree_node *pmin;

    uint64_t idx=0;
    VImage img = in->copy_memory();
    VipsImage *pointer_image = img.get_image();
    VipsPel *vpel;
    for(int l=0; l<h; l++){
        for(int c=0;c<w;c++){
            vpel = VIPS_IMAGE_ADDR(pointer_image,c,l);
            if(verbose) std::cout << (int)*vpel << "\n";
            data->push_back(new maxtree_node((int) (*vpel), idx));
            idx++;
        }
    }
    std::cout << "fim leitura\n";
    //print_matrix(data,h,w);

    pmin = min_gval(data);
    lmin = pmin->gval;
    std::cout << lmin << "\n\n";
    hqueue->at(lmin).push_back(pmin);
    levroot->at(lmin) = pmin;
    flood(lmin, pmin, hqueue, data, levroot, h, w);

    //visited->at(nextpix->idx) = true;
    //std::cout << "-------> visiting: " << *nextpix << "\n";

  
    return data;
}



int main(int argc, char **argv){
    VImage *in;
    std::vector<maxtree_node*> *tdata;
    if (VIPS_INIT (argv[0])) 
        vips_error_exit (NULL);

    in = new VImage(VImage::new_from_file(argv[1],NULL));
    int h,w;
    h=in->height();
    w=in->width();

    for(int i=0;i<argc;i++){
        std::cout << argv[i] << " ";
    }
    std::cout << "\n";


    if(argc < 2){
        std::cout << "usage: " << argv[0] << " input_image config_file\n";
        exit(0);
    }

    if (VIPS_INIT (argv[0])) {
        vips_error_exit (NULL);
    }
    bool verbose=false;
    in = new vips::VImage(vips::VImage::new_from_file(argv[1],NULL));


    if(argc > 2){    
        auto configs = parse_config(argv[2]);
        if (configs->find("verbose") != configs->end()){
            if(configs->at("verbose") == "true"){
                verbose=true;
            }
        }
    }

    vips::VImage cp = in->copy_memory();
    if(verbose) print_VImage_band(&cp);
    tdata=compute_maxtree(in);
    label_components(tdata);
    
    maxtree *t = new maxtree(tdata, h, w);
    //if(verbose) print_matrix(t, h, w);
    //if(verbose) print_labels(t, h, w);
    std::cout << "gval\n" << t->to_string(GVAL,false);
    std::cout << "label\n" << t->to_string(LABEL,false);
    std::cout << "parent\n" << t->to_string(PARENT,false);
    vips_shutdown();
    return 0;
}