import re
import argparse
import networkx as nx
import matplotlib.pyplot as plt

 
def get_field(line, name, tp, sep=','):
    tfield = line.split(name)[1]
    field = tfield.split(sep)[0]
    return tp(field)

parser = argparse.ArgumentParser()
parser.add_argument('file', help = 'filename')
parser.add_argument('--output', '-o', dest='output', default='fig.pdf', help='output file')
parser.add_argument('--show-values', '-s', dest='show_values', action='store_true', help='')
parser.add_argument('--filter-unimportant-nodes', '-f', dest='filter', action='store_true', help='remove nodes with attribute 0 or 1 that arent fathers on the tree')
args = parser.parse_args()

filter_nodes=args.filter

fn = args.file
with open(fn) as f:
    texto = f.read()

lines = texto.split('\n')

lines_data = []

for l in lines:
    a=re.search('.*idx:-?.*',l)
    if a!=None:
        p = l.find('(')
        lines_data.append(l[p:])

g = nx.DiGraph()

all_parents = {}

if filter_nodes:
    for ld in lines_data:
        parent = get_field(ld, "bound_parent:", int)
        all_parents[parent] = True


for ld in lines_data:
    idx = get_field(ld, "idx:", int)
    gval = get_field(ld, "gval:", int)
    attribute = get_field(ld, "attribute:", int, sep = ')')
    parent = get_field(ld, "bound_parent:", int)
    insert=True
    if filter_nodes and (not idx in all_parents):
        insert = False
    if insert:
        g.add_node(idx, gval=gval, attr=attribute, par = parent)

for u in g.nodes(data = True):
    idx = u[0]
    parent = u[1]["par"]
    if(parent != -1):
        g.add_edge(parent,idx)


pdot=nx.drawing.nx_pydot.pydot_layout(g, prog='dot')

node_colors = ["#"+3*(hex(g.nodes[node]['gval']).split('x')[-1].rjust(2).replace(' ','0')) for node in g.nodes()]

nsize = 300
fsize = nsize//10



nx.draw(g, with_labels=True, pos=pdot, node_size=nsize, font_size=fsize, font_color='red', node_color=node_colors)
out_name = args.output
plt.savefig(out_name)
