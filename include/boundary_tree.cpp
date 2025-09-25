#include "boundary_tree.hpp"


/* boundary_node::boundary_node(double gval, uint64_t maxtree_idx, uint64_t origin, 
                             uint64_t global_idx, Tattribute a , int64_t bound_parent, 
                             int64_t border_lr){
    this->gval = gval;
    this->maxtree_idx = maxtree_idx;
    this->origin = origin;
    this->global_idx = global_idx;
    this->boundary_parent = bound_parent;
    this->attr = a;
    this->border_lr = border_lr;

}
 */
boundary_node::boundary_node(maxtree_node *n, boundary_tree *bound_tree_ptr, 
                             int64_t bound_parent, int64_t border_lr){
    // this->gval = n->gval;
    // this->maxtree_idx = n->idx;
    this->origin = origin;
    this->bound_tree_ptr = bound_tree_ptr;
    this->boundary_parent = bound_parent;
    // this->attr = n->attribute;
    this->border_lr = border_lr;
    this->ptr_node = n;
    this->in_lroot_tree = true;
    this->visited = false;
    
}

boundary_node::boundary_node(boundary_node *b){
    this->border_lr = b->border_lr;
    this->bound_tree_ptr = b->bound_tree_ptr;
    this->boundary_parent = b->boundary_parent;
    this->in_lroot_tree = b->in_lroot_tree;
    this->origin = b->origin;
    this->ptr_node = new maxtree_node(b->ptr_node->gval, b->ptr_node->idx, b->ptr_node->global_idx, b->ptr_node->attribute, b->ptr_node->global_parent);
    this->visited = b->visited;
}

std::string boundary_node::to_string(){
    std::ostringstream ss;
    ss << this <<"( idx:" << this->ptr_node->global_idx << ",\tbound_parent:" 
       << this->boundary_parent << ",\tborder_lr:" << this->border_lr << ","
       << "\tgval:" << (int)this->ptr_node->gval 
       << ",\tattribute:" << this->ptr_node->attribute 
       << ")";

    return ss.str();

}

/* void boundary_node::accumulate_attr(boundary_node *merged){
    this->ptr_node->compute_attribute(merged->ptr_node->attribute);
} */
/* 
void boundary_node::accumulate_attr(Tattribute value){
    this->ptr_node->attribute += value;
}
 */

boundary_tree::boundary_tree(uint32_t h, uint32_t w, uint32_t grid_i, uint32_t grid_j){
    this->h = h;
    this->w = w;
    this->grid_i = grid_i;
    this->grid_j = grid_j;
    //this->border_elements = new std::vector<std::unordered_map<uint64_t, boundary_node *>*>();
    // this->border_elements = new std::vector<std::vector<boundary_node *>*>();
    this->border_elements = new std::vector<std::vector<uint64_t>*>();
    for(auto b : TBordersVector){
        //this->border_elements->push_back(new std::unordered_map<uint64_t, boundary_node *>());
        // this->border_elements->push_back(new std::vector<boundary_node *>());
        this->border_elements->push_back(new std::vector<uint64_t>());
    }
    this->tile_borders = new std::vector<bool>();
    for(int i=0; i<4; i++){
        this->tile_borders->push_back(false);
    }
    this->boundary_tree_lroot = new std::unordered_map<uint64_t, boundary_node*>();
    //this->border_lr = -1;
}
/*
boundary_tree::boundary_tree(std::vector<std::unordered_map<uint64_t, boundary_node *>*> *border_elements, 
                             uint32_t h, uint32_t w, uint32_t grid_i, uint32_t grid_j){
    this->h = h;
    this->w = w;
    this->grid_i = grid_i;
    this->grid_j = grid_j;
    this->border_elements=border_elements;
    this->boundary_tree_lroot = new std::unordered_map<uint64_t, boundary_node*>();
    this->border_lr = -1;
}*/

boundary_tree::~boundary_tree(){
    for(uint32_t i=0;i < this->border_elements->size(); i++){
        // for(auto e: *this->border_elements->at(i)){
            // delete e;
        // }
        delete this->border_elements->at(i);
    }
    delete this->border_elements;
    for(auto n: *(this->boundary_tree_lroot)){
/*         if(delete_tree_nodes){
            delete n.second;
        } */
        this->remove_bnode_lroot_tree(n.second->ptr_node->global_idx);

    }
    delete this->boundary_tree_lroot;
}

// boundary_node *boundary_tree::insert_border_element(boundary_node &n, enum borders b){
void boundary_tree::insert_border_element(uint64_t n_idx, enum borders b){
    // boundary_node *new_n;
    // auto border = this->border_elements->at(b);
    this->tile_borders->at(b) = true;
    // new_n = new boundary_node(n);
    // new_n->bound_tree_ptr = this;
    // border->push_back(new_n);
    this->border_elements->at(b)->push_back(n_idx);
    
}

bool boundary_tree::insert_bnode_lroot_tree(boundary_node *n, bool copy){
    //if(this->boundary_tree_lroot->find(n->ptr_node->global_idx) != this->boundary_tree_lroot->end()){
    if(this->get_border_node(n->ptr_node->global_idx) != NULL){
        //if(verbose) std::cout << "fail to inset: " << n->ptr_node->global_idx << " at tree\n";
        return false;
    }
    if(copy){
        n = new boundary_node(n);
        n->bound_tree_ptr = this;
    }
    this->boundary_tree_lroot->emplace(n->ptr_node->global_idx, n);
    //if(verbose) std::cout << "node " << n->ptr_node->global_idx << " sucessfully inserted \n";
    return true;
}

bool boundary_tree::remove_bnode_lroot_tree(int64_t global_idx){
    if(this->boundary_tree_lroot->find(global_idx) != this->boundary_tree_lroot->end()){
        this->boundary_tree_lroot->erase(global_idx);
        return true;
    }
    return false;
}

void boundary_tree::add_lroot_tree(boundary_node *levelroot, bool insert_ancestors,
                                   bool copy){
    bool inserted=true;
    char __CH;
    boundary_node *parent, *current, *lroot_at_this, *parent_at_this;
    int64_t pidx;
    boundary_tree *t_levelroot;


    t_levelroot = levelroot->bound_tree_ptr; 
    lroot_at_this=this->get_border_node(levelroot->ptr_node->global_idx);
/*     if(verbose){
        std::cout << "add levelroot by boundary node:" << levelroot->ptr_node->global_idx << "\n";
        if(lroot_at_this){
            std::cout << "lroot_at_this:" << lroot_at_this->ptr_node->global_idx << "\n";
        }else{
            std::cout << "lroot_at_this: NULL\n";
        }
    } */
    if(lroot_at_this == NULL){
        inserted = this->insert_bnode_lroot_tree(levelroot,copy);
    }
    if(insert_ancestors){
        current = levelroot;
        while(current != NULL && inserted){
            //if(verbose) std::cout << " This tree: "<< this->lroot_to_string() << "\n";
            //if(verbose) std::cout << " levelroot tree: "<< t_levelroot->lroot_to_string() << "\n";
            pidx = current->boundary_parent;
            if(pidx < 0){
                break;
            }
            parent = t_levelroot->get_border_node(pidx);
            if(verbose){
                if(parent != NULL && current != NULL){
                    // std::cout << "current: " << current->ptr_node->global_idx << " parent: " << parent->ptr_node->global_idx << "\n";
                }
            }
            current = parent;
            if(parent == NULL){
                break;
            }
            if(parent!=NULL){
                inserted = this->insert_bnode_lroot_tree(parent,copy);
                
            }
        }
    }
}

