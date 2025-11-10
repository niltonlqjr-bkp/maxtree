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
#include <cmath>
#include <sysexits.h>

#include "maxtree.hpp"
#include "maxtree_node.hpp"
#include "boundary_tree.hpp"
#include "const_enum_define.hpp"
#include "utils.hpp"
#include "bag_of_task.hpp"
#include "tasks.hpp"

using namespace vips;
bool print_only_trees;
bool verbose;

extern uint32_t GRID_LINE_SIZE;

/*unused code


std::mutex vec_mutex;
std::mutex mutex_start;
std::condition_variable c;
bool start;

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

void worker_input_prepare(std::deque<input_tile_task*> &task_queue, bag_of_tasks<input_tile_task*> &bag_out,
                          vips::VImage *img, uint32_t glines, uint32_t gcolumns){

    input_tile_task *t;
    bool got_task;

    while(!task_queue.empty()){
        got_task = false;
        if(!task_queue.empty()){
            vec_mutex.lock();
            if(!task_queue.empty()){
                t=task_queue.front();
                task_queue.pop_front();
                got_task = true;
                std::cout << "worker got task " << t->i << ", " << t->j << " to prepare\n";
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

void worker_read_tile(bag_of_tasks<input_tile_task*> &bag_prepared, bag_of_tasks<input_tile_task *> &full_tiles, vips::VImage *img){
    bool got_task;
    input_tile_task *t;
    while(bag_prepared.is_running()){
        got_task=bag_prepared.get_task(t);
        if(got_task){
            std::ostringstream os("");
            os << "worker got task " << t->i << ", " << t->j << " to read tile";
            os << ": " << t->reg_top << ", " << t->reg_left <<  "..." <<  t->reg_top+t->tile_lines << ", " << t->reg_left+t->tile_columns<<"\n";
            std::string s = os.str();
            std::cout << s; 
            t->read_tile(img);
            full_tiles.insert_task(t);
        }
    }
}
*/
/*
std::vector<input_tile*> bag_in;
bag_of_tasks<input_tile*> bag_prepare;

*/


/* main unused code 

    
    bag_of_tasks<input_tile_task*> bag_prepare, bag_tiles;
    bag_of_tasks<maxtree_task *> max_trees_tiles;
    std::vector<std::thread *> threads, threads_prep, threads_mt;


    ...

    start = false;
    
    uint32_t noborder_rt=0, noborder_rl, lines_inc, columns_inc;
    queue_fill(queue_in, glines, gcolumns);
    
    start = true;
    for(uint32_t i=0; i<1; i++){
        threads.push_back(new std::thread(worker_input_prepare, std::ref(queue_in), std::ref(bag_prepare), in, glines, gcolumns));
        // threads.push_back(new std::thread(worker_input_prepare, in, glines, gcolumns));
    }
    for(auto th: threads){
        th->join();
    }
    
    std::cout << "bag_in total tasks:" << queue_in.size() << "\n";
    bag_prepare.start();
    for(uint32_t i=0; i<1; i++){
        threads_prep.push_back(new std::thread(worker_read_tile, std::ref(bag_prepare), std::ref(bag_tiles), in ) );
    }
    
    wait_empty<input_tile_task *>(bag_prepare, num_th);
    
    for(auto th: threads_prep){
        th->join();
    
    }
    std::cout << "bag_tiles total tasks:" << bag_tiles.get_num_task() << "\n";
    */


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

template<typename T>
void wait_empty(bag_of_tasks<T> &b, uint64_t num_th){
    std::cout << "wait end\n";
    uint64_t x=0;
    while(true){
        if(verbose) std::cout << "wait empty " << x++ << "\n";
        b.wait_empty();
        if(b.is_running() && b.empty() && b.num_waiting() == num_th){
            b.notify_end();
            std::cout << "end notified\n";
            break;
        }
    }
}



void read_config(char conf_name[], 
                 std::string &out_name, std::string &out_ext,
                 uint32_t &glines, uint32_t &gcolumns, Tattribute lambda,
                 uint8_t &pixel_connection, bool colored, uint32_t &num_threads){
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
    num_threads = std::thread::hardware_concurrency();

    if(configs->find("threads") != configs->end()){
        num_threads = std::stoi(configs->at("threads"));
    }
    

    std::cout << "configurations:\n";
    print_unordered_map(configs);
    std::cout << "====================\n";
    
}

