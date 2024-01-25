# this is a generator to generate dag by to analynis node data dependency

import os
import sys
import json


NODE_TEMPLATE_PATH = './node_template.json'

if __name__ == '__main__':
    with open(NODE_TEMPLATE_PATH, 'r') as f:
        json_data = json.load(f)
    print(json_data)