void boundary_tree::add_lroot_tree(maxtree_node *levelroot, std::vector<maxtree_node*> *maxtree_data, bool insert_ancestors){
    maxtree_node *parent;
    boundary_node *current, *bound_parent;
    int64_t pidx;
    
    //if(this->boundary_tree_lroot->find(levelroot->ptr_node->global_idx) != this->boundary_tree_lroot->end()){ 
    if(this->get_border_node(levelroot->global_idx) != NULL){ 
        // levelroot found on boundary tree, the branch is already added to boundary tree
        //current = this->get_border_node(levelroot->idx);
        return;
    }else{
        current = new boundary_node(levelroot, this, -1);
        this->insert_bnode_lroot_tree(current,false);
    }

    if(insert_ancestors){
        while(current != NULL){
            pidx = maxtree_data->at(current->ptr_node->idx)->parent; // get parent idx of current boundary node
            if(pidx < 0){
                break;
            }
            /* global_pidx = maxtree_data->at(local_pidx)->idx;
            if(global_pidx < 0){
                break;
            } */
            parent = maxtree_data->at(pidx);
            if(this->boundary_tree_lroot->find(parent->global_idx) == this->boundary_tree_lroot->end()){// parent isn't in boundary tree
                bound_parent = new boundary_node(parent, this, -1); //create the parent node to add on bondary tree
                current->boundary_parent = bound_parent->ptr_node->global_idx; //vinculate the idx (used in maxtree) of parent to the current node
                if(!this->insert_bnode_lroot_tree(bound_parent)){ // try to insert parent node 
                    delete bound_parent; // if isn't possible insert node (the parent node is alredy in boundary tree) free memory of last 
                    break; // stop the insertion process (all the ancestors from current are in boundary tree)
                }
            }else{ // parent inode s in boundary tree, so it doesn't need to be inserted, just update parent relation and stop process
                current->boundary_parent = parent->global_idx;
                break;
            }
            current = this->get_border_node(parent->global_idx); // go to the parent and add its ancerstors
        }
    }
}

boundary_node *boundary_tree::get_border_node(int64_t global_idx){
    if(this->boundary_tree_lroot->find(global_idx) != this->boundary_tree_lroot->end())
        return this->boundary_tree_lroot->at(global_idx);
    return NULL;
}


/*
boundary_node *boundary_tree::get_bnode_levelroot(int64_t global_idx){
    boundary_node *n, *lr, *ant_lr;
    n = this->get_border_node(global_idx);
    
    //std::cout << "levelroot of:" << global_idx << "\n";
    if(n == NULL){
        return NULL;
    }
    // if(verbose) std::cout << "node:\n" << n->to_string() << " line: "<< __LINE__ << "\n";
    if(n->boundary_parent == NO_BOUNDARY_PARENT && n->border_lr == NO_BORDER_LEVELROOT){
        return n;
    }
    lr = this->get_border_node(n->boundary_parent);
    if(lr == NULL){
        lr = this->get_border_node(n->border_lr);
        std::cout << "par:" << n->boundary_parent << "par:" << n->boundary_parent << "\n";
        std::cout << this->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n");
        std::cout << "par:" << n->boundary_parent << "\n";
        std::cout << "n:" <<n->to_string() << "\n"; 
        // return lr;
    }
    if(lr != NULL && lr->ptr_node->gval != n->ptr_node->gval){
        return n;
    }
    if(verbose && lr) std::cout << "node:\n" << lr->to_string() << " line: "<< __LINE__ << "\n";
    while(lr != NULL && lr->ptr_node->gval == n->ptr_node->gval){
        if(lr != NULL){
            ant_lr = lr;
        }
        lr = this->get_border_node(lr->boundary_parent);
        if(lr == NULL){
            lr = this->get_border_node(lr->border_lr);
        }
        // if(verbose) if(lr!=NULL) std::cout << "node:\n" << lr->to_string() << " line: "<< __LINE__ << "\n";
    }
    return ant_lr;


}

*/
/* 
boundary_node *boundary_tree::get_bnode_levelroot(int64_t global_idx){
    boundary_node *n, *lr, *ant;
    n = this->get_border_node(global_idx);
    //std::cout << n->to_string() <<"\n";
    if(n == NULL){
        return NULL;
    }
    // if(n->border_lr == NO_BORDER_LEVELROOT){
    //     lr = this->get_border_node(n->boundary_parent);
    // }else{
    //     lr = this->get_border_node(n->border_lr);
    // }
    lr = this->get_border_node(n->boundary_parent);

    if(lr == NULL || n->ptr_node->gval != lr->ptr_node->gval){
        return n;
    }else{
        return this->get_bnode_levelroot(lr->ptr_node->global_idx);
    }
}
 */


boundary_node *boundary_tree::get_bnode_levelroot(int64_t global_idx){
    boundary_node *n, *lr, *ant;
    // std::cout << "\n____________________\n" << this->lroot_to_string() << "\n____________________\n";
    n = this->get_border_node(global_idx);
    
    if(n==NULL){
        return NULL;
    }
    //std::cout << "     " << n->to_string() << this->grid_i << " " << this->grid_j <<"\n";
    lr=this->get_border_node(n->boundary_parent);
    /* if(lr)
        std::cout << "     " << lr->to_string() << "\n"; */
    // else
    //     std::cout << "     NULL\n";
        
    if(lr != NULL && n->ptr_node->gval == lr->ptr_node->gval){
        return this->get_bnode_levelroot(lr->ptr_node->global_idx);
    }else{
        return n;
    }
}

// std::vector<boundary_node *> *boundary_tree::get_border(enum borders b){
std::vector<uint64_t> *boundary_tree::get_border(enum borders b){
    return this->border_elements->at(b);
}

uint64_t boundary_tree::get_lroot_tree_size(){
    return this->boundary_tree_lroot->size();
}

uint64_t boundary_tree::get_border_size(){
    uint64_t s = 0;
    for(enum borders b: TBordersVector){
        s += this->border_elements->at(b)->size();
    }
    return s;
}

bool boundary_tree::is_in_border(int64_t global_idx){
    auto x = this->get_border_node(global_idx);
    if(x != NULL){
        uint64_t xlidx = x->ptr_node->idx;
        uint32_t btw = this->w;
        uint32_t bth = this->h;
        std::cout << " xlidx:"<< xlidx << " btw:" << btw << " bth:" << bth << "\n";
        if(this->tile_borders->at(LEFT_BORDER) && ( xlidx % btw == 0)){
            return true;            
        }
        if(this->tile_borders->at(RIGHT_BORDER) && (xlidx % btw == btw-1)){
            return true;
        }
        if(this->tile_borders->at(TOP_BORDER) && (xlidx < btw)){
            return true;
        }
        if(this->tile_borders->at(BOTTOM_BORDER) && (xlidx > btw * (bth-1) )){
            return true;
        }
    }
    return false;
}

