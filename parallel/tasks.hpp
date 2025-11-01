#include <vips/vips8>

#include "maxtree.hpp"
#include "boundary_tree.hpp"
#ifndef __TASKS_HPP__
#define __TASKS_HPP__
// classes that defines the procedures to compute a tile maxtree

class comparable_task{
    public:

        virtual uint64_t size() = 0;
};


class input_tile_task: comparable_task{
    public:
        uint32_t reg_left, reg_top, i, j;
        uint32_t tile_columns, tile_lines;
        uint32_t noborder_rt, noborder_rl;
        maxtree *tile;
        input_tile_task(uint32_t i, uint32_t j, uint32_t nb_rt, uint32_t nb_rl);
        input_tile_task(uint32_t i, uint32_t j);
        uint64_t size();
        // this function receive the number of lines and columns in grid
        // and the i, j position of the tile that should be read. 
        void prepare(vips::VImage *img, uint32_t glines, uint32_t gcolumns);
        void read_tile(vips::VImage *img);
        

};

class maxtree_task: public comparable_task{
    public:
        maxtree *mt; 
        maxtree_task(input_tile_task *t, bool copy = false);
        uint64_t size();
};

class boundary_tree_task: public comparable_task{
    public:
        boundary_tree *bt;
        boundary_tree_task(maxtree_task *t);
        uint64_t size();
        
    
};


bool operator<(comparable_task &l, comparable_task &r);
bool operator>(comparable_task &l, comparable_task &r);
bool operator==(comparable_task &l, comparable_task &r);

#endif