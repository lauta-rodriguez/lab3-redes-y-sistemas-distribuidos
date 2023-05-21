# here will go helper functions for plot.py that will get the required information
# for a specific plot from the json files
import os
import json
import matplotlib.pyplot as plt

FILE_NAME_FORMAT = './data-samples/{type}-p{scenario}-c{case}-s{iteration}.json'
GRAPH_DIR = './graphs'

# helper function. Returns the index of the vector with the given module and name
def get_index(dict, module, name, data_type):
    for index, element in enumerate(dict[data_type]):
        if element['module'] == module and element['name'] == name:
            return index
    return -1  # Return -1 if the vector is not found

# returns the data of the vector/scalar with the given module, metric and name
def get_data(module, metric, name, data_type, sim, it):
    fvector = FILE_NAME_FORMAT.format(
        type=data_type, scenario=sim[0], case=sim[1], iteration=it)

    if os.path.isfile(fvector):
        # open the json file
        with open(fvector) as f:
            data = json.load(f)

        # the simulation data is stored in the first key of the json dictionary
        sim_dict = data[list(data.keys())[0]]

        # get the index of the vector
        index = get_index(sim_dict, module, metric, data_type)

        return sim_dict[data_type][index][name]

    else:
        print(f'File {fvector} not found.')

    return None

# saves a plot with the speccified filename
def save_plot(filename):
    filename = GRAPH_DIR + '/' + filename
    _, name, extension = filename.split('.')

    i = 0
    while (os.path.isfile(f'.{name}-{i}.{extension}')):
        i += 1

    # split the filename into name and extension
    filename = f'.{name}-{i}.{extension}'

    plt.savefig(filename)
    print(f'Figure saved as {filename}')