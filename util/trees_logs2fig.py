import re
import os
import argparse
import networkx as nx
import matplotlib.pyplot as plt

 
def get_field(line, name, tp, sep=','):
    tfield = line.split(name)[1]
    field = tfield.split(sep)[0]
    return tp(field)

parser = argparse.ArgumentParser()
parser.add_argument('file', help = 'filename')
parser.add_argument('--output-dir', '-d', dest='out_dir', default='.', help='output directory')
parser.add_argument('--output-prefix', '-o', dest='output', default='fig', help='output file prefix')
parser.add_argument('--output-ext', '-e', dest='ext', default='pdf', help='output file extension')
parser.add_argument('--show-values', '-s', dest='show_values', action='store_true', help='')
parser.add_argument('--node-size', '-n', dest='node_size', default=150, type=int, help='size of nodes in final figure')
parser.add_argument('--font-size', '-f', dest='font_size', default=8, type=int, help='size of nodes in final figure')
args = parser.parse_args()


fn = args.file
nsize = args.node_size
fsize = args.font_size
ext= args.ext
out_dir = args.out_dir
out_prefix = args.output
with open(fn) as f:
    texto = f.read()

lines = texto.split('\n')

all_trees = []
tree_data = []
type_line = ''
iter=0

os.makedirs(f'{out_dir}/txt',exist_ok=True)
os.makedirs(f'{out_dir}/imgs',exist_ok=True)

for l in lines:
    
    insere = False
    info = re.search(r'(BASE BOUNDARY TREE:.*\d+.*\d+.*|TO_MERGE BOUNDARY TREE:.*\d+.*\d+.*|MERGED BOUNDARY TREE:.*\d+.*\d+.*|Final Boundary Tree:.*)',l)
    if info != None:
        data_name = l.replace('BOUNDARY TREE','BT').replace(':','-').replace(' ','_')
        type_line = 'header'
        #print('---->',data_name,'<----')
    else:
        a=re.search(r'\(.*idx:.*-?\d+.*,.*border_lr:.*-?\d+.*,.*gval:.*-?\d+.*,.*attribute:.*-?\d+.*\)',l)
        if a!=None:
            spl=l.split(')')
            if(len(spl) <= 2):
               type_line='data'
        else:
            if type_line!='':
                insere = True
                type_line = ''

    if type_line == 'data':
        p = l.find('(')
        if p != -1:
            tree_data.append(l[p:])
        
    if insere:
        print('tree:')
        print(data_name)
        for node in tree_data:
            print(node)
        all_trees.append({'nome':str(iter)+'_'+data_name,'data':tree_data})
        tree_data = []
        iter+=1

root = -1

for tdata in all_trees:
    #print(tdata['nome'])
    out_name = f"{out_dir}/txt/{out_prefix}_{tdata['nome']}.txt"
    f=open(out_name,'w')
    for d in tdata['data']:
        f.write(d+'\n')
        #print(d)
    f.close()

    g = nx.DiGraph()
    
    for ld in tdata['data']:
        idx = get_field(ld, "idx:", int)
        gval = get_field(ld, "gval:", int)
        attribute = get_field(ld, "attribute:", int, sep = ')')
        parent = get_field(ld, "bound_parent:", int)
        if(parent == -1):
            root = idx
        insert=True
        g.add_node(idx, gval=gval, attr=attribute, par = parent)

    for u in g.nodes(data = True):
        idx = u[0]
        parent = u[1]["par"]
        if(parent != -1):
            g.add_edge(parent,idx)


    pdot=nx.drawing.nx_pydot.pydot_layout(g, prog='dot')

    spacing = (len(g.nodes())*(nsize//100))//10+1
    
    plt.figure(figsize=(spacing,spacing))

    
    for k in pdot:
        pdot[k] = (pdot[k][0], pdot[k][1]*spacing)
    
    node_colors = ["#"+3*(hex(g.nodes[node]['gval']).split('x')[-1].rjust(2).replace(' ','0')) for node in g.nodes()]

    nx.draw(g, with_labels=True, pos=pdot, node_size=nsize, font_size=fsize, font_color='red', node_color=node_colors)
    out_name = out_name = f"{out_dir}/imgs/{out_prefix}_{tdata['nome']}.{ext}"
    plt.savefig(out_name)
    plt.clf()
