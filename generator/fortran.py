#!/usr/bin/python3

import os
import re
import common

types = ""
interface = "\tinterface\n"
contains = "\tcontains\n"

def name_mangle(name):
    if len(name) > 62:
        if name == "so_DiagnosticPlotsStructuralModel_create_IndivPredictionVsObserv":
            mangled_name = "so_DiagnosticPlotsStructuralModel_create_IndivPredictionVsObs"
        else:
            raise Exception("name_mangle could not mangle: " + name)
    else:
        mangled_name = name
    return mangled_name

def create_common():
    global interface
    interface += '\t\tfunction so_strlen(string) bind(C, name="strlen")\n'
    interface += "\t\t\tuse iso_c_binding\n"
    interface += "\t\t\ttype(c_ptr),intent(in),value :: string\n"
    interface += "\t\t\tinteger(kind = c_size_t) :: so_strlen\n"
    interface += "\t\tend function so_strlen\n\n"

def parse_prototypes(name):
    global types
    protolist = []
    result = []
    with open("../../soc/include/so/" + name, "r") as header:
        for line in header:
            line = line.rstrip()
            if "(" in line and line.endswith(");"):
                protolist.append(line)
            m = re.match("^typedef struct (\w+)", line)
            if m:
                types += "\ttype " + m.group(1) + "\n"
                types += "\t\ttype(c_ptr) :: ptr\n"
                types += "\tend type " + m.group(1) + "\n\n"

    for p in protolist:
        function = {}
        m = re.match("(\w+(?: \*)?)\s*(\w+)", p)
        function['return_type'] = m.group(1)
        function['function_name'] = name_mangle(m.group(2))
        function['c_function_name'] = m.group(2)
        m = re.search("\((.*)\)", p)
        arglist = m.group(1).split(",")
        function['argnames'] = []
        function['argtypes'] = []
        for a in arglist:
            a = a.lstrip()
            m = re.match("(\w+(?: \*)?)\s*(\w+)", a)
            if m:
                function['argnames'].append(m.group(2))
                function['argtypes'].append(m.group(1))
        result.append(function)

    return result

def get_c_type_declaration(c_type, name, result=False): 
    if not result:
        value = ",value"
    else:
        value = ""

    decl = ""
    if c_type == "char *" and not result:
        decl = "character(kind=c_char) :: " + name + "(*)"
    elif c_type == "char *" and result:
        decl = "type(c_ptr) :: " + name
    elif c_type == "int":
        decl = "integer(c_int)" + value + " :: " + name
    elif "*" in c_type:
        decl = "type(c_ptr)" + value + " :: " + name

    decl += "\n"

    return decl

def get_fort_type_declaration(c_type, name, result=False):
    decl = ""

    if c_type == "char *" and result:
        decl = "character,pointer,dimension(:) :: " + name 
    elif c_type == "char *" and not result:
        decl = "character(len=*) :: " + name
    elif c_type == "int":
        decl = "integer :: " + name
    elif c_type == "int *":
        decl = "integer,pointer :: " + name
    elif c_type == "double":
        decl = "real(kind=dp) :: " + name
    elif c_type == "double *":
        if name == "so_Matrix_get_data":
            decl = "real(kind=dp),dimension(:,:),pointer :: " + name
        else:
            decl = "real(kind=dp),pointer :: " + name
    elif "*" in c_type:
        decl = "type(" + c_type[0:-2] + ")" + " :: " + name
    elif c_type == "so_xml":
        decl = "type(so_xml) :: " + name

    decl += "\n"

    return decl

def get_argument_conversion(c_type, name):
    # Create code and type declarations for conversion of fortran argument into c argument
    if c_type == "char *":
        declaration = "character(len=len_trim(" + name + ") + 1) :: new_" + name + "\n"
        code = "new_" + name + " = trim(" + name + ") // c_null_char\n"
    elif c_type == "int":
        declaration = "integer(kind=c_int) :: new_" + name + "\n"
        code = "new_" + name + " = " + name + "\n"
    elif c_type == "int *":
        declaration = "type(c_ptr) :: new_" + name + "\n"
        code = "new_" + name + " = c_loc(" + name + ")\n"
    elif c_type == "double":
        declaration = "real(kind=c_double) :: new_" + name + "\n"
        code = "new_" + name + " = " + name + "\n"
    elif c_type == "double *":
        declaration = "type(c_ptr) :: new_" + name + "\n"
        code = "new_" + name + " = c_loc(" + name + ")\n"
    elif "*" in c_type:
        declaration = "type(c_ptr) :: new_" + name + "\n"
        code = "new_" + name + " = " + name + "%ptr\n"

    return (declaration, code)

