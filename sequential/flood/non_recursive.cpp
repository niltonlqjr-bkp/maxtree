#include <vips/vips8>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
#include <string>
#include <ostream>
#include <map>

#include "maxtree_node.hpp"
#include "utils.hpp"

#define INQUEUE -2

using namespace vips;

bool verbose;

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

        T second(){
            return this->c.at(1);
        }

        void print(){
            for(T n: this->c){
                std::cout << *n;
            }
            std::cout << "\n";
        }

};

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


std::vector<maxtree_node*> *maxtree(VImage *in, int band = 0){
    std::vector<maxtree_node*> *data;
    data = new std::vector<maxtree_node*>;
    int h=in->height();
    int w=in->width();
    //std::vector<bool> *visited = new std::vector<bool>(h*w, false);
    //std::cout << visited->size() << " <--- visited size\n"; 
    std::priority_queue<maxtree_node*> pixel_pq;
    std::stack<maxtree_node*> pixel_stack;
    std::map<double, int> in_stack;
    maxtree_node *q, *p, *r;


    uint64_t idx=0;
    VImage img = in->copy_memory();
    VipsImage *pointer_image = img.get_image();
    VipsPel *vpel;
    for(int l=0; l<h; l++){
        for(int c=0;c<w;c++){
            vpel = VIPS_IMAGE_ADDR(pointer_image,c,l);
            // std::cout << (int)*vpel << "\n";
            data->push_back(new maxtree_node((int) (*vpel), idx));
            idx++;
        }
    }
    std::cout << "fim leitura\n";

    //print_matrix(data,h,w);

    //maxtree_node *base_pilha = new maxtree_node(-10,-1,-10);

    p = min_gval(data);
    p->parent = INQUEUE;
    //pixel_stack.push(base_pilha);
    pixel_pq.push(p);
    pixel_stack.push(p);

    //visited->at(nextpix->idx) = true;
    //std::cout << "-------> visiting: " << *nextpix << "\n";

    //int visited = 0;
    bool subir, descer;

    while(!pixel_pq.empty()){
        
/*         std::cout << "--------------- " << ++iter << " ---------------\n";
        print_pq(pixel_pq);
        print_stack(pixel_stack);
        std::cout << "_______________________________________________\n"; */

        subir = false;
        descer = false;
        p = pixel_pq.top();
        r = pixel_stack.top();
        /*std::cout << "processando o pixel:" << *p << "\n";
        print_pq(pixel_pq);
        print_stack(pixel_stack);
        std::cout << "=========================================\n";*/

        auto N=get_neighbours(p,data,h,w);

        for(maxtree_node *n: N){
            if(n->parent == -1){
                pixel_pq.push(n);
                n->parent = INQUEUE;
                if(p->gval < n->gval){
                    subir=true;
                    q=n;
                    break;
                }
            }
            
        }
        if(subir){
            pixel_stack.push(q);
        }else{
            pixel_pq.pop();
            p->parent = pixel_stack.top()->idx;
            if(!pixel_pq.empty() && pixel_pq.top()->gval < p->gval){
                descer = true;
            }
            if(descer){
                if(!pixel_stack.empty()){
                    p->parent = pixel_stack.top()->idx;
                }
                pixel_stack.pop();
                if(!pixel_stack.empty()){
                    r->parent = pixel_stack.top()->idx;
                }
                    
            }else{
                if(!pixel_stack.empty() && pixel_stack.top()->gval != p->gval){
                    pixel_stack.push(p);
                }
            }
        }
        //print_matrix(data,h,w);
    }
    
    return data;
}
/* 
void label_components(std::vector<maxtree_node*> *mt){
    for(auto p: *mt){print_labels
        if(p->gval == mt->at(p->parent)->gval){
            p->label = p->parent;
        }else{
            p->label = p->idx;
        }
    }
}
 */

int main(int argc, char **argv){
    VImage *in;
    std::vector<maxtree_node*> *t;
    if (VIPS_INIT (argv[0])) 
        vips_error_exit (NULL);

    in = new VImage(VImage::new_from_file(argv[1],NULL));
    int h,w;
    h=in->height();
    w=in->width();
    print_VImage_band(in);
    t=maxtree(in);
    print_matrix(t, h, w);
    label_components(t);
    print_labels(t, h, w);
    vips_shutdown();
    return 0;
}