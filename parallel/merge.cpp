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

extern std::pair<uint32_t, uint32_t> GRID_DIMS;

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

/* ======================= signatures ================================= */
template<typename T>
void wait_empty(bag_of_tasks<T> &b, uint64_t num_th);

void verify_args(int argc, char *argv[]);
void read_config(char conf_name[], 
                 std::string &out_name, std::string &out_ext,
                 uint32_t &glines, uint32_t &gcolumns, Tattribute &lambda,
                 uint8_t &pixel_connection, bool &colored, uint32_t &num_threads);
void read_sequential_file(bag_of_tasks<input_tile_task*> &bag, vips::VImage *in, uint32_t glines, uint32_t gcolumns);
bool inside_rectangle(std::pair<uint32_t, uint32_t> c, std::pair<uint32_t, uint32_t> r);
std::pair<uint32_t, uint32_t> get_task_index(boundary_tree_task *t);

void worker_maxtree_calc(bag_of_tasks<input_tile_task *> &bag_tiles, bag_of_tasks<maxtree_task *> &max_trees);
void worker_get_boundary_tree(bag_of_tasks<maxtree_task *> &maxtrees, bag_of_tasks<boundary_tree_task *> &boundary_trees, bag_of_tasks<maxtree_task *> &maxtree_dest);
void worker_search_pair(bag_of_tasks<boundary_tree_task *> &btrees_bag, bag_of_tasks<merge_btrees_task *> &merge_bag);
void worker_merge_local(bag_of_tasks<merge_btrees_task *> &merge_bag, bag_of_tasks<boundary_tree_task *> &btrees_bag);
void worker_update(bag_of_tasks<maxtree_task *> &src, bag_of_tasks<maxtree_task *> &dest, boundary_tree *global_bt);

/* ======================= implementations ================================= */


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
    if(verbose) std::cout << "wait end\n";
    uint64_t x=0;
    while(true){
        // if(verbose) std::cout << "wait empty " << x++ << "\n";
        b.wait_empty();
        if(b.is_running() && b.empty() && b.num_waiting() == num_th){
            b.notify_end();
            // if(verbose) std::cout << "end notified\n";
            break;
        }
    }
}



