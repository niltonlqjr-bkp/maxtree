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
    t->compute_sequential_recursive();
    if(verbose){
        std::cout<<"__________________GVAL________________\n";
        std::cout << t->to_string(GVAL,false,5);
        std::cout<<"__________________PARENT________________\n";
        std::cout << t->to_string(PARENT,false,5);
        std::cout<<"__________________IDX________________\n";
        std::cout << t->to_string(IDX,false,5);
        std::cout << "________________LEVELROOT________________\n";
        std::cout << t->to_string(LEVELROOT,false,5);
        std::cout<<"__________________ATRIBUTE________________\n";
        std::cout << t->to_string(ATTRIBUTE,false, 5);
    }

    if(verbose){
        std::cout << "filter data\n";
    }
    t->filter(lambda);
    if(verbose){
        std::cout << "saving image\n";
    }
    t->save(out_name, LABEL);
    
    return 0;
}
