#Flags on comand line:
#	OPT flag to describe the optimization flags (e.g make OPT=-O3)
#	TYPE flag to describe if you want to compile for (D)ebug or (R)elease, default is Debug (e.g. make TYPE=R)

quadratic=quadratic
union_rank=union_by_rank
UbR_level_compression=level_compression
ifeq (${TYPE},R)
	OBJ_CPP_FLAGS=-c 
else 
	OBJ_CPP_FLAGS=-g -c
endif


test_files=../../testes/fig.png ../../testes/example.png ../../testes/high_border.png


maxtree_node_dir=../../include
utils_dir=../../include
maxtree_dir=../../include

src_quadratic = ${quadratic}.cpp
src_rank = ${union_rank}.cpp
src_level_comp = ${UbR_level_compression}.cpp

vips_args = `pkg-config vips-cpp --cflags --libs`

MKDIR=@mkdir
RM=rm -rf

dest_rank = exec/${union_rank}
dest_quadratic = exec/${quadratic}
dest_level_comp = exec/${UbR_level_compression}

all: quadratic rank level_compression

quadratic: maxtree_node.o utils.o maxtree.o
	${MKDIR} -p exec
	g++ ${OPT} ${src_quadratic} ${utils_dir}/utils.o ${maxtree_node_dir}/maxtree_node.o ${maxtree_dir}/maxtree.o \
        -I${maxtree_node_dir}  -I${maxtree_dir} -I${utils_dir} ${vips_args} -o ${dest_quadratic}

rank: maxtree_node.o utils.o maxtree.o 
	${MKDIR} -p exec
	g++ ${OPT} ${src_rank} ${utils_dir}/utils.o ${maxtree_node_dir}/maxtree_node.o ${maxtree_dir}/maxtree.o \
        -I${maxtree_node_dir}  -I${maxtree_dir} -I${utils_dir} ${vips_args} -o ${dest_rank}

level_compression: maxtree_node.o utils.o maxtree.o
	${MKDIR} -p exec
	g++ ${OPT} ${src_level_comp} ${utils_dir}/utils.o ${maxtree_node_dir}/maxtree_node.o ${maxtree_dir}/maxtree.o \
        -I${maxtree_node_dir}  -I${maxtree_dir} -I${utils_dir} ${vips_args} -o ${dest_level_comp}

maxtree.o:
	g++ ${OBJ_CPP_FLAGS} ${maxtree_dir}/maxtree.cpp -I${maxtree_dir} ${vips_args} -o ${maxtree_dir}/maxtree.o

maxtree_node.o:
	g++ ${OBJ_CPP_FLAGS} ${maxtree_node_dir}/maxtree_node.cpp -I${maxtree_node_dir} -I${utils_dir} ${vips_args} -o ${maxtree_node_dir}/maxtree_node.o

utils.o:
	g++ ${OBJ_CPP_FLAGS} ${utils_dir}/utils.cpp ${vips_args} -I${utils_dir} -o ${utils_dir}/utils.o


clean:
	${RM} exec/${dest_quadratic} ${utils_dir}/*.o ${maxtree_node_dir}/*.o

tests: all
	for input_test in ${test_files} ; do \
		./${dest_quadratic} $$input_test ; \
	done