void read_sequential_file(bag_of_tasks<input_tile_task*> &bag, vips::VImage *in, uint32_t glines, uint32_t gcolumns){
    input_tile_task *nt;
    for(uint32_t i=0; i<glines; i++){
        for(uint32_t j=0; j<gcolumns; j++){
            nt = new input_tile_task(i,j);
            nt->prepare(in,glines,gcolumns);
            nt->read_tile(in);
            bag.insert_task(nt);
        }
    }

}

//get a task from bag and compute maxtree of the tile of this task
void worker_maxtree_calc(bag_of_tasks<input_tile_task *> &bag_tiles, bag_of_tasks<maxtree_task *> &max_trees){
    bool got_task;
    input_tile_task *t;
    maxtree_task *mt;
    while(bag_tiles.is_running()){
        if(verbose){
            std::ostringstream os("");
            os << "thread " << std::this_thread::get_id() << " trying to get task\n";
            std::string s = os.str();
            std::cout << s;
        }
        got_task=bag_tiles.get_task(t);
        if(got_task){
            if(verbose){
                std::ostringstream os("");
                os << "worker " <<  std::this_thread::get_id() << " got task " << t->i << ", " << t->j << " to calculate maxtree\n";
                std::string s = os.str();
                std::cout << s;
            }
            mt = new maxtree_task(t);
            max_trees.insert_task(mt);
        }else if(verbose){
            std::ostringstream os("");
            os << "thread " << std::this_thread::get_id() << " couldnt get task\n";
            std::string s = os.str();
            std::cout << s; 
        }
    }
}

void worker_get_boundary_tree(bag_of_tasks<maxtree_task *> &maxtrees, bag_of_tasks<boundary_tree_task *> &boundary_trees){
    bool got_task;
    maxtree_task *mtt;
    boundary_tree_task *btt;
    while(maxtrees.is_running()){
        got_task = maxtrees.get_task(mtt);
        std::string stmt = std::to_string(mtt->mt->grid_i) + "," + std::to_string(mtt->mt->grid_j) +"\n";
        stmt += mtt->mt->to_string(GLOBAL_IDX,false) + "\n";
        std::cout << stmt;
        if(got_task){
            btt = new boundary_tree_task(mtt);
            boundary_trees.insert_task(btt);
        }
    }

}
uint64_t get_task_index(boundary_tree_task *t){
    return t->index;
}

