#include "tasks.hpp"

 bool operator<(comparable_task &l, comparable_task &r){
    return l.size() < r.size();
}

 bool operator>(comparable_task &l, comparable_task &r){
    return l.size() > r.size();
}

 bool operator==(comparable_task &l,  comparable_task &r){
    return l.size() == r.size();
}
// Input tile task
input_tile_task::input_tile_task(uint32_t i, uint32_t j, uint32_t nb_rt, uint32_t nb_rl){
    this->i = i;
    this->j = j;
    this->tile_columns = 0;
    this->tile_lines = 0;
    this->reg_left = 0;
    this->reg_top = 0;
    this->noborder_rt = nb_rt;
    this->noborder_rl = nb_rl;
}

input_tile_task::input_tile_task(uint32_t i, uint32_t j){
    this->i = i;
    this->j = j;
    this->tile_columns = 0;
    this->tile_lines = 0;
    this->reg_left = 0;
    this->reg_top = 0;
    this->noborder_rt = 0;
    this->noborder_rl = 0;
}

uint64_t input_tile_task::size(){
    return this->tile_columns * this->tile_lines;
}

void input_tile_task::prepare(vips::VImage *img, uint32_t glines, uint32_t gcolumns){
                         
    uint32_t h,w;
    h=img->height();
    w=img->width();
    
    uint32_t h_trunc = h/glines;
    uint32_t w_trunc = w/gcolumns;

    uint32_t num_h_ceil = h%glines;
    uint32_t num_w_ceil = w%gcolumns;


    bool left, top, right, bottom;
    // uint32_t x = 0;
    uint32_t lines_inc, columns_inc; // original tiles (without borders) size variables
    //uint32_t reg_top, reg_left, tile_lines, tile_columns; // tiles used by algorithm (with border) size variables

    if(this->i < num_h_ceil){
        noborder_rt = this->i * (h_trunc+1);
    }else{
        noborder_rt = num_h_ceil + this->i * h_trunc;
    }

    if(this->j < num_w_ceil){
        noborder_rl= this->j * (w_trunc+1);
    }else{
        noborder_rl = num_w_ceil + this->j * w_trunc;
    }

    std::cout << this->i << ", " << this->j << " real top:" << noborder_rt << " real left: " << noborder_rl << "\n";

    std::vector<bool> borders(4,false);
    
    lines_inc = this->i < num_h_ceil ? h_trunc +1 : h_trunc;

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
    
    borders.at(LEFT_BORDER) = false;
    borders.at(RIGHT_BORDER) = false;
    columns_inc = this->j < num_w_ceil ? w_trunc+1 : w_trunc;
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
    this->tile = new maxtree(borders, this->tile_lines, this->tile_columns, this->i, this->j);

    // prepare only the region of interest in this task to be read;
    
}

void input_tile_task::read_tile(vips::VImage *img){
    uint32_t h,w;
    h=img->height();
    w=img->width();
    
    vips::VRegion reg = img->region(this->reg_left, this->reg_top, this->tile_columns, this->tile_lines);
    reg.prepare(this->reg_left, this->reg_top, this->tile_columns, this->tile_lines);
    
    this->tile->fill_from_VRegion(reg, this->reg_top, this->reg_left, h, w);
    vips_region_invalidate(reg.get_region());

}



// maxtree_task class

/* maxtree_task::maxtree_task(input_tile_task *t, bool copy){
    if(copy){
        std::cerr << "not ready yet\n creating a maxtreetask with pointer instead of a copy\n";
        this->mt = t->tile;
    }else{
        this->mt = t->tile;
    }
    this->mt->compute_sequential_recursive();
} */

uint64_t maxtree_task::size(){
    return this->mt->get_size();
}


//boundary_task class

boundary_tree_task::boundary_tree_task(maxtree_task *t){
    this->bt = t->mt->get_boundary_tree();
}


uint64_t boundary_tree_task::size(){
    return this->bt->boundary_tree_lroot->size();
}