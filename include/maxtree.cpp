#include "maxtree.hpp"


component::component(uint64_t id, std::vector<int> p, int parent, int attr){
    this->pixels = p;
    this->parent = parent;
    this->attribute = attr;
    this->id = id;
}

std::vector<int> component::get_pixels_index(){
    return this->pixels;
}

std::string component::to_string(){
    std::string s;
    s+= "id: "+ std::to_string(this->id) +" parent: "+ std::to_string(this->parent) + " pixels: ";
    for(auto p: this->pixels){
        s += std::to_string(p) + " ";
    }
    return s;
}

void component::insert_pixel(int p){
    this->pixels.push_back(p);
}

maxtree::maxtree(uint32_t h, uint32_t w, uint32_t grid_i, uint32_t grid_j){
    this->h = h;
    this->w = w;
    this->grid_i = grid_i;
    this->grid_j = grid_j;
    // this->data = new std::unordered_map<int, maxtree_node*>();
    this->data = new std::vector<maxtree_node*>();
    this->levelroots = new std::vector<maxtree_node*>();
    this->tile_borders = new std::vector<bool>(4, false);
}

// maxtree::maxtree(std::unordered_map<int, maxtree_node*> *data, int32_t h, int32_t w){
maxtree::maxtree(std::vector<maxtree_node*> *data, uint32_t h, uint32_t w, uint32_t grid_i, uint32_t grid_j){
    this->h = h;
    this->w = w;
    this->grid_i = grid_i;
    this->grid_j = grid_j;
    this->data = data;
    this->levelroots = new std::vector<maxtree_node*>();
    this->tile_borders = new std::vector<bool>(4, false);
}


maxtree::maxtree(std::vector <bool> borders, uint32_t h, uint32_t w, uint32_t grid_i, uint32_t grid_j){
    this->h = h;
    this->w = w;
    this->grid_i = grid_i;
    this->grid_j = grid_j;
    // this->data = new std::unordered_map<int, maxtree_node*>();
    this->data = new std::vector<maxtree_node*>();
    this->levelroots = new std::vector<maxtree_node*>();
    this->tile_borders = new std::vector<bool>(borders);
}

maxtree::maxtree(std::vector<maxtree_node*> *data, std::vector<bool> borders, uint32_t h, uint32_t w, uint32_t grid_i, uint32_t grid_j){
    this->h = h;
    this->w = w;
    this->grid_i = grid_i;
    this->grid_j = grid_j;
    this->data = data;
    this->tile_borders = new std::vector<bool>(borders);
    this->levelroots = new std::vector<maxtree_node*>();
}

uint64_t maxtree::index_of(uint32_t l, uint32_t c){
    return l * this->w + c;
}

std::tuple<uint32_t, uint32_t> maxtree::lin_col(uint64_t index){
    return std::make_tuple(index / this->w, index % this->w);
}

maxtree_node *maxtree::at_pos(int64_t l, int64_t c){
    int32_t idx = this->index_of(l, c); 
    return this->data->at(idx);
}

maxtree_node *maxtree::at_pos(int64_t index){
    if(this->data->size() > index){
        return this->data->at(index);
    }
    return NULL;
}

void maxtree::set_pixel(maxtree_node *p, int64_t idx){
    if(idx < this->data->size()){
        this->data->at(idx) = p;
    }
}
// void maxtree::set_pixel(maxtree_node &p, int64_t idx){
//     maxtree_node *new_pix;
//     if(idx < this->data->size()){
//         if(this->data->at(idx) == NULL){
//             new_pix = new maxtree_node(p.gval, idx, p.global_idx, p.attribute, p.global_parent);
//         }else{
//             new_pix = this->data->at(idx);
//             new_pix->gval = p.gval;
//             new_pix->idx = idx;
//             new_pix->global_idx = p.global_idx;
//             new_pix->attribute = p.attribute;
//             new_pix->global_parent = p.global_parent;
//         }
//         new_pix->attr_final = p.attr_final;
//         new_pix->label = p.label;
//         new_pix->labeled = p.labeled;
//         new_pix->parent = p.parent;
//     }

// }

maxtree_node *maxtree::get_parent(uint64_t node_idx){
    if(node_idx < 0){
        return NULL;
    }
    maxtree_node *node  = this->at_pos(node_idx);
    if(node->parent < 0){
        return NULL;
    }
    return this->at_pos(node->parent);
}

