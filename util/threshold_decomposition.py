import skimage as sk
import numpy as np
import argparse, operator

def bool_to_int(v):
    if(v==True):
        return 1
    else:
        return 0

def print_matrix(m):
    print(f'     ',end='')
    for k in range(len(m[0])):
        print(f'{k: 5d}',end='')
    print('')
    for i in range(len(m)):
        print(f'{i:5d}',end='')
        for j in range(len(m[i])):
            print(f'{m[i,j]: 5d}',end='')
        print('')


def neighbors(p, connectivity=4):
    ns = []
    if connectivity >= 4:
        ns+=[(-1,0), (1,0), (0,-1),(0,1)]
    if connectivity >=8:
        ns+=[(-1,-1), (1,-1), (-1,1), (1,1)]
    
    return [tuple(map(operator.add,p,n)) for n in ns]

def busca_largura(m,c=4):
    ccs=[]
    s = []
    visited={}
    for i in range(len(m)):
        for j in range(len(m[i])):
            if (not ((i,j) in visited)) and (m[i,j] != 0):
                s.append((i,j))
                cc=[(i,j)]
                visited[(i,j)]=True
                while s != []:
                    p = s[-1]
                    if not p in visited:
                        cc.append(p)
                        visited[p] = True
                    #print(p)
                    del s[-1]
                    val = m[p]
                    ns = neighbors(p,c)
                    #print(ns)
                    for n in ns:
                        if 0 <= n[0] < len(m) and 0 <= n[1] < len(m[i]):
                            n_val=m[n]
                            if (n_val == val) and (not (n in visited)):
                                s.append(n)
                        else:
                            if(verbose):
                                print('posicao invalida',n)
                ccs.append(cc)
                print(cc, len(cc))
    return ccs



def apply_threshold(img, th):
    new = img >= th
    int_vec = np.vectorize(bool_to_int)
    return np.array(int_vec(new), dtype=np.uint8)


parser = argparse.ArgumentParser()
parser.add_argument('file', help = 'filename')
parser.add_argument('--output', '-o', dest='output', default='fig', help='output file prefix')
parser.add_argument('--ext', '-e', dest='ext', default='png', help='output format')
parser.add_argument('--verbose', '-v', action='store_true', dest='verbose', help='print images')
args = parser.parse_args()


out_prefix = args.output
out_ext = args.ext
verbose = args.verbose

img = sk.io.imread(args.file)

#img = img*255

img = img.astype(np.uint8)
if verbose:
    print(img)

dec={}
attrs={}
old = apply_threshold(img,0)
dec[0] = old
ccs=busca_largura(old)
attrs[0] = [len(ccs[i]) for i in range(len(ccs))]

for i in range(1,256):
    new = apply_threshold(img, i)
    d = new != old
    if len(np.extract(d, d)) > 0:
        dec[i] = new
        ccs=busca_largura(new)
        attrs[i] = [len(ccs[ci]) for ci in range(len(ccs))]
        old = new

print('writing files')
txtname=f'{out_prefix}.txt'
str_out_text=''

for k in dec:
    fname = f'{out_prefix}_{k}.{out_ext}'
    if verbose:
        print(fname)
        print('')
        print_matrix(dec[k])
        print('=======================')
    im_save = dec[k]*255
    sk.io.imsave(fname,im_save,check_contrast=False)
    str_out_text += 'threshold:' + str(k) + ' --- area:' + str(attrs[k])+'\n'


f=open(txtname,'w')
f.write(str_out_text)
f.close()

