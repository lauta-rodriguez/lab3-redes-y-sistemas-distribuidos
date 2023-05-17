import os
import json
import matplotlib.pyplot as plt

param = 'config'  # key for the simulation parameters
scalar = 'scalars'  # key for the scalars array
vector = 'vectors'

# DATA extractor
def get_scalar_data(p,c):
    carga_ofrecida = [0]
    carga_util = [0]
    retraso = [0]

    for i in range(5, 0, -1):
        fscalar = f"scalar-p{p}-c{c}-0{i}.json"
        print(fscalar)

        if os.path.isfile(fscalar):
            # open the json file
            with open(fscalar) as f:
                data = json.load(f)

            # the simulation is stored in the first key of the dictionary
            sim = list(data.keys())[0]
            # simulation parameters
            sim_time = get_sim_time(data, sim)
            # simulation results
            avg_sent = get_avg_sent(data, sim, sim_time)
            avg_delivered = get_avg_delivered(data, sim, sim_time)
            avg_delay = get_avg_delay(data, sim)
            # append value to each axis data
            carga_ofrecida.append(float(avg_sent))
            carga_util.append(float(avg_delivered))
            retraso.append(float(avg_delay))

        else:  # there aren't any more simulations for this parte/caso
            break

    return (carga_ofrecida, carga_util, retraso)

def get_vector_data(p,c):
    tx_time = [0]
    tx_size = [0]
    subnet_time = [0]
    subnet_size = [0]
    rx_time = [0]
    rx_size = [0]

    for i in range(1, 5):
        fvector = f"vector-p{p}-c{c}-0{i}.json"
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

    return (tx_time, tx_size, subnet_time, subnet_size, rx_time, rx_size)

# JSON HELPERS
def get_index_of_scalar(name, scalars):
    for index, scalar in enumerate(scalars):
        if scalar["name"] == name:
            return index

# DATA GETTERS
def get_sim_time(data, sim):
    sim_time_dict = data[sim][param][1]
    sim_time = sim_time_dict['sim-time-limit']
    sim_time = float(sim_time.split('s')[0])

    return sim_time

def get_avg_sent(data, sim, sim_time):
    idx_sent_packets = get_index_of_scalar('sent packets', data[sim][scalar])
    total_sent = data[sim][scalar][idx_sent_packets]['value']

    return total_sent/sim_time

def get_avg_delivered(data, sim, sim_time):
    scalar_array = data[sim][scalar]

    idx_delivered_packets = get_index_of_scalar('delivered packets', scalar_array)
    total_delivered = scalar_array[idx_delivered_packets]['value']

    return total_delivered/sim_time

def get_avg_delay(data, sim):
    idx_avg_delay = get_index_of_scalar('average delay', data[sim][scalar])

    return data[sim][scalar][idx_avg_delay]['value']


def get_vector_time(data, sim, module):
    idx_module = None

    if module == 'tx':
        idx_module = 0
    elif module == 'subnet':
        idx_module = 1
    elif module == 'rx':
        idx_module = 2

    return data[sim][vector][idx_module]['time']


def get_vector_size(data, sim, module):
    idx_module = None

    if module == 'tx':
        idx_module = 0
    elif module == 'subnet':
        idx_module = 1
    elif module == 'rx':
        idx_module = 2

    return data[sim][vector][idx_module]['value']
