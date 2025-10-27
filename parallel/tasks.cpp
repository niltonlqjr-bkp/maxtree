#include "tasks.hpp"

input_tile::input_tile(vips::VImage *img, uint32_t glines, 
                       uint32_t gcolumns, uint32_t i, uint32_t j){

    uint32_t h,w;
    h=img->height();
    w=img->width();
    
    uint32_t h_trunc = h/glines;
    uint32_t w_trunc = w/gcolumns;

    uint32_t num_h_ceil = h%glines;
    uint32_t num_w_ceil = w%gcolumns;


    bool left, top, right, bottom;
    // uint32_t x = 0;
    uint32_t noborder_rt=0, noborder_rl, lines_inc, columns_inc; // original tiles (without borders) size variables
    //uint32_t reg_top, reg_left, tile_lines, tile_columns; // tiles used by algorithm (with border) size variables


    std::vector<bool> borders(4,false);
    
    lines_inc = i < num_h_ceil ? h_trunc +1 : h_trunc;

    // check which borders the tile must have.
    this->tile_lines = lines_inc;
    this->reg_top = noborder_rt;
    if(noborder_rt > 0){ //check if there is a top border
        this->tile_lines++;
        this->reg_top--;
        borders.at(TOP_BORDER) = true;
    }
    if(noborder_rt+lines_inc < h - 1){//check if there is a bottom border
        this->tile_lines++;
        borders.at(BOTTOM_BORDER) = true;
    }
    noborder_rl=0;
    borders.at(LEFT_BORDER) = false;
    borders.at(RIGHT_BORDER) = false;
    columns_inc = j < num_w_ceil ? w_trunc+1 : w_trunc;
    this->tile_columns = columns_inc;
    this->reg_left = noborder_rl;
    if(noborder_rl > 0){//check if there is a left border
        this->tile_columns++;
        this->reg_left--;
        borders.at(LEFT_BORDER) = true;
    }
    if(noborder_rl+columns_inc < w - 1){//check if there is a right border
        this->tile_columns++;
        borders.at(RIGHT_BORDER) = true;
    }

    // create the class for tile representation
    this->tile = new maxtree(borders, this->tile_lines, this->tile_columns, i, j);

    // prepare only the region of interest in this task to be read;
    
}

void input_tile::read_tile(vips::VImage *img){
    uint32_t h,w;
    h=img->height();
    w=img->width();
    
    vips::VRegion reg = img->region(this->reg_left, this->reg_top, this->tile_columns, this->tile_lines);
    reg.prepare(this->reg_left, this->reg_top, this->tile_columns, this->tile_lines);
    
    this->tile->fill_from_VRegion(reg, this->reg_top, this->reg_left, h, w);
    vips_region_invalidate(reg.get_region());

}