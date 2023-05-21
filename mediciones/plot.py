from data import *
import matplotlib.pyplot as plt

AMOUNT_SIM = 5
SIM_TIME = 200

simulations = {
    'SIM_1': (1, 1),
    'SIM_2': (1, 2),
    'SIM_3': (2, 1),
    'SIM_4': (2, 2)
}

# TODO: definir colores para cada modulo en cada parte
# TODO: no hardcodear los nombres de los modulos, ponerlos en un diccionario

################################################################################
# SIMPLE PLOTS
################################################################################

def time_vs_buffer(module, sim, it=1):
    time = get_data(module, 'buffer size', 'time', 'vectors', sim, it)
    time = list(map(float, time))

    buffer_size = get_data(module, 'buffer size', 'value', 'vectors', sim, it)
    buffer_size = list(map(float, buffer_size))

    plt.plot(time, buffer_size, label=f'caso {sim[1]}')


def offered_vs_payload(sim):
    offered_load = [0]
    payload = [0]

    if (sim[0] == 1):
        transmitter = 'Network.nodeTx.queue'
        receiver = 'Network.nodeRx.sink'
    else:
        transmitter = 'Network.Transmitter.traTx'
        receiver = 'Network.Receiver.sink'

    for it in range(AMOUNT_SIM, 0, -1):
        # (module, metric, name, data_type, case_study)
        sent_packets = get_data(transmitter, 'sent packets',
                                'value', 'scalars', sim, it)
        offered_load.append(float(sent_packets)/SIM_TIME)

        delivered_packets = get_data(
            receiver, 'delivered packets', 'value', 'scalars', sim, it)
        payload.append(float(delivered_packets)/SIM_TIME)

    plt.plot(offered_load, payload, label=f'caso {sim[1]}')


def offered_vs_dropped(sim):
    offered_load = [0]
    dropped = [0]

    if (sim[0] == 1):
        transmitter = 'Network.nodeTx.queue'
        subnet = 'Network.queue'
        receiver = 'Network.nodeRx.queue'
    else:
        transmitter = 'Network.Transmitter.traTx'
        subnet = 'Network.Subnet'
        receiver = 'Network.Receiver.traRx'

    if (sim[1] == 1):
        module = receiver
    else:
        module = subnet

    for it in range(AMOUNT_SIM, 0, -1):
        # (module, metric, name, data_type, case_study)
        sent_packets = get_data(
            transmitter, 'sent packets', 'value', 'scalars', sim, it)
        offered_load.append(float(sent_packets)/SIM_TIME)

        dropped_packets = get_data(
            module, 'dropped packets', 'value', 'scalars', sim, it)
        dropped.append(float(dropped_packets))

    plt.plot(offered_load, dropped, label=f'caso {sim[1]}')

################################################################################
# COMPUND PLOTS
################################################################################

def time_vs_buffer_cmp(sim):
    plt.figure()

    plt.suptitle('Tiempo vs tamaño de búfer')

    plt.xlim(0, 200)
    plt.ylim(0, 200)

    time_vs_buffer('Network.nodeTx.queue', sim)
    time_vs_buffer('Network.queue', sim)
    time_vs_buffer('Network.nodeRx.queue', sim)

    plt.legend(loc='upper left')

    save_plot(f'time-buffer-p{sim[0]}c{sim[1]}.png')
    # plt.show()
    plt.clf()


def offered_vs_payload_cmp():
    plt.figure()

    plt.suptitle('Carga ofrecida vs Carga útil')

    plt.xlim(0, 10)
    plt.ylim(0, 10)

    for _, key in enumerate(simulations):
        offered_vs_payload(simulations[key])

    plt.legend(loc='upper left')

    save_plot('offered-payload.png')
    # plt.show()
    plt.clf()


def offered_vs_dropped_cmp():
    plt.figure()

    plt.suptitle('Carga ofrecida vs paquetes perdidos')

    plt.xlim(0, 10)
    plt.ylim(0, 1000)

    for _, key in enumerate(simulations):
        offered_vs_dropped(simulations[key])

    plt.legend(loc='upper left')

    save_plot('offered-dropped.png')
    #plt.show()
    plt.clf()
