import os
import common
from structure import need_name

class genclass:
    def __init__(self, name, structure):
        self.name = name
        self.structure = structure
        entry = structure[name]
        self.children = entry.get('children', None)
        self.attributes = entry.get('attributes', None)
        self.extends = entry.get('extends', None)
        self.xpath = entry['xpath']
        self.extends = entry.get('extends', None)

    def create_code(self):
        with open(self.name + ".c", "w") as f:
            common.output_file = f
            common.print_license_string()
            self.c_file = f
            self.create_includes()
            self.create_new()
            self.create_free()
            self.create_ref_unref()
            self.create_getters()
            self.create_setters()
            self.create_create()
            self.create_array_add()
            if self.extends:
                self.create_get_base()
            self.create_xml()
            self.create_start()
            self.create_end()
            self.create_characters()
            self.create_init_attributes()

    def create_includes(self):
        f = self.c_file
        print("#include <string.h>", file=f)
        print("#include <libxml/tree.h>", file=f)
        print('#include <so/', self.name, '.h>', sep='', file=f)
        print('#include <so/private/', self.name, '.h>', sep='', file=f)
        print('#include <so/private/util.h>', file=f)
        print(file=f)

    def create_new(self):
        f = self.c_file
        print("so_", self.name, " *so_", self.name, "_new(", sep='', end='', file=f)
        if self.name in need_name:
            print("char *name", end='', file=f)
        print(")", file=f)
        print("{", file=f);
        print("\tso_", self.name, " *object = extcalloc(sizeof(so_", self.name, "));", sep='', file=f)
        print("\tobject->reference_count = 1;", file=f)
        print(file=f)
        if self.extends:
            print("\tobject->base = so_", self.extends, "_new(", end='', sep='', file=f)
            if self.extends in need_name:
                print("name", sep='', end='', file=f)
            print(");", file=f)
            print(file=f)
        print("\treturn object;", file=f)
        print("}", file=f)
        print(file=f)

    def create_free(self):
        f = self.c_file
        print("void so_", self.name, "_free(so_", self.name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\tif (self) {", file=f)

        if self.children:
            for e in self.children:
                is_array = e.get('array', False)
                if is_array:
                    print("\t\tfor(int i = 0; i < self->num_", e['name'], "; i++) {", sep='', file=f)
                    print("\t\t\tso_", e['type'], "_unref(self->", e['name'], "[i]);", sep='', file=f)
                    print("\t\t}", sep='', file=f)
                    print("\t\tfree(self->", e['name'], ");", sep='', file=f)
                else:
                    if e['type'] == 'type_string':
                        print("\t\tfree(self->", e['name'], ");", sep='', file=f)
                    else:
                        print("\t\tso_", e['type'], "_unref(self->", e['name'], ");", sep='', file=f)
        if self.attributes:
            for a in self.attributes:
                print("\t\tif (self->", a, ") free(self->", a, ");", sep='', file=f)
        if self.extends:
            print("\t\tfree(self->base);", file=f)
        print("\t\tfree(self);", file=f)
        print("\t}", file=f)
        print("}", file=f)
        print(file=f)

    def create_ref_unref(self):
        f = self.c_file
        print("void so_", self.name, "_ref(so_", self.name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\tself->reference_count++;", file=f)
        print("}", file=f)
        print(file=f)
        print("void so_", self.name, "_unref(so_", self.name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\tif (self) {", file=f)
        print("\t\tself->reference_count--;", file=f)
        print("\t\tif (!self->reference_count) {", file=f)
        print("\t\t\tso_", self.name, "_free(self);", sep='', file=f)
        print("\t\t}", file=f)
        print("\t}", file=f)
        print("}", file=f)
        print(file=f)

    def create_getters(self):
        f = self.c_file
        # getters for attributes
        if self.attributes:
            for a in self.attributes:
                print("char *so_", self.name, "_get_", a, "(so_", self.name, " *self)", sep='', file=f)
                print("{", file=f)
                print("\treturn self->", a, ";", sep='', file=f)
                print("}", file=f)
                print(file=f)

        # getters for child elements
        if self.children:
            for e in self.children:
                if e['type'] == 'type_string':
                    print("char *so_", self.name, "_get_", e['name'], "(so_", self.name, " *self)", sep='', file=f)
                    print("{", file=f)
                    print("\treturn self->", e['name'], ";", sep='', file=f)
                    print("}", file=f)
                    print(file=f)
                else:
                    print("so_", e['type'], " *so_", self.name, "_get_", e['name'], "(so_", self.name,  " *self", sep='', end='', file=f)
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
            for e in self.children:
                if e.get('array', False):
                    print("int so_", self.name, "_get_number_of_", e['name'], "(so_", self.name, " *self)", sep='', file=f)
                    print("{", file=f)
                    print("\treturn self->num_", e['name'], ";", sep='', file=f)
                    print("}", file=f)
                    print(file=f)

    def create_setters(self):
        f = self.c_file
        # setters for attributes
        if self.attributes:
            for a in self.attributes:
                print("void so_", self.name, "_set_", a, "(so_", self.name, " *self, char *value)", sep='', file=f)
                print("{", file=f)
                print("\tfree(self->", a, ");", sep='', file=f)
                print("\tself->", a, " = extstrdup(value);", sep='', file=f)
                print("}", file=f)
                print(file=f)

        # setters for child elements
        if self.children:
            for e in self.children:
                if not e.get('array', False):       # No setters for arrays
                    if e['type'] == "type_string":
                        print("void so_", self.name, "_set_", e['name'], "(so_", self.name, " *self, char *value)", sep='', file=f)
                        print("{", file=f)
                        print("\tfree(self->", e['name'], ");", sep='', file=f)
                        print("\tself->", e['name'], " = extstrdup(value);", sep='', file=f)
                        print("}", file=f)
                        print(file=f)
                    else:
                        print("void so_", self.name, "_set_", e['name'], "(so_", self.name, " *self, ", "so_", e['type'], " *value)", sep='', file=f)
                        print("{", file=f)
                        print("\tself->", e['name'], " = value;", sep='', file=f)
                        print("}", file=f)
                        print(file=f)

    def create_create(self):
        f = self.c_file
        if self.children:
            for e in self.children:
                if e['type'] != "type_string":
                    is_array = e.get('array', False)
                    print("so_", e['type'], " *so_", self.name, "_create_", e['name'], "(so_", self.name, " *self)", sep='', file=f)
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

    def create_array_add(self):
        f = self.c_file
        if self.children:
            for e in self.children:
                if e.get('array', False):
                    print("void so_", self.name, "_add_", e['name'], "(so_", self.name, " *self, so_", e['type'], " *child)", sep='', file=f)
                    print("{", file=f)
                    print("\tself->num_", e['name'], "++;", sep='', file=f)
                    print("\tself->", e['name'], " = extrealloc(self->", e['name'], ", self->num_", e['name'], " * sizeof(so_", e['type'], " *));", sep='', file=f)
                    print("\tself->", e['name'], "[self->num_", e['name'], " - 1] = child;", sep='', file=f)
                    print("}", file=f)
                    print(file=f)

    def create_get_base(self):
        f = self.c_file
        print("so_", self.extends, " *so_", self.name, "_get_base(so_", self.name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\treturn self->base;", file=f)
        print("}", file=f)
        print(file=f)

    def create_xml(self):
        f = self.c_file
        print("so_xml so_", self.name, "_xml(so_", self.name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\txmlNodePtr xml = NULL;", file=f)
        print(file=f)

        if self.children:
            # Check if any sub-element is non-NULL
            print("\tif (", end='', file=f) 
            for e in self.children[:-1]:
                print("self->", e['name'], ' || ', sep='', end='', file=f)
            print("self->", self.children[-1]['name'], ") {", sep='', file=f)

        if self.extends:
            print("\t\txml = so_", self.extends, "_xml(self->base);", sep='', file=f) 
        else:
            print('\t\txml = xmlNewNode(NULL, BAD_CAST "', self.name, '");', sep='', file=f)

        if self.attributes:
            for a in self.attributes:
                print('\t\tif (self->', a, ")", sep='', file=f)
                print('\t\t\txmlNewProp(xml, BAD_CAST "', a, '", BAD_CAST self->', a, ");", sep='', file=f)

        if self.children:
            for e in self.children:
                print("\t\tif (self->", e['name'], ") {", sep='', file=f)
                is_array = e.get('array', False)
                if is_array:
                    print("\t\t\tfor (int i = 0; i < self->num_", e['name'], "; i++) {" ,sep='', file=f)
                    print("\t\t\t\txmlNodePtr ", e['name'], " = so_", e['type'], "_xml(self->", e['name'], "[i]);", sep='', file=f)
                    print("\t\t\t\txmlAddChild(xml, ", e['name'], ");", sep='', file=f)
                    print("\t\t\t}", file=f)
                else:
                    if e['type'] == "type_string":
                        print("\t\t\txmlNodePtr ", e['name'], " = xmlNewNode(NULL, BAD_CAST \"", e['prefix'], ":", e['name'],  "\");", sep='', file=f)
                        print("\t\t\txmlAddChild(", e['name'], ", xmlNewText(BAD_CAST self->", e['name'], "));", sep='', file=f)
                        print("\t\t\txmlAddChild(xml, ", e['name'], ");", sep='', file=f)
                    else:
                        print("\t\t\txmlNodePtr ", e['name'], " = so_", e['type'], "_xml(self->", e['name'], ");", sep='', file=f)
                        print("\t\t\txmlAddChild(xml, ", e['name'], ");", sep='', file=f)

                print("\t\t}", file=f)

            print("\t}", file=f)

        print("\treturn xml;", file=f)
        print("}", file=f)
        print(file=f)

    def create_start(self):
        f = self.c_file
        print("void so_", self.name, "_start_element(so_", self.name, " *self, const char *localname, int nb_attributes, const char **attributes)", sep='', file=f)
        print("{", file=f)

        if self.children:
            first = True
            for i in range(0, len(self.children)):
                if self.children[i]['type'] != "type_string":
                    if not first:
                        print(" else ", end='', file=f)
                    else:
                        print("\t", end='', file=f)
                        first = False
                    print("if (self->in_", self.children[i]['name'], ") {", sep='', file=f)
                    is_array = self.children[i].get('array', False)
                    if is_array:
                        print("\t\tso_", self.children[i]['type'], "_start_element(self->", self.children[i]['name'], "[self->num_", self.children[i]['name'], " - 1], localname, nb_attributes, attributes);", sep='', file=f)
                    else:
                        print("\t\tso_", self.children[i]['type'], "_start_element(self->", self.children[i]['name'], ", localname, nb_attributes, attributes);", sep='', file=f)
                    print("\t}", end='', file=f)

            for e in self.children:
                if not first:
                    print(" else ", end='', file=f)
                else:
                    print("\t", end='', file=f)
                    first = False
                print('if (strcmp(localname, "', e['name'], '") == 0) {', sep='', file=f)
                if e['type'] != "type_string":
                    print("\t\tso_", e['type'], " *", e['name'], " = so_", self.name, "_create_", e['name'], "(self);", sep='', file=f)
                    if e['type'] in self.structure:
                        if 'attributes' in self.structure[e['type']]:
                            print("\t\tso_", e['type'], "_init_attributes(", e['name'], ", nb_attributes, attributes);", sep='', file=f)
                print("\t\tself->in_", e['name'], " = 1;", sep='', file=f)
                print("\t}", end='', file=f)

        if self.extends:
            if self.children:
                print(" else {", file=f)
                print("\t", end='', file=f)
            print("\tso_", self.extends, "_start_element(self->base, localname, nb_attributes, attributes);", sep='', file=f)
            if self.children:
                print("\t}", end='', file=f)

        if self.children:
            print(file=f)

        print("}", file=f)
        print(file=f)

    def create_end(self):
        f = self.c_file
        print("void so_", self.name, "_end_element(so_", self.name, " *self, const char *localname)", sep='', file=f)
        print("{", file=f)

        if self.children:
            for i in range(0, len(self.children)):
                if i != 0:
                    print(" else ", end='', file=f)
                else:
                    print("\t", end='', file=f)
                print('if (strcmp(localname, "', self.children[i]['name'], '") == 0 && self->in_', self.children[i]['name'], ") {", sep='', file=f)
                print("\t\tself->in_", self.children[i]['name'], " = 0;", sep='', file=f)
                print("\t}", end='', file=f)

            for e in self.children:
                if e['type'] != "type_string":
                    print(" else if (self->in_", e['name'], ") {", sep='', file=f)
                    print("\t\tso_", e['type'], "_end_element(self->", e['name'], sep='', end='', file=f)
                    if e.get("array", False):
                        print("[self->num_", e['name'], " - 1]", sep='', end='', file=f)
                    print(", localname);", file=f)
                    print("\t}", end='', file=f)

            print(file=f)

        if self.extends:
            if self.children:
                print(" else {", file=f)
                print("\t", end='', file=f)
            print("\tso_", self.extends, "_end_element(self->base, localname);", sep='', file=f)
            if self.children:
                print("\t}", end='', file=f)

        print("}", file=f)
        print(file=f)

    def create_characters(self):
        f = self.c_file
        print("void so_", self.name, "_characters(so_", self.name, " *self, const char *ch, int len)", sep='', file=f)
        print("{", file=f)

        if self.children:
            for i in range(0, len(self.children)):
                if i != 0:
                    print(" else ", end='', file=f)
                else:
                    print("\t", end='', file=f)
                print("if (self->in_", self.children[i]['name'], ") {", sep='', file=f)
                if self.children[i]['type'] == "type_string":
                    print("\t\tself->", self.children[i]['name'], " = extstrndup(ch, len);", sep='', file=f)
                else:
                    print("\t\tso_", self.children[i]['type'], "_characters(self->", self.children[i]['name'], sep='', end='', file=f)
                    if self.children[i].get("array", False):
                        print("[self->num_", self.children[i]['name'], " - 1]", sep='', end='', file=f)
                    print(", ch, len);", file=f)
                print("\t}", end='', file=f)

            print(file=f)

        if self.extends:
            if self.children:
                print(" else {", file=f)
                print("\t", end='', file=f)
            print("\tso_", self.extends, "_characters(self->base, ch, len);", sep='', file=f)
            if self.children:
                print("\t}", end='', file=f)

        print("}", file=f)

    def create_init_attributes(self):
        f = self.c_file
        if self.attributes:
            print(file=f)
            print("void so_", self.name, "_init_attributes(so_", self.name, " *self, int nb_attributes, const char **attributes)", sep='', file=f)
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
            for a in self.attributes:
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

    def create_headers(self):
        with open(self.name + ".h", "w") as f:
            common.output_file = f
            common.print_license_string()
            print("#ifndef _SO_", self.name.upper(), "_H", sep='', file=f)
            print("#define _SO_", self.name.upper(), "_H", sep='', file=f)
            print(file=f)


            included = [ 'type_string' ]
            if self.children:
                for e in self.children:
                    if e['type'] not in included:
                        included.append(e['type'])
                        print('#include <so/', e['type'], '.h>', sep='', file=f)

            if self.extends and self.extends not in included:
                included.append(self.extends)
                print("#include <so/", self.extends, ".h>", sep='', file=f)

            print(file=f)
            print("/** \\struct so_", self.name, sep='', file=f)
            print("\t \\brief A structure representing an", self.structure[self.name]['xpath'], "element", file=f)
            print("*/", file=f)

            print("typedef struct so_", self.name, " so_", self.name, ";", sep='', file=f)
            print(file=f)

            # function prototypes
            print("/** \\memberof so_", self.name, sep='', file=f)
            print(" * Create a new empty so_", self.name, " structure.", sep='', file=f)
            print(" * \\return A pointer to the newly created struct", file=f)
            print(" * \\sa so_", self.name, "_free", sep='', file=f)
            print(" */", file=f)
            
            print("so_", self.name, " *so_", self.name, "_new();", sep='', file=f)

            print("/** \\memberof so_", self.name, sep='', file=f)
            print(" * Free all memory associated with a so_", self.name, " structure and its children.", sep='', file=f)
            print(" * \\param self - a pointer to the structure to free", file=f)
            print(" * \\sa so_", self.name, "_new", sep='', file=f)
            print(" */", file=f)

            print("void so_", self.name, "_free(so_", self.name, " *self);", sep='', file=f)

            print("void so_", self.name, "_ref(so_", self.name, " *self);", sep='', file=f)
            print("void so_", self.name, "_unref(so_", self.name, " *self);", sep='', file=f)
            if self.extends:
                print("so_", self.extends, " *so_", self.name, "_get_base(so_", self.name, " *self);", sep='', file=f)

            if self.attributes:
                for a in self.attributes:
                    print("/** \\memberof so_", self.name, sep='', file=f)
                    print(" * Get the value of the ", a, " attribute", sep='', file=f)
                    print(" * \\param self - pointer to a so_", self.name, sep='', file=f)
                    print(" * \\return A pointer to the attribute string", file=f)
                    print(" * \\sa so_", self.name, "_set_", a, sep='', file=f)
                    print(" */", file=f)

                    print("char *so_", self.name, "_get_", a, "(so_", self.name, " *self);", sep='', file=f)

                    print("/** \\memberof so_", self.name, sep='', file=f)
                    print(" * Set the value of the ", a, " attribute", sep='', file=f)
                    print(" * \\param self - pointer to a so_", self.name, sep='', file=f)
                    print(" * \\param value - A pointer to the attribute string", file=f)
                    print(" * \\sa so_", self.name, "_get_", a, sep='', file=f)
                    print(" */", file=f)

                    print("void so_", self.name, "_set_", a, "(so_", self.name, " *self, char *value);", sep='', file=f)

            if self.children:
                for e in self.children:
                    print("/** \\memberof so_", self.name, sep='', file=f)
                    print(" * Get the", e['name'], "element", file=f)
                    print(" * \\param self - pointer to a so_", self.name, sep='', file=f)
                    if e.get("array", False):
                        print(" * \\param number - An index to the specific element", file=f)
                    print(" * \\return A pointer to the structure representing the", e['name'], "element", file=f)
                    print(" * \\sa so_", self.name, "_set_", e['name'], sep='', file=f)
                    print(" */", file=f)

                    if e['type'] == "type_string":
                        return_type = "char"
                    else:
                        return_type = "so_" + e['type']
                    print(return_type, " *so_", self.name, "_get_", e['name'], "(so_", self.name, " *self", sep='', end='', file=f)
                    if e.get("array", False):
                        print(", int number", end='', file=f)
                    print(");", file=f)

                    # _get_number_of_ for arrays
                    if e.get("array", False):
                        print("/** \\memberof so_", self.name, sep='', file=f)
                        print(" * Get the number of", e['name'], "currently contained in the", self.name, "structure", file=f)
                        print(" * \\param self - pointer to a so_", self.name, sep='', file=f)
                        print(" * \\return The number of ", e['name'], "s", sep='', file=f)
                        print(" */", file=f)
                        print("int so_", self.name, "_get_number_of_", e['name'], "(so_", self.name, " *self);", sep='', file=f)

                for e in self.children:
                    if not e.get('attribute', False):   # No setters for arrays
                        if e['type'] == "type_string":
                            man_type = "string"
                            param_type = "char"
                        else:
                            man_type = "so_" + e['type']
                            param_type = man_type
                        print("/** \\memberof so_", self.name, sep='', file=f)
                        print(" * Set the ", e['name'], " element", sep='', file=f)
                        print(" * \\param self - pointer to a so_", self.name, sep='', file=f)
                        print(" * \\param value - A pointer to a \\a ", man_type, " to set.", sep='', file=f)
                        print(" * \\sa so_", self.name, "_get_", e['name'], sep='', file=f)
                        print(" */", file=f)

                        print("void so_", self.name, "_set_", e['name'], "(so_", self.name, " *self, ", param_type, " *value);", sep='', file=f)

                for e in self.children:
                    if e['type'] != "type_string":
                        print("/** \\memberof so_", self.name, sep='', file=f)
                        print(" * Create a new ", e['name'], " element and insert it into the so_", self.name, sep='', file=f)
                        print(" * \\param self - pointer to a so_", self.name, sep='', file=f)
                        print(" * \\return A pointer to the newly created structure", file=f)
                        print(" */", file=f)

                        print("so_", e['type'], " *so_", self.name, "_create_", e['name'], "(so_", self.name, " *self);", sep='', file=f)
                        if e.get('array', False):
                            print("void so_", self.name, "_add_", e['name'], "(so_", self.name, " *self, so_", e['type'], " *child);", sep='', file=f)

            print(file=f)
            print("#endif", file=f)

        os.chdir("private")
        with open(self.name + ".h", "w") as f:
            common.output_file = f
            common.print_license_string()
            print("#ifndef _SO_PRIVATE_", self.name.upper(), "_H", sep='', file=f)
            print("#define _SO_PRIVATE_", self.name.upper(), "_H", sep='', file=f)

            print(file=f)
            print('#include <so/xml.h>', file=f)

            included = [ 'type_string' ]
            if self.children:
                for e in self.children:
                    if e['type'] not in included:
                        included.append(e['type'])
                        print('#include <so/private/', e['type'], '.h>', sep='', file=f)

            if self.extends and self.extends not in included:
                included.append(self.extends)
                print("#include <so/private/", self.extends, ".h>", sep='', file=f)

            print(file=f)
            print("struct so_", self.name, " {", sep='', file=f)

            if self.extends:
                print("\tso_", self.extends, " *base;", sep='', file=f)

            if self.attributes:
                for a in self.attributes:
                    print("\tchar *", a, ";", sep='', file=f) 

            if self.children:
                for e in self.children:
                    if e['type'] == 'type_string':
                        print("\tchar *", e['name'], ";", sep='', file=f)
                    else:
                        print("\tso_", e['type'], " *", sep='', end='', file=f)
                        if e.get('array', False):
                            print("*", end='', file=f)    
                        print(e['name'], ";", sep='', file=f)
                for e in self.children:
                    if e.get('array', False):
                        print("\tint num_", e['name'], ";", sep='', file=f)
                for e in self.children:
                    print("\tint in_", e['name'], ";", sep='', file=f)

            print("\tint reference_count;", file=f)
            print("};", file=f)
     
            print(file=f)
            print("void so_", self.name, "_start_element(so_", self.name, " *self, const char *localname, int nb_attributes, const char **attributes);", sep='', file=f)
            print("void so_", self.name, "_end_element(so_", self.name, " *self, const char *localname);", sep='', file=f)
            print("void so_", self.name, "_characters(so_", self.name, " *self, const char *ch, int len);", sep='', file=f)
            print("so_xml so_", self.name, "_xml(so_", self.name, " *self);", sep='', file=f)
            if self.attributes:
                print("void so_", self.name, "_init_attributes(so_", self.name, " *self, int nb_attributes, const char **attributes);", sep='', file=f)
            print(file=f)
            print("#endif", file=f)
        os.chdir("..")
