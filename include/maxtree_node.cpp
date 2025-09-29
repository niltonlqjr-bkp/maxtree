#include "maxtree_node.hpp"
#include <string>


maxtree_node::maxtree_node(Tpixel_value g, uint64_t i, uint64_t global_idx, Tattribute attr, int64_t global_parent){
    this->idx = i;
    this->gval = g;
    this->attribute = attr;
    this->label = Tattr_NULL;
    this->labeled = false;
    this->parent = NO_PARENT;
    this->global_idx = global_idx;
    this->attr_final=false;
    this->global_parent = global_parent;
}

/* void maxtree_node::compute_attribute(Tattribute a){
    this->attribute += a;
} */

/* std::string maxtree_node::to_str(){
    std::string s;
    s = "(id:"+ std::to_string(this->idx) 
        +", parent:"+ std::to_string(this->parent) 
        +", gval:"+std::to_string(this->gval)+")";
    return s;
} */

void maxtree_node::set_label(Tpixel_value l){
    this->label = l;
    this->labeled = true;
}

void maxtree_node::compute_attribute(Tattribute a){
    this->attribute = a;
    this->attr_final = true;
}

bool maxtree_node::operator>(const maxtree_node &r){
    return this->gval > r.gval;
}
bool maxtree_node::operator>=(const maxtree_node &r){
    return this->gval >= r.gval;
}
bool maxtree_node::operator<(const maxtree_node &r){
    return this->gval < r.gval;
}
bool maxtree_node::operator<=(const maxtree_node &r){
    return this->gval <= r.gval;
}
bool maxtree_node::operator==(const maxtree_node &r){
    return this->gval == r.gval;
}
bool maxtree_node::operator!=(const maxtree_node &r){
    return this->gval != r.gval;
}