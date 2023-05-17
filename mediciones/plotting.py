# argument manipulation
import argparse
# file manipulation
import os
import glob
# data parsing
import json
from data import *

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

# iterate over all simulation scalar results for this parte/caso
for i in range(1, 20):
    fscalar = f"scalar-p{p}-c{c}-0{i}.json"
    fvector = f"vector-p{p}-c{c}-0{i}.json"

    if os.path.isfile(fscalar):
        # open the json file
        with open(fscalar) as f:
            data = json.load(f)

        # the simulation is stored in the first key of the dictionary
        sim = list(data.keys())[0]

        # simulation parameters
        sim_time = get_sim_time(data, sim)
        gen_interval = get_gen_interval(data, sim, p)

        # simulation results
        avg_delivered = get_avg_delivered(data, sim, sim_time)
        #print(f"avg_delivered: {avg_delivered}")
        avg_delay = get_avg_delay(data, sim)
        #print(f"avg_delay: {avg_delay}")

        # append value to each axis data
        carga_ofrecida.append(gen_interval)
        carga_util.append(avg_delivered)
        retraso.append(avg_delay)

    else:  # there aren't any more simulations for this parte/caso
        break

    if os.path.isfile(fvector):
        # open the json file
        with open(fvector) as g:
            data = json.load(g)

        # the simulation is stored in the first key of the dictionary
        sim = list(data.keys())[0]

        tx_time = get_vector_time(data, sim, 'tx')
        tx_size = get_vector_size(data, sim, 'tx')
        subnet_time = get_vector_time(data, sim, 'subnet')
        subnet_size = get_vector_size(data, sim, 'subnet')
        rx_time = get_vector_time(data, sim, 'rx')
        rx_size = get_vector_size(data, sim, 'rx')

    else:  # there aren't any more simulations for this parte/caso
        break

ofrecida_vs_util(carga_ofrecida, carga_util, p, c)
ofrecida_vs_retardo(carga_ofrecida, retraso, p, c)
time_vs_bufferSize(tx_time, tx_size, subnet_time, subnet_size, rx_time, rx_size, p, c)