// void boundary_tree::change_border(std::vector<boundary_node *> *new_border, enum borders b){
void boundary_tree::change_border(std::vector<uint64_t> *new_border, enum borders b){
    delete this->border_elements->at(b);
    this->border_elements->at(b) = new_border;
    if(new_border->size() <= 0){
        this->tile_borders->at(b) = false;
    }else{
        this->tile_borders->at(b) = true;
        if(b == LEFT_BORDER || b == RIGHT_BORDER){
            this->h = new_border->size();
        }else if(b == TOP_BORDER || b == BOTTOM_BORDER){
            this->w = new_border->size();
        }
    }
}

bool boundary_tree::is_root(uint64_t n_idx){
    if(this->get_border_node(n_idx) == NULL){
        return false;
    }
    int64_t bound_par_idx = this->get_border_node(n_idx)->boundary_parent;
    if(bound_par_idx != NO_BOUNDARY_PARENT){
        return false;
    }
    return true;
}


/* ======== Based on Kazemier and Gazagnes papers*/

void boundary_tree::merge_branches_gaz(boundary_node *x, boundary_node *y, std::unordered_map<uint64_t, bool> &acc){
   
    boundary_node *z, *aux;
    Tattribute a, b;
    a = b = Tattr_NULL;
    uint64_t xidx, yidx;
    x = x->bound_tree_ptr->get_border_node(x->ptr_node->global_idx);
    y = y->bound_tree_ptr->get_border_node(y->ptr_node->global_idx);
    if(x->ptr_node->gval < y->ptr_node->gval){
        auto aux = x;
        x = y;
        y = aux;
    }
    while(y != NULL && x != y){//} && x->ptr_node->global_idx != y->ptr_node->global_idx){
        z = x->bound_tree_ptr->get_border_node(x->boundary_parent);
        xidx = x->ptr_node->global_idx;
        yidx = y->ptr_node->global_idx;
        if(z != NULL && z->ptr_node->gval >= y->ptr_node->gval){
 
            this->add_lroot_tree(z); 
            aux=this->get_border_node(x->ptr_node->global_idx);
            if(aux != NULL){
                x = aux;
            }else{
                this->add_lroot_tree(x);
                x=this->get_border_node(x->ptr_node->global_idx);
            }    
            x->ptr_node->attribute += a;
            acc[xidx] = true;
            //x->visited = true;
            x = z;
        }else{
            if(acc.find(xidx) == acc.end() || !acc[xidx]){
            //if(!x->visited){
                aux=this->get_border_node(x->ptr_node->global_idx);
                if(aux != NULL){
                    x = aux;
                }else{
                    this->add_lroot_tree(x);
                    x=this->get_border_node(x->ptr_node->global_idx);
                }
                x->border_lr = yidx;
                b = x->ptr_node->attribute + a;
                a = x->ptr_node->attribute;
                x->ptr_node->attribute = b;
                acc[xidx] = true;
                //x->visited = true;
/*                 if(!x->bound_tree_ptr->is_in_border(x->ptr_node->global_idx)){
                } */
            }
            x = y;
            y = z;
        }
    }
    if(y == NULL){
        while(x != NULL){
            xidx = x->ptr_node->global_idx;
            //if(acc.find(xidx) == acc.end() || !acc[xidx]){
            //if(!x->visited){
                x->ptr_node->attribute += a;
                acc[xidx] = true;
                //x->visited = true;
            //}
            //}
            x = x->bound_tree_ptr->get_border_node(x->boundary_parent);
        }
    }
}

bool boundary_tree::search_cicle(int64_t s){
    std::vector<int64_t> visited;

    auto n = this->get_border_node(s);
    bool cicle=false;
    while(n != NULL && !cicle){
        if(std::find(visited.begin(), visited.end(), n->ptr_node->global_idx)!=visited.end()){
            cicle = true;
            std::cout << "cicle:\n   ";
            for(auto x: visited){
                std::cout << x << " ";
            }
            std::cout << n->ptr_node->global_idx << "\n";
            
        }else{
            visited.push_back(n->ptr_node->global_idx);
        }
        n = this->get_bnode_levelroot(n->border_lr);
    }
    return cicle;

}


