#!/usr/bin/python3

# SO XML data binding for ISO C

import os
import sys
import shutil
import common
from structure import structure, need_name

def create_includes(f, name):
    print("#include <string.h>", file=f)
    print("#include <libxml/tree.h>", file=f)
    print('#include <so/', name, '.h>', sep='', file=f)
    print('#include <so/private/', name, '.h>', sep='', file=f)
    print('#include <so/private/util.h>', file=f)
    print(file=f)

def create_new(f, name):
    print("so_", name, " *so_", name, "_new()", sep='', file=f)
    print("{", file=f);
    print("\tso_", name, " *object = extcalloc(sizeof(so_", name, "));", sep='', file=f)
    print("\tobject->reference_count = 1;", file=f)
    print(file=f)
    print("\treturn object;", file=f)
    print("}", file=f)

def create_free(f, name, elements):
    print("void so_", name, "_free(so_", name, " *self)", sep='', file=f)
    print("{", file=f)
    print("\tif (self) {", file=f)

    if 'children' in elements:
        children = elements['children']
        for e in children:
            is_array = e.get('array', False)
            if is_array:
                print("\t\tfor(int i = 0; i < self->num_", e['name'], "; i++) {", sep='', file=f)
                print("\t\t\tso_", e['type'], "_unref(self->", e['name'], "[i]);", sep='', file=f)
                print("\t\t}", sep='', file=f)
                print("\t\tfree(self->", e['name'], ");", sep='', file=f)
            else:
                print("\t\tso_", e['type'], "_unref(self->", e['name'], ");", sep='', file=f)
    if 'attributes' in elements:
        for a in elements['attributes']:
            print("\t\tif (self->", a, ") free(self->", a, ");", sep='', file=f)
    print("\t\tfree(self);", file=f);
    print("\t}", file=f)
    print("}", file=f)

def create_ref_unref(f, name):
    print("void so_", name, "_ref(so_", name, " *self)", sep='', file=f)
    print("{", file=f)
    print("\tself->reference_count++;", file=f)
    print("}", file=f)
    print(file=f)
    print("void so_", name, "_unref(so_", name, " *self)", sep='', file=f)
    print("{", file=f)
    print("\tif (self) {", file=f)
    print("\t\tself->reference_count--;", file=f)
    print("\t\tif (!self->reference_count) {", file=f)
    print("\t\t\tso_", name, "_free(self);", sep='', file=f)
    print("\t\t}", file=f)
    print("\t}", file=f)
    print("}", file=f)
    print(file=f)

def create_getters(f, name, elements):
    # getters for attributes
    if 'attributes' in elements:
        for a in elements['attributes']:
            print("char *so_", name, "_get_", a, "(so_", name, " *self)", sep='', file=f)
            print("{", file=f)
            print("\treturn self->", a, ";", sep='', file=f)
            print("}", file=f)
            print(file=f)

    # getters for child elements
    if 'children' in elements:
        children = elements['children']
        for e in children:
            print("so_", e['type'], " *so_", name, "_get_", e['name'], "(so_", name,  " *self", sep='', end='', file=f)
            if e.get('array', False):
                print(", int number)", file=f)
            else:
                print(")", file=f)
            print("{", file=f)
            print("\treturn self->", e['name'], sep='', end='', file=f)
            if e.get('array', False):
                print("[number]", end='', file=f);
            print(";", file=f)
            print("}", file=f)
            print(file=f)

        # get_number_of_ for arrays
        for e in children:
            if e.get('array', False):
                print("int so_", name, "_get_number_of_", e['name'], "(so_", name, " *self)", sep='', file=f)
                print("{", file=f)
                print("\treturn self->num_", e['name'], ";", sep='', file=f)
                print("}", file=f)
                print(file=f)

def create_setters(f, name, elements):
    # setters for attributes
    if 'attributes' in elements:
        for a in elements['attributes']:
            print("void so_", name, "_set_", a, "(so_", name, " *self, char *value)", sep='', file=f)
            print("{", file=f)
            print("\tfree(self->", a, ");", sep='', file=f)
            print("\tself->", a, " = extstrdup(value);", sep='', file=f)
            print("}", file=f)
            print(file=f)

    # setters for child elements
    if 'children' in elements:
        children = elements['children']
        for e in children:
            if not e.get('array', False):       # No setters for arrays
                print("void so_", name, "_set_", e['name'], "(so_", name, " *self, ", "so_", e['type'], " *value)", sep='', file=f)
                print("{", file=f)
                print("\tself->", e['name'], " = value;", sep='', file=f)
                print("}", file=f)
                print(file=f)


