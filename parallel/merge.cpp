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



void worker_input_prepare(std::deque<input_tile_task*> &task_queue, bag_of_tasks<input_tile_task*> &bag_out,
                          vips::VImage *img, uint32_t glines, uint32_t gcolumns){
// void worker_input_prepare(vips::VImage *img, uint32_t glines, uint32_t gcolumns){
    input_tile_task *t;
    bool got_task;
    if(!start){
        std::unique_lock<std::mutex> ul(mutex_start);
        c.wait(ul);
    }
    while(!task_queue.empty()){
        got_task = false;
        if(!task_queue.empty()){
            vec_mutex.lock();
            if(!task_queue.empty()){
                t=task_queue.front();
                task_queue.pop_front();
                got_task = true;
                std::cout << "worker got task " << t->i << ", " << t->j << "\n";
            }
            vec_mutex.unlock();
            if(got_task){
                t->prepare(img, glines, gcolumns);
                std::ostringstream os("");
                os << t->tile->h << ", " << t->tile->w << "\n";
                std::string s = os.str();
                std::cout << s;
                bag_out.insert_task(t);
            }
        }
    }
}

void worker_read_tile(vips::VImage *img, bag_of_tasks<input_tile_task*> &bag_prepared, bag_of_tasks<input_tile_task *> &full_tiles){
    bool got_task;
    input_tile_task *t;
    while(bag_prepared.is_running()){
        got_task=bag_prepared.get_task(t);
        if(got_task){
            t->read_tile(img);
            full_tiles.insert_task(t);
        }
    }
}

void worker_maxtree_calc(){

}

void queue_fill(std::deque<input_tile_task*> &queue_in, uint32_t glines, uint32_t gcolumns){
    queue_in.push_back(new input_tile_task(0,0));
    start = true;
    c.notify_all();
    for(uint32_t j=1; j<gcolumns; j++){
        vec_mutex.lock();
        queue_in.push_back(new input_tile_task(0 ,j));
        vec_mutex.unlock();
    }

    for(uint32_t i=1; i<glines; i++){
        for(uint32_t j=0; j<gcolumns; j++){
            vec_mutex.lock();
            queue_in.push_back(new input_tile_task(i ,j));
            vec_mutex.unlock();
        }
    }
}

int main(int argc, char *argv[]){
    vips::VImage *in;
    maxtree *t;
    input_tile_task *tile;
    std::string out_name, out_ext;
    uint32_t glines, gcolumns;
    uint8_t pixel_connection;
    bool colored;
    Tattribute lambda=2;
    uint32_t num_th = 4;
    
    std::deque<input_tile_task*> queue_in;
    bag_of_tasks<input_tile_task*> bag_prepare, bag_tiles;
    std::vector<std::thread *> threads, threads_prep;

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
    
    uint32_t noborder_rt=0, noborder_rl, lines_inc, columns_inc;
    
    for(uint32_t i=0; i<num_th; i++){
        threads.push_back(new std::thread(worker_input_prepare, std::ref(queue_in), std::ref(bag_prepare), in, glines, gcolumns));
        // threads.push_back(new std::thread(worker_input_prepare, in, glines, gcolumns));
    }

    queue_fill(queue_in, glines, gcolumns);

    for(auto th: threads){
        th->join();
    }
    bag_prepare.start();
    std::cout << "bag_prepare total tasks:" << bag_prepare.get_num_task() << "\n";
    std::cout << "bag_in total tasks:" << queue_in.size() << "\n";
    for(uint32_t i=0; i<num_th; i++){
        threads_prep.push_back(new std::thread(worker_read_tile, in, std::ref(bag_prepare), std::ref(bag_tiles) ));
    }
    std::cout << "wait end\n";
    while(true){
        bag_prepare.wait_empty();
        if(bag_prepare.is_running() && bag_prepare.empty() && bag_prepare.num_waiting() == num_th){
            bag_prepare.notify_end();
            std::cout << "end notified\n";
            break;
        }
    }
    for(auto th: threads_prep){
        th->join();
    }
    std::cout << "bag_prepare total tasks:" << bag_prepare.get_num_task() << "\n";
}