/* ver o merge das linhas do grid 4 e 5 na divisão 8x8*/
void boundary_tree::merge_branches(boundary_node *x, boundary_node *y, 
                                   std::unordered_map<uint64_t, bool> &accx, 
                                   std::unordered_map<uint64_t, bool> &accy){
    boundary_node *z, *thisx, *thisy, *xpar, *ypar, *xold, *yold, *thisxold, *thisyold;
    bool addx, addy;
    Tattribute a, b, carryx, carryy;
    a = b = carryx = carryy = Tattr_NULL;
    uint64_t xidx, yidx, carryyidx, carryxidx;
    // if(verbose){
    //     std::cout << "x tree:" << x->bound_tree_ptr << "\n" << x->bound_tree_ptr->lroot_to_string(BOUNDARY_ALL_FIELDS, "\n") << "\n";
    //     std::cout << "y tree:" << y->bound_tree_ptr << "\n" << y->bound_tree_ptr->lroot_to_string(BOUNDARY_ALL_FIELDS, "\n") << "\n";
    // }
    xold = x;
    yold = y;
    x = x->bound_tree_ptr->get_bnode_levelroot(x->ptr_node->global_idx);
    y = y->bound_tree_ptr->get_bnode_levelroot(y->ptr_node->global_idx);
    if(verbose){
        std::cout << "   merge nodes: x=" << x->to_string() << " with " << y->to_string() << "\n";
    }    
    while(y != NULL && x != NULL){
        xidx = x->ptr_node->global_idx;
        yidx = y->ptr_node->global_idx;
        // this->add_lroot_tree(x,false,true);
        // this->add_lroot_tree(y,false,true);
        auto insert_x = this->insert_bnode_lroot_tree(x,true);
        auto insert_y = this->insert_bnode_lroot_tree(y,true);
        if(verbose){
            std::cout << "   insert x: " << insert_x << " insert y:" << insert_y << "\n";
        }
        thisx = this->get_border_node(xidx);
        thisy = this->get_border_node(yidx);

        auto x_at_ytree = y->bound_tree_ptr->get_border_node(x->ptr_node->global_idx);
        auto y_at_xtree = x->bound_tree_ptr->get_border_node(y->ptr_node->global_idx);

        addx = addy = false;
        b = Tattr_NULL;
        if(x->ptr_node->gval == y->ptr_node->gval){
            if(verbose){
                std::cout << "   case 1\n";
                std::cout << "   x " << x->to_string() << " and y "<< y->to_string() <<" has the same gval\n";
            }
            xpar=x->bound_tree_ptr->get_bnode_levelroot(x->boundary_parent);
            ypar=y->bound_tree_ptr->get_bnode_levelroot(y->boundary_parent);

            if(accx.find(xidx) == accx.end() || !accx[xidx]){
                b+=x->ptr_node->attribute;
                carryx = x->ptr_node->attribute;
                carryxidx = xidx;
                addx = accx[xidx] = true;
            }
            if(accy.find(yidx) == accy.end() || !accy[yidx]){
                b+=y->ptr_node->attribute;
                carryy = y->ptr_node->attribute;
                carryyidx = yidx;
                addy = accy[yidx] = true;
            }
            if(verbose)
                std::cout << "      addx:" << addx << " addy:" << addy << "\n";
            if(addx && addy){
                thisx->ptr_node->attribute = b;
                thisy->ptr_node->attribute = b;
            // }else if(addx){
            //     thisy->ptr_node->attribute += carryx;
            //     carryy = Tattr_NULL;
            //     carryyidx = NULL_IDX;
            //     thisx->ptr_node->attribute = thisy->ptr_node->attribute;
            // }else if(addy){
            //     thisx->ptr_node->attribute += carryy;
            //     carryx = Tattr_NULL;
            //     carryxidx = NULL_IDX;
            //     thisy->ptr_node->attribute = thisx->ptr_node->attribute;
            }else{
                carryx = carryy = Tattr_NULL;
                carryxidx = carryyidx = NULL_IDX;;
            }
            
            if(addx || addy){
                if(verbose) std::cout << "      antes - x: "<< thisx->to_string() << " y: " << thisy->to_string() << "\n";
                if(y->ptr_node->global_idx != x->ptr_node->global_idx){
                    if(x->boundary_parent == NO_BOUNDARY_PARENT){
                        thisx->border_lr = yidx;
                    }else{
                        thisy->border_lr = xidx;
                    }
                }
                if(verbose) std::cout << "      depois - x: "<< thisx->to_string() << " y: " << thisy->to_string() << "\n";
            }
            xold=x;
            yold=y;
            x=xpar;
            y=ypar;
        }else if(x->ptr_node->gval > y->ptr_node->gval){ // >>>>>>>>>>>>>>>>>>>>> need to test this case <<<<<<<<<<<<<<<<<<<<<<<<<
            if(verbose){
                std::cout << "   case 2\n";
                std::cout << "      x " << x->to_string() << " > y "<< y->to_string() <<" gval\n";
            }
            thisyold = this->get_border_node(yold->ptr_node->global_idx);
            if(accx.find(xidx) == accx.end() || !accx[xidx]){
                carryx = x->ptr_node->attribute;
                carryxidx = xidx;
                accx[xidx] = true;
            }
            if(verbose) std::cout << "      antes - x: "<< thisx->to_string() << " y: " << thisy->to_string() << "\n";
            thisx->ptr_node->attribute += carryy;//yold->ptr_node->attribute ;
            if(verbose) std::cout << "      depois - x: "<< thisx->to_string() << " y: " << thisy->to_string() << "\n";

            xpar=x->bound_tree_ptr->get_bnode_levelroot(x->boundary_parent);
            if(!xpar || xpar->ptr_node->gval < y->ptr_node->gval){ 
                thisx->border_lr = y->ptr_node->global_idx;
                
            }
            bool update_parent = true;
            auto yold_treex = x->bound_tree_ptr->get_border_node(yold->ptr_node->global_idx);
            if(yold_treex != NULL && yold_treex->ptr_node->parent == xold->ptr_node->idx){
                update_parent = false;
            }
            auto xold_treey = x->bound_tree_ptr->get_border_node(xold->ptr_node->global_idx);
            if(xold_treey != NULL && xold_treey->ptr_node->parent == yold->ptr_node->idx){
                update_parent = false;
            }
            if(update_parent){
                thisyold->border_lr = x->ptr_node->global_idx;
            }
            xold=x;
            x=xpar;
        }else if(x->ptr_node->gval < y->ptr_node->gval){ // >>>>>>>>>>>>>>>>>>>>> need to test this case <<<<<<<<<<<<<<<<<<<<<<<<<
            if(verbose){
                std::cout << "   case 3\n";
                std::cout << "      x " << x->to_string() << " < y "<< y->to_string() <<" gval\n";
            }
            thisxold = this->get_border_node(xold->ptr_node->global_idx);
            if(accy.find(yidx) == accy.end() || !accy[yidx]){
                carryy = y->ptr_node->attribute;
                carryyidx = yidx;
                accy[yidx] = true;
            }
            if(verbose) std::cout << "      antes - x: "<< thisx->to_string() << " y: " << thisy->to_string() << "\n";
            thisy->ptr_node->attribute += carryx;//xold->ptr_node->attribute;
            if(verbose) std::cout << "      depois - x: "<< thisx->to_string() << " y: " << thisy->to_string() << "\n";
        
            ypar=y->bound_tree_ptr->get_bnode_levelroot(y->boundary_parent);
            if(!ypar || ypar->ptr_node->gval < x->ptr_node->gval) { 
                thisy->border_lr = x->ptr_node->global_idx;
                
            }

            bool update_parent = true;
            auto yold_treex = x->bound_tree_ptr->get_border_node(yold->ptr_node->global_idx);
            if(yold_treex != NULL && yold_treex->ptr_node->parent == xold->ptr_node->idx){
                update_parent = false;
            }
            auto xold_treey = x->bound_tree_ptr->get_border_node(xold->ptr_node->global_idx);
            if(xold_treey != NULL && xold_treey->ptr_node->parent == yold->ptr_node->idx){
                update_parent = false;
            }
            if(update_parent){
                thisxold->border_lr = y->ptr_node->global_idx;
                
            }
            yold=y;
            y=ypar;    
        }        
    }

    while(y!=NULL){
        yidx = y->ptr_node->global_idx;
        thisy = this->get_border_node(yidx);
        if(thisy == NULL){
            this->add_lroot_tree(y,false,true);
            thisy = this->get_border_node(yidx);
        }
        thisy->ptr_node->attribute += carryx;

        y=y->bound_tree_ptr->get_bnode_levelroot(y->boundary_parent);
    }
    while(x!=NULL){
        xidx = x->ptr_node->global_idx;
        thisx = this->get_border_node(xidx);
        if(thisx == NULL){
            this->add_lroot_tree(x,false,true);
            thisx = this->get_border_node(xidx);
        }
        thisx->ptr_node->attribute += carryy;
        x=x->bound_tree_ptr->get_bnode_levelroot(x->boundary_parent);
    }
}

// erro aqui
//erro
void boundary_tree::combine_lroot_trees(boundary_tree *t1, boundary_tree *t2){
    boundary_node *n, *insert_node;

    for(auto node: *t1->boundary_tree_lroot){
        n = this->get_border_node(node.first);
        insert_node = new boundary_node(node.second);
        insert_node->bound_tree_ptr = this;
        //std::cout << "insert:" << insert_node->to_string() << "<--------------------------------------------------------------------------\n";
        // if(n && n->ptr_node->gval == node.second->ptr_node->gval){
        //     insert_node->boundary_parent = n->boundary_parent;
        //     insert_node->ptr_node->attribute = n->ptr_node->attribute;
        // }
        this->insert_bnode_lroot_tree(insert_node,true);

    }
    for(auto node: *t2->boundary_tree_lroot){
        n = this->get_border_node(node.first);
        insert_node = new boundary_node(node.second);
        insert_node->bound_tree_ptr = this;
        //std::cout << "insert:" << insert_node->to_string() << "<--------------------------------------------------------------------------\n";
        // if(n && n->ptr_node->gval == node.second->ptr_node->gval){
        //     insert_node->boundary_parent = n->boundary_parent;
        //     insert_node->ptr_node->attribute = n->ptr_node->attribute;
        // }
        this->insert_bnode_lroot_tree(insert_node,true);
    }
}

