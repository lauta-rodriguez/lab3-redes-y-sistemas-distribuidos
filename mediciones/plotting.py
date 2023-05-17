from data import *

#carga_ofrecida_p2c1, carga_util_p2c1, retraso_p2c1 = get_scalar_data(2, 1)
#carga_ofrecida_p2c2, carga_util_p2c2, retraso_p2c2 = get_scalar_data(2, 2)
#
#xlim = max(max(carga_ofrecida_p2c1),max(carga_ofrecida_p2c2))
#ylim = max(max(carga_util_p2c1),max(carga_util_p2c2))
#
#plt.plot(carga_ofrecida_p2c1, carga_util_p2c1, label='caso 1')
#plt.plot(carga_ofrecida_p2c2, carga_util_p2c2, label='caso 2')
#
#plt.xlabel('Carga ofrecida (paquetes/seg)')
#plt.ylabel('Carga útil (paquetes/seg)')
#
#plt.xlim(0, xlim)
#plt.ylim(0, ylim)
#
#plt.legend(loc='upper left')
#
#plt.savefig('ofrecida_vs_util_caso1.png')
#
#plt.clf()
#
#xlim = max(max(carga_ofrecida_p2c1),max(carga_ofrecida_p2c2))
#ylim = max(max(retraso_p2c1),max(retraso_p2c2))
#
#plt.plot(carga_ofrecida_p2c1, retraso_p2c1, label='caso 1')
#plt.plot(carga_ofrecida_p2c2, retraso_p2c2, label='caso 2')
#
#plt.xlabel('Carga ofrecida (paquetes/seg)')
#plt.ylabel('Retardo (seg)')
#
#plt.xlim(0, xlim)
#plt.ylim(0, ylim)
#
#plt.legend(loc='upper left')
#plt.savefig('ofrecida_vs_retardo_caso1.png')
#
#plt.clf()

tx_time_p2c1, tx_size_p2c1, subnet_time_p2c1, subnet_size_p2c1, rx_time_p2c1, rx_size_p2c1 = get_vector_data(2, 1)
tx_time_p2c2, tx_size_p2c2, subnet_time_p2c2, subnet_size_p2c2, rx_time_p2c2, rx_size_p2c2 = get_vector_data(2, 2)

plt.plot(tx_time_p2c1, tx_size_p2c1, label='tx')
plt.plot(subnet_time_p2c1, subnet_size_p2c1, label='subnet')
plt.plot(rx_time_p2c1, rx_size_p2c1, label='rx')

plt.xlabel('Tiempo (seg)')
plt.ylabel('Ocupación del buffer')
plt.legend(loc='upper left')

plt.show()
