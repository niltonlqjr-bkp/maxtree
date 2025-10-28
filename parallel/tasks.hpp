#include <vips/vips8>

#include "maxtree.hpp"

#ifndef __TASKS_HPP__
#define __TASKS_HPP__
// class that defines the procedures to compute a tile maxtree


class input_tile{
    public:
        uint32_t reg_left, reg_top, i, j;
        uint32_t tile_columns, tile_lines;
        uint32_t noborder_rt, noborder_rl;
        maxtree *tile;
        input_tile(uint32_t i, uint32_t j, uint32_t nb_rt, uint32_t nb_rl);
        input_tile(uint32_t i, uint32_t j);
        uint64_t size();
        // this function receive the number of lines and columns in grid
        // and the i, j position of the tile that should be read. 
        void prepare(vips::VImage *img, uint32_t glines, uint32_t gcolumns);
        void read_tile(vips::VImage *img);
        

};
 bool operator<( input_tile &l,  input_tile &r);
 bool operator>( input_tile &l,  input_tile &r);
 bool operator==( input_tile &l,  input_tile &r);

#endif