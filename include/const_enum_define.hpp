#ifndef __CONST_ENUM_DEFINE_HPP__
#define __CONST_ENUM_DEFINE_HPP__

#define NO_BORDER_LEVELROOT -1
#define NO_BOUNDARY_PARENT -1
#define NO_PARENT -1
#define NULL_IDX -1
#define Tattribute uint64_t
#define Tattr_default 1
#define Tattr_NULL 0
#define Tpixel_value uint8_t
#define Tpixel_NULL 0
#define Tboundary_tree_lroot std::unordered_map<uint64_t, boundary_node*>
enum neighbor_direction{
    NB_AT_BOTTOM = 0, 
    NB_AT_RIGHT = 1, 
    NB_AT_TOP = 2, 
    NB_AT_LEFT = 3
};
static const std::vector<std::pair<int32_t, int32_t>> NEIGHBOR_DIRECTION = {
    {1,0}, {0,1}, {-1,0}, {0,-1}
};
enum merge_directions{
    MERGE_HORIZONTAL_BORDER, MERGE_VERTICAL_BORDER
};
enum boundary_tree_field{
    BOUNDARY_PARENT, 
    MAXTREE_IDX, 
    BOUNDARY_GVAL, 
    BOUNDARY_BORDER_LR, 
    BOUNDARY_GLOBAL_IDX, 
    BOUNDARY_LABEL, 
    BOUNDARY_ATTR, 
    BOUNDARY_ALL_FIELDS
};
enum maxtee_node_field {
    PARENT, 
    PARENT_IJ, 
    LABEL, 
    IDX, 
    IDX_IJ, 
    GVAL, 
    LEVELROOT, 
    ATTRIBUTE, 
    GLOBAL_IDX
};
enum borders{
    LEFT_BORDER,    TOP_BORDER,     RIGHT_BORDER,   BOTTOM_BORDER
};
static const std::vector<enum borders> TBordersVector({
    LEFT_BORDER,    TOP_BORDER,     RIGHT_BORDER,   BOTTOM_BORDER
});
static const std::vector<std::string> NamesBordersVector({
    "LEFT_BORDER",  "TOP_BORDER",   "RIGHT_BORDER", "BOTTOM_BORDER"
});

#endif