def create_array_add(f, name, elements):
    for e in elements['children']:
        if e.get('array', False):
            print("void so_", name, "_add_", e['name'], "(so_", name, " *self, so_", e['type'], " *child)", sep='', file=f)
            print("{", file=f)
            print("\tself->num_", e['name'], "++;", sep='', file=f)
            print("\tself->", e['name'], " = extrealloc(self->", e['name'], ", self->num_", e['name'], " * sizeof(so_", e['type'], " *));", sep='', file=f)
            print("\tself->", e['name'], "[self->num_", e['name'], " - 1] = child;", sep='', file=f)
            print("}", file=f)
            print(file=f)


def create_create(f, name, elements):
    for e in elements['children']:
        is_array = e.get('array', False)
        print("so_", e['type'], " *so_", name, "_create_", e['name'], "(so_", name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\tso_", e['type'], " *obj = so_", e['type'], "_new(", sep='', end='', file=f)
        if e['type'] in need_name:
            print('"', end='', file=f)
            if 'prefix' in e:
                print(e['prefix'], ":", sep='', end='', file=f)
            print(e['name'], '"', sep='', end='', file=f)
        print(");", sep='', file=f)
        if is_array:
            print("\tself->num_", e['name'], "++;", sep='', file=f)
            print("\tself->", e['name'], " = extrealloc(self->", e['name'], ", self->num_", e['name'], " * sizeof(so_", e['type'], " *));", sep='', file=f)
            print("\tself->", e['name'], "[self->num_", e['name'], " - 1] = obj;", sep='', file=f)
        else:
            print("\tself->", e['name'], " = obj;", sep='', file=f)
        print("\treturn obj;", file=f)
        print("}", file=f)
        print(file=f)


def create_xml(f, name, elements):
    print("so_xml so_", name, "_xml(so_", name, " *self)", sep='', file=f)
    print("{", file=f)
    print("\txmlNodePtr xml = NULL;", file=f)
    print(file=f)

    if 'children' in elements:
        children = elements['children']
        # Check if any sub-element is non-NULL
        print("\tif (", end='', file=f) 
        for e in children[:-1]:
            print("self->", e['name'], ' || ', sep='', end='', file=f)
        print("self->", children[-1]['name'], ") {", sep='', file=f)

    print('\t\txml = xmlNewNode(NULL, BAD_CAST "', name, '");', sep='', file=f)
    if 'attributes' in elements:
        for a in elements['attributes']:
            print('\t\tif (self->', a, ")", sep='', file=f)
            print('\t\t\txmlNewProp(xml, BAD_CAST "', a, '", BAD_CAST self->', a, ");", sep='', file=f)

    if 'children' in elements:
        children = elements['children']
        for e in children:
            print("\t\tif (self->", e['name'], ") {", sep='', file=f)
            is_array = e.get('array', False)
            if is_array:
                print("\t\t\tfor (int i = 0; i < self->num_", e['name'], "; i++) {" ,sep='', file=f)
                print("\t\t\t\txmlNodePtr ", e['name'], " = so_", e['type'], "_xml(self->", e['name'], "[i]);", sep='', file=f)
                print("\t\t\t\txmlAddChild(xml, ", e['name'], ");", sep='', file=f)
                print("\t\t\t}", file=f)
            else:
                print("\t\t\txmlNodePtr ", e['name'], " = so_", e['type'], "_xml(self->", e['name'], ");", sep='', file=f)
                print("\t\t\txmlAddChild(xml, ", e['name'], ");", sep='', file=f)

            print("\t\t}", file=f)

        print("\t}", file=f)

    print("\treturn xml;", file=f)
    print("}", file=f)


def create_start(f, name, a):
    print("void so_", name, "_start_element(so_", name, " *self, const char *localname, int nb_attributes, const char **attributes)", sep='', file=f)
    print("{", file=f)

    if 'children' in a:
        elements = a['children']
        for i in range(0, len(elements)):
            if i != 0:
                print(" else ", end='', file=f)
            else:
                print("\t", end='', file=f)
            print("if (self->in_", elements[i]['name'], ") {", sep='', file=f)
            is_array = elements[i].get('array', False)
            if is_array:
                print("\t\tso_", elements[i]['type'], "_start_element(self->", elements[i]['name'], "[self->num_", elements[i]['name'], " - 1], localname, nb_attributes, attributes);", sep='', file=f)
            else:
                print("\t\tso_", elements[i]['type'], "_start_element(self->", elements[i]['name'], ", localname, nb_attributes, attributes);", sep='', file=f)
            print("\t}", end='', file=f)

        for e in elements:
            print(' else if (strcmp(localname, "', e['name'], '") == 0) {', sep='', file=f)
            print("\t\tso_", e['type'], " *", e['name'], " = so_", name, "_create_", e['name'], "(self);", sep='', file=f)
            if e['type'] in structure:
                if 'attributes' in structure[e['type']]:
                    print("\t\tso_", e['type'], "_init_attributes(", e['name'], ", nb_attributes, attributes);", sep='', file=f)
            print("\t\tself->in_", e['name'], " = 1;", sep='', file=f)
            print("\t}", end='', file=f)

        print(file=f)

    print("}", file=f)


def create_end(f, name, a):
    print("void so_", name, "_end_element(so_", name, " *self, const char *localname)", sep='', file=f)
    print("{", file=f)

    if 'children' in a:
        elements = a['children']
        for i in range(0, len(elements)):
            if i != 0:
                print(" else ", end='', file=f)
            else:
                print("\t", end='', file=f)
            print('if (strcmp(localname, "', elements[i]['name'], '") == 0 && self->in_', elements[i]['name'], ") {", sep='', file=f)
            print("\t\tself->in_", elements[i]['name'], " = 0;", sep='', file=f)
            print("\t}", end='', file=f)

        for e in elements:
            print(" else if (self->in_", e['name'], ") {", sep='', file=f)
            print("\t\tso_", e['type'], "_end_element(self->", e['name'], sep='', end='', file=f)
            if e.get("array", False):
                print("[self->num_", e['name'], " - 1]", sep='', end='', file=f)
            print(", localname);", file=f)
            print("\t}", end='', file=f)

        print(file=f)

    print("}", file=f)


def create_characters(f, name, a):
    print("void so_", name, "_characters(so_", name, " *self, const char *ch, int len)", sep='', file=f)
    print("{", file=f)

    if 'children' in a:
        elements = a['children']
        for i in range(0, len(elements)):
            if i != 0:
                print(" else ", end='', file=f)
            else:
                print("\t", end='', file=f)
            print("if (self->in_", elements[i]['name'], ") {", sep='', file=f)
            print("\t\tso_", elements[i]['type'], "_characters(self->", elements[i]['name'], sep='', end='', file=f)
            if elements[i].get("array", False):
                print("[self->num_", elements[i]['name'], " - 1]", sep='', end='', file=f)
            print(", ch, len);", file=f)
            print("\t}", end='', file=f)

        print(file=f)

    print("}", file=f)


def create_init_attributes(f, name, attributes):
    print("void so_", name, "_init_attributes(so_", name, " *self, int nb_attributes, const char **attributes)", sep='', file=f)
    print("{", file=f)
    print("\tunsigned int index = 0;", file=f)
    print("\tfor (int i = 0; i < nb_attributes; i++, index += 5) {", file=f)
    print("\t\tconst char *localname = attributes[index];", file=f)
    print("\t\tconst char *prefix = attributes[index + 1];", file=f)
    print("\t\tconst char *nsURI = attributes[index + 2];", file=f)
    print("\t\tconst char *valueBegin = attributes[index + 3];", file=f)
    print("\t\tconst char *valueEnd = attributes[index + 4];", file=f)
    print(file=f)

    first = True
    for a in attributes:
        if first:
            print('\t\tif', end='', file=f)
            first = False
        else:
            print('\t\t} else if', end='', file=f)

        print(' (strcmp(localname, "', a, '") == 0) {', sep='', file=f)
        print("\t\t\tself->", a, " = extstrndup(valueBegin, valueEnd - valueBegin);", sep='', file=f)

    print("\t\t}", file=f)
    print("\t}", file=f)
    print("}", file=f)


def create_headers(name, elements):
    with open(name + ".h", "w") as f:
        common.output_file = f
        common.print_license_string()
        print("#ifndef _SO_", name.upper(), "_H", sep='', file=f)
        print("#define _SO_", name.upper(), "_H", sep='', file=f)
        print(file=f)

        if 'children' in elements:
            children = elements['children']
            included = []
            for e in children:
                if e['type'] not in included:
                    included.append(e['type'])
                    print('#include <so/', e['type'], '.h>', sep='', file=f)

        print(file=f)
        print("/** \\struct so_", name, sep='', file=f)
        print("\t \\brief A structure representing an", structure[name]['xpath'], "element", file=f)
        print("*/", file=f)

        print("typedef struct so_", name, " so_", name, ";", sep='', file=f)
        print(file=f)

        # function prototypes
        print("/** \\memberof so_", name, sep='', file=f)
        print(" * Create a new empty so_", name, " structure.", sep='', file=f)
        print(" * \\return A pointer to the newly created struct", file=f)
        print(" * \\sa so_", name, "_free", sep='', file=f)
        print(" */", file=f)
        
        print("so_", name, " *so_", name, "_new();", sep='', file=f)

        print("/** \\memberof so_", name, sep='', file=f)
        print(" * Free all memory associated with a so_", name, " structure and its children.", sep='', file=f)
        print(" * \\param self - a pointer to the structure to free", file=f)
        print(" * \\sa so_", name, "_new", sep='', file=f)
        print(" */", file=f)

        print("void so_", name, "_free(so_", name, " *self);", sep='', file=f)

        print("void so_", name, "_ref(so_", name, " *self);", sep='', file=f)
        print("void so_", name, "_unref(so_", name, " *self);", sep='', file=f)

        if 'attributes' in elements:
            for a in elements['attributes']:
                print("/** \\memberof so_", name, sep='', file=f)
                print(" * Get the value of the ", a, " attribute", sep='', file=f)
                print(" * \\param self - pointer to a so_", name, sep='', file=f)
                print(" * \\return A pointer to the attribute string", file=f)
                print(" * \\sa so_", name, "_set_", a, sep='', file=f)
                print(" */", file=f)

                print("char *so_", name, "_get_", a, "(so_", name, " *self);", sep='', file=f)

                print("/** \\memberof so_", name, sep='', file=f)
                print(" * Set the value of the ", a, " attribute", sep='', file=f)
                print(" * \\param self - pointer to a so_", name, sep='', file=f)
                print(" * \\param value - A pointer to the attribute string", file=f)
                print(" * \\sa so_", name, "_get_", a, sep='', file=f)
                print(" */", file=f)

                print("void so_", name, "_set_", a, "(so_", name, " *self, char *value);", sep='', file=f)

        if 'children' in elements:
            children = elements['children']
            for e in children:
                print("/** \\memberof so_", name, sep='', file=f)
                print(" * Get the", e['name'], "element", file=f)
                print(" * \\param self - pointer to a so_", name, sep='', file=f)
                if e.get("array", False):
                    print(" * \\param number - An index to the specific element", file=f)
                print(" * \\return A pointer to the structure representing the", e['name'], "element", file=f)
                print(" * \\sa so_", name, "_set_", e['name'], sep='', file=f)
                print(" */", file=f)

                print("so_", e['type'], " *so_", name, "_get_", e['name'], "(so_", name, " *self", sep='', end='', file=f)
                if e.get("array", False):
                    print(", int number", end='', file=f)
                print(");", file=f)

                # _get_number_of_ for arrays
                if e.get("array", False):
                    print("/** \\memberof so_", name, sep='', file=f)
                    print(" * Get the number of", e['name'], "currently contained in the", name, "structure", file=f)
                    print(" * \\param self - pointer to a so_", name, sep='', file=f)
                    print(" * \\return The number of ", e['name'], "s", sep='', file=f)
                    print(" */", file=f)
                    print("int so_", name, "_get_number_of_", e['name'], "(so_", name, " *self);", sep='', file=f)

            for e in children:
                if not e.get('attribute', False):   # No setters for arrays
                    print("/** \\memberof so_", name, sep='', file=f)
                    print(" * Set the ", e['name'], " element", sep='', file=f)
                    print(" * \\param self - pointer to a so_", name, sep='', file=f)
                    print(" * \\param value - A pointer to a \\a so_", e['type'], " to set.", sep='', file=f)
                    print(" * \\sa so_", name, "_get_", e['name'], sep='', file=f)
                    print(" */", file=f)

                    print("void so_", name, "_set_", e['name'], "(so_", name, " *self, so_", e['type'], " *value);", sep='', file=f)

            for e in children:
                print("/** \\memberof so_", name, sep='', file=f)
                print(" * Create a new ", e['name'], " element and insert it into the so_", name, sep='', file=f)
                print(" * \\param self - pointer to a so_", name, sep='', file=f)
                print(" * \\return A pointer to the newly created structure", file=f)
                print(" */", file=f)

                print("so_", e['type'], " *so_", name, "_create_", e['name'], "(so_", name, " *self);", sep='', file=f)
                if e.get('array', False):
                    print("void so_", name, "_add_", e['name'], "(so_", name, " *self, so_", e['type'], " *child);", sep='', file=f)

        print(file=f)
        print("#endif", file=f)

    os.chdir("private")
    with open(name + ".h", "w") as f:
        common.output_file = f
        common.print_license_string()
        print("#ifndef _SO_PRIVATE_", name.upper(), "_H", sep='', file=f)
        print("#define _SO_PRIVATE_", name.upper(), "_H", sep='', file=f)

        print(file=f)
        print('#include <so/xml.h>', file=f)

        if 'children' in elements:
            children = elements['children']
            included = []
            for e in children:
                if e['type'] not in included:
                    included.append(e['type'])
                    print('#include <so/private/', e['type'], '.h>', sep='', file=f)

        print(file=f)
        print("struct so_", name, " {", sep='', file=f)

        if 'attributes' in elements:
            for a in elements['attributes']:
                print("\tchar *", a, ";", sep='', file=f) 

        if 'children' in elements:
            children = elements['children']
            for e in children:
                print("\tso_", e['type'], " *", sep='', end='', file=f)
                if e.get('array', False):
                    print("*", end='', file=f)    
                print(e['name'], ";", sep='', file=f)
            for e in children:
                if e.get('array', False):
                    print("\tint num_", e['name'], ";", sep='', file=f)
            for e in children:
                print("\tint in_", e['name'], ";", sep='', file=f)
        
        print("\tint reference_count;", file=f)
        print("};", file=f)
 
        print(file=f)
        print("void so_", name, "_start_element(so_", name, " *self, const char *localname, int nb_attributes, const char **attributes);", sep='', file=f)
        print("void so_", name, "_end_element(so_", name, " *self, const char *localname);", sep='', file=f)
        print("void so_", name, "_characters(so_", name, " *self, const char *ch, int len);", sep='', file=f)
        print("so_xml so_", name, "_xml(so_", name, " *self);", sep='', file=f)
        if 'attributes' in elements:
            print("void so_", name, "_init_attributes(so_", name, " *self, int nb_attributes, const char **attributes);", sep='', file=f)
        print(file=f)
        print("#endif", file=f)
    os.chdir("..")


def create_code(name, elements):
    with open(name + ".c", "w") as f:
        common.output_file = f
        common.print_license_string()
        create_includes(f, name)
        create_new(f, name)
        print(file=f)
        create_free(f, name, elements)
        print(file=f)
        create_ref_unref(f, name)
        create_getters(f, name, elements)
        create_setters(f, name, elements)
        if 'children' in elements:
            create_create(f, name, elements)
            create_array_add(f, name, elements)
        create_xml(f, name, elements)
        print(file=f)
        create_start(f, name, elements)
        print(file=f)
        create_end(f, name, elements)
        print(file=f)
        create_characters(f, name, elements)
        if 'attributes' in elements:
            print(file=f)
            create_init_attributes(f, name, elements['attributes'])


if len(sys.argv) > 1 and sys.argv[1] == "clean":
    shutil.rmtree("../gen", True)
    for name in structure:
        os.remove("../include/so/" + name + ".h")
        os.remove("../include/so/private/" + name + ".h")
    sys.exit()
    

try:
    os.makedirs("../gen")
except:
    pass
    
os.chdir("..")

for name in structure:
    os.chdir("gen")
    create_code(name, structure[name])
    os.chdir("../include/so")
    create_headers(name, structure[name])
    os.chdir("../..")