void boundary_tree::combine_borders(boundary_tree *t1, boundary_tree *t2, enum merge_directions d){
    // std::vector<boundary_node *> *v_t1, *v_t2, *new_border;
    std::vector<uint64_t> *v_t1, *v_t2, *new_border;
    enum borders first_border, second_border, third_border, fourth_border;
    uint64_t ini ;
    // boundary_node *node, *node_tree, *new_node;

    if(d == MERGE_VERTICAL){
        first_border=LEFT_BORDER; second_border=RIGHT_BORDER; 
        third_border=TOP_BORDER; fourth_border=BOTTOM_BORDER;
    }else if(d == MERGE_HORIZONTAL){
        first_border=TOP_BORDER; second_border=BOTTOM_BORDER; 
        third_border=LEFT_BORDER; fourth_border=RIGHT_BORDER;
    }
    bool has_border[] = {false, false, false, false};
    //first_border comes from t1, second_border comes from t2, third_border (and fourth) are merged first half from t1, second half from t2
    
    /*copy first border from t1*/
    // new_border = new std::vector<boundary_node *>();
    new_border = new std::vector<uint64_t>();
    v_t1 = t1->get_border(first_border);
    
    for(uint64_t i = 0; i < v_t1->size(); i++){
        // new_node = new boundary_node(v_t1->at(i));
        // new_node->bound_tree_ptr = this;
        // new_border->push_back(new_node);
        new_border->push_back(v_t1->at(i));
    }
    this->change_border(new_border, first_border);

    /*copy second border from t2*/
    // new_border = new std::vector<boundary_node *>();
    new_border = new std::vector<uint64_t>();
    v_t2 = t2->get_border(second_border);
    
    for(uint64_t i = 0; i < v_t2->size(); i++){
        // new_node = new boundary_node(v_t2->at(i));
        // new_node->bound_tree_ptr = this;
        // new_border->push_back(new_node);
        new_border->push_back(v_t2->at(i));
    }
    this->change_border(new_border, second_border);
    
    /*merge third border, first half from t1, second from t2*/
    // new_border = new std::vector<boundary_node *>();
    new_border = new std::vector<uint64_t>();
    v_t1 = t1->get_border(third_border);
    v_t2 = t2->get_border(third_border);
    
    for(uint64_t i = 0; i < v_t1->size(); i++){
        // node = v_t1->at(i);
        // node_tree = t1->get_border_node(node->boundary_parent);
        // if(node_tree == NULL){
        //     node_tree = t2->get_border_node(node->boundary_parent);
        // }
        // if(node_tree == NULL){
        //     node_tree = t1->get_border_node(node->ptr_node->global_idx);   
        // }
        // if(node_tree == NULL){
        //     node_tree = t2->get_border_node(node->ptr_node->global_idx);
        // }
        // if(node_tree == NULL){
            
        //     exit(EXIT_FAILURE);
        // }
        // if(node == node_tree){
            
        //     exit(EXIT_FAILURE);
        // }
        // node->boundary_parent = node_tree->ptr_node->global_idx;
        // //std::cout << "node idx:" << node->ptr_node->global_idx << " new parent:" <<node->boundary_parent<< "\n";
        // new_node = new boundary_node(node);
        // new_node->bound_tree_ptr = this;
        // new_border->push_back(node);
        new_border->push_back(v_t1->at(i));
    }

    ini = 0 + t2->tile_borders->at(first_border) + t1->tile_borders->at(second_border);
    for(uint64_t i = ini; i < v_t2->size(); i++){
        // new_node = new boundary_node(v_t2->at(i));
        // new_node->bound_tree_ptr = this;
        // new_border->push_back(new_node);
        new_border->push_back(v_t2->at(i));
    }
    this->change_border(new_border, third_border);
    
    // new_border = new std::vector<boundary_node *>();
    new_border = new std::vector<uint64_t>();
    v_t1 = t1->border_elements->at(fourth_border);
    v_t2 = t2->border_elements->at(fourth_border);

    for(uint64_t i = 0; i < v_t1->size(); i++){
        // node = v_t1->at(i);
        // node_tree = t1->get_border_node(node->boundary_parent);
        // if(node_tree == NULL){
        //     node_tree = t2->get_border_node(node->boundary_parent);
        // }
        // if(node_tree == NULL){
        //     node_tree = t1->get_border_node(node->ptr_node->global_idx);   
        // }
        // if(node_tree == NULL){
        //     node_tree = t2->get_border_node(node->ptr_node->global_idx);
        // }
        // if(node_tree == NULL){
        //     std::cerr << "VAI PRA PUTA QUE PARIU!!! ESSA MERDA NAO TA DANDO CERTO PORRA!\n";
        //     exit(EXIT_FAILURE);
        // }
        // if(node == node_tree){
        //     std::cerr << "isso nao deveria acontecer\n";
        //     exit(EXIT_FAILURE);
        // }
        // node->boundary_parent = node_tree->ptr_node->global_idx;
        // //std::cout << "node idx:" << node->ptr_node->global_idx << " new parent:" <<node->boundary_parent<< "\n";
        // new_node = new boundary_node(node);
        // new_node->bound_tree_ptr = this;
        // new_border->push_back(node);
        new_border->push_back(v_t1->at(i));
    }

    ini = 0 + t2->tile_borders->at(first_border) + t1->tile_borders->at(second_border);

    for(uint64_t i = ini; i < v_t2->size(); i++){
        // new_node = new boundary_node(v_t2->at(i));
        // new_node->bound_tree_ptr = this;
        // new_border->push_back(new_node);
        new_border->push_back(v_t2->at(i));
    }
    this->change_border(new_border, fourth_border);
    
}

boundary_tree *boundary_tree::get_copy(bool deep_copy){
    boundary_tree *copy;
    copy = new boundary_tree(this->h, this->w, this->grid_i, this->grid_j);
    /* copy all borders and insert its subtrees*/
    for(borders b: TBordersVector){
/*         for(boundary_node *n: *(this->border_elements->at(b))){
            if(deep_copy){
                auto new_node = new boundary_node(n);
                copy->insert_border_element(*new_node, b);
            }else{
                copy->insert_border_element(*n, b);
            }
        } */
        for(auto n: *(this->border_elements->at(b))){
            copy->insert_border_element(n,b);
        }
    }
    
    for(auto pair: *(this->boundary_tree_lroot)){
        boundary_node *n;
        if(deep_copy){
            n = new boundary_node(pair.second);
        }else{
            n = new boundary_node(*(pair.second));
        }
        if(!copy->insert_bnode_lroot_tree(n,true)){
            delete n;
        }
    }
    return copy;
}

