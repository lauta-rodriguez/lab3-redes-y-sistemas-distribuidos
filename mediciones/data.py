def get_gen_interval(data, sim):
    # simulation parameters are stored under the key 'config'
    gen_interval_dict = data[sim]['config'][2]

    # get the generation interval (carga ofrecida) for this simulation
    gen_interval = gen_interval_dict['Network.nodeTx.gen.generationInterval']
    # get the number value from the key value
    gen_interval = float(gen_interval.split('(')[1].split(')')[0])

    return gen_interval


def get_sim_time(data, sim):
    sim_time_dict = data[sim]['config'][1]
    sim_time = sim_time_dict['sim-time-limit']
    sim_time = float(sim_time.split('s')[0])

    return sim_time


def get_delivered_packets(data, sim):
    return data[sim]['vectors'][5]['value'][-1]

def get_delay(data, sim):
    return data[sim]['vectors'][6]['value'][-1]
