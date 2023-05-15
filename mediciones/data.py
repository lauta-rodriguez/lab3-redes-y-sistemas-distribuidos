import matplotlib.pyplot as plt

param = 'config' # key for the simulation parameters
scalar = 'scalars' # key for the scalars array
idx_delivered_packets = 7 # index of the delivered packets in the scalars json file
idx_avg_delay = 5 # index of the average delay in the scalars json file

def get_gen_interval(data, sim):
    # simulation parameters are stored under the key 'config'
    gen_interval_dict = data[sim][param][2]

    # get the generation interval (carga ofrecida) for this simulation
    gen_interval = gen_interval_dict['Network.nodeTx.gen.generationInterval']
    # get the number value from the key value
    gen_interval = float(gen_interval.split('(')[1].split(')')[0])

    return gen_interval


def get_sim_time(data, sim):
    sim_time_dict = data[sim][param][1]
    sim_time = sim_time_dict['sim-time-limit']
    sim_time = float(sim_time.split('s')[0])

    return sim_time


def get_avg_delivered(data, sim, sim_time):
    total_delivered = data[sim][scalar][idx_delivered_packets]['value']
    #print(f"total_delivered: {total_delivered}")
    return total_delivered/sim_time


def get_avg_delay(data, sim):
    return data[sim][scalar][idx_avg_delay]['value']

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