void read_config(char conf_name[], 
                 std::string &out_name, std::string &out_ext, uint32_t &glines, uint32_t &gcolumns, 
                 Tattribute &lambda, uint8_t &pixel_connection, bool &colored, uint32_t &num_threads,
                bool &join_image){
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
    
    join_image = false;
    if(configs->find("join_image") != configs->end()){
        if(configs->at("join_image") == "true"){
            join_image = true;
        }
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
/* check if pair of coordinates c is inside the rectangle r (starting at origin (0,0) and 
    ending in position (r.first-1, r.second-1) */
bool inside_rectangle(std::pair<uint32_t, uint32_t> c, std::pair<uint32_t, uint32_t> r){
    if(c.first >= r.first || c.first < 0 || c.second >= r.second || c.second < 0){
        return false;
    }
    return true;
}

//get a task from bag and compute maxtree of the tile of this task
void worker_maxtree_calc(bag_of_tasks<input_tile_task *> &bag_tiles, bag_of_tasks<maxtree_task *> &max_trees){
    bool got_task;
    input_tile_task *t;
    maxtree_task *mt;
    while(bag_tiles.is_running()){
        // if(verbose){
        //     std::ostringstream os("");
        //     os << "thread " << std::this_thread::get_id() << " trying to get task\n";
        //     std::string s = os.str();
        //     std::cout << s;
        // }
        got_task=bag_tiles.get_task(t);
        if(got_task){
            // if(verbose){
            //     std::ostringstream os("");
            //     os << "worker " <<  std::this_thread::get_id() << " got task " << t->i << ", " << t->j << " to calculate maxtree\n";
            //     std::string s = os.str();
            //     std::cout << s;
            // }
            mt = new maxtree_task(t);
            max_trees.insert_task(mt);
        }
        // else if(verbose){
        //     std::ostringstream os("");
        //     os << "thread " << std::this_thread::get_id() << " couldnt get task\n";
        //     std::string s = os.str();
        //     std::cout << s; 
        // }
    }
}

void worker_get_boundary_tree(bag_of_tasks<maxtree_task *> &maxtrees,
                              bag_of_tasks<boundary_tree_task *> &boundary_trees, 
                              bag_of_tasks<maxtree_task *>  &maxtree_dest){
    bool got_task;
    maxtree_task *mtt;
    boundary_tree_task *btt;
    bag_of_tasks<maxtree_task *> maxtree_aux;
    while(maxtrees.is_running()){
        got_task = maxtrees.get_task(mtt);
        if(got_task){
            // std::string stmt = std::to_string(mtt->mt->grid_i) + "," + std::to_string(mtt->mt->grid_j) +"\n";
            // stmt += mtt->mt->to_string(GLOBAL_IDX,false) + "\n";
            // std::cout << stmt;
            btt = new boundary_tree_task(mtt, std::make_pair<uint32_t,uint32_t>(0,1));
            boundary_trees.insert_task(btt);
            maxtree_dest.insert_task(mtt);
        }

    }

}
std::pair<uint32_t, uint32_t> get_task_index(boundary_tree_task *t){
    return t->index;
}


void worker_search_pair(bag_of_tasks<boundary_tree_task *> &btrees_bag, bag_of_tasks<merge_btrees_task *> &merge_bag){
    boundary_tree_task *btt, *n, *aux;
    std::pair<uint32_t, uint32_t> idx_nb;
    enum neighbor_direction nb_direction;
    enum merge_directions merge_dir;
    uint32_t new_distance;
    bool change_dir;
    std::string s;
    while(btrees_bag.is_running() || !btrees_bag.get_num_task() > 1){

        bool got = btrees_bag.get_task(btt);
        if(got){
            if(btt->nb_distance.first == 0){
                merge_dir = MERGE_VERTICAL_BORDER;
            }else if(btt->nb_distance.second == 0){
                merge_dir = MERGE_HORIZONTAL_BORDER;
            }else{
                std::cerr << "merge distance invalid: " << int_pair_to_string(btt->nb_distance) << "\n";
                exit(EXIT_FAILURE);
            }
            // s = "got tree: " + std::to_string(btt->bt->grid_i) + "," + std::to_string(btt->bt->grid_j) + 
            //     " with distance: "+ int_pair_to_string(btt->nb_distance) +"\n";
            // std::cout << s;
            if(merge_dir == MERGE_VERTICAL_BORDER){
                if(btt->bt->grid_j % int_pow(2,btt->nb_distance.second) == 0){
                    nb_direction = NB_AT_RIGHT;
                }else{
                    nb_direction = NB_AT_LEFT;
                }
            }else if(merge_dir == MERGE_HORIZONTAL_BORDER){
                if(btt->bt->grid_i % int_pow(2,btt->nb_distance.first) == 0){
                    nb_direction = NB_AT_BOTTOM;
                }else{
                    nb_direction = NB_AT_TOP;
                }
            }
            idx_nb = btt->neighbor_idx(nb_direction);
            if(inside_rectangle(idx_nb, GRID_DIMS) && inside_rectangle(btt->index, GRID_DIMS)){
                try{
                    auto got_n = btrees_bag.get_task_by_field<std::pair<uint32_t,uint32_t>>(n, idx_nb, get_task_index);
                    
                    if(!got_n){
                        btrees_bag.insert_task(btt);
                    }else if (n->nb_distance == btt->nb_distance){

                        if((merge_dir == MERGE_VERTICAL_BORDER) && (btt->bt->grid_j > n->bt->grid_j) ||
                           (merge_dir == MERGE_HORIZONTAL_BORDER) && (btt->bt->grid_i > n->bt->grid_i)){
                            // std::cout << "swap btt with n\n";
                            aux = btt;
                            btt = n;
                            n = aux;
                        }
                        
                        // s = "creating task with btt " + std::to_string(btt->bt->grid_i) + "," + std::to_string(btt->bt->grid_j) + "   ";
                        // s += "n: "  + std::to_string(n->bt->grid_i) + "," + std::to_string(n->bt->grid_j) + "distance ";
                        // s += int_pair_to_string(btt->nb_distance) +"\n";
                        // // std::cout << s;
                        auto new_merge_task = new merge_btrees_task(btt->bt, n->bt, merge_dir, btt->nb_distance);
                        merge_bag.insert_task(new_merge_task);
                    }else{
                        /* if(btt->next_merge_distance > n->next_merge_distance){
                            n->next_merge_distance *= 2;
                        }else if(n->next_merge_distance > btt->next_merge_distance){
                            btt->next_merge_distance *= 2;
                        } */
                        btrees_bag.insert_task(btt);
                        btrees_bag.insert_task(n);
                    }
                }
                catch(std::runtime_error &e){
                    std::cerr << e.what();
                    // s = "pair of " + std::to_string(btt->bt->grid_i) + "," + std::to_string(btt->bt->grid_j) + " not found ";
                    // s += std::to_string(idx_nb.first) + "," + std::to_string(idx_nb.second) + "\n";
                    // std::cout << s;
                    btrees_bag.insert_task(btt);
                }catch(std::out_of_range &r){
                    std::cerr << "try to access an out of range element\n";
                }
            }else if (inside_rectangle(btt->index, GRID_DIMS)){ // the neighbor of btt is not inside the grid
                btrees_bag.insert_task(btt);
            }else{  // the bounday tree in btt task is not inside grid
                btrees_bag.insert_task(n);
            }
            
        }
    }
    std::cout << "end worker search pair\n";
}
void worker_merge_local(bag_of_tasks<merge_btrees_task *> &merge_bag, bag_of_tasks<boundary_tree_task *> &btrees_bag){
    merge_btrees_task *mbt;
    boundary_tree_task *btt;
    boundary_tree *nbt;
    std::pair<uint32_t, uint32_t> dist;
    std::string s;
    while(merge_bag.is_running() || !merge_bag.empty()){

        bool got_mt = merge_bag.get_task(mbt);
        if(got_mt){
            // if(verbose){
            //     s = "-------------------TREE 1------------------\n";
            //     // mbt->bt1->print_tree();
            //     s+=mbt->bt1->border_to_string();
            //     s+=mbt->bt1->lroot_to_string();
            //     s+="\n-----------------------------------------------\n";
            //     s = "-------------------TREE 2------------------\n";
            //     // mbt->bt2->print_tree();
            //     s+=mbt->bt2->border_to_string();
            //     s+=mbt->bt2->lroot_to_string();
            //     s+="\n-----------------------------------------------\n";
            //     // std::cout << s;
            // }
            // if(mbt->bt1 != mbt->bt2){        
            // std::cout << s;
            // s = "task will merge: " + std::to_string(mbt->bt1->grid_i) + ", " + std::to_string(mbt->bt1->grid_j) ;
            // s += " with " + std::to_string(mbt->bt2->grid_i) + ", " + std::to_string(mbt->bt2->grid_j) + " ---";
            
            nbt = mbt->execute();

            dist.second = (mbt->bt2->grid_j - mbt->bt1->grid_j) * 2;
            dist.first = (mbt->bt2->grid_i - mbt->bt1->grid_i) * 2;
            if(dist.second >= GRID_DIMS.second){
                dist.first = 1;
                dist.second = 0;
            }else if(dist.second == 0 && dist.first >= GRID_DIMS.first){
                // std::string s = "ending  merge --- dist:" + int_pair_to_string(dist) + " " ;
                // s += std::to_string(mbt->bt1->grid_i) + ", " + std::to_string(mbt->bt1->grid_j) ;
                // s += " with " + std::to_string(mbt->bt2->grid_i) + ", " + std::to_string(mbt->bt2->grid_j) + "\n";
                // std::cout << s;
                merge_bag.notify_end();
                btrees_bag.notify_end();
            }

            // if(verbose) nbt->print_tree();
            btt = new boundary_tree_task(nbt, dist);
            // s += " task inserted with index:" + std::to_string(btt->bt->grid_i) + ", " + std::to_string(btt->bt->grid_j);
            // s += " and distance " + int_pair_to_string(btt->nb_distance) + "\n";
            // std::cout << s;
            
            btrees_bag.insert_task(btt);
        }
    }
    std::cout << "end worker local merge\n";
}


void worker_update_filter(bag_of_tasks<maxtree_task *> &src, bag_of_tasks<maxtree_task *> &dest, boundary_tree *global_bt, Tattribute lambda){
    bool got_task;
    maxtree_task *mtt;
    std::string s;
    std::cout << "worker update\n";
    while(src.is_running()){
        got_task = src.get_task(mtt);
        // s = "updating (" + std::to_string(mtt->mt->grid_i) + "," + std::to_string(mtt->mt->grid_j) + ") \n";
        // std::cout << s;
        mtt->mt->update_from_boundary_tree(global_bt);
        dest.insert_task(mtt);
        mtt->mt->filter(lambda, global_bt);
        // s = "task of grid (" + std::to_string(mtt->mt->grid_i) + "," + std::to_string(mtt->mt->grid_j) + ") update\n";
        // std::cout << s;
    }
}

int main(int argc, char *argv[]){
    vips::VImage *in;
    std::string out_name, out_ext;
    
    uint32_t glines, gcolumns;
    uint8_t pixel_connection;
    uint32_t num_th;
    bool colored;
    bool join_image;
    Tattribute lambda=2;
    maxtree *t;
    input_tile_task *tile;
    
    bag_of_tasks<input_tile_task*> bag_tiles;
    bag_of_tasks<maxtree_task*> maxtree_tiles_pre_btree, maxtree_tiles, updated_trees;
    bag_of_tasks<boundary_tree_task *> boundary_bag, boundary_bag_aux;
    bag_of_tasks<merge_btrees_task *> merge_bag;

    std::vector<std::thread*> threads_g1, threads_g2, threads_g3, threads_g4;

    verify_args(argc, argv);
    read_config(argv[2], out_name, out_ext, glines, gcolumns, lambda, pixel_connection, colored, num_th, join_image);

    GRID_DIMS = std::make_pair(glines,gcolumns);

    if (VIPS_INIT(argv[0])) { 
        vips_error_exit (NULL);
    } 
    // SEE VIPS_CONCURRENCY
    // check https://github.com/libvips/libvips/discussions/4063 for improvements on read
    in = new vips::VImage(
            vips::VImage::new_from_file(argv[1],
            VImage::option()->set ("access",  VIPS_ACCESS_SEQUENTIAL)
        )
    );
    std::cout << "number of threads "<< num_th << "\n";  
    std::cout << "start\n";
    read_sequential_file(bag_tiles, in, glines, gcolumns);
    std::cout << "bag_tiles.get_num_task:" << bag_tiles.get_num_task() << "\n";
    // bag_tiles.start();

    bag_tiles.start();
    for(uint32_t i=0; i<num_th; i++){
        threads_g1.push_back(new std::thread(worker_maxtree_calc, std::ref(bag_tiles), std::ref(maxtree_tiles_pre_btree)));
    }
    maxtree_task *mtt;
    wait_empty<input_tile_task *>(bag_tiles, num_th);
    
    for(uint32_t i=0; i<num_th; i++){
        threads_g1[i]->join();
        delete threads_g1[i];
    }
    threads_g1.erase(threads_g1.begin(),threads_g1.end());

    std::cout << "max_trees_tiles.get_num_task:" << maxtree_tiles_pre_btree.get_num_task() << "\n";
    
    maxtree_tiles_pre_btree.start();
    for(uint32_t i=0; i<num_th;i++){
       threads_g1.push_back(new std::thread( worker_get_boundary_tree, std::ref(maxtree_tiles_pre_btree), std::ref(boundary_bag), std::ref(maxtree_tiles) ));
    }
    wait_empty<maxtree_task *>(maxtree_tiles_pre_btree, num_th);

    for(uint32_t i=0; i<num_th; i++){
        threads_g1[i]->join();
        delete threads_g1[i];
        
    }
    threads_g1.erase(threads_g1.begin(),threads_g1.end());
    
    //ver possibilidade de transformar as bags em filas hierarquicas
    //task to get pairs of boundary trees.

    boundary_bag.start();
    merge_bag.start();

    for(uint32_t i=0; i<num_th;i++){
        threads_g1.push_back(new std::thread(worker_search_pair, std::ref(boundary_bag), std::ref(merge_bag) ));
        // threads_g3.push_back(new std::thread(worker_search_pair, &boundary_bag, &merge_bag ));
    }
    
    for(uint32_t i=0; i<num_th;i++){
        threads_g2.push_back(new std::thread(worker_merge_local, std::ref(merge_bag), std::ref(boundary_bag) ));
        // threads_g4.push_back(new std::thread(worker_merge_local, &merge_bag, &boundary_bag ));
    }

    for(uint32_t i=0; i<num_th; i++){
        threads_g1[i]->join();
        delete threads_g1[i];
    } 
    threads_g1.erase(threads_g1.begin(),threads_g1.end());

    for(uint32_t i=0; i<num_th; i++){
        threads_g2[i]->join();
        delete threads_g2[i];
    } 
    threads_g2.erase(threads_g2.begin(),threads_g2.end());
    std::cout << "merge done\n";
    boundary_tree_task *btree_final_task;
    boundary_bag.get_task(btree_final_task);
    // std::cout << btree_final_task->bt->grid_i << "," << btree_final_task->bt->grid_j << "\n";
    
    maxtree_tiles.start();
    for(uint32_t i = 0; i < num_th; i++){
        std::cout << "Creating thread\n";
        threads_g3.push_back(new std::thread(worker_update_filter, std::ref(maxtree_tiles), std::ref(updated_trees),  btree_final_task->bt, lambda));
    }
    // std::cout << "waiting end of update\n";
    wait_empty<maxtree_task*>(maxtree_tiles, num_th);

    for(uint32_t i=0; i < num_th; i++){
        threads_g3[i]->join();
        delete threads_g3[i];
    }
    // std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>> update done <<<<<<<<<<<<<<<<<<<<<<<<<<<\n";

    // vips::VImage end_img;
    // end_img.new_from_memory_copy(in->data(), sizof(Tpixel_value), in->width(), in->height(), 1,  VIPS_FORMAT_UCHAR);
    
    // std::vector<Tpixel_value> data(in->width()*in->height(), 0);
    // input_tile_task aux;
    updated_trees.start();
    maxtree *final_image = new maxtree(in->height(),in->width(),0,0);
    final_image->get_data()->resize(in->height()*in->width());
    while(!updated_trees.empty()){
        bool got = updated_trees.get_task(mtt);
        if(got){
            // std::cout << mtt->mt->grid_i << "," << mtt->mt->grid_j << "\n";
            // mtt->mt->filter(lambda, btree_final_task->bt);
            std::string name = out_name + "_" + std::to_string(mtt->mt->grid_i) + "-" + std::to_string(mtt->mt->grid_j) + "." + out_ext;
            mtt->mt->save(name);
            if(join_image){
                for(int n = 0; n < mtt->mt->get_size(); n++){
                    maxtree_node *pix = mtt->mt->at_pos(n);
                    final_image->set_pixel(pix, pix->global_idx);
                }
            }
        }
    }
    if(join_image){
        final_image->save(out_name+"."+out_ext);
    }
}