// std::unordered_map<int, maxtree_node*> *maxtree::get_data(){
std::vector<maxtree_node*> *maxtree::get_data(){
    return this->data;
}

uint64_t maxtree::get_size(){
    return this->data->size();
}

std::vector<maxtree_node *> *maxtree::get_levelroots(){
    return this->levelroots;
}


void maxtree::fill_from_VImage(vips::VImage &img_in, uint32_t global_nlines, uint32_t global_ncols){
    this->h = img_in.height();
    this->w = img_in.width();
    vips::VImage img = img_in.copy_memory();
    auto img_pels = img.get_image();
    
    char aux_enum_c[][50] = {"VIPS_FORMAT_UCHAR", "VIPS_FORMAT_CHAR", "VIPS_FORMAT_USHORT", "VIPS_FORMAT_SHORT", 
         "VIPS_FORMAT_UINT", " VIPS_FORMAT_INT", " VIPS_FORMAT_FLOAT", " VIPS_FORMAT_COMPLEX", " VIPS_FORMAT_DOUBLE", 
         "VIPS_FORMAT_DPCOMPLEX", "VIPS_FORMAT_LAST"};
    if(verbose){
        std::cout << "pixel format: ";
        if (img_pels->BandFmt > 0){
            std::cout << aux_enum_c[img_pels->BandFmt];
        }else {
            std::cout << "VIPS_FORMAT_NOTSET";
        }
        std::cout << "\n";
    }
    
    for(int l = 0; l < this->h; l++){
        for(int c = 0; c < this->w; c++){
            int x = this->index_of(l, c);
            VipsPel *vpel = VIPS_IMAGE_ADDR(img_pels, c, l);
            this->data->push_back(new maxtree_node((*vpel), x));
        }
    }
}


void maxtree::fill_from_VRegion(vips::VRegion &reg_in, uint32_t base_h, uint32_t base_w, 
                                uint32_t l_tiles, uint32_t c_tiles){
    VipsRegion *c_region = reg_in.get_region();
    uint64_t global_idx;
    Tattribute attr_ini;
    this->h = vips_region_height(c_region);
    this->w = vips_region_width(c_region);
    
    
    for(int l = 0; l < this->h; l++){
        for(int c = 0; c < this->w; c++){
/*             if(verbose)
                std::cout << "accessing:" << "(" << l << ", " << c << ")\n"; */
            int x = this->index_of(l, c);
            //VipsPel *vpel__ = VIPS_IMAGE_ADDR(c_region, c, l);
            global_idx = ((base_h+l) * c_tiles) + (c+base_w);
/*             if(verbose)
                std::cout << "local:(" << l << ", " << c << ") Global:(" << l+base_h << ", "<< c+base_w << ")\n"; */
            VipsPel *vpel = VIPS_REGION_ADDR(c_region, c+base_w, l+base_h);
            /* check if it is a border pixel, so this attr should not be computed */
            if((this->tile_borders->at(LEFT_BORDER)  && c == 0)           ||
              (this->tile_borders->at(RIGHT_BORDER)  && c == this->w - 1) ||
              (this->tile_borders->at(TOP_BORDER)    && l == 0)           ||
              (this->tile_borders->at(BOTTOM_BORDER) && l == this->h - 1)){
                attr_ini = Tattr_NULL;
            }else{
                attr_ini = Tattr_default;
            }
            this->data->push_back(new maxtree_node((*vpel), x, global_idx, attr_ini));
        }
    }
} 

