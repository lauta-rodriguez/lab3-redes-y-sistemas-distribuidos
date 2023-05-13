# argument manipulation
import argparse
# file manipulation
import os
import glob
# data parsing
import json
from data import *
# plotting
import matplotlib.pyplot as plt

# create an ArgumentParser object
parser = argparse.ArgumentParser(
    usage='%(prog)s --parte=<1,2> --caso=<1,2>'
)

# add the arguments
parser.add_argument('--parte', type=int, help='an integer for parte')
parser.add_argument('--caso', type=int, help='an integer for caso')

# parse the arguments
args = parser.parse_args()

# check if arguments are valid
if args.parte is None or args.caso is None:
    parser.print_usage()
    exit()

# store the arguments in variables
p = args.parte
c = args.caso

# create list for each axis of both plots
carga_ofrecida = [0]
carga_util = [0]
retraso = [0]

# iterate over all simulation results for this parte/caso
for i in range(1, 20):
    filename = f"p{p}-c{c}-0{i}.json"

    if os.path.isfile(filename):
        # open the json file
        with open(f"p{p}-c{c}-0{i}.json") as f:
            data = json.load(f)

        # the simulation is stored in the first key of the dictionary
        sim = list(data.keys())[0]

        # get the data from the simulation
        sim_time = get_sim_time(data, sim)
        gen_interval = get_gen_interval(data, sim)
        delivered_total = get_delivered_packets(data, sim)

        # todavia no guardamos el delay
        # delay = get_delay(data, sim)

        # carga útil se mide en paquetes por segundo
        delivered_avg = delivered_total / sim_time

        carga_ofrecida.append(gen_interval)
        carga_util.append(delivered_avg)

    else: # there aren't any more simulations for this parte/caso
        break

print(carga_ofrecida)
print(carga_util)

plt.plot(carga_ofrecida, carga_util)

plt.xlabel('Carga ofrecida (paquetes/seg)')
plt.ylabel('Carga útil (paquetes/seg)')

plt.xlim(0, 1)
plt.ylim(0, 50)

plt.show()
#plt.savefig(f"p{p}-c{c}.png")

# TODO: crear y guardar grafico de retraso vs carga ofrecida
