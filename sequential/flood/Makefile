#Flags on comand line:
#	OPT flag to describe the optimization flags (e.g make OPT=-O3)
#	TYPE flag to describe if you want to compile for (D)ebug or (R)elease, default is Debug (e.g. make TYPE=R)

non_rec_wilk_name=non_recursive_wilk
non_rec_name=non_recursive
rec_name=recursive

ifeq (${TYPE},R)
	OBJ_CPP_FLAGS=-c 
else 
	OBJ_CPP_FLAGS=-g -c
endif


test_files=../../testes/fig.png ../../testes/example.png ../../testes/high_border.png


maxtree_node_dir=../../include
utils_dir=../../include
maxtree_dir=../../include
boundary_tree_dir=../../include


source_non_rec_wilk= ${non_rec_wilk_name}.cpp
source_non_rec = ${non_rec_name}.cpp
source_rec= ${rec_name}.cpp

vips_args = `pkg-config vips-cpp --cflags --libs`

MKDIR=@mkdir
RM=rm -rf


dest_nonrec_wilk = exec/${non_rec_wilk_name}
dest_nonrec = exec/${non_rec_name}
dest_rec = exec/${rec_name}

all: non_rec rec wilkinson

all+bkp: non_rec rec wilkinson wilkinson_old


non_rec: base
	${MKDIR} -p exec
	g++ ${OPT} ${source_non_rec} ${utils_dir}/utils.o ${maxtree_node_dir}/maxtree_node.o \
		${maxtree_dir}/maxtree.o ${boundary_tree_dir}/boundary_tree.o \
        -I${maxtree_node_dir}  -I${maxtree_dir} -I${utils_dir}  ${vips_args} -o ${dest_nonrec}

rec: base
	${MKDIR} -p exec
	g++ ${OPT} ${source_rec} ${utils_dir}/utils.o ${maxtree_node_dir}/maxtree_node.o \
		${boundary_tree_dir}/boundary_tree.o ${maxtree_dir}/maxtree.o \
        -I${maxtree_node_dir}  -I${maxtree_dir} -I${utils_dir} ${vips_args} -o ${dest_rec}

wilkinson: base
	${MKDIR} -p exec
	g++ ${OPT} ${source_non_rec_wilk} ${utils_dir}/utils.o ${maxtree_node_dir}/maxtree_node.o \
		${boundary_tree_dir}/boundary_tree.o ${maxtree_dir}/maxtree.o \
    	-I${maxtree_node_dir}  -I${maxtree_dir} -I${utils_dir} ${vips_args} -o ${dest_nonrec_wilk}

base: 
	@make -C ${maxtree_node_dir}
	@make -C ${maxtree_dir}
	@make -C ${boundary_tree_dir}
	@make -C ${utils_dir}

wilkinson_old: base
	g++ ${OPT} wilk_bkp.cpp ${utils_dir}/utils.o ${maxtree_node_dir}/maxtree_node.o \
	${boundary_tree_dir}/boundary_tree.o \
    -I ${maxtree_node_dir} ${vips_args} -o exec/wilk_bkp


clean:
	@make -C ${maxtree_node_dir} clean 
	@make -C ${maxtree_dir} clean
	@make -C ${boundary_tree_dir} clean
	@make -C ${utils_dir} clean
	${RM} exec/${non_rec_wilk_name} exec/${non_rec_name} exec/${rec_name}

tests: all
	for input_test in ${test_files} ; do \
		./${dest_nonrec_wilk} $$input_test ; \
		./${dest_nonrec} $$input_test ; \
		./${dest_rec} $$input_test ; \
	done