void maxtree::compute_sequential_iterative(){
    std::priority_queue<maxtree_node*, std::vector<maxtree_node*> , cmp_maxtree_nodes> pixel_pq;
    std::stack<maxtree_node*> pixel_stack;
    maxtree_node *xm, *nextpix, *p;
    uint64_t idx=0;
    

    xm = min_gval(this->get_data());
    pixel_pq.push(xm);
    xm->parent = INQUEUE;
    pixel_stack.push(xm);
    nextpix = xm;
    do{
        p = nextpix;
        auto N = this->get_neighbours(p->idx);
        for(auto q: N){
            if(q->parent == -1){// if q not visited 
                q->parent = INQUEUE;
                pixel_pq.push(q);
                if(q->gval > p->gval){
                    break;
                }
            }
        }
        nextpix = pixel_pq.top();

        if(nextpix->gval > p->gval){
            pixel_stack.push(nextpix);
        }else if(nextpix->global_idx != p->global_idx) {
            std::cerr << "nextpix and p are not the same!!!! it should not happen!!!\n";
        }else{// p and nextpix are the same.
            p=pixel_pq.top();
            pixel_pq.pop(); //remove  from queue
            if(p!=pixel_stack.top()){
                p->parent = pixel_stack.top()->idx;
                //pixel_stack.top()->compute_attribute(p->attribute);
                auto top = pixel_stack.top();
                top->attribute = top->attribute + p->attribute;
                if(verbose){
                    std::cout << "computing attributes of:" << p->global_idx << "\n";
                }
            }
            
            /* if(pixel_pq.empty()){
                nextpix=p;
            }else{
                nextpix = pixel_pq.top();
            } */
            nextpix = pixel_pq.top();

            if(nextpix->gval < p->gval){
                do{
                //while(!pixel_stack.empty() && nextpix->gval < pixel_stack.top()->gval){
                    auto st = pixel_stack.top();
                    pixel_stack.pop();
                    this->levelroots->push_back(st);
                    if(!pixel_stack.empty()){
                        //pixel_stack.top()->compute_attribute(st->attribute);
                        auto top = pixel_stack.top();
                        top->attribute = top->attribute + st->attribute;
                        if(verbose){
                            std::cout << "computing attributes of:" << st->global_idx << ": <-------LEVELROOT\n";
                        }
                        st->parent = pixel_stack.top()->idx;
                    }
                }while(!pixel_stack.empty() && nextpix->gval < pixel_stack.top()->gval);
                if(pixel_stack.top()->gval < nextpix->gval){
                    pixel_stack.push(nextpix);
                } 
            }

        }

    }while(!pixel_pq.empty());

    while(!pixel_stack.empty()){
        p = pixel_stack.top();
        pixel_stack.pop();
        if(!pixel_stack.empty()){
            p->parent = pixel_stack.top()->idx;
            //pixel_stack.top()->compute_attribute(p->attribute);
            auto top = pixel_stack.top();
            top->attribute = top->attribute + p->attribute;
        }else{
            pixel_stack.push(p);
            break;
        }
    }

    maxtree_node *root = pixel_stack.top();
    this->levelroots->push_back(root);
    root->parent = -1;
    this->root = root;
}

int maxtree::flood(int lambda, maxtree_node *r, std::vector<std::deque<maxtree_node*>> *hqueue, 
                   std::vector<maxtree_node *> *levelroot, std::vector<maxtree_node*> &S){
    maxtree_node *p;
    
    while (!hqueue->at(lambda).empty()){
        p=hqueue->at(lambda).front();
        hqueue->at(lambda).pop_front();
        p->parent = r->idx;
        if(p->idx!=r->idx){
            S.push_back(p);
        }
        auto N = this->get_neighbours(p->idx);
        for(auto n: N){
            if(n->parent == -1){
                int l = n->gval;
                if(levelroot->at(l) == NULL){
                    levelroot->at(l)=n;
                }
                hqueue->at(l).push_back(n);
                n->parent = INQUEUE;
                while(l>lambda){
                    l=this->flood(l, levelroot->at(l), hqueue, levelroot, S);
                }
            }
        }
    
    }
    
    levelroot->at(lambda) = NULL;

    int lpar = lambda-1;
    while(lpar >= 0 && levelroot->at(lpar) == NULL){
        lpar--;
    }
    if(lpar != -1){
        r->parent = levelroot->at(lpar)->idx;
    }
    S.push_back(r);
    return lpar;   
}

void maxtree::compute_sequential_recursive(int gl){
    std::vector<maxtree_node*> *levelroot = new std::vector<maxtree_node*>(gl, NULL);
    std::vector<std::deque<maxtree_node*>> *hqueue = new std::vector<std::deque<maxtree_node*>>(gl);
    std::vector<maxtree_node*> S;
    maxtree_node *pmin = min_gval(this->data);
    Tpixel_value lmin = pmin->gval;
    this->root = pmin;
    hqueue->at(lmin).push_back(pmin);
    levelroot->at(lmin) = pmin;
    
    this->flood(lmin, pmin, hqueue, levelroot, S);

    this->root->parent = -1;

    for(auto p: S){
        if(p->idx !=this->root->idx){
            auto q = this->get_parent(p->idx);
            //q->compute_attribute(p->attribute);
            q->attribute = q->attribute + p->attribute;
        }
    }
/*     std::vector<Tattribute> attrs(this->get_size(), 0); // = new std::vector<Tattribute>(this->get_size(), Tattr_NULL);
    for(auto p: *this->data){
        auto lr = this->get_levelroot(p);
        if(p->idx != lr->idx){
            //attrs[p->idx] = p->attribute;
            attrs[lr->idx] += p->attribute;   
        }
    }
    for(auto p: *this->data){
        p->compute_attribute(attrs[p->idx]);
    }
         */
    delete levelroot;
    delete hqueue;

}

