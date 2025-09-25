#include <unordered_map>
#include <string>
#include <tuple>
#include <mutex>
#include <vector>
#include <cinttypes>
#include <sysexits.h>

#include "utils.hpp"
#include "maxtree_node.hpp"
#include "boundary_tree.hpp"

#ifndef __MAXTREE_HPP__
#define __MAXTREE_HPP__

#define INQUEUE -2

class component{
    private:
        std::vector<int> pixels;
        int parent;
        int attribute;
        std::vector<Tpixel_value> adjacents;


    public:
        component(uint64_t id = -1, std::vector<int> p = std::vector<int>(), int parent = -1, int attribute=0);
        std::string to_string();
        void insert_pixel(int p);
        int id;
        std::vector<int> get_pixels_index();
        //compare with component
/* 
        bool operator>(const component &r);
        bool operator<(const component &r);
        bool operator==(const component &r);

        //compare with integer
        bool operator>(const int &r);
        bool operator<(const int &r);
        bool operator==(const int &r);
 */

};

class maxtree{
    private:
        // std::unordered_map<int, maxtree_node*> *data;
        maxtree_node *root;
        
        std::vector<maxtree_node *> *data;
        std::vector<maxtree_node *> *levelroots;
        //std::unordered_map<Tpixel_value, std::vector<component>> components;
        //std::unordered_map<Tpixel_value, std::mutex> threshold_locks;
        //std::mutex data_lock;
        
        std::vector<bool> *tile_borders;

        int flood(int lambda, maxtree_node *r, std::vector<std::deque<maxtree_node*>> *hqueue, 
            std::vector<maxtree_node *> *levelroot, std::vector<maxtree_node*> &S);

        
    public:
        
        uint32_t h;
        uint32_t w;
        uint32_t grid_i;
        uint32_t grid_j;
        maxtree(uint32_t h, uint32_t w, uint32_t grid_i=-1, uint32_t grid_j=-1);
        // maxtree(std::unordered_map<int, maxtree_node*> *data, int32_t h, int32_t w);
        maxtree(std::vector<maxtree_node*> *data, uint32_t h, uint32_t w, uint32_t grid_i=-1, uint32_t grid_j=-1);
        maxtree(std::vector <bool> borders, uint32_t h, uint32_t w, uint32_t grid_i=-1, uint32_t grid_j=-1);
        maxtree(std::vector<maxtree_node*> *data, std::vector<bool> borders, uint32_t h, uint32_t w, uint32_t grid_i=-1, uint32_t grid_j=-1);
        
        uint64_t index_of(uint32_t i, uint32_t j);
        std::tuple<uint32_t,uint32_t> lin_col(uint64_t index);
        maxtree_node *at_pos(int64_t h, int64_t w);
        maxtree_node *at_pos(int64_t index);

        maxtree_node *get_parent(uint64_t node_idx);

        void fill_from_VImage(vips::VImage &img, uint32_t global_nlines=0, uint32_t global_ncols=0);
        void fill_from_VRegion(vips::VRegion &reg_in, uint32_t base_h, uint32_t base_w,
                               uint32_t l_tiles, uint32_t c_tiles);
        void save(std::string name, enum maxtee_node_field f = LABEL);

        void insert_component(std::vector<int> component, int64_t parent, Tpixel_value threshold, uint64_t id=-1);
        void insert_component(component c, Tpixel_value threshold);
        
        std::vector<component> components_at(Tpixel_value threshold);
        std::vector<Tpixel_value> all_thresholds();
        std::vector<maxtree_node *> *get_levelroots();
        maxtree_node *get_levelroot(maxtree_node *n);
        maxtree_node *get_levelroot(int64_t idx);


        boundary_tree *get_boundary_tree(uint8_t connectivity=4);

        /* Update maxtree attribute and global parent given a boundary tree
        this must be used after compress_path of bt */
        void update_from_boundary_tree(boundary_tree *bt);
        boundary_tree *get_boundary_tree_no_overlap(uint8_t connectivity=4);
        
        std::string to_string(enum maxtee_node_field field = PARENT,bool colored = true, uint8_t spaces = 5, uint8_t decimal = 0);
        std::string string_borders();
        
        /* filter a elements using boundary tree */
        void filter(Tattribute a, boundary_tree *bt);
        
        void filter(Tattribute a);

        uint64_t get_size();
        
        // std::unordered_map<int, maxtree_node*> *get_data();

        std::vector<maxtree_node*> *get_data();
        std::vector<maxtree_node*> get_neighbours(uint64_t pixel, uint8_t con=4);
        //std::vector<maxtree_node*> get_neighbours(int l, int c, int con=4);
        
        void compute_sequential_iterative();
        void compute_sequential_recursive(int gl=256);

};

#endif