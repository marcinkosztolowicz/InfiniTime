import numpy as np


def most_overlap_index(arr1, arr2):
    """
    this function find index of this first array where the overlap begins
    [1,2,3,4,5]
    [3,4,5,6,7]
    result: 2,3

    :param arr1: numpy array
    :param arr2: numpy array 
    :returns ind: index of first array wheere overlap begins, could be None when there is no overlap (less then minimum overlap size)
    :returns overlapped_size: estimeated size of overlap could be different (bigger) then real overlap
    """
    ind = None
    overlapped_size = 20 # guess
    for i in range(1, int(len(arr1)/2)):
        diff_arr = arr1[i:] - arr2[:-i]
        zeros_count = len(diff_arr) - np.count_nonzero(diff_arr)
        if overlapped_size < zeros_count:
            ind = i
            overlapped_size = zeros_count
    return ind, overlapped_size


def diff_subset_range(arr1, arr2):
    """
    If two arrays are always the same but have some different values, this function gets start and end index of such different values.
    It is usefull when ppg algorithm in infinitime is reseted

    :param arr1: numpy array
    :param arr2: numpy array 
    :returns : start and end index of different values range 
    """
    diff_arr = (arr1 - arr2)[:-4] #-4 for safety
    non_zero_indeces = np.nonzero(diff_arr)[0]
    return (min(non_zero_indeces),max(non_zero_indeces))


def add_new_data(aggregated_data, arr1, arr2):       
    ind, zeros = most_overlap_index(arr1, arr2)
    if ind:
        bad_ending_count = -(64 - ind - zeros)
        aggregator_striped =  aggregated_data[:bad_ending_count] if bad_ending_count < 0 else aggregated_data
        new_values = arr2[-ind:]
        result = np.append(aggregator_striped, new_values)        
    else:  #when ind is None usually when there is reset in ppg algorithm on infinitime
        inds = diff_subset_range(arr1 , arr2)
        new_values=(arr2)[inds[0]:inds[1]+1]
        result = np.append(aggregated_data, new_values)
    return result


async def process(data_generator, iteration_listener_func = None):
    last_data_array_awaitbale  = anext(data_generator)
    last_data_array = await last_data_array_awaitbale
    aggregated_data_output = last_data_array
    async for data in data_generator:              
        arr1 = last_data_array
        arr2 = data
        last_data_array = arr2
        aggregated_data_output = add_new_data(aggregated_data_output, arr1, arr2)        
        if iteration_listener_func:
            iteration_listener_func(aggregated_data_output, arr1, arr2)
    return aggregated_data_output


async def dataframe_to_generator(dataframe):
    dataframe = dataframe.drop(columns=["time"], errors='ignore')
    for i in dataframe.index:
        yield dataframe.iloc[i].to_numpy()