//ver o por que o get_border_node devolve um no que a árvore não é a mesma que chamou o merge.

boundary_tree *boundary_tree::merge(boundary_tree *t, enum merge_directions d, uint8_t connection){
    if(connection != 4){
        std::cerr << "connection != 4 not implemented yet\n";
        exit(EX_DATAERR);
    }
    uint64_t i;
    //std::unordered_map<uint64_t, boundary_node *> *v_this, *v_t, *aux_border;
    // std::vector< boundary_node *> *v_this, *v_t, *aux_border, *v_ret;
    std::vector< uint64_t > *v_this, *v_t, *aux_border, *v_ret;
    boundary_tree *ret_tree, *merge_tree;
    std::unordered_map<uint64_t, bool> accumulatedx, accumulatedy;
    std::vector<uint64_t> swap_nodes;
    boundary_node *e_parent;
    /*given that the levelroots will be visited a lot of times, it is needed to sum only once,
     so this map keeps the global ids already summed in this merge operation*/


    //ret_tree = this->get_copy();
    //merge_tree = t->get_copy();
    //ret_tree = this;
    //ret_tree = this->get_copy(true);
    ret_tree = new boundary_tree(this->h, this->w, this->grid_i, this->grid_j);
    merge_tree = t;
   /*  for(int i=0; i<NamesBordersVector.size(); i++){
        // if(verbose) std::cout << NamesBordersVector.at(i) << "\n";
        if(merge_tree->tile_borders->at(i)){
            std::vector<boundary_node *> *border = merge_tree->border_elements->at(i);
            for(auto e: *border){
                // if(verbose) std::cout << e->to_string() << "\n";
                if(e->boundary_parent == NO_BOUNDARY_PARENT){
                    e_parent = merge_tree->get_border_node(e->ptr_node->global_idx);    
                }else{
                    e_parent = merge_tree->get_border_node(e->boundary_parent);
                }
                if(e_parent!=NULL){
                    // if(verbose) std::cout << e_parent->ptr_node->global_idx << " being added\n";
                    ret_tree->add_lroot_tree(e_parent,true,true);
                    // if(verbose) std::cout << e->ptr_node->global_idx << " and parents added\n";
                }
                
            }
            
        }
    } */
    if(d == MERGE_HORIZONTAL){  // prepare data to merge borders placed on horizontal (this tree bottom border and merged tree top border)
        if(this->grid_i < merge_tree->grid_i){
            
            v_this = this->border_elements->at(BOTTOM_BORDER);
            //v_ret = ret_tree->border_elements->at(BOTTOM_BORDER);
            v_t = merge_tree->border_elements->at(TOP_BORDER);

            if(v_t->size() != v_this->size()){
                std::cerr << "invalid borders for tiles (" << this->grid_i << ", " << this->grid_j
                          << ") and (" << merge_tree->grid_i  << ", " << merge_tree->grid_j << "\n";
                exit(EX_DATAERR);
            }     
        }else{
            std::cerr << "Invalid merge call: destiny tree is in the postion ("
                      << this->grid_i << ", " << this->grid_j << ") and the merged tree is in the position("
                      << merge_tree->grid_i << ", " << merge_tree->grid_j << "). The destiny tree must be at top left of merged tree.\n";
            exit(EX_DATAERR);
        }
    }else if(d == MERGE_VERTICAL){ // prepare data to merger borders placed on vertical (this tree right border and merged tree left border)
        if(this->grid_j < merge_tree->grid_j){
            
            v_this = this->border_elements->at(RIGHT_BORDER);
            //v_ret = ret_tree->border_elements->at(RIGHT_BORDER);
            v_t = merge_tree->border_elements->at(LEFT_BORDER);

            if(v_t->size() != v_this->size()){
                std::cerr << "Invalid borders for tiles (" << this->grid_i << ", " << this->grid_j
                        << ") and (" << merge_tree->grid_i  << ", " << merge_tree->grid_j << "\n";
                exit(EX_DATAERR);
            }
        }else{
            std::cerr   << "Invalid merge call: destiny tree is in the postion ("
                        << this->grid_i << ", " << this->grid_j
                        << ") and the merged tree is in the position("
                        << merge_tree->grid_i << ", " << merge_tree->grid_j 
                        << "). The destiny tree must be at top left of merged tree.\n";
            exit(EX_DATAERR);
        }
    }
    
/* 
    if(verbose){
        std::cout << "merge new tree:\n";//" << t->lroot_to_string() << "\n";
        merge_tree->print_tree();
        std::cout << "Atributes:\n";
        std::cout << merge_tree->lroot_to_string(BOUNDARY_ATTR) << "\n";
        std::cout << "____________________________________________________________\n";

        std::cout << "ret_tree new tree:\n";// << ret_tree->lroot_to_string() << "\n";
        ret_tree->print_tree();
        std::cout << "Atributes:\n";
        std::cout << ret_tree->lroot_to_string(BOUNDARY_ATTR) << "\n";
        std::cout << "____________________________________________________________\n";
    }

 */
/*     if(verbose){
        std::cout << "this: " << this << " \n" << this->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") << "\n";
        std::cout << "merge_tree: " << merge_tree << " \n" << merge_tree->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") << "\n";
        std::cout << "ret: " << ret_tree << " \n" << ret_tree->lroot_to_string() << "\n";
    } */
    for(uint32_t i=0; i<v_this->size(); i++){
        if(verbose){
            std::cout << "border node x:"  << this->get_border_node(v_this->at(i))->ptr_node->global_idx << 
                         " border node y:" << t->get_border_node( v_t->at(i))->ptr_node->global_idx << "\n";
            std::cout << "levelroot x:"  << this->get_bnode_levelroot(v_this->at(i))->ptr_node->global_idx << 
                         " levelroot y:" << t->get_bnode_levelroot(v_t->at(i))->ptr_node->global_idx << "\n";
        }
        // boundary_node *x = this->get_bnode_levelroot(v_this->at(i)->boundary_parent);
        /* boundary_node *x = this->get_border_node(this->get_border_node(v_this->at(i))->boundary_parent);
        if(x == NULL){
            x = this->get_border_node(this->get_border_node(v_this->at(i))->ptr_node->global_idx);
        }
        
        // boundary_node *y = merge_tree->get_bnode_levelroot(v_t->at(i)->boundary_parent);

        boundary_node *y = merge_tree->get_border_node(t->get_border_node( v_t->at(i))->boundary_parent);
        if(y == NULL){
            y = merge_tree->get_border_node(t->get_border_node( v_t->at(i))->ptr_node->global_idx);
        } */

        boundary_node *x = this->get_bnode_levelroot(v_this->at(i));
        boundary_node *y = t->get_bnode_levelroot(v_t->at(i));
        ret_tree->merge_branches(x, y, accumulatedx, accumulatedy);
        
        // ret_tree->merge_branches_gaz(x, y, accumulatedx);

    }
    ret_tree->combine_borders(this, t, d);
    ret_tree->combine_lroot_trees(this,merge_tree);

   /*  if(verbose){
        std::cout << "____________________________________________________________\n";
        std::cout << "this new tree:\n";//" << this->lroot_to_string() << "\n";
        this->print_tree();
        std::cout << "Atributes:\n";
        std::cout << this->lroot_to_string(BOUNDARY_ATTR) << "\n";
        std::cout << this->lroot_to_string(BOUNDARY_GVAL) << "\n";
        std::cout << "____________________________________________________________\n";
        std::cout << "merge new tree:\n";//" << t->lroot_to_string() << "\n";
        merge_tree->print_tree();
        std::cout << "Atributes:\n";
        std::cout << merge_tree->lroot_to_string(BOUNDARY_ATTR) << "\n";
        std::cout << merge_tree->lroot_to_string(BOUNDARY_GVAL) << "\n";
        std::cout << "____________________________________________________________\n";
        std::cout << "ret_tree new tree:\n";// << ret_tree->lroot_to_string() << "\n";
        ret_tree->print_tree();
        std::cout << "Atributes:\n";
        std::cout << ret_tree->lroot_to_string(BOUNDARY_ATTR) << "\n";
        std::cout << ret_tree->lroot_to_string(BOUNDARY_GVAL) << "\n";
        std::cout << "____________________________________________________________\n";
    }  */

    //delete merge_tree;
    if(verbose){
        std::cout << "ret_tree:" << ret_tree->lroot_to_string() << "\n";
        std::cout << "gval:" << ret_tree->lroot_to_string(BOUNDARY_GVAL) << "\n";
        std::cout << "attributes:" << ret_tree->lroot_to_string(BOUNDARY_ATTR) << "\n________________\n";
    }
    
    return ret_tree;
}



