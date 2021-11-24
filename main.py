import pylab
import matplotlib.pyplot as plt
import numpy as np
from numpy import ma
from matplotlib import ticker, cm
##
import matplotlib.collections
from matplotlib import colors
from PIL import Image

#############

xmin = 0.0
xmax = 20.0
ymin = 0.0
ymax = 20.0
#
sizes = 0.8 
#### 
num_pic = 10

filename_base = 'coordinate_'

images = []

for number in range(num_pic):
    filename = filename_base + str(number) + '.txt'
    
    data = np.loadtxt(filename, skiprows=0)

    fig, ax = plt.subplots()
    coll = matplotlib.collections.EllipseCollection(sizes,  sizes,
                                                np.zeros_like(sizes),
                                                offsets=data, units='x',
                                                facecolors='g',
                                                transOffset=ax.transData)#,
#                                                **kwargs)
    ax.add_collection(coll)
    ax.margins(0.01)
    ax.set_xlim(xmin, xmax)
    ax.set_ylim(ymin, ymax)

    outfile = filename_base + str(number) + '.png'
    plt.savefig(outfile)
    im = Image.open(outfile)
    images.append(im)


##################
# convert ot gif
images[0].save('animation.gif', save_all=True, append_images=images[1:], optimize=False, duration=300, loop=0)
