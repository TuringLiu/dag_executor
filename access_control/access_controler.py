import json, os, sys, re

JSON_PATH = 'access_template.json'
DATA_DEFINE = 'data_define_template.thrift'

CLASS_MAP = {'i32': 'int', 
                   'i64': 'long long', 
                   'list': 'std::vector', 
                   'map': 'std::unordered_map', 
                   'set': 'std::set', 
                   'string': 'std::string'}

class_template = '''
#include "dag.h"

class %s: public Node
{
public:
    %s(const std::string& node_name) : Node(node_name) {} 
    virtual void access_control(Context& ctx) override {
        if(circuit_flag_ == true) copy_from_ctx(ctx);
        run_(%s);
        if(circuit_flag_ == true && status_ == true) copy_on_success(ctx);
    }
private:
    void run_(%s) ;

    virtual void copy_from_ctx(Context& ctx) override{
%s
    }
    virtual void copy_on_success(Context& ctx) override{
%s
    }
private:
    std::atomic<bool> circuit_flag_{false};
    std::atomic<bool> status_{false};

%s
};        
'''


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

def find_node_type(DIY_type, root_type="Context"):
    name_to_type = dict()
    def dfs(data_type, prefix=''):
        for data_type, data_name in DIY_type[data_type]:
            if prefix == '':
                complete_name = data_name
            else:
                complete_name = prefix+'.'+data_name

            name_to_type[complete_name] = data_type

            if data_type in DIY_type.keys():
                dfs(data_type, complete_name)
    
    dfs(root_type)
    # print(name_to_type)

    return name_to_type

def generate_access_func():
    with open(JSON_PATH, 'r') as f:
        json_data = json.load(f)

    DIY_type = IDL_parser(DATA_DEFINE)
    name_to_type = find_node_type(DIY_type)

    for node_name, node_info in json_data['node_info'].items():
        ctx_args = node_info['data']
        ctx_args_value = node_info['data']

        if 'circuit' in node_info.keys():
            circuit_flag = True
        else:
            circuit_flag = False


        func_args = [name_to_type[key] for key in ctx_args.keys()]

        ctx_args_str = ''
        ctx_args_upper = []
        ctx_args_upper_value_dict = dict()

        circuit_args_ctx_upper_dict = dict()

        # 映射大小写
        for arg, value in ctx_args.items():
            arg_list = arg.split(sep='.')
            arg_list_ = [a.upper() for a in arg_list[:-1]]
            arg_list_.append(arg_list[-1])
            ctx_args_upper.append('CTX_' + '_'.join(arg_list_))

            ctx_args_upper_value_dict[ctx_args_upper[-1]] = ctx_args_value[arg]
            if circuit_flag == True and value < 1:
                circuit_args_ctx_upper_dict[ctx_args_upper[-1]] = 'ctx.' + arg


            if len(ctx_args_str) == 0:
                start_str = ''
            else:
                start_str = ', '
            
            if circuit_flag == False or value > 0:
                ctx_args_str += start_str + 'ctx.' + arg

        # 映射 形参类型
        func_type_list = []
        for func_type in func_args:
            for thrift_type, cpp_type in CLASS_MAP.items():
                func_type = func_type.replace(thrift_type, cpp_type)
            func_type_list.append(func_type)

        func_args_str = ''
        circuit_args_type_name_dict = dict()

        for i, arg_type in enumerate(func_type_list):
            if len(func_args_str) == 0:
                start_str = ''
            else:
                start_str = ', '

            if ctx_args_upper_value_dict[ctx_args_upper[i]] > 0:
                func_args_str += start_str + 'const ' + arg_type + '& ' + ctx_args_upper[i]
            else:
                if circuit_flag == False:
                    func_args_str += start_str + arg_type + ' ' + ctx_args_upper[i]
                else:
                    circuit_args_type_name_dict[ctx_args_upper[i]] = func_type_list[i]

        # 熔断参数保护
        copy_from_ctx_str = ''
        copy_on_success_str = ''
        struct_define_str = ''

        if circuit_flag == True:
            for arg_name, arg_type in circuit_args_type_name_dict.items():
                struct_define_str += '  ' + arg_type + ' ' + arg_name + ';\n'
            
            for arg_name, ctx_name in circuit_args_ctx_upper_dict.items():
                copy_from_ctx_str += '      ' + arg_name + ' = ' + ctx_name + ';\n'
                copy_on_success_str += '    ' + ctx_name + ' = ' + arg_name + ';\n   '
        
        # output the class define
        class_str = class_template % (node_info['class_name'], 
                                node_info['class_name'], 
                                ctx_args_str, 
                                func_args_str,
                                copy_from_ctx_str,
                                copy_on_success_str,
                                struct_define_str)
        with open(node_name+'.h', 'w+') as f:
            f.write(class_str)

        

if __name__ == '__main__':
    generate_access_func()