//ver essa funcao
// ver essa funcao
/* 
void boundary_tree::update_tree(boundary_tree *merged){
    auto new_tree = merged->boundary_tree_lroot;
    //this->update_borders(merged);
    for(auto node: *new_tree){
        if(verbose)
            std::cout << "updating " << node.first << " \n";
        if(node.second->border_lr == NO_BORDER_LEVELROOT){
            //does not need to change levelroot of the node
            if(this->boundary_tree_lroot->find(node.first) != this->boundary_tree_lroot->end()){
                // if node is in this boundary tree just update this attribute
                this->boundary_tree_lroot->at(node.first)->ptr_node->attribute = node.second->ptr_node->attribute;
            }
        }else{ // here node has a border levelroot so it need to be updated
            if (this->boundary_tree_lroot->find(node.first) != this->boundary_tree_lroot->end()){
                //the node is in this tree, so it need to be updated
                auto merged_node = merged->get_border_node(node.second->border_lr);
                auto this_node = this->get_border_node(node.first);
                if (verbose){
                    if(merged_node == NULL){
                        std::cout << "searching for: " << node.second->border_lr << " - merged null\n";
                    }
                    if(this_node == NULL){
                        std::cout << "this null\n";
                    }
                    std::cout << "node_second:  " << node.second->to_string() << "\n";
                    std::cout << "merged:       " << merged_node->to_string() << "\n";
                    std::cout << "this:         " << this_node->to_string() <<  "\n";
                }
                this_node->border_lr = merged_node->ptr_node->global_idx;
                if(merged_node->ptr_node->gval == this_node->ptr_node->gval){
                    //if the merged node and this node has the same gval, so they are on the same component
                    //then we need to copy the attributes
                    this_node->ptr_node->attribute = merged_node->ptr_node->attribute;
                    if(verbose) {
                        std::cout << "node:" << node.first << " attributes changing to: " 
                                << merged_node->ptr_node->attribute << " that came from:" 
                                << merged_node->ptr_node->global_idx << "\n";

                    }
                }
            }
        }
    
        if(verbose){
            std::cout << node.first << " updated \n" << node.second->to_string() << "\n_____________________\n";
        }   
    }
    if(verbose) std::cout << "\n";
} */



 
 void boundary_tree::update_tree(boundary_tree *merged){
    auto t = merged->boundary_tree_lroot;
    auto tmp_updated = std::unordered_map<uint64_t, boundary_node*>();
    for(auto node: *t){
        tmp_updated.emplace(node.first, new boundary_node(node.second));
    }
    for(auto node: tmp_updated){
        // the node doesn't change its levelroot
        if(node.second->border_lr == NO_BORDER_LEVELROOT){
            // if node is in this boundary tree just update this attribute
            this->boundary_tree_lroot->at(node.first)->ptr_node->attribute = node.second->ptr_node->attribute;
        }else{// here node has a border levelroot so it need to be updated
            if (this->boundary_tree_lroot->find(node.first) != this->boundary_tree_lroot->end()){
                //the node is in this tree, so it need to be updated
                auto merged_node = tmp_updated.at(node.second->border_lr);
                auto this_node = this->get_border_node(node.first);
                this_node->border_lr = merged_node->ptr_node->global_idx;
                if(merged_node->ptr_node->gval == this_node->ptr_node->gval){
                    //if the merged node and this node has the same gval, so they are on the same component
                    //then we need to copy the attributes
                    this_node->ptr_node->attribute = merged_node->ptr_node->attribute;
                }
            }
        }
    }
} 

void boundary_tree::compress_path(){
    boundary_node *n;
    for(auto node: *(this->boundary_tree_lroot)){
        n = node.second;
        //check if n has the border_lr, so, it isn't a levelroot of the merge process 
        if(n->border_lr != NO_BORDER_LEVELROOT){
            // when the node isn't the levelroot of merge, we need to find the levelroot of this merge and
            // set it as the parent of n.
            
            //get levelroot of node that is border_lr of this node
            auto new_bound_par = this->get_border_node(n->border_lr); 
            //if the p
            while(new_bound_par && new_bound_par->border_lr != NO_BORDER_LEVELROOT){
                new_bound_par = this->get_border_node(new_bound_par->border_lr);
            }
            n->boundary_parent = new_bound_par != NULL ? new_bound_par->ptr_node->global_idx : NO_BOUNDARY_PARENT;
            
            n->border_lr = NO_BORDER_LEVELROOT;
            //n->in_lroot_tree = false;
            //delete n;
        }
    }
}


void boundary_tree::update_borders(){
    /* boundary_node *e_parent;
    for(int i=0; i<NamesBordersVector.size(); i++){
        // if(verbose) std::cout << NamesBordersVector.at(i) << "\n";
        if(this->tile_borders->at(i)){
            // std::vector<boundary_node *> *border = this->border_elements->at(i);
            for(auto e: *border){
                // if(verbose) std::cout << "node:" << e->ptr_node->global_idx << "\n";
                // if(verbose) std::cout << "node:" << e->to_string() << "\n";
                // if(verbose) std::cout << "boundary_parent:" << e->boundary_parent << " border_lr:" << e->border_lr << " \n";
                // e_parent = this->get_border_node(e->boundary_parent);
                // if(verbose) std::cout << " invalid parent: " << e_parent->to_string() << "\n";
                // while(e_parent != NULL){
                //     e_parent=this->get_border_node(e_parent->boundary_parent);
                //     // if(verbose) if(e_parent != NULL) std::cout << " invalid parent: " << e_parent->to_string() << "\n";
                // }

            }
            
        }
    } */
}

