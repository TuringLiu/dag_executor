# this is a generator to generate dag by to analynis node data dependency

from dag_util import *
import json
import re
from copy import deepcopy

# check two points:
# 1. the non-negative number can only appear once
# 2. the height of data will increase one by one
def check_height(node_data, DIY_expand):
    data_height_list = dict()
    
    for node_name, data in node_data.items():
        for d, height in data:
            if d not in data_height_list:
                data_height_list[d] = [height]
            else:
                data_height_list[d].append(height)
    
    for d, height_list in data_height_list.items():
        height_list = sorted(height_list)
        min_height = height_list[0]
        max_height = height_list[-1]
        if max_height > (abs(min_height) + 1):
            print("error: the height of %s is not correct: %s" % (d, height_list))
            return False
        
        if height_list[abs(min_height)] != 0 or (len(height_list) > (abs(min_height) + 1) and height_list[abs(min_height)+1] != 1):
            print("error: the height of %s is not correct: %s" % (d, height_list))
            return False

    print('check height success')   
    return True 

# phase1: get the data structure in IDL: data_define.thrift
def phase_1():
    data_structure = IDL_parser(IDL_PATH)
    DIY_expand = expand_DIY_type(data_structure)
    return DIY_expand

# phase2: check node and generate dag by data structure
def phase_2(DIY_expand):
    with open(NODE_TEMPLATE_PATH, 'r') as f:
        json_data = json.load(f)

    node_data = expand_node_data(json_data, DIY_expand)

    if not check_height(node_data, DIY_expand):
        exit(1)

    dag_dep = generate_dag(node_data, DIY_expand)

    return dag_dep   

# phase3: add the node class file
def phase3():
    pass

if __name__ == '__main__':
    DIY_expand = phase_1()
    dag_dep = phase_2(DIY_expand['Request'])
    simplify_node_dep_ = simplify_node_dep(dag_dep)
    
    # generate dag.json
    with open(NODE_TEMPLATE_PATH, 'r') as f:
        json_data = json.load(f)
    dag_json = dict()
    dag_json['dag'] = json_data['dag']
    dag_json['node_dep'] = simplify_node_dep_
    dag_path = './%s.json' % json_data['dag']
    with open(dag_path, 'w') as f:
        json.dump(dag_json, f, indent=4)
    print('generate dag success')

    phase3()
    # dot = create_dependency_graph(simplify_node_dep_)
    # print(dot.source)