maxtree_node *maxtree::get_levelroot(maxtree_node *n){
    maxtree_node *n_parent;
    if(n->parent != NO_PARENT){
        n_parent = this->at_pos(n->parent);
    }else{
        return n;
    }
    while(n->gval == n_parent->gval){
        n = this->data->at(n->parent);
        if(n->parent <= -1){
            break;
        }
        n_parent = this->data->at(n_parent->parent);
    }
    return n;
}

maxtree_node *maxtree::get_levelroot(int64_t idx){
    if(idx != NO_PARENT){
        auto n = this->at_pos(idx);
        return this->get_levelroot(n);
    }
    return NULL;
}


boundary_tree *maxtree::get_boundary_tree(uint8_t connectivity){
    boundary_tree *bound_tree;
    boundary_node *bound_parent, *current;
    maxtree_node *to_merge_lr, *parent, *to_merge, *neighbour;
    uint32_t i, j, pidx; 

    if(connectivity != 4){
        std::cerr << "Connectivity different of 4-connect not implemented yet\n";
        exit(EX_SOFTWARE);
    }

    bound_tree = new boundary_tree(this->h, this->w, this->grid_i, this->grid_j);

    if(this->tile_borders->at(TOP_BORDER)){
        for(j=0; j<this->w; j++){
            to_merge = this->at_pos(1, j);    
            neighbour = this->at_pos(0, j);
            if(neighbour->gval > to_merge->gval){
                to_merge=neighbour;
            }
            to_merge_lr = this->get_levelroot(to_merge);
            // boundary_node n(to_merge, bound_tree, to_merge_lr->global_idx);
            bound_tree->insert_border_element(to_merge_lr->global_idx, TOP_BORDER);
            bound_tree->add_lroot_tree(to_merge_lr, this->get_data());
            
        }
    }

    if(this->tile_borders->at(RIGHT_BORDER)){
        for(i=0; i<this->h; i++){
            to_merge = this->at_pos(i, this->w-2);
            neighbour = this->at_pos(i, this->w-1);
            if(neighbour->gval > to_merge->gval){
                to_merge=neighbour;
            }
            
            to_merge_lr = this->get_levelroot(to_merge);
            // boundary_node n(to_merge, bound_tree, this->get_levelroot(to_merge)->global_idx);
            bound_tree->insert_border_element(to_merge_lr->global_idx, RIGHT_BORDER);
            bound_tree->add_lroot_tree(to_merge_lr, this->get_data());
        }
    }

    if(this->tile_borders->at(BOTTOM_BORDER)){
        //for(j = this->w-1; (int32_t)j >= 0; j--){
        for(j=0; j < this->w; j++){
            to_merge = this->at_pos(this->h-2, j);
            neighbour = this->at_pos(this->h-1, j);
            if(neighbour->gval > to_merge->gval){
                to_merge = neighbour;
            }
            to_merge_lr = this->get_levelroot(to_merge);
            // boundary_node n(to_merge, bound_tree, this->get_levelroot(to_merge)->global_idx);
            bound_tree->insert_border_element(to_merge_lr->global_idx, BOTTOM_BORDER);
            bound_tree->add_lroot_tree(to_merge_lr, this->get_data());
            
        }
    }

    if(this->tile_borders->at(LEFT_BORDER)){
        //for(i=this->h-1; (int32_t)i >=0; i--){
        for(i=0; i < this->h; i++){
            to_merge = this->at_pos(i, 1);
            neighbour = this->at_pos(i, 0);
            if(neighbour->gval > to_merge->gval){
                to_merge=neighbour;
            }
            to_merge_lr = this->get_levelroot(to_merge);
            // boundary_node n(to_merge, bound_tree, this->get_levelroot(to_merge)->global_idx);
            bound_tree->insert_border_element(to_merge_lr->global_idx, LEFT_BORDER);
            bound_tree->add_lroot_tree(to_merge_lr, this->get_data());
        }
    }
    return bound_tree;
}