//ver essa funcao
// ver essa funcao 
/*
void boundary_tree::compress_path(){
    boundary_node *n, *e_parent;
    int64_t e_idx;
/*    if(verbose){
        for(auto node: *(this->boundary_tree_lroot)){
            n = node.second;
            std::cout << "(idx: " << n->ptr_node->global_idx << ",\t"
                    << "border_lr: " << n->border_lr << ",\t"
                    << "boundary_parent: " << n->boundary_parent << ",\t"
                    << "attribute: " << n->ptr_node->attribute << ",\t"
                    << "gval: " << (int)n->ptr_node->gval << ")\n";
        }
        std::cout << "===================\n";
    }*/ 
    /*
    for(auto node: *(this->boundary_tree_lroot)){
        n = node.second;
        if(n->border_lr != NO_BORDER_LEVELROOT){
            auto new_bound_par = this->get_border_node(n->border_lr);
            while(new_bound_par != NULL && ! new_bound_par->in_lroot_tree){
                new_bound_par = this->get_border_node(new_bound_par->border_lr);
            }
            n->boundary_parent = new_bound_par != NULL ? new_bound_par->ptr_node->global_idx : -1; // ver linha
            
            n->border_lr = NO_BORDER_LEVELROOT;
            n->in_lroot_tree = false;
            //delete n;
        }
    }
    if(verbose){
        for(auto node: *(this->boundary_tree_lroot)){
            n = node.second;
            std::cout << "(idx: " << n->ptr_node->global_idx << ",\t"
                    << "border_lr: " << n->border_lr << ",\t"
                    << "boundary_parent: " << n->boundary_parent << ",\t"
                    << "attribute: " << n->ptr_node->attribute << ",\t"
                    << "gval: " << (int)n->ptr_node->gval << ")\n";
        }
        std::cout << "===================\n";
    }
    for(int i=0; i<NamesBordersVector.size(); i++){
        // if(verbose) std::cout << NamesBordersVector.at(i) << "\n";
        if(this->tile_borders->at(i)){
            std::vector<boundary_node *> *border = this->border_elements->at(i);
            for(auto e: *border){
                // if(verbose) std::cout << "node:" << e->ptr_node->global_idx << "\n";
                // if(verbose) std::cout << "node:" << e->to_string() << "\n";
                // if(verbose) std::cout << "boundary_parent:" << e->boundary_parent << " border_lr:" << e->border_lr << " \n";
                e_parent = this->get_border_node(e->boundary_parent);
                // if(verbose) std::cout << " invalid parent: " << e_parent->to_string() << "\n";
                while(e_parent != NULL && !e_parent->in_lroot_tree){
                    e_parent=this->get_border_node(e_parent->boundary_parent);
                    // if(verbose) if(e_parent != NULL) std::cout << " invalid parent: " << e_parent->to_string() << "\n";
                }
                if(e_parent != NULL){
                    // if(verbose)std::cout << " parent: " << e_parent->to_string() << "\n";
                    
                    e->boundary_parent = e_parent->ptr_node->global_idx;
                    if(e == e_parent){
                        std::cerr << "isso nao deveria acontecer\n";
                        exit(EXIT_FAILURE);
                    }
                    // if(verbose) std::cout << "the new parent of:" << e->ptr_node->global_idx << " is:" << e->boundary_parent 
                                        // << "\n_______________________________________________________________________________________\n";
                }
            }    
        }
        
    }
    if(verbose) std::cout << "===================\n";
}
*/
uint64_t boundary_tree::index_of(uint32_t l, uint32_t c){
    return l * this->w + c;
}

std::tuple<uint32_t, uint32_t> boundary_tree::lin_col(uint64_t index){
    return std::make_tuple(index / this->w, index % this->w);
}

std::string boundary_tree::lroot_to_string(enum boundary_tree_field f, std::string endl){
    std::ostringstream ss;
    for(auto bn: *(this->boundary_tree_lroot)){
        if(f == BOUNDARY_PARENT){
            ss << "(" << bn.first << ", " << bn.second->boundary_parent << ")";
        }else if(f == BOUNDARY_BORDER_LR){
            ss << "(" << bn.first << ", blr: " << bn.second->border_lr 
               << ", bpar: " << bn.second->boundary_parent << ")";            
        }else if(f == BOUNDARY_LABEL){
            ss << "(" << bn.first << ", " << bn.second->ptr_node->label << ")"; 
        }else if(f == MAXTREE_IDX){
            ss << "(" << bn.first << ", " << bn.second->ptr_node->idx << ")";
        }else if(f == BOUNDARY_GVAL){
            ss << "(" << bn.first << ", " << (int)bn.second->ptr_node->gval << ")";
        }else if(f==BOUNDARY_GLOBAL_IDX){
            ss << "(" << bn.first << ", " << bn.second->ptr_node->global_idx << ")";
        }else if(f==BOUNDARY_ATTR){
            ss << "(n:" << bn.first << ", a:" << bn.second->ptr_node->attribute << ")";
        }else if(f==BOUNDARY_ALL_FIELDS){
            ss << bn.second->to_string();
        }
        ss << endl;
    }
    return ss.str();
}

std::string boundary_tree::border_to_string(enum boundary_tree_field f, std::string endl, std::string sep){
    
    std::ostringstream ss;
    for(int i=0; i<NamesBordersVector.size();i++){
        ss << fill(NamesBordersVector[i], 16) << ": ";
        auto v = *(this->border_elements->at(i));
        for(auto idx: v){
            ss << idx << sep;
        }
        /*for(auto pairs: v){
            auto bn = pairs.second;*/
        /* for(auto bn: v){
            if(f == BOUNDARY_PARENT){
                ss << bn->boundary_parent;
            }else if(f == BOUNDARY_BORDER_LR){
                if(bn->border_lr != NO_BORDER_LEVELROOT){
                    ss << bn->border_lr;
                }else{
                    ss << bn->boundary_parent;
                }           
            }else if(f == MAXTREE_IDX){
                ss << bn->ptr_node->idx;
            }else if(f == BOUNDARY_GVAL){
                ss << bn->ptr_node->gval;
            }else if(f==BOUNDARY_GLOBAL_IDX){
                ss << bn->ptr_node->global_idx;
            }else if(f==BOUNDARY_ALL_FIELDS){
                ss << bn->to_string();
            }
            ss << endl;
        } */
        ss << endl;
    }
    return ss.str();
}



void boundary_tree::print_tree(enum boundary_tree_field lrootf, enum boundary_tree_field borderf){
    std::cout << "\n____________________________________________________\n";
    std::cout << ">>>>>>>>> All borders: <<<<<<<<<<<\n" << this->border_to_string(borderf);
    std::cout << ">>>>>>>>> Tree: <<<<<<<<<\n" << this->lroot_to_string(lrootf);
    std::cout << "\n_____________________________________________________\n";
}