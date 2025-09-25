import skimage as sk
import numpy as np
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('file', help = 'filename')
parser.add_argument('--output', '-o', dest='output', default='fig.png', help='output file')
parser.add_argument('--show-values', '-s', dest='show_values', action='store_true', help='print values and columns length')

args = parser.parse_args()

arr=[]

with open(args.file) as f:
    lines = f.readlines()
    for line in lines:
        spl=line.split()
        if args.show_values:
            print(spl, len(spl))
        arr.append([int(x) for x in spl])


img = np.array(arr,dtype=np.uint8)

if args.show_values:
    print(img)


sk.io.imsave(args.output,img)