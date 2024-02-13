'''
Desctiption: This script is used to visualize the DAGs in the form of graphviz.

function:
    1. find the dependency links about some data
    2. find the node dependency link

usage:
    cmd:  python3 dag_visualizer.py -data_list video.model_list
'''


import os
import sys
import json
import re
# import graphviz
from copy import deepcopy
import argparse
from dag_util import *

def get_node_attr(node_name: str, node_attr: dict):
    pass



def data_dot(args):
    data_structure = IDL_parser(args.IDL_PATH)
    DIY_expand = expand_DIY_type(data_structure)
    data_list = args.data_list

    with open(NODE_TEMPLATE_PATH, 'r') as f:
        json_data = json.load(f)

    node_data = expand_node_data(json_data, DIY_expand['Request'])
    dag_dep = generate_dag(node_data, DIY_expand)
    simplify_node_dep_ = simplify_node_dep(dag_dep)

    relative_nodes = dict()
    
    for node_name, data_deps in node_data.items():
        for data_dep in data_deps:
            flag = False
            for data_name in data_list:
                if data_name in data_dep[0]:
                    flag = True
                    break
            if flag == True:
                relative_nodes[node_name] = {'shape': 'box'}
                break

    graph = Dot()
    for node in relative_nodes.items():
        graph.add_node(node[0], node[1])
    
    for node_name, deps in simplify_node_dep_.items():
        if node_name not in relative_nodes:
            graph.add_node(node_name, {})
        for dep in deps:
            graph.add_edge(node_name, dep, {})
    
    # print(graph.dot())
    return graph.dot()
    

def node_dot(args):
    pass


def main(args):
    if args.v == False:
        dot = data_dot(args)
    else:
        dot = node_dot(args)
    
    if args.output == 'std':
        print(dot)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='DAG visualizer')

    parser.add_argument('-input',  required=False, help='node json file path', default='node_template.json')
    parser.add_argument('-output', required=False, help='output file path', default='std')
    parser.add_argument('-IDL_PATH', required=False, help='IDL file path', default='data_define.thrift')

    parser.add_argument('-v', action='store_true', help='False=data dependency, True=node dependency', default=False)
    parser.add_argument('-data_list', nargs='*', help='data list', default=[])
    parser.add_argument('-node_list', nargs='*', help='node list', default=[])

    args = parser.parse_args()

    main(args)


