#include <string>
#include <cinttypes>
#include <vector>
#ifndef __MAXTREE_NODE_HPP__
#define __MAXTREE_NODE_HPP__

extern bool verbose;

enum maxtee_node_field {
    PARENT, PARENT_IJ, LABEL, IDX, IDX_IJ, GVAL, LEVELROOT, ATTRIBUTE, GLOBAL_IDX
};

#define Tattribute uint64_t
#define Tattr_default 1
#define Tattr_NULL 0
#define NO_PARENT -1
#define Tpixel_value uint8_t
#define Tpixel_NULL 0

enum borders{
    LEFT_BORDER,
    TOP_BORDER, 
    RIGHT_BORDER, 
    BOTTOM_BORDER
};

static const std::vector<enum borders> TBordersVector({
    LEFT_BORDER,
    TOP_BORDER, 
    RIGHT_BORDER, 
    BOTTOM_BORDER
});

static const std::vector<std::string> NamesBordersVector({
    "LEFT_BORDER",
    "TOP_BORDER", 
    "RIGHT_BORDER", 
    "BOTTOM_BORDER"
});

enum merge_directions{
    MERGE_HORIZONTAL,
    MERGE_VERTICAL
};


class maxtree_node{
    public:
        int64_t parent;
        int64_t global_parent;
        Tpixel_value label; // greylevel at output
        uint64_t idx;//local index
        uint64_t global_idx;// global index
        Tpixel_value gval; // input greylevel
        bool visited;
        bool labeled; // already get its output label? (its greylevel in filtered image)
        Tattribute attribute;

        maxtree_node(Tpixel_value g, uint64_t i, uint64_t global_idx = 0, Tattribute attr = Tattr_default, int64_t global_parent = NO_PARENT);
        // void compute_attribute(Tattribute);

        void set_label(Tpixel_value l);

        bool operator>(const maxtree_node &r);
        bool operator>=(const maxtree_node &r);
        bool operator<(const maxtree_node &r);
        bool operator<=(const maxtree_node &r);
        bool operator==(const maxtree_node &r);
        bool operator!=(const maxtree_node &r);
};

#endif




