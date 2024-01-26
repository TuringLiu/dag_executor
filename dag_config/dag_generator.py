# this is a generator to generate dag by to analynis node data dependency

import os
import sys
import json
import re
import graphviz
from copy import deepcopy

IDL_PATH = './data_define.thrift'
NODE_TEMPLATE_PATH = './node_template.json'

SYS_BASIC = ['i32', 'i64', 'double', 'string', 'bool']
SYS_SET = ['map', 'list', 'set']

# 3: required map<string, list<double>> scores,required map<string, list<double>> scores,

def IDL_parser(IDL_PATH):
    def parse_line(line):
        line = re.split(r'\s|:', line)
        line_list = []
        temp = ""
        for li in line:
            if "," in li and "<" not in li:
                li = li.replace(",", "")
                temp += li
                temp = temp.replace(":", " ")
                temp = temp.replace("required", "")
                temp = temp.replace("optional", "")
                temp = re.sub(r'\s+', ' ', temp)
                temp = re.sub(r'^\s?\d+', '', temp)
                temp = re.split(r'\s', temp)
                temp = list(filter(lambda x: x != "", temp))
                data_name = temp[-1]
                data_type = "".join(temp[:-1])
                line_list.append((data_type, data_name))
                temp = ""
            else:
                temp += (li + " ")

        if temp != "" and len(temp) > 1:
            temp = temp.replace(":", " ")
            temp = temp.replace("required", "")
            temp = temp.replace("optional", "")
            temp = re.sub(r'\s+', ' ', temp)
            temp = re.sub(r'^\s?\d+', '', temp)
            temp = re.split(r'\s', temp)
            temp = list(filter(lambda x: x != "", temp))

            data_name = temp[-1]
            data_type = "".join(temp[:-1])
            if(len(data_name) > 0 and len(data_type) > 0):
                line_list.append((data_type, data_name))
            
        return line_list

    DIY_type = dict(list())
    with open(IDL_PATH, 'r') as f:
        idl_data = f.readlines()
    status = "wait_struct"

    sturct_name = ""
    for line in idl_data:
        if status == "wait_struct":
            if "struct" in line:
                re_str = r'struct\s+(\w+)\s+'
                matches = re.findall(re_str, line)
                if len(matches) > 0:
                    status = "left_brace"
                    for match in matches:
                        sturct_name = match
                        DIY_type[sturct_name] = list()
        if status == "left_brace":
            if "{" in line:
                status = "wait_field"
        
        if status == "wait_field":
            DIY_type[sturct_name] += parse_line(line)
            if "}" in line:
                status = "wait_struct"
    # for key, d in DIY_type.items():
    #     print(key, d)
    return DIY_type

def expand_DIY_type(DIY_type):
    def dfs(diy_type):
        diy_type = DIY_type[diy_type]
        diy_type_name = []
        for data_type, data_name in diy_type:
            flag = 0
            for diy in DIY_type.keys():
                if diy in data_type:
                    flag = 1
                    for son in dfs(diy):
                        diy_type_name.append(data_name+ '.'+ son)
            if flag == 0:
                diy_type_name.append(data_name)
        return diy_type_name

    DIY_expand = dict(list())
    for key, d in DIY_type.items():
        DIY_expand[key] = dfs(key)
    return DIY_expand


def expand_node_data(json_data, DIY_expand):
    node_info = json_data['node_info']
    node_data = dict()
    for node_name, node_detail in node_info.items():
        data = node_detail['data']
        data_keys = list(data.keys())
        for idx in range(0, len(data_keys)):
            for idx2 in range(idx+1, len(data_keys)):
                if data_keys[idx] in data_keys[idx2]:
                    print("error: the data name of %s and %s is not correct " % (data_keys[idx], data_keys[idx2]))
                    exit(1)
        # check the data name is corrcet
        node_data[node_name] = []
        for d, height in data.items():
            for min_data in DIY_expand:
                if d in min_data:
                    node_data[node_name].append((min_data, height))
    return node_data
            

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


def generate_dag(node_data, DIY_expand):
    node_dep = dict()
    for key in node_data.keys():
        node_dep[key] = []
    
    for node_name, node_data_ in node_data.items():
        for data, height in node_data_:
            if height == 0:
                continue
            flag = 0
            pre_data_height = height
            if height < 0:
                pre_data_height = abs(height)
            if height > 0:
                pre_data_height = - (height - 1)

            for pre_node_name, pre_node_data in node_data.items():
                if pre_node_name == node_name:
                    continue

                for pre_data, pre_height in pre_node_data:
                    if pre_data == data and pre_height == pre_data_height:
                        node_dep[node_name].append(pre_node_name)
                        flag = 1
            
            if height < 0 and flag == 0:
                pre_data_height = height + 1
                for pre_node_name, pre_node_data in node_data.items():
                    if pre_node_name == node_name:
                        continue

                    for pre_data, pre_height in pre_node_data:
                        if pre_data == data and pre_height == pre_data_height:
                            node_dep[node_name].append(pre_node_name)
                            flag = 1

            if flag == 0:
                print("error: exist a wrong data height: %s: %s %s" % (node_name, data, height))
                exit(1)
    

    # print(node_dep)

    print('generate dag success')
    return node_dep

def create_dependency_graph(dependencies):
    dot = graphviz.Digraph()

    # add node
    for node in dependencies:
        dot.node(node)

    # add edge
    for node, deps in dependencies.items():
        for dep in deps:
            dot.edge(node, dep)

    return dot

def simplify_node_dep(node_dep):
    # remove redundant dependency
    for key in node_dep.keys():
        node_dep[key] = list(set(node_dep[key]))

    # remove corss-layer dependency     
    new_node_dep = dict()
    for key in node_dep.keys():
        new_node_dep[key] = []
    
    def dfs(node_name, deps):
        if node_name in deps:
            return True
        for dep in deps:
            if dfs(node_name, node_dep[dep]):
                return True
        return False
        
    for node, deps in node_dep.items():
        for dep in deps:
            flag = 0
            for dep_ in deps:
                if dep == dep_:
                    continue
                if dfs(dep, node_dep[dep_]):
                    flag = 1
            if flag == 0:
                new_node_dep[node].append(dep)
    print(new_node_dep)
    return new_node_dep

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

    node_dep = generate_dag(node_data, DIY_expand)

    return node_dep   

if __name__ == '__main__':
    DIY_expand = phase_1()
    node_dep = phase_2(DIY_expand['Request'])
    simplify_node_dep_ = simplify_node_dep(node_dep)
    
    # generate dag.json
    with open(NODE_TEMPLATE_PATH, 'r') as f:
        json_data = json.load(f)
    dag_json = dict()
    dag_json['dag'] = json_data['dag']
    dag_json['node_dep'] = simplify_node_dep_
    dag_path = './%s.json' % json_data['dag']
    with open(dag_path, 'w') as f:
        json.dump(dag_json, f, indent=4)

    # dot = create_dependency_graph(simplify_node_dep_)
    # print(dot.source)
