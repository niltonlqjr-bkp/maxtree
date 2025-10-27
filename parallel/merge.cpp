#include <vips/vips8>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
#include <string>
#include <ostream>
#include <sysexits.h>

#include "maxtree.hpp"
#include "maxtree_node.hpp"
#include "boundary_tree.hpp"

#include "utils.hpp"

#include "bag_of_task.hpp"
#include "tasks.hpp"

using namespace vips;
bool print_only_trees;
bool verbose;


void read_config(char conf_name[], 
                 std::string &out_name, std::string &out_ext,
                 uint32_t &glines, uint32_t &gcolumns, Tattribute lambda,
                 uint8_t &pixel_connection, bool colored){
    /*
        Reading configuration file
    */
    verbose=false;
    print_only_trees=false;
    

    auto configs = parse_config(conf_name);

    if (configs->find("verbose") != configs->end()){
        if(configs->at("verbose") == "true"){
            verbose=true;
        }
    }
    if(configs->find("print_only_trees") != configs->end()){
        if(configs->at("print_only_trees") == "true"){
            print_only_trees = true;
        }
    }

    colored = false;
    if(configs->find("colored") != configs->end()){
        if(configs->at("colored") == "true"){
            colored = true;
        }
    }
    if(configs->find("lambda") != configs->end()){
        lambda = std::stod(configs->at("lambda"));
    }

    if(configs->find("glines") == configs->end() || configs->find("gcolumns") == configs->end()){
        std::cout << "you must specify the the image division on config file:" << conf_name <<"\n";
        std::cout << "example:\n";
        std::cout << "glines=8 #divide image in 8 vertical tiles\n";
        std::cout << "gcolumns=6 #divide image in 6 vertical tiles\n";
        exit(EX_CONFIG);
    }
    
    out_name = "output";
    if (configs->find("output") != configs->end()){
            out_name = configs->at("output");
    }

    out_ext = "png";
    if(configs->find("output_ext") != configs->end()){
        out_ext = configs->at("output_ext");
    }

    pixel_connection = 4;

    glines = std::stoi(configs->at("glines"));
    gcolumns = std::stoi(configs->at("gcolumns"));

    std::cout << "configurations:\n";
    print_unordered_map(configs);
    std::cout << "====================\n";
    
}



int main(int argc, char *argv[]){
    vips::VImage *in;
    maxtree *t;
    std::string out_name, out_ext;
    uint32_t glines, gcolumns;
    uint8_t pixel_connection;
    bool colored;
    Tattribute lambda=2;

    uint32_t num_th = 4;

    bag_of_tasks<input_tile*> bag_in;

    std::cout << "argc: " << argc << " argv:" ;
    for(int i=0;i<argc;i++){
        std::cout << argv[i] << " ";
    }
    std::cout << "\n";

    if(argc <= 2){
        std::cout << "usage: " << argv[0] << " input_image config_file\n";
        exit(EX_USAGE);
    }

    if (VIPS_INIT(argv[0])) { 
        vips_error_exit (NULL);
    } 

    in = new vips::VImage(
                vips::VImage::new_from_file(argv[1],
                VImage::option ()->set ("access", VIPS_ACCESS_SEQUENTIAL)
            )
        );

    read_config(argv[2], out_name, out_ext, glines, gcolumns, lambda, pixel_connection, colored);
    std::cout << "start\n";
    uint32_t noborder_rt=0, noborder_rl, lines_inc, columns_inc;
    for(uint32_t i=0; i<glines; i++){
        noborder_rl=0;
        for(uint32_t j=0; j<gcolumns; j++){
            bag_in.insert_task(new input_tile(in, glines, gcolumns, i ,j));
        }

    }


}