void maxtree::update_node_attr(maxtree_node *n, boundary_tree *bt){
    auto glr = bt->get_bnode_levelroot(n->global_idx); // global levelroot
    if(glr != NULL){
        n->compute_attribute(glr->ptr_node->attribute);
        n->global_parent = glr->ptr_node->global_idx;
    }else{
        auto llr = this->get_levelroot(n);
        n->compute_attribute(llr->attribute);
        n->global_parent = llr->global_idx;
    }
}

void maxtree::update_from_boundary_tree(boundary_tree *bt){

    for(auto n: *(this->data)){
        auto llr = this->get_levelroot(n); // local levelroot
        if(!llr->attr_final){
            this->update_node_attr(llr, bt);
        }
        n->compute_attribute(llr->attribute);
    }

}



void maxtree::filter(Tattribute lambda){
    maxtree_node *aux, *p, *lr , *q, *r = this->root;

    std::vector<maxtree_node *> stack;

    if(r->attribute < lambda){
        r->set_label(Tpixel_NULL);//this->get_levelroot(this->at_pos(r->parent))->gval;
    } else {
        r->set_label(r->gval);
    }
    uint64_t tot_labeled = 1;
    uint64_t first_not_labeled=0;
    p=this->at_pos(first_not_labeled);
    while(first_not_labeled < this->get_size()){
        p=this->at_pos(first_not_labeled);
        if(p->attribute >= lambda){ 
            p->set_label(p->gval);
        }else{ // p->atrribute < lambda and p is not labeled
            lr = this->get_levelroot(p);
            if(!lr->labeled){
                if(lr->attribute >= lambda){
                    lr->set_label(lr->gval);
                }else{// levelroot has no label and must be filtered off (attribute of component is not greater than lambda)
                    while(!lr->labeled && lr->attribute < lambda){
                        stack.push_back(lr);
                        lr=this->get_parent(lr->idx);
                    }
                    //levelroot that pass on filter or labeled found in backward path
                    if(!lr->labeled){
                        lr->set_label(lr->gval);
                    }
                    while(!stack.empty()){
                        aux=stack.back();
                        stack.pop_back();
                        aux->set_label(lr->label);
                    }
                }
            }
            p->set_label(lr->label);
        }
        first_not_labeled++;
    }
}

/* 
maxtree_node *maxtree::up_tree_filter(maxtree_node *n, Tattribute lambda, boundary_tree *bt){
    maxtree_node *llr, *llr_last, *llr_par, *label_lr;
    boundary_node *glr;
    llr = this->get_levelroot(n);
    if(llr->labeled){
        return llr;
    }
    if(llr->attribute >= lambda){
        llr->set_label(llr->gval);
        label_lr =  llr;
    }else if(llr->parent != NO_PARENT){
        llr_par = this->get_levelroot(llr->parent);
        label_lr = this->up_tree_filter(llr_par, lambda, bt); 
    }else{
        label_lr = bt->up_tree_filter(llr->global_idx, lambda);
    }
    llr->set_label(label_lr->label);
    return label_lr;
}
*/
    
maxtree_node *maxtree::up_tree_filter(maxtree_node *n, Tattribute lambda, boundary_tree *bt){
    maxtree_node *llr, *llr_last, *llr_par, *label_lr;
    boundary_node *glr;
    llr = this->get_levelroot(n);
    if(llr->labeled){
        return llr;
    }
    if(llr->attribute >= lambda){
        llr->set_label(llr->gval);
        label_lr =  llr;
    }else{
        glr = bt->get_bnode_levelroot(llr->global_parent);
        if(glr!=NULL){
            label_lr = bt->up_tree_filter(glr->ptr_node->global_idx, lambda);
        }else{
            llr_par = this->get_levelroot(llr->parent);
            label_lr = this->up_tree_filter(llr_par, lambda, bt); 
        }
    }
    llr->set_label(label_lr->label);
    return label_lr;
}