void worker_search_pair(bag_of_tasks<boundary_tree_task *> &btrees_bag, bag_of_tasks<merge_btrees_task *> &merge_bag){
    boundary_tree_task *btt, *n, *aux;
    uint64_t idx;
    enum neighbor_direction nb_direction;
    while(btrees_bag.is_running()){
        std::cout << "total of tasks:" << btrees_bag.get_num_task() << "\n";
        bool got = btrees_bag.get_task(btt);
        if(got){
            // btt->bt->print_tree();
            if(btt->bt->grid_j % 2 == 0){
                nb_direction = NB_AT_RIGHT;
            }else{
                nb_direction = NB_AT_LEFT;
            }
            if(btt->bt->grid_j + NEIGHBOR_DIRECTION[nb_direction].first * btt->next_merge_distance < GRID_LINE_SIZE){
                idx = btt->neighbor_idx(nb_direction);
                
                try{
                    // auto pos = btrees_bag.search_by_field<uint64_t>(idx,get_task_index);
                    auto got_n = btrees_bag.get_task_by_field<uint64_t>(n, idx, get_task_index);
                    //Olhar melhor essa questao do swap, falha de segmentacao por bordas estarem erradas.
                    if(got_n){
                        if(btt->bt->grid_j % 2 != 0){
                            std::cout << "swap btt with n\n";
                            aux = btt;
                            btt = n;
                            n = aux;
                        }
                        // std::cout << "+++++++++++++++ neighbor task of " << btt->index << " has index: " << n->index << "\n";
                        // n->bt->print_tree();
                        if(n->bt->grid_i == btt->bt->grid_i && std::abs(btt->next_merge_distance) == std::abs(n->next_merge_distance)){
                            std::cout << "btt " << btt->bt->grid_i << "," << btt->bt->grid_j << "   " 
                                    << "n: " << n->bt->grid_i << "," << n->bt->grid_j << "\n";
                            merge_bag.insert_task(new merge_btrees_task(btt->bt, n->bt, MERGE_VERTICAL_BORDER));
                        }else{
                            merge_bag.insert_task(new merge_btrees_task(btt->bt, NULL, MERGE_VERTICAL_BORDER));
                            merge_bag.insert_task(new merge_btrees_task(n->bt, NULL, MERGE_VERTICAL_BORDER));
                        }
                    }
                }catch(std::runtime_error &e){
                    std::cout << "\nneighbor task of " << btt->index << " not found\n";
                }catch(std::out_of_range &r){
                    std::cerr << "try to access an out of range element\n";
                }
            }else{
                merge_bag.insert_task(new merge_btrees_task(btt->bt, NULL, MERGE_VERTICAL_BORDER));
            }
        }
    }
}
void worker_merge_local(bag_of_tasks<merge_btrees_task *> &merge_bag, bag_of_tasks<boundary_tree_task *> &btrees_bag){
    merge_btrees_task *mbt;
    boundary_tree_task *btt;
    boundary_tree *nt;
    int32_t dist;
    while(merge_bag.is_running()){
        bool got_mt = merge_bag.get_task(mbt);
        std::cout << "task will merge: " << mbt->bt1->grid_i << ", " << mbt->bt1->grid_j << " with "
                                         << mbt->bt2->grid_i << ", " << mbt->bt2->grid_j << "\n";
        
        if(got_mt){
            if(verbose){
                std::string s = "-------------------TREE 1------------------\n";
                // mbt->bt1->print_tree();
                s+=mbt->bt1->border_to_string();
                s+=mbt->bt1->lroot_to_string();
                s+="\n-----------------------------------------------\n";
                std::cout << "-------------------TREE 2------------------\n";
                // mbt->bt2->print_tree();
                s+=mbt->bt2->border_to_string();
                s+=mbt->bt2->lroot_to_string();
                s+="\n-----------------------------------------------\n";
                std::cout << s;
            }
            if(mbt->bt1 != NULL && mbt->bt2 !=NULL){
                if(mbt->direction = MERGE_VERTICAL_BORDER){
                    dist = mbt->bt2->grid_i - mbt->bt1->grid_i;
                }else{
                    dist = mbt->bt2->grid_j - mbt->bt1->grid_j;
                }
                nt = mbt->execute();
                if(verbose) nt->print_tree();
            }else{
                nt = (boundary_tree *) ((uintptr_t) mbt->bt1 | (uintptr_t) mbt->bt2); // one of the trees is null, so, or bitwise will be the other one
            }
            btt = new boundary_tree_task(nt, dist*2);
            btrees_bag.insert_task(btt);
        }
    }
}
int main(int argc, char *argv[]){
    vips::VImage *in;
    std::string out_name, out_ext;
    
    uint32_t glines, gcolumns;
    uint8_t pixel_connection;
    uint32_t num_th;
    bool colored;
    
    Tattribute lambda=2;
    maxtree *t;
    input_tile_task *tile;
    
    bag_of_tasks<input_tile_task*> bag_tiles;
    bag_of_tasks<maxtree_task*> maxtree_tiles;
    bag_of_tasks<boundary_tree_task *> boundary_bag, boundary_bag_aux, *bound_bag_source, *bound_bag_dest;
    bag_of_tasks<merge_btrees_task *> merge_bag;

    std::vector<std::thread*> threads_g1, threads_g2;

    verify_args(argc, argv);
    read_config(argv[2], out_name, out_ext, glines, gcolumns, lambda, pixel_connection, colored, num_th);

    GRID_LINE_SIZE = gcolumns;

    if (VIPS_INIT(argv[0])) { 
        vips_error_exit (NULL);
    } 
    // SEE VIPS_CONCURRENCY
    // check https://github.com/libvips/libvips/discussions/4063 for improvements on read
    in = new vips::VImage(
                vips::VImage::new_from_file(argv[1],
                VImage::option ()->set ("access",  VIPS_ACCESS_SEQUENTIAL)
            )
        );
    std::cout << "number of threads "<< num_th << "\n";  
    std::cout << "start\n";
    read_sequential_file(bag_tiles, in, glines, gcolumns);
    std::cout << "bag_tiles.get_num_task:" << bag_tiles.get_num_task() << "\n";
    // bag_tiles.start();

    bag_tiles.start();
    for(uint32_t i=0; i<num_th; i++){
        threads_g1.push_back(new std::thread(worker_maxtree_calc, std::ref(bag_tiles), std::ref(maxtree_tiles)));
    }
    maxtree_task *mtt;
    wait_empty<input_tile_task *>(bag_tiles, num_th);
    
    for(uint32_t i=0; i<num_th; i++){
        threads_g1[i]->join();
        delete threads_g1[i];
    } 
    std::cout << "max_trees_tiles.get_num_task:" << maxtree_tiles.get_num_task() << "\n";
    threads_g1.erase(threads_g1.begin(),threads_g1.end());
    // maxtree_tiles.start();
    /* while(total_print < glines*gcolumns){
        bool aux=maxtree_tiles.get_task(mtt);
        if(aux){
            std::cout << "====== tile "<< mtt->mt->grid_i << ", " << mtt->mt->grid_j << " =====\n";
            // std::cout << mtt->mt->to_string(PARENT)<<"\n\n";
            std:: cout << total_print++ << "\n";
        }
    } */
   
    maxtree_tiles.start();
    for(uint32_t i=0; i<num_th;i++){
       threads_g1.push_back(new std::thread(worker_get_boundary_tree, std::ref(maxtree_tiles), std::ref(boundary_bag) ));
    }
    wait_empty<maxtree_task *>(maxtree_tiles, num_th);

    for(uint32_t i=0; i<num_th; i++){
        threads_g1[i]->join();
        delete threads_g1[i];
    } 
    threads_g1.erase(threads_g1.begin(),threads_g1.end());
    // std::thread thr(worker_local_merge, std::ref(boundary_bag));

    // wait_empty<boundary_tree_task *>(boundary_bag, num_th);


    //ver possibilidade de transformar as bags em filas hierarquicas

    //task to get pairs of boundary trees.
    for(uint32_t i=0; i<num_th;i++){
       threads_g1.push_back(new std::thread(worker_search_pair, std::ref(boundary_bag), std::ref(merge_bag) ));
    }

    for(uint32_t i=0; i<num_th;i++){
       threads_g1.push_back(new std::thread(worker_merge_local, std::ref(merge_bag), std::ref(boundary_bag) ));
    }

    wait_empty<boundary_tree_task *>(boundary_bag, num_th);
    wait_empty<merge_btrees_task *>(merge_bag, num_th);
/* 
    boundary_tree_task *btt, *n, *aux;
    uint64_t idx;
    enum neighbor_direction nb_direction;
    while(boundary_bag.get_num_task()){
        std::cout << "total of tasks:" << boundary_bag.get_num_task() << "\n";
        bool got = boundary_bag.get_task(btt);
        if(got){
            // btt->bt->print_tree();
            if(btt->bt->grid_j % 2 == 0){
                nb_direction = NB_AT_RIGHT;
            }else{
                nb_direction = NB_AT_LEFT;
            }
            idx = btt->neighbor_idx(nb_direction);
            try{
                // auto pos = boundary_bag.search_by_field<uint64_t>(idx,get_task_index);
                auto got_n = boundary_bag.get_task_by_field<uint64_t>(n, idx, get_task_index);
                //Olhar melhor essa questao do swap, falha de segmentacao por bordas estarem erradas.
                if(got_n){
                    if(btt->bt->grid_j % 2 != 0){
                        std::cout << "swap btt with n\n";
                        aux = btt;
                        btt = n;
                        n = aux;
                    }
                    // std::cout << "+++++++++++++++ neighbor task of " << btt->index << " has index: " << n->index << "\n";
                    // n->bt->print_tree();
                    if(n->bt->grid_i == btt->bt->grid_i){
                        std::cout << "btt " << btt->bt->grid_i << "," << btt->bt->grid_j << "   " 
                                  << "n: " << n->bt->grid_i << "," << n->bt->grid_j << "\n";
                        merge_bag.insert_task(new merge_btrees_task(btt->bt, n->bt, MERGE_VERTICAL_BORDER));
                    }
                }
            }catch(std::runtime_error &e){
                std::cout << "\nneighbor task of " << btt->index << " not found\n";
            }catch(std::out_of_range &r){
                std::cerr << "try to access an out of range element\n";
            }
        }
        std::cout << "====================================================================================\n";
    }




    merge_btrees_task *mbt;
    while(merge_bag.get_num_task()){
        bool got_mt = merge_bag.get_task(mbt);
        std::cout << "task will merge: " << mbt->bt1->grid_i << ", " << mbt->bt1->grid_j << " with "
                                         << mbt->bt2->grid_i << ", " << mbt->bt2->grid_j << "\n";
        
        if(got_mt){
            if(verbose){
                std::cout << "-------------------TREE 1------------------\n";
                mbt->bt1->print_tree();
                std::cout << "-------------------TREE 2------------------\n";
                mbt->bt2->print_tree();
            }
            auto nt = mbt->execute();
            if(verbose) nt->print_tree();

        }
    }

 */
}