def get_return_conversion(c_type, name):
    code = ""
    if c_type == "char *":
        code = "call c_f_pointer(res, " + name + ", [so_strlen(res)])"
    elif c_type == "int":
        code = name + " = res"
    elif c_type == "int *":
        code = "call c_f_pointer(res, " + name + ")"
    elif c_type == "double":
        code = name + " = res"
    elif c_type == "double *":
        if name == "so_Matrix_get_data":
            code = "call c_f_pointer(res, " + name + ", [so_Matrix_get_number_of_columns(self), so_Matrix_get_number_of_rows(self)])"
        else:
            code = "call c_f_pointer(res, " + name + ")"
    elif "*" in c_type:
        code = name + "%ptr = res"
    return code + "\n"

def add_header(module):
    global types, interface, contains
    prototypes = parse_prototypes(module)

    for fn in prototypes:
        if fn['return_type'] == 'void':
            fn_or_sub = "subroutine"
        else:
            fn_or_sub = "function"
        interface += "\t\t" + fn_or_sub + " c" + fn['function_name'] + "(" + ", ".join(fn['argnames']) + ")"
        interface += ' bind(C, name="' + fn['c_function_name'] + '")' + "\n"
        interface += "\t\t\tuse iso_c_binding\n"
        for (argname, argtype) in zip(fn['argnames'], fn['argtypes']):
            interface += "\t\t\t" + get_c_type_declaration(argtype, argname)
        if fn['return_type'] != "void":
            interface += "\t\t\t" + get_c_type_declaration(fn['return_type'], "c" + fn['function_name'], result=True)
        interface += "\t\tend " + fn_or_sub + " c" + fn['function_name'] + "\n"
        interface += "\n"

    # Create wrapper function
    for fn in prototypes:
        # create function header
        if fn['return_type'] == 'void':
            fn_or_sub = "subroutine"
        else:
            fn_or_sub = "function"
        contains += "\t\t" + fn_or_sub + " " + fn['function_name'] + "(" + ", ".join(fn['argnames']) + ")" + "\n"

        declarations = ""
        argument_conversion = ""
        code = ""

        # create argument declarations and conversions
        for (argname, argtype) in zip(fn['argnames'], fn['argtypes']):
            declarations += "\t\t\t" + get_fort_type_declaration(argtype, argname)
            (argument_conversion_declaration, argument_conversion_code) = get_argument_conversion(argtype, argname)
            declarations += "\t\t\t" + argument_conversion_declaration
            code += "\t\t\t" + argument_conversion_code
            
        if fn['return_type'] != "void":
            declarations += "\t\t\t" + get_fort_type_declaration(fn['return_type'], fn['function_name'], result=True)
            declarations += "\t\t\t" + get_c_type_declaration(fn['return_type'], "res", result=True)

        # create call to c function
        call_string = "\t\t\t"
        if fn['return_type'] == 'void':
            call_string += "call "
        else:
            call_string += "res = "

        newnames = []
        for argname in fn['argnames']:
            newnames.append("new_" + argname)
        call_string += "c" + fn['function_name'] + "(" + ", ".join(newnames) + ")\n"

        if fn['return_type'] != 'void':
            call_string += "\t\t\t" + get_return_conversion(fn['return_type'], fn['function_name'])

        contains += declarations + code + call_string
        contains += "\t\tend " + fn_or_sub + " " + fn['function_name'] + "\n\n"

def line_break(lines):
    line_array = lines.splitlines()
    line_array_linebreak = ""
    for line in line_array:
        if len(line) > 131:
            line_array_linebreak += line[:131] + "&\n"
            line_array_linebreak += line[131:] + "\n"
        else:
            line_array_linebreak += line + "\n"
    return line_array_linebreak

def create_module():
    global types, interface, contains
    with open("../fortran/libsoc.f03", "w") as f: 
        common.output_file = f
        common.print_license_string(language="fortran")
        print(file=f)
        print("module libsoc", file=f)
        print(file=f)
        print("\tuse iso_c_binding", file=f)
        print(file=f)
        print("\timplicit none", file=f)
        print(file=f)
        print("\tinteger, parameter :: dp = selected_real_kind(15, 307)", file=f)
        print(file=f)

        print(types, end='', file=f)

        interface += "\tend interface\n"
        interface = line_break(interface)
        print(interface, file=f)

        contains = line_break(contains)
        print(contains, end='', file=f)
        print("end module libsoc", file=f)

header_files = os.listdir("../include/so/")
header_files.remove("xml.h")
header_files.remove("Table.h")
for name in header_files:
    if not name.endswith(".h"):
        header_files.remove(name)

create_common()

for m in header_files:
    add_header(m)

create_module()
