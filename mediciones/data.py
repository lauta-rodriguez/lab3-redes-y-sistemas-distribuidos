import matplotlib.pyplot as plt

param = 'config'  # key for the simulation parameters
scalar = 'scalars'  # key for the scalars array
vector = 'vectors'
#idx_delivered_packets = 7  # index of the delivered packets in the scalars json file
#idx_avg_delay = 5  # index of the average delay in the scalars json file
idx_gen_interval = 3

# JSON HELPERS
def get_index_of(name, scalars):
    for index, scalar in enumerate(scalars):
        if scalar["name"] == name:
            return index

#def get_index_of(name, config):
#    for index, config in enumerate(config):
#        if scalar[config] == name:
#            return index

# DATA GETTERS
def get_gen_interval(data, sim, p):
    # simulation parameters are stored under the key 'config'
    #config_dict = data[sim][param]

    if (p == 1):
        id = 2
        key_gen = 'Network.nodeTx.gen.generationInterval'
    else:
        id = 3
        key_gen = 'Network.Transmitter.gen.generationInterval'

    # get the generation interval (carga ofrecida) for this simulation
    gen_interval = data[sim]['config'][id][key_gen]
    print(f"gen_interval: {gen_interval}")
    # get the number value from the key value
    gen_interval = float(gen_interval.split('(')[1].split(')')[0])

    return gen_interval


def get_sim_time(data, sim):
    sim_time_dict = data[sim][param][1]
    sim_time = sim_time_dict['sim-time-limit']
    sim_time = float(sim_time.split('s')[0])

    return sim_time


def get_avg_delivered(data, sim, sim_time):
    scalar_array = data[sim][scalar]  # [idx_delivered_packets]['value']
    # print(f"total_delivered: {total_delivered}")
    idx_delivered_packets = get_index_of('Delivered packets', scalar_array)
    total_delivered = scalar_array[idx_delivered_packets]['value']
    #print(f"idx_delivered_packets: {idx_delivered_packets}")
    return total_delivered/sim_time

def get_avg_delay(data, sim):
    idx_avg_delay = get_index_of('Avg delay', data[sim][scalar])
    #print(f"idx_avg_delay: {idx_avg_delay}")
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

# GRAPHS


def ofrecida_vs_util(carga_ofrecida, carga_util, p, c):
    plt.plot(carga_ofrecida, carga_util)

    plt.xlabel('Carga ofrecida (paquetes/seg)')
    plt.ylabel('Carga útil (paquetes/seg)')

    plt.xlim(0, 1)
    plt.ylim(0, 50)

    plt.savefig(f"p{p}-c{c}-util.png")
    plt.clf()


def ofrecida_vs_retardo(carga_ofrecida, retraso,  p, c):
    plt.plot(carga_ofrecida, retraso)

    plt.xlabel('Carga ofrecida (paquetes/seg)')
    plt.ylabel('Retardo (seg)')

    plt.xlim(0, 1)
    plt.ylim(0, 50)

    plt.savefig(f"p{p}-c{c}-retardo.png")
    plt.clf()


def time_vs_bufferSize(t_tx, b_tx, t_sn, b_sn, t_rx, b_rx, p, c):
    plt.plot(t_tx, b_tx, label='Tx', color='red')

    plt.xlabel('Tiempo (seg)')
    plt.ylabel('Tamaño del buffer (paquetes)')

    plt.xlim(0, 200)
    plt.ylim(0, 200)

    plt.plot(t_sn, b_sn, label='Subnet', color='green')
    plt.plot(t_rx, b_rx, label='Rx', color='blue')

    plt.legend()

    plt.savefig(f"p{p}-c{c}-bufferSize.png")
    plt.clf()
