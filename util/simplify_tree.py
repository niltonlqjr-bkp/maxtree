import re
import argparse
import matplotlib.pyplot as plt

 
def get_field(line, name, tp, sep=','):
    tfield = line.split(name)[1]
    field = tfield.split(sep)[0]
    return tp(field)

parser = argparse.ArgumentParser()
parser.add_argument('file', help = 'filename')      
parser.add_argument('--show-values', '-s', dest='show_values', action='store_true', help='')

args = parser.parse_args()

fn = args.file
with open(fn) as f:
    texto = f.read()

lines = texto.split('\n')

lines_data = []

for l in lines:
    a=re.search('.*\(.*idx:-?.*',l)
    if a!=None:
        p = l.find('(')
        lines_data.append(l[p:])

all_parents = {}

for ld in lines_data:
    parent = get_field(ld, "bound_parent:", int)
    attr = get_field(ld, "attribute:", int, sep=')')
    all_parents[parent] = True

nodes_to_use={}

for ld in lines_data:
    idx = get_field(ld, "idx:", int)
    gval = get_field(ld, "gval:", int)
    attribute = get_field(ld, "attribute:", int, sep = ')')
    parent = get_field(ld, "bound_parent:", int)
    insert=True
    if (not idx in all_parents):
        insert = False
    if insert:
        nodes_to_use[idx] = True


for ld in lines_data:
    idx = get_field(ld, "idx:", int)
    if idx in nodes_to_use:
        print(ld)