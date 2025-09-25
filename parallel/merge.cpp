#include <vips/vips8>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
#include <string>
#include <ostream>
#include <sysexits.h>

#include "maxtree.hpp"
#include "maxtree_node.hpp"
#include "boundary_tree.hpp"

#include "utils.hpp"



using namespace vips;

bool verbose;
/* 
void print_pq(std::priority_queue<maxtree_node*, std::vector<maxtree_node*> ,cmp_maxtree_nodes> pq){
    std::cout <<"===========QUEUE=============\n";
    while(!pq.empty()){
        auto e=pq.top();
        std::cout<<*e<<" ";
        //pq.remove(e);
        pq.pop();
    }
    std::cout<<"\n";
    std::cout <<"============================\n";
} */

int main(int argc, char *argv[]){
    vips::VImage *in;
    maxtree *t;
    std::string out_name, out_ext;


    std::cout << "argc: " << argc << " argv:" ;
    for(int i=0;i<argc;i++){
        std::cout << argv[i] << " ";
    }
    std::cout << "\n";

    if(argc <= 2){
        std::cout << "usage: " << argv[0] << " input_image config_file\n";
        exit(EX_USAGE);
    }

    if (VIPS_INIT (argv[0])) {
        vips_error_exit (NULL);
    }

    /*
        Reading configuration file
    */
    verbose=false;
    Tattribute lambda=2;

    in = new vips::VImage(
                vips::VImage::new_from_file(argv[1],
                VImage::option ()->set ("access", VIPS_ACCESS_SEQUENTIAL)
            )
        );
    auto configs = parse_config(argv[2]);

    if (configs->find("verbose") != configs->end()){
        if(configs->at("verbose") == "true"){
            verbose=true;
        }
    }

    bool colored = false;
    if(configs->find("colored") != configs->end()){
        if(configs->at("colored") == "true"){
            colored = true;
        }
    }
    if(configs->find("lambda") != configs->end()){
        lambda = std::stod(configs->at("lambda"));
    }

    if(configs->find("glines") == configs->end() || configs->find("gcolumns") == configs->end()){
        std::cout << "you must specify the the image division on config file:" << argv[2] <<"\n";
        std::cout << "example:\n";
        std::cout << "glines=8 #divide image in 8 vertical tiles\n";
        std::cout << "gcolumns=6 #divide image in 6 vertical tiles\n";
        exit(EX_CONFIG);
    }
    
    out_name = "output";
    if (configs->find("output") != configs->end()){
            out_name = configs->at("output");
    }

    out_ext = "png";
    if(configs->find("output_ext") != configs->end()){
        out_ext = configs->at("output_ext");
    }

    uint8_t pixel_connection = 4;

    uint32_t glines = std::stoi(configs->at("glines"));
    uint32_t gcolumns = std::stoi(configs->at("gcolumns"));

    std::cout << "configurations:\n";
    print_unordered_map(configs);
    std::cout << "====================\n";
    std::cout << "start\n";
    
    uint32_t h,w;
    h=in->height();
    w=in->width();
    
    uint32_t h_trunc = h/glines;
    uint32_t w_trunc = w/gcolumns;

    uint32_t num_h_ceil = h%glines;
    uint32_t num_w_ceil = w%gcolumns;

    if(verbose){
        std::cout << "h trunc:" << h_trunc << "\n";
        std::cout << "w trunc:" << w_trunc << "\n";
        std::cout << "num_h_ceil:" << num_h_ceil << "\n";
        std::cout << "num_w_ceil:" << num_w_ceil << "\n";
    }

    /*end config*/

    /*
    split image
    */

    std::vector<std::vector<maxtree *>> tiles;
    bool left, top, right, bottom;
    uint32_t i,j,x = 0;
    uint32_t noborder_rt=0, noborder_rl, lines_inc, columns_inc; // original tiles (without borders) size variables
    uint32_t reg_top, reg_left, tile_lines, tile_columns; // tiles used by algorithm (with border) size variables

    for(i=0; i<glines; i++){
        std::vector<bool> borders(4,false);
        lines_inc = i < num_h_ceil ? h_trunc +1 : h_trunc;
        
        tile_lines = lines_inc;
        reg_top = noborder_rt;
        if(noborder_rt > 0){ //check if there is a top border
            tile_lines++;
            reg_top--;
            borders.at(TOP_BORDER) = true;
        }
        if(noborder_rt+lines_inc < h - 1){//check if there is a bottom border
            tile_lines++;
            borders.at(BOTTOM_BORDER) = true;
        }
        noborder_rl=0;
        tiles.push_back(std::vector<maxtree *>());
        for(j=0; j<gcolumns; j++){
            borders.at(LEFT_BORDER) = false;
            borders.at(RIGHT_BORDER) = false;
         /*    if(verbose){
                std::cout << "===============inner loop=======================\n";
                std::cout << x++ << "->" << i << "," << j <<"\n";
            } */
            columns_inc = j < num_w_ceil ? w_trunc+1 : w_trunc;
            tile_columns = columns_inc;
            reg_left = noborder_rl;
            if(noborder_rl > 0){//check if there is a left border
                tile_columns++;
                reg_left--;
                borders.at(LEFT_BORDER) = true;
            }
            if(noborder_rl+columns_inc < w - 1){//check if there is a right border
                tile_columns++;
                borders.at(RIGHT_BORDER) = true;
            }
            
            //std::cout << "filling: " << noborder_rt << "," << noborder_rl << "..." << noborder_rt + lines_inc << "," << noborder_rl + columns_inc <<"\n";
/*             if(verbose){
                std::cout << "with borders:\n";
                std::cout << reg_left << "," << reg_top << "," << tile_columns << "," << tile_lines << "\nno borders: \n";
                std::cout << noborder_rl << "," << noborder_rt << "," << columns_inc << "," << lines_inc << "\n------------\n";
            } */
            
            maxtree *new_tree = new maxtree(borders, tile_lines, tile_columns, i, j);
            vips::VRegion reg = in->region(reg_left, reg_top, tile_columns, tile_lines);
            
            reg.prepare(reg_left, reg_top, tile_columns, tile_lines);
            
            new_tree->fill_from_VRegion(reg, reg_top, reg_left, h, w);
            tiles.at(i).push_back(new_tree);
            vips_region_invalidate(reg.get_region());
            
            //std::cout << new_tree->to_string(GVAL,5);
            noborder_rl+=columns_inc;
/*             if(verbose){
                std::cout << "\n\n\n=====================================================================================\n\n\n";
            } */
        }
        noborder_rt+=lines_inc;
    }

    /*
    computing tiles maxtree
    */
    std::cout << "computing tiles trees\n";
    for(int i=0; i < glines; i++){
        for(int j=0;j<gcolumns; j++){
            t = tiles.at(i).at(j);

/*             if (verbose) {
                std::cout << "=> computing tile (stored in maxtree):" << t->grid_i << ", " << t->grid_j << " size:" << t->h << ", " <<  t->w << "\n";
                std::cout << "i:" << i << " j:" << j << "\n";
            } */
            //t->compute_sequential_iterative();
            t->compute_sequential_recursive();
        }
    }

    if(verbose){
        for(i=0; i<glines; i++){
            for(j=0; j<gcolumns;j++){
                t = tiles.at(i).at(j);
                std::cout << "Boundary Tile:(" << i << "," << j <<")\n";
                std::cout << ">>>>> tile:" << i << " " << j << "\n";
                std::cout << ">>> maxtree: grid_i = " << t->grid_i << " grid_j = " << t->grid_j << "\n";
                
                std::cout << "__________________GVAL________________\n";
                std::cout << t->to_string(GVAL,colored,8,2);
/*                 std::cout << "_________________PARENT_IJ_______________\n";
                std::cout << t->to_string(PARENT_IJ,colored,10); */
                std::cout << "________________LEVELROOT________________\n";
                std::cout << t->to_string(LEVELROOT,colored,5);
                std::cout << "_______________PARENT_________________\n";
                std::cout << t->to_string(PARENT,colored,5);
                std::cout << "________________LOCAL IDX________________\n";
                std::cout << t->to_string(IDX, colored, 5);
                std::cout << "_______________GLOBAL IDX_________________\n";
                std::cout << t->to_string(GLOBAL_IDX,colored,5);
                std::cout << "________________ATTRIBUTE________________\n";
                std::cout << t->to_string(ATTRIBUTE,colored,5);

                std::cout << "Levels roots:";
                for(auto r: *(t->get_levelroots())){
                    std::cout << r->idx << " ";
                }
                std::cout << "\n";
            }
        }
    }

    /* 
    for(i=0; i<glines; i++){
        for(j=0; j<gcolumns;j++){
            t = tiles.at(i).at(j);
            t->filter(lambda);
            if(verbose){  
                std::cout << "_______________LABELS_________________\n";
                std::cout << t->to_string(LABEL,colored,8,2);
            }
            t->save(std::string(argv[1]) + std::to_string(i) + std::to_string(j) + ".png");
        }
    }

    exit(0);
 */
    if(verbose){
        std::cout <<"\n===============BOUNDARY TREES=================\n";
        std::cout <<"==============================================\n";
    }

    /*
    computing boundary trees
    */
    std::cout << "computing boundary trees\n";
    std::vector<std::vector<boundary_tree *> > tiles_table;
    for(i=0; i < glines; i++){
        tiles_table.push_back(std::vector<boundary_tree *>());
        for(j=0;j<gcolumns; j++){
            t = tiles.at(i).at(j);
            
            boundary_tree *bt = t->get_boundary_tree();
            tiles_table.at(i).push_back(bt);
            if(verbose){
                std::cout << i << ", " << j <<"\n";
                // std::cout << "borders:" << t->string_borders() << "\n";
                std::cout << "boundary border nodes:\n" << bt->border_to_string(BOUNDARY_ALL_FIELDS,"\n") << "\n";
                std::cout << "boundary tree:\n" << bt->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") << "\n";
                std::cout << "\n==========================================================================================================\n\n\n";
            }
            //delete bt;
        }
    }






    /*
        realizar o merge:
        1. montar uma tabela de boundary trees fazendo um mapeamento de "linha/coluna"
           do grid para a boundary tree 
           - ok (tiles_table)
        2. conectar dois tiles do grid vendo os vizinhos de acordo com a tabela;
        3. atualizar a tabela de forma que as duas entradas dos tiles conectados 
           apontem para a mesma boundary tree

        Otimização (fazer depois): fazer na maior dimensão do grid e depois na menor
    */
    int64_t ntrees = glines * gcolumns;
    
    boundary_tree *merged;

    if(verbose){
        std::cout << ">>>>>>>>> merge columns <<<<<<<<\n";
    }
    std::vector<std::vector<boundary_tree *> > aux_tile_table = tiles_table;

    uint32_t grid_col_inc = 2; 
    while(ntrees > glines){ /* merge vertical border --- creating "big lines"*/     
        
        for(i = 0; i < glines; i++){
            for(j = 0; j+grid_col_inc/2 < gcolumns; j+=grid_col_inc){
                boundary_tree *base_bt = aux_tile_table[i][j];
                boundary_tree *to_merge = aux_tile_table[i][j+grid_col_inc/2];
                boundary_tree *del_bt = base_bt;

                if(verbose) std::cout << "base before merge: " << i << " " << j << "\n";
                //base_bt->print_tree();
                if(verbose) std::cout << "to merge before merge: "<< i << " " << j+grid_col_inc/2 <<"\n";
                //to_merge->print_tree();
                
                std::cout << "merge boundary tree: " << i << " " << j << " with " << i << " " << j+grid_col_inc/2 << "\n";

                // if(verbose){
                    // std::cout << "---------------------before merge---------------------\n";
                    std::cout << "BASE BOUNDARY TREE: " << base_bt->grid_i << ", " << base_bt->grid_j <<  "\n";
                    std::cout << base_bt->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") <<"\n";
                    std::cout << "=================================================================\n";
                    std::cout << "TO_MERGE BOUNDARY TREE:" << to_merge->grid_i << ", " << to_merge->grid_j <<  "\n";
                    std::cout << to_merge->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") <<"\n";
                    std::cout << "=================================================================\n";
                // }
                merged = base_bt->merge(to_merge,MERGE_VERTICAL,pixel_connection);

                // base_bt->update_tree(merged);
                // base_bt->update_borders(merged);
                //base_bt->compress_path();

                // to_merge->update_tree(merged);
                // to_merge->update_borders(merged);
                //to_merge->compress_path();
                // if(verbose){
                    // std::cout << "---------------------before update and compress---------------------\n";
                    // std::cout << "merged boundary tree:\n";
                    // std::cout << merged->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") <<"\n";
                    // std::cout << "_________________________________________________________________\n";
                // }


                //auto copy_merged = merged->get_copy(true);
                merged->update_tree(merged);
                // merged->update_borders();
                merged->compress_path();
                //delete copy_merged;
                aux_tile_table[i][j] = merged;
                
                /* std::cout << i << " " << j << "\n";*/
                //t = tiles.at(i).at(j);
                //t->update_from_boundary_tree(merged);
                /*std::cout << t->to_string(PARENT,colored,5) << "\n\n";

                if(j+grid_col_inc/2 < gcolumns){
                    t = tiles.at(i).at(j+grid_col_inc/2);
                    t->update_from_boundary_tree(merged);
                    std::cout << t->to_string(PARENT,colored,5) << "\n\n";
                }  */

                // if(verbose){
                    std::cout << "---------------------after update and compress---------------------\n";
                    // std::cout << "BASE BOUNDARY TREE:" << base_bt->grid_i << ", " << base_bt->grid_j <<  "\n";
                    // std::cout << base_bt->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") <<"\n";
                    // std::cout << "=================================================================\n";
                    // std::cout << "TO_MERGE BOUNDARY TREE:" << to_merge->grid_i << ", " << to_merge->grid_j <<  "\n";
                    // std::cout << to_merge->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") <<"\n";
                    // std::cout << "=================================================================\n";
                    std::cout << "MERGED BOUNDARY TREE:" << merged->grid_i << ", " << merged->grid_j <<  "\n";
                    std::cout << merged->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") <<"\n";
                    std::cout << "_________________________________________________________________\n";
                // }
                    
                /* std::cout << "<><><><><><><><><> AFTER MERGE: "<< i << " " << j <<" <><><><><><><><><> \n";
                base_bt->print_tree();
                 */
                
                ntrees--;
                if(verbose){
                    std::cout << "Merge tiles: (" << base_bt->grid_i << ", " << base_bt->grid_j << ") <===> "
                              << "(" << to_merge->grid_i << ", " << to_merge->grid_j << ")\n";
                    std::cout << "ntrees:" << ntrees << " glines:"  << glines << " gcol:" <<  gcolumns <<"\n";
                    std::cout << "================\n";
                }
                std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
            }
        }
        grid_col_inc*=2;
        if(verbose){
            std::cout << "+++++++++++++++++++++++++++++++++++\n";
        }
    }
    uint32_t grid_lin_inc = 2; 
    if(verbose){
        std::cout << ">>>>>>>>> merge lines <<<<<<<<\n";
    }
    
    while(ntrees > 1){/* Merge lines "recreating maxtree of the whole image"*/
        for(i=0; i + grid_lin_inc/2 < glines; i+=grid_lin_inc){
            boundary_tree *base_bt = aux_tile_table[i][0];
            boundary_tree *to_merge = aux_tile_table[i+grid_lin_inc/2][0];
            boundary_tree *del_bt = base_bt;
            
            // if(verbose){
                std::cout << "BASE BOUNDARY TREE:" << base_bt->grid_i << ", " << base_bt->grid_j <<  "\n";
                std::cout << base_bt->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") <<"\n";
                std::cout << "=================================================================\n";
                std::cout << "TO_MERGE BOUNDARY TREE:" << to_merge->grid_i << ", " << to_merge->grid_j <<  "\n";
                std::cout << to_merge->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") <<"\n";
                std::cout << "=================================================================\n";
            // }
            merged=base_bt->merge(to_merge,MERGE_HORIZONTAL,pixel_connection);
            // if(verbose){
                // std::cout << "---------------------before update and compress---------------------\n";
                // std::cout << "merged boundary tree:" << merged->grid_i << ", " << merged->grid_j <<  "\n";
                // std::cout << merged->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") <<"\n";
                // std::cout << "_________________________________________________________________\n";
            // }
            // base_bt->update_tree(merged);
            // base_bt->update_borders(merged);
            //base_bt->compress_path();

            // to_merge->update_tree(merged);
            // to_merge->update_borders(merged);
            //to_merge->compress_path();
            //auto copy_merged = merged->get_copy(true);
            merged->update_tree(merged);
            // merged->update_borders();
            merged->compress_path();
            
            auto del_tree = base_bt;
            aux_tile_table[i][0] = merged;
            /* delete del_tree;
            delete to_merge; */

            //t = tiles.at(i).at(0);
            //t->update_from_boundary_tree(merged);
            //std::cout << t->to_string(GLOBAL_IDX,colored,8,2) << "\n\n";


            // if(verbose){

                std::cout << "MERGED BOUNDARY TREE:" << merged->grid_i << ", " << merged->grid_j <<  "\n";
                std::cout << merged->lroot_to_string(BOUNDARY_ALL_FIELDS,"\n") <<"\n";
                std::cout << "_________________________________________________________________\n";
            // }

            /* 
            std::cout << "<><><><><><><><><> AFTER MERGE: "<< i << " " << j <<" <><><><><><><><><> \n";
            base_bt->print_tree();
            */
            ntrees--;

            if(verbose){
                std::cout << ">> Merge tiles: (" << base_bt->grid_i << ", " << base_bt->grid_j << ") <===> "
                          << "(" << to_merge->grid_i << ", " << to_merge->grid_j << ")\n";
                std::cout << "ntrees:" << ntrees << " glines:"  << glines << " gcol:" <<  gcolumns <<"\n";
                std::cout << "base new tree:" << base_bt->lroot_to_string() << "\n";
                std::cout << "to merge new tree:" << to_merge->lroot_to_string() << "\n";
                std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
            }
        }
        grid_lin_inc*=2;
        if(verbose){
            std::cout << "+++++++++++++++++++++++++++++++++++\n";
        }
    }
    // if(verbose){
        std::cout << "Final Boundary Tree:\n";
        std::cout << "-------------NODE INFO--------------\n";
        std::cout << merged->lroot_to_string(BOUNDARY_ALL_FIELDS, "\n") << "\n";
        // std::cout << "------------ATTRIBUTES--------------\n";
        // std::cout << merged->lroot_to_string(BOUNDARY_ATTR) << "\n";
        // std::cout << "---------------GVAL-----------------\n";
        // std::cout << merged->lroot_to_string(BOUNDARY_GVAL) << "\n";
        std::cout << "======================================================\n";
        std::cout << "_________________________________________________________________\n";
    // }
    for(int i=0; i < glines; i++){
        for(int j=0;j<gcolumns; j++){
            t = tiles.at(i).at(j);
            t->update_from_boundary_tree(merged);
            std::cout << "tree (" << t->grid_i << "," << t->grid_j <<" )updated\n";
            t->filter(lambda,merged);
            std::cout << "filter done\n";
            t->save(out_name+"_"+ std::to_string(t->grid_i) + "-" + std::to_string( t->grid_j)+ "." + out_ext);
            if(verbose){
                std::cout << "__________________GVAL________________\n";
                std::cout << t->to_string(GVAL,colored,8,2);
                std::cout << "__________________LABEL________________\n";
                std::cout << t->to_string(LABEL,colored,8,2);
                std::cout << "________________ATTRIBUTE________________\n";
                std::cout << t->to_string(ATTRIBUTE,colored,5);
            }
        }
    }


    //as boundary trees tem alturas e tamanhos distintos, logo é possível estimar o custo de um merge

    return 0;
}
