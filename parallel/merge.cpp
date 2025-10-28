#include <vips/vips8>
#include <iostream>
#include <vector>
#include <deque>
#include <stack>
#include <tuple>
#include <string>
#include <ostream>
#include <thread>
#include <mutex>
#include <condition_variable>


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
std::mutex vec_mutex;
std::mutex mutex_start;
std::condition_variable c;
bool start;


/* std::vector<input_tile*> bag_in;
bag_of_tasks<input_tile*> bag_prepare; */

void verify_args(int argc, char *argv[]){
    std::cout << "argc: " << argc << " argv:" ;
    for(int i=0;i<argc;i++){
        std::cout << argv[i] << " ";
    }
    std::cout << "\n";

    if(argc <= 2){
        std::cout << "usage: " << argv[0] << " input_image config_file\n";
        exit(EX_USAGE);
    }
}

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



void worker_input_prepare(std::deque<input_tile*> &bag_in, bag_of_tasks<input_tile*> &bag,
                          vips::VImage *img, uint32_t glines, uint32_t gcolumns){
// void worker_input_prepare(vips::VImage *img, uint32_t glines, uint32_t gcolumns){
    input_tile *t;
    bool got_task;
    if(!start){
        std::unique_lock<std::mutex> ul(mutex_start);
        c.wait(ul);
    }
    while(!bag_in.empty()){
        got_task = false;
        if(!bag_in.empty()){
            vec_mutex.lock();
            if(!bag_in.empty()){
                t=bag_in.front();
                bag_in.pop_front();
                got_task = true;
                std::cout << "worker got task " << t->i << ", " << t->j << "\n";
            }
            vec_mutex.unlock();
            if(got_task){
                t->prepare(img, glines, gcolumns);
                t->read_tile(img);
                t->tile->to_string();
            }
        }
    }
}


int main(int argc, char *argv[]){
    vips::VImage *in;
    maxtree *t;
    input_tile *tile;
    std::string out_name, out_ext;
    uint32_t glines, gcolumns;
    uint8_t pixel_connection;
    bool colored;
    Tattribute lambda=2;
    uint32_t num_th = 4;
    
    std::deque<input_tile*> bag_in;
    bag_of_tasks<input_tile*> bag_prepare;
    verify_args(argc, argv);
    read_config(argv[2], out_name, out_ext, glines, gcolumns, lambda, pixel_connection, colored);


    if (VIPS_INIT(argv[0])) { 
        vips_error_exit (NULL);
    } 

    in = new vips::VImage(
                vips::VImage::new_from_file(argv[1],
                VImage::option ()->set ("access", VIPS_ACCESS_SEQUENTIAL)
            )
        );
    std::cout << "start\n";
    start = false;
    std::vector<std::thread *> threads;
    
    uint32_t noborder_rt=0, noborder_rl, lines_inc, columns_inc;
    
    bag_in.push_back(new input_tile(0,0));
    start = true;
    c.notify_all();

    for(uint32_t i=0; i<num_th; i++){
        threads.push_back(new std::thread(worker_input_prepare, std::ref(bag_in), std::ref(bag_prepare), in, glines, gcolumns));
        // threads.push_back(new std::thread(worker_input_prepare, in, glines, gcolumns));
    }

    for(uint32_t j=1; j<gcolumns; j++){
        vec_mutex.lock();
        bag_in.push_back(new input_tile(0 ,j));
        vec_mutex.unlock();
    }

    for(uint32_t i=1; i<glines; i++){
        for(uint32_t j=0; j<gcolumns; j++){
            vec_mutex.lock();
            bag_in.push_back(new input_tile(i ,j));
            vec_mutex.unlock();
        }
    }

    for(auto th: threads){
        th->join();
    }



}