void maxtree::filter(Tattribute lambda, boundary_tree *bt){
    std::vector<maxtree_node *> llr_stack;
    std::vector<boundary_node *> glr_stack;
    maxtree_node *llr, *llr_last, *llr_par, *label_lr;
    boundary_node *glr; 
    
    for(auto node: *(this->data)){
        label_lr = this->up_tree_filter(node, lambda, bt);
        node->set_label(label_lr->label);
    }
    // std::cout << "\n";
}



//this code is to "divide" strategy that performed pooly (very worst than sequential)

/* 
void maxtree::insert_component(component c, Tpixel_value gval){
    auto comps = this->components.find(gval);
    if(comps == this->components.end()){
        this->components[gval] = std::vector<component>();
    }
    this->components[gval].push_back(c);
}
 */
/* 
void maxtree::insert_component(std::vector<int> comp, int64_t parent, Tpixel_value threshold, uint64_t id){
    auto comps = this->components.find(threshold);
    if(comps == this->components.end()){
        this->threshold_locks[threshold].lock();
        if(comps == this->components.end()){
            this->components[threshold] = std::vector<component>();
        }
        this->threshold_locks[threshold].unlock();
    }

    this->threshold_locks[threshold].lock();
    component new_comp = component(id, comp, parent);
    this->components[threshold].push_back(new_comp);
    this->threshold_locks[threshold].unlock();
    
    int lower_idx = comp[0];
    for(int pidx:comp){
        if( this->data->at(pidx)->gval < this->data->at(lower_idx)->gval){
            lower_idx = pidx;
        }
    }
}

 */

std::string maxtree::string_borders(){
    std::string ret="";
    if(this->tile_borders->at(LEFT_BORDER)) ret+="Left ";
    if(this->tile_borders->at(TOP_BORDER)) ret+="Top ";
    if(this->tile_borders->at(RIGHT_BORDER)) ret+="Rigth ";
    if(this->tile_borders->at(BOTTOM_BORDER)) ret+="Bottom ";
    if(ret == "") ret+="No Borders";
    return ret;
}

/* 
std::vector<component> maxtree::components_at(Tpixel_value threshold){
    return this->components[threshold];
}
std::vector<Tpixel_value> maxtree::all_thresholds(){
    std::vector<Tpixel_value> ret;
    for(auto t: this->components){
        ret.push_back(t.first);
    }
    return ret;
}
 */
std::vector<maxtree_node*> maxtree::get_neighbours(uint64_t pixel, uint8_t con){
    std::vector<maxtree_node*> v;
    int64_t idx, pl, pc;
    std::tie(pl, pc) = this->lin_col(pixel);
    if(con == 4 || con == 8){
        if(pl >= 1){
            idx = index_of(pl-1, pc);
            v.push_back(this->data->at(idx));
        }
        if(pl < (uint64_t)h - 1){
            idx = index_of(pl+1, pc);
            v.push_back(this->data->at(idx));
        }
        if(pc >= 1){
            idx = index_of(pl, pc-1);
            v.push_back(this->data->at(idx));
        }
        if(pc < (uint64_t)w - 1){
            idx = index_of(pl, pc+1);
            v.push_back(this->data->at(idx));
        }
    }
    if(con == 8){
        if(pl >= 1 && pc >=1){
            idx = index_of(pl-1, pc-1);
            v.push_back(this->data->at(idx));
        }
        if(pl < (uint64_t)h - 1 && pc < (uint64_t)w - 1){
            idx = index_of(pl+1, pc+1);
            v.push_back(this->data->at(idx));
        }
        if(pc >= 1 && pl < (uint64_t)h - 1){
            idx = index_of(pl+1, pc-1);
            v.push_back(this->data->at(idx));
        }
        if(pc < (uint64_t)w - 1 && pl >= 1){
            idx = index_of(pl-1, pc+1);
            v.push_back(this->data->at(idx));
        }
    }
    return v;
}

void maxtree::save(std::string name, enum maxtee_node_field f){
    
    std::vector<uint8_t> data;
    if(f == LABEL){
        for(uint64_t i=0; i < this->get_size(); i++){
            data.push_back((uint8_t) this->at_pos(i)->label);
        }
    }else if(f == GVAL){
        for(uint64_t i=0; i < this->get_size(); i++){
            data.push_back((uint8_t)this->at_pos(i)->gval);
        }
    }
    vips::VImage out = vips::VImage::new_from_memory(data.data(), this->w * this->h * sizeof(uint8_t), this->w, this->h, 1, VIPS_FORMAT_UCHAR);

    out.write_to_file(name.c_str());
    
}


