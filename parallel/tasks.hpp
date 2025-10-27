#include <vips/vips8>

#include "maxtree.hpp"

#ifndef __TASKS_HPP__
#define __TASKS_HPP__
// class that defines the procedures to compute a tile maxtree
class input_tile{
    private:
        maxtree *tile;
        uint32_t reg_left, reg_top, tile_columns, tile_lines;
    public:
        // the constructor receive the number of lines and columns in grid
        // and the i, j position of the tile that should be read. 
        input_tile(vips::VImage *img, uint32_t glines, 
                   uint32_t gcolumns, uint32_t i, uint32_t j);
        void read_tile(vips::VImage *img);

};

#endif