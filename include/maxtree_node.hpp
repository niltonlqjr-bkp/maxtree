#include <string>
#include <cinttypes>
#include <vector>
#include "const_enum_define.hpp"

#ifndef __MAXTREE_NODE_HPP__
#define __MAXTREE_NODE_HPP__

extern bool verbose;

class maxtree_node{
    public:
        int64_t parent;
        int64_t global_parent;
        Tpixel_value label; // greylevel at output
        uint64_t idx;//local index
        uint64_t global_idx;// global index
        Tpixel_value gval; // input greylevel
        bool attr_final;
        bool labeled; // already get its output label? (its greylevel in filtered image)
        Tattribute attribute;

        maxtree_node(Tpixel_value g, uint64_t i, uint64_t global_idx = 0, Tattribute attr = Tattr_default, int64_t global_parent = NO_PARENT);
        // void compute_attribute(Tattribute);
        
        // set the label of node and mark its field "labeled" as true
        void set_label(Tpixel_value l);

        //set the attribute of node to attr_val and set its field "attr_final" as true
        void compute_attribute(Tattribute attr_val);

        bool operator>(const maxtree_node &r);
        bool operator>=(const maxtree_node &r);
        bool operator<(const maxtree_node &r);
        bool operator<=(const maxtree_node &r);
        bool operator==(const maxtree_node &r);
        bool operator!=(const maxtree_node &r);
};

#endif