std::string maxtree::to_string(enum maxtee_node_field field, bool colored, uint8_t spaces, uint8_t decimal ){
    std::string r;
    //a lot of ifs with same for code inside to avoid branches inside for
    if(field == PARENT){
        for(int i=0; i < this->h; i++){
            for(int j=0; j < this->w; j++){
                auto point = this->data->at(this->index_of(i, j))->parent;
                if(colored)
                    r+=terminal_color_string(point % 8);
                r += fill(std::to_string(point), spaces-1) + " " ;
            }
            r += "\n";
        }
    }else if(field == PARENT_IJ){
        uint32_t pi, pj;
        for(int i=0; i < this->h; i++){
            for(int j=0; j < this->w; j++){
                auto point = this->data->at(this->index_of(i, j))->parent;
                std::tuple <uint32_t, uint32_t> parent_lc = this->lin_col(point);
                if(colored)
                    r+=terminal_color_string(point % 8);
                if(point != -1){
                    pi = std::get<0>(parent_lc);
                    pj = std::get<1>(parent_lc);    
                    r += fill("("+std::to_string(pi) + ", " + std::to_string(pj) + ")", (spaces-1)) + " ";
                }else{
                    r += fill("("+std::to_string(-1) + ", " + std::to_string(-1) + ")", (spaces-1)) + " ";
                }
            }
            r += "\n";
        }
    }else if(field == LABEL){
        for(int i=0; i < this->h; i++){
            for(int j=0; j < this->w; j++){
                auto dpoint = this->data->at(this->index_of(i, j))->label;
                if(colored)
                    r+=terminal_color_string(dpoint / 31);
                auto ds = double_to_string(dpoint, decimal);
                r += fill(ds, spaces-1) + " " ;
            }
            r += "\n";
        }
    }else if(field == IDX){
        for(int i=0; i < this->h; i++){
            for(int j=0; j < this->w; j++){
                auto point = this->data->at(this->index_of(i, j))->idx;
                if(colored)
                    r+=terminal_color_string(point % 8);
                r += fill(std::to_string(point), spaces-1) + " " ;
            }
            r += "\n";
        }
    }else if(field == IDX_IJ){
        for(int i=0; i < this->h; i++){
            for(int j=0; j < this->w; j++){
                auto point = this->data->at(this->index_of(i, j))->idx;
                if(colored)
                    r+=terminal_color_string(point % 8);
                r += "("+fill(std::to_string(j), (spaces-4)/2) + ", " + fill(std::to_string(j), (spaces-4)/2) + ") ";
            }
            r += "\n";
        }
    }else if(field == GLOBAL_IDX){
        for(int i=0; i < this->h; i++){
            for(int j=0; j < this->w; j++){
                auto point = this->data->at(this->index_of(i, j))->global_idx;
                if(colored)
                    r+=terminal_color_string(point % 8);
                r += fill(std::to_string(point), spaces-1) + " " ;
            }
            r += "\n";
        }
    }else if (field == GVAL){
        for(int i=0; i < this->h; i++){
            for(int j=0; j < this->w; j++){
                auto dpoint = this->data->at(this->index_of(i, j))->gval;
                if(colored)
                    r+=terminal_color_string(dpoint / 31);
                auto ds = double_to_string(dpoint, decimal);
                r += fill(ds, spaces-1) + " " ;
            }
            r += "\n";
        }
    }else if(field == LEVELROOT){
        for(int i=0; i < this->h; i++){
            for(int j=0; j < this->w; j++){
                maxtree_node *lroot = this->get_levelroot(this->data->at(this->index_of(i, j)));
                auto dpoint = lroot->idx;
                if(colored)
                    r+=terminal_color_string(dpoint % 8);
                r += fill(std::to_string(dpoint), spaces-1) + " " ;
            }
            r += "\n";
        }
    }else if(field == ATTRIBUTE){
        for(int i=0; i < this->h; i++){
            for(int j=0; j < this->w; j++){
                auto dpoint = this->data->at(this->index_of(i, j))->attribute;
                if(colored)
                    r+=terminal_color_string(dpoint % 8);
                r += fill(std::to_string(dpoint), spaces-1) + " " ;
            }
            r += "\n";
        }
    }
    if(colored)
        r+=terminal_color_string(RESET);
    return r;
}
