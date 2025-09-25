#include <vips/vips8>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
#include <string>
#include <ostream>

#include "maxtree.hpp"
#include "maxtree_node.hpp"
#include "utils.hpp"

#define INQUEUE -2

using namespace vips;

bool verbose;
/* 
void print_pq(std::priority_queue<maxtree_node*, std::vector<maxtree_node*> ,cmp_maxtree_nodes> pq){
    std::cout <<"===========QUEUE=============\n";
    while(!pq.empty()){
        auto e=pq.top();
        std::cout<<*e<<" ";
        //pq.remove(e);
        pq.pop();
    }
    std::cout<<"\n";
    std::cout <<"============================\n";
}


void compute_maxtree(maxtree *t){
    std::priority_queue<maxtree_node*, std::vector<maxtree_node*> ,cmp_maxtree_nodes> pixel_pq;
    std::stack<maxtree_node*> pixel_stack;
    maxtree_node *xm, *nextpix, *p;
    uint64_t idx=0;
    

    xm = min_gval(t->get_data());
    pixel_pq.push(xm);
    pixel_stack.push(xm);
    nextpix = xm;

    do{
        p = nextpix;
        auto N = t->get_neighbours(p->idx);
        for(auto q: N){
            if(q->parent == -1){// if q not visited 
                q->parent = INQUEUE;
                pixel_pq.push(q);
                if(q->gval > p->gval){
                    break;
                }
            }
        }
        nextpix = pixel_pq.top();

        if(nextpix->gval > p->gval){
            pixel_stack.push(nextpix);
        }else{
            pixel_pq.pop();
            if(p!=pixel_stack.top()){
                p->parent = pixel_stack.top()->idx;
            }
            
            if(pixel_pq.empty()){
                nextpix=p;
            }else{
                nextpix = pixel_pq.top();
            }

            if(nextpix->gval < p->gval){
                
                while(!pixel_stack.empty() && nextpix->gval < pixel_stack.top()->gval){
                    auto st = pixel_stack.top();
                    pixel_stack.pop();
                    if(!pixel_stack.empty())
                        st->parent = pixel_stack.top()->idx;
                }
                if(pixel_stack.empty() || pixel_stack.top()->gval < nextpix->gval){
                    pixel_stack.push(nextpix);
                }
            }

        }

    }while(!pixel_pq.empty());
    maxtree_node *root = pixel_stack.top();
    root->parent = root->idx;
    std::cout << "-----------------END---------------\n";
    //print_pq(pixel_pq);
    //print_stack(pixel_stack);
    std::cout <<"____________________________________\n";
}

 */
int main(int argc, char *argv[]){
    vips::VImage *in;
    maxtree *t;
    std::string out_name;
    Tattribute lambda;
    
    std::cout << "argc: " << argc << " argv:" ;
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
        if (configs->find("output") != configs->end()){
            out_name = configs->at("output");
        }else{
            out_name = "output.png";
        }
        if (configs->find("lambda") != configs->end()){
            lambda = std::atoi(configs->at("lambda").c_str());
        }else{
            lambda = 2;
        }
    }

    std::cout << "start\n";
    
    int h,w;
    h=in->height();
    w=in->width();
    t = new maxtree(h,w);
    vips::VImage cp = in->copy_memory();
    t->fill_from_VImage(cp);
    t->compute_sequential_iterative();
    if(verbose){
        std::cout<<"__________________GVAL________________\n";
        std::cout << t->to_string(GVAL,false,5);
        std::cout<<"__________________PARENT________________\n";
        std::cout << t->to_string(PARENT,false,5);
        std::cout << "________________LEVELROOT________________\n";
        std::cout << t->to_string(LEVELROOT,false,5);
        std::cout<<"__________________IDX________________\n";
        std::cout << t->to_string(IDX,false,5);
        std::cout<<"_________________ATTRIBUTE________________\n";
        std::cout << t->to_string(ATTRIBUTE,false,5);
        
    }

    t->filter(lambda);
    t->save(out_name, LABEL);
    
    return 0;
}
