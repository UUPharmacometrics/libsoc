# libsoc - Library to handle standardised output files
# Copyright (C) 2015 Rikard Nordgren
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
# 
# his library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/>.

import os
import common
from structure import need_name

class genclass:
    def __init__(self, name, structure, namespaces):
        self.name = name
        self.structure = structure
        entry = structure[name]
        self.namespace = entry['namespace']
        self.class_name = self.namespace + "_" + self.name
        self.children = entry.get('children', None)
        self.attributes = entry.get('attributes', None)
        self.fixed_attributes = entry.get('fixed_attributes', None)
        self.xpath = entry['xpath']
        self.extends = entry.get('extends', None)
        self.named = entry.get('named', None)
        self.element_name = entry.get('element_name', None)
        self.fields = entry.get('fields', None)
        self.namespaces = namespaces
        self.prefix = entry.get('prefix', None)
        if self.prefix:
            self.prefix += ":"
        else:
            self.prefix = ""
        self.xml_injection = entry.get('xml_injection', None)

    def prefix_class(self, name):
        # prefix a class name with the namespace prefix
        return self.namespaces[name] + "_" + name

    def create_code(self):
        with open(self.name + ".c", "w") as f:
            common.output_file = f
            common.print_license_string()
            self.c_file = f
            self.create_includes()
            self.create_new()
            self.create_copy()
            self.create_free()
            self.create_ref_unref()
            self.create_getters()
            self.create_setters()
            self.create_create()
            self.create_array_add()
            if self.extends:
                self.create_get_set_base()
            self.create_xml()
            self.create_start()
            self.create_end()
            self.create_characters()
            self.create_init_attributes()

    def create_includes(self):
        f = self.c_file
        print("#include <string.h>", file=f)
        print("#include <libxml/xmlwriter.h>", file=f)
        print("#include <pharmml/common_types.h>", file=f)
        print("#include <pharmml/string.h>", file=f)
        print('#include <', self.namespace, '/', self.name, '.h>', sep='', file=f)
        print('#include <', self.namespace, '/private/', self.name, '.h>', sep='', file=f)
        print(file=f)

    def create_new(self):
        f = self.c_file
        print(self.class_name, " *", self.class_name, "_new()", sep='', file=f)
        print("{", file=f);
        print("\t", self.class_name, " *object = calloc(sizeof(", self.class_name, "), 1);", sep='', file=f)
        print("\tif (object) {", file=f)
        print("\t\tobject->reference_count = 1;", file=f)
        if self.extends:
            print("\t\tobject->base = ", self.prefix_class(self.extends), "_new();", sep='', file=f)
            print("\t\tif (!object->base) {", file=f)
            print("\t\t\tfree(object);", file=f)
            print("\t\t\tobject = NULL;", file=f)
            print("\t\t}", file=f)
        print("\t}", file=f)
        print(file=f)
        print("\treturn object;", file=f)
        print("}", file=f)
        print(file=f)

    def create_copy(self):

        def copy_type(so_type, name, array=False):
            if so_type == 'type_string':
                result = "pharmml_strdup(self->" + name + ");"
            elif so_type == 'type_real' or so_type == 'type_int':
                result = "self->" + name + ";"
            else:
                result = self.prefix_class(so_type) + "_copy(self->" + name
                if array:
                    result += "[i]"
                result += ");"
            return result

        f = self.c_file
        print(self.class_name, " *", self.class_name, "_copy(", self.class_name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\t", self.class_name, " *dest = ", self.class_name, "_new();", sep='', file=f)
        print("\tif (dest) {", file=f)
        if self.extends:
            print("\t\tdest->base = ", self.prefix_class(self.extends), "_copy(self->base);", sep='', file=f)
            print("\t\tif (!dest->base) {", file=f)
            print("\t\t\t", self.class_name, "_free(dest);", sep='', file=f)
            print("\t\t\treturn NULL;", file=f)
            print("\t\t}", file=f)
        if self.attributes:
            for attr in self.attributes:
                if attr['type'] == "type_string":
                    print("\t\tif (self->", attr['name'], ") {", sep='', file=f)
                    print("\t\t\tdest->", attr['name'], " = pharmml_strdup(self->", attr['name'], ");", sep='', file=f)
                    print("\t\t\tif (!dest->", attr['name'], ") {", sep='', file=f)
                    print("\t\t\t\t", self.class_name, "_free(dest);", sep='', file=f)
                    print("\t\t\t\treturn NULL;", file=f)
                    print("\t\t\t}", file=f)
                    print("\t\t}", file=f)
                elif attr['type'] == "type_int":
                    print("\t\tif (self->", attr['name'], ") {", sep='', file=f)
                    print("\t\t\tdest->", attr['name'], "_number = self->", attr['name'], "_number;", sep='', file=f)
                    print("\t\t\tdest->", attr['name'], " = &(dest->", attr['name'], "_number);", sep='', file=f)
                    print("\t\t}", file=f)
        if self.children:
            for e in self.children:
                if e.get('array', False):
                    print("\t\tif (self->num_", e['name'], ") {", sep='', file=f)
                    print("\t\t\tdest->", e['name'], " = calloc(self->num_", e['name'], " * sizeof(", self.prefix_class(e['type']), " *), 1);", sep='', file=f)
                    print("\t\t\tif (!dest->", e['name'], ") {", sep='', file=f)
                    print("\t\t\t\t", self.class_name, "_free(dest);", sep='', file=f)
                    print("\t\t\t\treturn NULL;", file=f)
                    print("\t\t\t}", file=f)
                    print("\t\t\tdest->num_", e['name'], " = self->num_", e['name'], ";", sep='', file=f)
                    print("\t\t\tfor (int i = 0; i < self->num_", e['name'], "; i++) {", sep='', file=f)
                    # Should not need check for NULL here
                    print("\t\t\t\tdest->", e['name'], "[i] = ", copy_type(e['type'], e['name'], array=True), sep='', file=f)
                    print("\t\t\t\tif (!dest->", e['name'], "[i]) {", sep='', file=f)
                    print("\t\t\t\t\t", self.class_name, "_free(dest);", sep='', file=f)
                    print("\t\t\t\t\treturn NULL;", file=f)
                    print("\t\t\t\t}", file=f)
                    print("\t\t\t}", file=f)
                    print("\t\t}", file=f)
                elif e['type'] == 'type_int' or e['type'] == 'type_real':
                    print("\t\tif (self->", e['name'], ") {", sep='', file=f)
                    print("\t\t\tdest->", e['name'], "_number = self->", e['name'], "_number;", sep='', file=f)
                    print("\t\t\tdest->", e['name'], " = &(dest->", e['name'], "_number);", sep='', file=f)
                    print("\t\t}", file=f)
                else:
                    print("\t\tif (self->", e['name'], ") {", sep='', file=f)
                    print("\t\t\tdest->", e['name'], " = ", copy_type(e['type'], e['name']), sep='', file=f)
                    print("\t\t\tif (!dest->", e['name'], ") {", sep='', file=f)
                    print("\t\t\t\t", self.class_name, "_free(dest);", sep='', file=f)
                    print("\t\t\t\treturn NULL;", file=f)
                    print("\t\t\t}", file=f)
                    print("\t\t}", file=f)
        print("\t}", file=f)
        print(file=f)
        print("\treturn dest;", file=f)
        print("}", file=f)
        print(file=f)

    def create_free(self):
        f = self.c_file
        print("void ", self.class_name, "_free(", self.class_name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\tif (self) {", file=f)

        if self.children:
            for e in self.children:
                is_array = e.get('array', False)
                if is_array:
                    print("\t\tfor(int i = 0; i < self->num_", e['name'], "; i++) {", sep='', file=f)
                    print("\t\t\t", self.prefix_class(e['type']), "_unref(self->", e['name'], "[i]);", sep='', file=f)
                    print("\t\t}", sep='', file=f)
                    print("\t\tfree(self->", e['name'], ");", sep='', file=f)
                else:
                    if e['type'] == 'type_string':
                        print("\t\tfree(self->", e['name'], ");", sep='', file=f)
                    elif e['type'] == 'type_real' or e['type'] == 'type_int':
                        pass
                    else:
                        print("\t\t", self.prefix_class(e['type']), "_unref(self->", e['name'], ");", sep='', file=f)
        if self.attributes:
            for a in self.attributes:
                if a['type'] == 'type_string':
                    print("\t\tif (self->", a['name'], ") free(self->", a['name'], ");", sep='', file=f)
        if self.extends:
            print("\t\tfree(self->base);", file=f)
        print("\t\tfree(self);", file=f)
        print("\t}", file=f)
        print("}", file=f)
        print(file=f)

    def create_ref_unref(self):
        f = self.c_file
        print("void ", self.class_name, "_ref(", self.class_name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\tself->reference_count++;", file=f)
        print("}", file=f)
        print(file=f)
        print("void ", self.class_name, "_unref(", self.class_name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\tif (self) {", file=f)
        print("\t\tself->reference_count--;", file=f)
        print("\t\tif (!self->reference_count) {", file=f)
        print("\t\t\t", self.class_name, "_free(self);", sep='', file=f)
        print("\t\t}", file=f)
        print("\t}", file=f)
        print("}", file=f)
        print(file=f)

    def create_getters(self):
        f = self.c_file
        # getters for attributes
        if self.attributes:
            for a in self.attributes:
                if a['type'] == 'type_string':
                    print("char *", self.class_name, "_get_", a['name'], "(", self.class_name, " *self)", sep='', file=f)
                    print("{", file=f)
                    print("\treturn self->", a['name'], ";", sep='', file=f)
                    print("}", file=f)
                    print(file=f)
                elif a['type'] == 'type_int':
                    print("int *", self.class_name, "_get_", a['name'], "(", self.class_name, " *self)", sep='', file=f)
                    print("{", file=f)
                    print("\treturn self->", a['name'], ";", sep='', file=f)
                    print("}", file=f)
                    print(file=f)

        # getters for child elements
        if self.children:
            for e in self.children:
                if e['type'] == 'type_string' or e['type'] == 'type_real' or e['type'] == 'type_int':
                    if e['type'] == 'type_string':
                        return_type = "char *"
                    elif e['type'] == 'type_int':
                        return_type = "int *"
                    else:
                        return_type = "double *"
                    print(return_type, self.class_name, "_get_", e['name'], "(", self.class_name, " *self)", sep='', file=f)
                    print("{", file=f)
                    print("\treturn self->", e['name'], ";", sep='', file=f)
                    print("}", file=f)
                    print(file=f)
                else:
                    print(self.prefix_class(e['type']), " *", self.class_name, "_get_", e['name'], "(", self.class_name,  " *self", sep='', end='', file=f)
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
                    print("int ", self.class_name, "_get_number_of_", e['name'], "(", self.class_name, " *self)", sep='', file=f)
                    print("{", file=f)
                    print("\treturn self->num_", e['name'], ";", sep='', file=f)
                    print("}", file=f)
                    print(file=f)

    def create_setters(self):
        f = self.c_file
        # setters for attributes
        if self.attributes:
            for a in self.attributes:
                if a['type'] == 'type_string':
                    print("int ", self.class_name, "_set_", a['name'], "(", self.class_name, " *self, char *value)", sep='', file=f)
                    print("{", file=f)
                    print("\tif (!value) {", file=f)
                    print("\t\tself->", a['name'], " = value;", sep='', file=f)
                    print("\t\treturn 0;", file=f)
                    print("\t}", file=f)
                    print("\tchar *new_value = pharmml_strdup(value);", file=f)
                    print("\tif (new_value) {", file=f)
                    print("\t\tfree(self->", a['name'], ");", sep='', file=f)
                    print("\t\tself->", a['name'], " = new_value;", sep='', file=f)
                    print("\t\treturn 0;", file=f)
                    print("\t} else {", file=f)
                    print("\t\treturn 1;", file=f)
                    print("\t}", file=f)
                    print("}", file=f)
                    print(file=f)
                elif a['type'] == 'type_int':
                    print("void ", self.class_name, "_set_", a['name'], "(", self.class_name, " *self, int *value)", sep='', file=f)
                    print("{", file=f)
                    print("\tif (value) {", file=f)
                    print("\t\tself->", a['name'], "_number = *value;", sep='', file=f)
                    print("\t\tself->", a['name'], " = &(self->", a['name'], "_number);", sep='', file=f)
                    print("\t} else {", file=f)
                    print("\t\tself->", a['name'], " = value;", sep='', file=f)
                    print("\t}", file=f)
                    print("}", file=f)
                    print(file=f)

        # setters for child elements
        if self.children:
            for e in self.children:
                if not e.get('array', False):       # No setters for arrays
                    if e['type'] == "type_string":
                        print("int ", self.class_name, "_set_", e['name'], "(", self.class_name, " *self, char *value)", sep='', file=f)
                        print("{", file=f)
                        print("\tif (!value) {", file=f)
                        print("\t\tself->", e['name'], " = value;", sep='', file=f)
                        print("\t\treturn 0;", file=f)
                        print("\t}", file=f)
                        print("\tchar *new_value = pharmml_strdup(value);", file=f)
                        print("\tif (new_value) {", file=f)
                        print("\t\tfree(self->", e['name'], ");", sep='', file=f)
                        print("\t\tself->", e['name'], " = new_value;", sep='', file=f)
                        print("\t\treturn 0;", file=f)
                        print("\t} else {", file=f)
                        print("\t\treturn 1;", file=f)
                        print("\t}", file=f)
                        print("}", file=f)
                        print(file=f)
                    elif e['type'] == "type_real":
                        print("void ", self.class_name, "_set_", e['name'], "(", self.class_name, " *self, double *value)", sep='', file=f)
                        print("{", file=f)
                        print("\tif (value) {", file=f)
                        print("\t\tself->", e['name'], "_number = *value;", sep='', file=f)
                        print("\t\tself->", e['name'], " = &(self->", e['name'], "_number);", sep='', file=f)
                        print("\t} else {", file=f)
                        print("\t\tself->", e['name'], " = value;", sep='', file=f)
                        print("\t}", file=f)
                        print("}", file=f)
                        print(file=f)
                    elif e['type'] == "type_int":
                        print("void ", self.class_name, "_set_", e['name'], "(", self.class_name, " *self, int *value)", sep='', file=f)
                        print("{", file=f)
                        print("\tif (value) {", file=f)
                        print("\t\tself->", e['name'], "_number = *value;", sep='', file=f)
                        print("\t\tself->", e['name'], " = &(self->", e['name'], "_number);", sep='', file=f)
                        print("\t} else {", file=f)
                        print("\t\tself->", e['name'], " = value;", sep='', file=f)
                        print("\t}", file=f)
                        print("}", file=f)
                        print(file=f)
                    else:
                        print("void ", self.class_name, "_set_", e['name'], "(", self.class_name, " *self, ", self.prefix_class(e['type']), " *value)", sep='', file=f)
                        print("{", file=f)
                        print("\t", self.prefix_class(e['type']), "_unref(self->", e['name'], ");", sep='', file=f)
                        print("\tself->", e['name'], " = value;", sep='', file=f)
                        print("}", file=f)
                        print(file=f)

    def create_create(self):
        f = self.c_file
        if self.children:
            for e in self.children:
                if e['type'] != "type_string" and e['type'] != "type_real" and e['type'] != "type_int":
                    is_array = e.get('array', False)
                    print(self.prefix_class(e['type']), " *", self.class_name, "_create_", e['name'], "(", self.class_name, " *self)", sep='', file=f)
                    print("{", file=f)
                    print("\t", self.prefix_class(e['type']), " *obj = ", self.prefix_class(e['type']), "_new(", sep='', end='', file=f)
                    if e['type'] in need_name:
                        print('"', end='', file=f)
                        if 'prefix' in e:
                            print(e['prefix'], ":", sep='', end='', file=f)
                        print(e['name'], '"', sep='', end='', file=f)
                    print(");", sep='', file=f)
                    print("\tif (obj) {", file=f)
                    if is_array:
                        print("\t\t", self.prefix_class(e['type']), " **newblock = realloc(self->", e['name'], ", (self->num_", e['name'], " + 1) * sizeof(", self.prefix_class(e['type']), " *));", sep='', file=f)
                        print("\t\tif (newblock) {", file=f)
                        print("\t\t\tself->", e['name'], " = newblock;", sep='', file=f)
                        print("\t\t\tself->", e['name'], "[self->num_", e['name'], "] = obj;", sep='', file=f)
                        print("\t\t\tself->num_", e['name'], "++;", sep='', file=f)
                        print("\t\t} else {", file=f)
                        print("\t\t\t", self.prefix_class(e['type']), "_free(obj);", sep='', file=f)
                        print("\t\t\tobj = NULL;", file=f)
                        print("\t\t}", file=f)
                    else:
                        print("\t\tself->", e['name'], " = obj;", sep='', file=f)
                    print("\t}", file=f)
                    print("\treturn obj;", file=f)
                    print("}", file=f)
                    print(file=f)

    def create_array_add(self):
        f = self.c_file
        if self.children:
            for e in self.children:
                if e.get('array', False):
                    print("int ", self.class_name, "_add_", e['name'], "(", self.class_name, " *self, ", self.prefix_class(e['type']), " *child)", sep='', file=f)
                    print("{", file=f)
                    print("\t", self.prefix_class(e['type']), " **new_array = realloc(self->", e['name'], ", (self->num_", e['name'], " + 1) * sizeof(", self.prefix_class(e['type']), " *));", sep='', file=f)
                    print("\tif (!new_array) {", file=f)
                    print("\t\treturn 1;", file=f)
                    print("\t}", file=f)
                    print("\tself->", e['name'], " = new_array;", sep='', file=f)
                    print("\tself->", e['name'], "[self->num_", e['name'], "] = child;", sep='', file=f)
                    print("\tself->num_", e['name'], "++;", sep='', file=f)
                    print("\treturn 0;", file=f)
                    print("}", file=f)
                    print(file=f)

                    print("int ", self.class_name, "_remove_", e['name'], "(", self.class_name, " *self, int index)", sep='', file=f)
                    print("{", file=f)
                    print("\tint size = self->num_", e['name'], ";", sep='', file=f)
                    print("\tif (index >= size) {", file=f)
                    print("\t\treturn 0;", file=f)
                    print("\t}", file=f)
                    print("\tif (size == 1) {", file=f)
                    print("\t\tfree(self->", e['name'], ");", sep='', file=f)
                    print("\t\tself->", e['name'], " = NULL;", sep='', file=f)
                    print("\t\treturn 0;", file=f)
                    print("\t}", file=f);
                    print("\t", self.prefix_class(e['type']), " *final = self->", e['name'], "[size - 1];", sep='', file=f)
                    print("\t", self.prefix_class(e['type']), " *remove = self->", e['name'], "[index];", sep='', file=f)
                    print("\t", self.prefix_class(e['type']), " **new_array = realloc(self->", e['name'], ", (self->num_", e['name'], " - 1) * sizeof(", self.prefix_class(e['type']), " *));", sep='', file=f)
                    print("\tif (!new_array) {", file=f)
                    print("\t\treturn 1;", file=f)
                    print("\t}", file=f)
                    print("\tself->num_", e['name'], "--;", sep='', file=f)
                    print("\tif (final == remove) {", file=f)
                    print("\t\treturn 0;", file=f)
                    print("\t}", file=f)
                    print("\tfor (int i = index; i < size - 3; i++) {", file=f)
                    print("\t\tself->", e['name'], "[i] = self->", e['name'], "[i + 1];", sep='', file=f)
                    print("\t}", file=f)
                    print("\tself->", e['name'], "[size - 2] = final;", sep='', file=f)
                    print("\t", self.prefix_class(e['type']), "_unref(remove);", sep='', file=f)
                    print("\treturn 0;", file=f)
                    print("}", file=f)
                    print(file=f)

    def create_get_set_base(self):
        f = self.c_file
        print(self.prefix_class(self.extends), " *", self.class_name, "_get_base(", self.class_name, " *self)", sep='', file=f)
        print("{", file=f)
        print("\treturn self->base;", file=f)
        print("}", file=f)
        print(file=f)
        print("int ", self.class_name, "_set_base(", self.class_name, " *self, ", self.prefix_class(self.extends), " *value)", sep='', file=f)
        print("{", file=f)
        print("\t", self.prefix_class(self.extends), "_unref(value);", sep='', file=f)
        print("\tself->base = value;", file=f) 
        print("\treturn 0;", file=f)
        print("}", file=f)
        print(file=f)

    def create_xml(self):
        f = self.c_file
        if self.xml_injection:
            print(self.xml_injection, file=f)
            return
        print("int ", self.class_name, "_xml(", self.class_name, " *self, xmlTextWriterPtr writer", end='', sep='', file=f)
        if self.name in need_name:
            print(", char *element_name", end='', file=f)
        print(")", sep='', file=f)
        print("{", file=f)
        print("\tint rc;", file=f)

        if self.children or self.attributes:
            items_to_test = []
            if self.children:
                items_to_test += self.children
            if self.attributes:
                items_to_test += self.attributes
            # Check if any sub-element is non-NULL
            print("\tif (", end='', file=f) 
            for e in items_to_test[:-1]:
                print("self->", e['name'], ' || ', sep='', end='', file=f)
            print("self->", items_to_test[-1]['name'], ") {", sep='', file=f)

        if self.extends:
            print("\t\trc = ", self.prefix_class(self.extends), "_xml(self->base, writer, element_name);", sep='', file=f) 
            print("\t\tif (rc != 0) return rc;", file=f)
        else:
            if self.element_name:
                name = self.element_name
            else:
                name = self.name
            if self.name in need_name:
                print('\t\trc = xmlTextWriterStartElement(writer, BAD_CAST element_name);', sep='', file=f)
                print('\t\tif (rc < 0) return 1;', file=f)
            else:
                print('\t\trc = xmlTextWriterStartElement(writer, BAD_CAST "', self.prefix, name, '");', sep='', file=f)
                print('\t\tif (rc < 0) return 1;', file=f)

        if self.fixed_attributes:
            for a in self.fixed_attributes:
                print('\t\trc = xmlTextWriterWriteAttribute(writer, BAD_CAST "', a['name'], '", BAD_CAST "', a['value'], '");', sep='', file=f)
                print('\t\tif (rc < 0) return 1;', file=f)

        if self.attributes:
            for a in self.attributes:
                print('\t\tif (self->', a['name'], ") {", sep='', file=f)
                if a['type'] == 'type_string':
                    print('\t\t\trc = xmlTextWriterWriteAttribute(writer, BAD_CAST "', a['name'], '", BAD_CAST self->', a['name'], ");", sep='', file=f)
                    print('\t\t\tif (rc < 0) return 1;', file=f)
                elif a['type'] == 'type_int':
                    print("\t\t\tchar *attr_string = pharmml_int_to_string(self->", a['name'], "_number);", sep='', file=f)
                    print("\t\t\tif (!attr_string) return 1;", file=f)
                    print('\t\t\trc = xmlTextWriterWriteAttribute(writer, BAD_CAST "', a['name'], '", BAD_CAST ', "attr_string);", sep='', file=f)
                    print("\t\t\tfree(attr_string);", file=f)
                    print('\t\t\tif (rc < 0) return 1;', file=f)
                print("\t\t}", file=f)

        if self.children:
            for e in self.children:
                print("\t\tif (self->", e['name'], ") {", sep='', file=f)
                if e['type'] in need_name:
                    extra = ", \"" + e['name'] + "\""
                else:
                    extra = ""
                is_array = e.get('array', False)
                if is_array:
                    print("\t\t\tfor (int i = 0; i < self->num_", e['name'], "; i++) {" ,sep='', file=f)
                    print("\t\t\t\trc = ", self.prefix_class(e['type']), "_xml(self->", e['name'], "[i], writer", extra, ");", sep='', file=f)
                    print("\t\t\t\tif (rc != 0) return 1;", file=f)
                    print("\t\t\t}", file=f)
                else:
                    if e['type'] == "type_string" or e['type'] == "type_real" or e['type'] == "type_int":
                        element_name = e['name']
                        if e.get('prefix', False):
                            element_name = e['prefix'] + ":" + element_name
                        if e['type'] == "type_string":
                            print("\t\t\trc = xmlTextWriterWriteElement(writer, BAD_CAST \"", element_name, "\", BAD_CAST self->", e['name'], ");", sep='', file=f)
                            print("\t\t\tif (rc < 0) return 1;", file=f)
                        elif e['type'] == "type_real":
                            print("\t\t\tchar *number_string = pharmml_double_to_string(self->", e['name'], "_number);", sep='', file=f)
                            print("\t\t\tif (!number_string) return 1;", file=f)
                            print("\t\t\trc = xmlTextWriterWriteElement(writer, BAD_CAST \"", element_name, "\", BAD_CAST number_string);", sep='', file=f)
                            print("\t\t\tfree(number_string);", file=f)
                            print("\t\t\tif (rc < 0) return 1;", file=f)
                        elif e['type'] == "type_int":
                            print("\t\t\tchar *number_string = pharmml_int_to_string(self->", e['name'], "_number);", sep='', file=f)
                            print("\t\t\tif (!number_string) return 1;", file=f)
                            print("\t\t\trc = xmlTextWriterWriteElement(writer, BAD_CAST \"", element_name, "\", BAD_CAST number_string);", sep='', file=f)
                            print("\t\t\tfree(number_string);", file=f)
                            print("\t\t\tif (rc < 0) return 1;", file=f)
                    else:
                        print("\t\t\trc = ", self.prefix_class(e['type']), "_xml(self->", e['name'], ", writer", extra, ");", sep='', file=f)
                        print("\t\t\tif (rc != 0) return rc;", file=f)

                print("\t\t}", file=f)

        if self.children or self.attributes:
            print("\t\trc = xmlTextWriterEndElement(writer);", file=f)
            print("\t\tif (rc < 0) return 1;", file=f)
            print("\t}", file=f)

        print("\treturn 0;", file=f)
        print("}", file=f)
        print(file=f)

    def create_start(self):
        f = self.c_file
        print("int ", self.class_name, "_start_element(", self.class_name, " *self, const char *localname, int nb_attributes, const char **attributes)", sep='', file=f)
        print("{", file=f)

        if self.children:
            first = True
            for i in range(0, len(self.children)):
                if self.children[i]['type'] != "type_string" and self.children[i]['type'] != "type_real" and self.children[i]['type'] != "type_int":
                    if not first:
                        print(" else ", end='', file=f)
                    else:
                        print("\t", end='', file=f)
                        first = False
                    print("if (self->in_", self.children[i]['name'], ") {", sep='', file=f)
                    is_array = self.children[i].get('array', False)
                    if is_array:
                        print("\t\tint fail = ", self.prefix_class(self.children[i]['type']), "_start_element(self->", self.children[i]['name'], "[self->num_", self.children[i]['name'], " - 1], localname, nb_attributes, attributes);", sep='', file=f)
                    else:
                        print("\t\tint fail = ", self.prefix_class(self.children[i]['type']), "_start_element(self->", self.children[i]['name'], ", localname, nb_attributes, attributes);", sep='', file=f)
                    print("\t\tif (fail) {", sep='', file=f)
                    print("\t\t\treturn fail;", file=f)
                    print("\t\t}", sep='', file=f)
                    print("\t}", end='', file=f)

            for e in self.children:
                if not first:
                    print(" else ", end='', file=f)
                else:
                    print("\t", end='', file=f)
                    first = False
                print('if (strcmp(localname, "', e['name'], '") == 0) {', sep='', file=f)
                if e['type'] != "type_string" and e['type'] != "type_real" and e['type'] != "type_int":
                    if e['type'] in self.structure and 'attributes' in self.structure[e['type']]:
                        print("\t\t", self.prefix_class(e['type']), " *", e['name'], " = ", self.prefix_class(e['type']), "_new(", end='', sep='', file=f)
                        if e['type'] in need_name:  # FIXME this should be put into a function
                            print('"', end='', file=f)
                            if 'prefix' in e:
                                print(e['prefix'], ":", sep='', end='', file=f)
                            print(e['name'], '"', sep='', end='', file=f)
                        print(");", sep='', file=f)
                    else:
                        print("\t\t", self.prefix_class(e['type']), " *", e['name'], " = ", self.class_name, "_create_", e['name'], "(self);", sep='', file=f)
                    print("\t\tif (!", e['name'], ") {", sep='', file=f)
                    print("\t\t\treturn 1;", file=f)
                    print("\t\t}", file=f)
                    if e['type'] in self.structure:
                        if 'attributes' in self.structure[e['type']]:
                            print("\t\tint fail = ", self.prefix_class(e['type']), "_init_attributes(", e['name'], ", nb_attributes, attributes);", sep='', file=f)
                            print("\t\tif (fail) {", file=f)
                            print("\t\t\t", self.prefix_class(e['type']), "_free(", e['name'], ");", sep='', file=f)
                            print("\t\t\treturn 1;", file=f)
                            print("\t\t}", file=f)
                            if e.get('array', False):
                                print("\t\tfail = ", self.class_name, "_add_", e['name'], "(self, ", e['name'], ");", sep='', file=f)
                                print("\t\tif (fail) {", file=f)
                                print("\t\t\t", self.prefix_class(e['type']), "_free(", e['name'], ");", sep='', file=f)
                                print("\t\t\treturn 1;", file=f)
                                print("\t\t}", file=f)
                            else:
                                print("\t\t", self.class_name, "_set_", e['name'], "(self, ", e['name'], ");", sep='', file=f)
                print("\t\tself->in_", e['name'], " = 1;", sep='', file=f)
                print("\t}", end='', file=f)

        if self.extends:
            if self.children:
                print(" else {", file=f)
                print("\t", end='', file=f)
            print("\tint fail = ", self.prefix_class(self.extends), "_start_element(self->base, localname, nb_attributes, attributes);", sep='', file=f)
            print("\tif (fail) {", file=f)
            print("\t\treturn fail;", file=f)
            print("\t}", file=f)

            if self.children:
                print("\t}", end='', file=f)

        if self.children:
            print(file=f)

        print("\treturn 0;", file=f)
        print("}", file=f)
        print(file=f)

    def create_end(self):
        f = self.c_file
        print("void ", self.class_name, "_end_element(", self.class_name, " *self, const char *localname)", sep='', file=f)
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
                if e['type'] != "type_string" and e['type'] != "type_real" and e['type'] != "type_int":
                    print(" else if (self->in_", e['name'], ") {", sep='', file=f)
                    print("\t\t", self.prefix_class(e['type']), "_end_element(self->", e['name'], sep='', end='', file=f)
                    if e.get("array", False):
                        print("[self->num_", e['name'], " - 1]", sep='', end='', file=f)
                    print(", localname);", file=f)
                    print("\t}", end='', file=f)

            print(file=f)

        if self.extends:
            if self.children:
                print(" else {", file=f)
                print("\t", end='', file=f)
            print("\t", self.prefix_class(self.extends), "_end_element(self->base, localname);", sep='', file=f)
            if self.children:
                print("\t}", end='', file=f)

        print("}", file=f)
        print(file=f)

    def create_characters(self):
        f = self.c_file
        print("int ", self.class_name, "_characters(", self.class_name, " *self, const char *ch, int len)", sep='', file=f)
        print("{", file=f)

        if self.children:
            for i in range(0, len(self.children)):
                if i != 0:
                    print(" else ", end='', file=f)
                else:
                    print("\t", end='', file=f)
                print("if (self->in_", self.children[i]['name'], ") {", sep='', file=f)
                if self.children[i]['type'] == "type_string":
                    print("\t\tself->", self.children[i]['name'], " = pharmml_strndup(ch, len);", sep='', file=f)
                    print("\t\tif (!self->", self.children[i]['name'], ") {", sep='', file=f)
                    print("\t\t\treturn 1;", file=f)
                    print("\t\t}", file=f)
                elif self.children[i]['type'] == "type_real":
                    print("\t\tself->", self.children[i]['name'], "_number = pharmml_string_to_double(ch);", sep='', file=f)
                    print("\t\tself->", self.children[i]['name'], " = &(self->", self.children[i]['name'], "_number);", sep='', file=f)
                elif self.children[i]['type'] == "type_int":
                    print("\t\tself->", self.children[i]['name'], "_number = pharmml_string_to_int(ch);", sep='', file=f)
                    print("\t\tself->", self.children[i]['name'], " = &(self->", self.children[i]['name'], "_number);", sep='', file=f)
                else:
                    print("\t\tint fail = ", self.prefix_class(self.children[i]['type']), "_characters(self->", self.children[i]['name'], sep='', end='', file=f)
                    if self.children[i].get("array", False):
                        print("[self->num_", self.children[i]['name'], " - 1]", sep='', end='', file=f)
                    print(", ch, len);", file=f)
                    print("\t\tif (fail) return 1;", file=f)
                print("\t}", end='', file=f)

            print(file=f)

        if self.extends:
            if self.children:
                print(" else {", file=f)
                print("\t", end='', file=f)
            print("\tint fail = ", self.prefix_class(self.extends), "_characters(self->base, ch, len);", sep='', file=f)
            print("\tif (fail) return 1;", file=f)
            if self.children:
                print("\t}", end='', file=f)

        print("\treturn 0;", file=f)
        print("}", file=f)

    def create_init_attributes(self):
        f = self.c_file
        if self.attributes:
            print(file=f)
            print("int ", self.class_name, "_init_attributes(", self.class_name, " *self, int nb_attributes, const char **attributes)", sep='', file=f)
            print("{", file=f)
            print("\tunsigned int index = 0;", file=f)
            print("\tfor (int i = 0; i < nb_attributes; i++, index += 5) {", file=f)
            print("\t\tconst char *localname = attributes[index];", file=f)
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

                print(' (strcmp(localname, "', a['name'], '") == 0) {', sep='', file=f)
                if a['type'] == 'type_string':
                    print("\t\t\tself->", a['name'], " = pharmml_strndup(valueBegin, valueEnd - valueBegin);", sep='', file=f)
                    print("\t\t\tif (!self->", a['name'], ") {", sep='', file=f)
                    print("\t\t\t\treturn 1;", file=f)
                    print("\t\t\t}", file=f)
                elif a['type'] == 'type_int':
                    print("\t\t\tchar *final_char = (char *) valueEnd;", file=f)
                    print("\t\t\tchar end_char = *final_char;", file=f)
                    print("\t\t\t*final_char = '\\0';", file=f)
                    print("\t\t\tself->", a['name'], "_number = pharmml_string_to_int(valueBegin);", sep='', file=f)
                    print("\t\t\tself->", a['name'], " = &(self->", a['name'], "_number);", sep='', file=f)
                    print("\t\t\t*final_char = end_char;", file=f)

            print("\t\t}", file=f)
            print("\t}", file=f)
            print("\treturn 0;", file=f)
            print("}", file=f)

    def create_headers(self):
        with open(self.name + ".h", "w") as f:
            common.output_file = f
            common.print_license_string()
            print("#ifndef _", self.namespace.upper(), "_", self.name.upper(), "_H", sep='', file=f)
            print("#define _", self.namespace.upper(), "_", self.name.upper(), "_H", sep='', file=f)
            print(file=f)

            included = [ 'type_string', 'type_real', 'type_int' ]
            if self.children:
                for e in self.children:
                    if e['type'] not in included:
                        included.append(e['type'])
                        print('#include <', self.namespaces[e['type']], '/', e['type'], '.h>', sep='', file=f)

            if self.extends and self.extends not in included:
                included.append(self.extends)
                print("#include <", self.namespaces[self.extends], "/", self.extends, ".h>", sep='', file=f)

            print(file=f)
            print("/** \\struct ", self.class_name, sep='', file=f)
            print("\t \\brief A structure representing an", self.structure[self.name]['xpath'], "element", file=f)
            print("*/", file=f)

            print("typedef struct ", self.class_name, " ", self.class_name, ";", sep='', file=f)
            print(file=f)

            # function prototypes
            print("/** \\memberof ", self.class_name, sep='', file=f)
            print(" * Create a new empty ", self.class_name, " structure.", sep='', file=f)
            print(" * \\return A pointer to the newly created struct or NULL if memory allocation failed", file=f)
            print(" * \\sa ", self.class_name, "_copy, ", self.class_name, "_free", sep='', file=f)
            print(" */", file=f)
            
            print(self.class_name, " *", self.class_name, "_new();", sep='', file=f)

            print("/** \\memberof ", self.class_name, sep='', file=f)
            print(" * Create a copy of a ", self.class_name, " structure.", sep='', file=f)
            print(" * \\return A pointer to the newly created struct or NULL if memory allocation failed", file=f)
            print(" * \\sa ", self.class_name, "_new", sep='', file=f)
            print(" */", file=f)

            print(self.class_name, " *", self.class_name, "_copy(", self.class_name, " *self);", sep='', file=f)

            print("/** \\memberof ", self.class_name, sep='', file=f)
            print(" * Free all memory associated with a ", self.class_name, " structure and its children.", sep='', file=f)
            print(" * \\param self - a pointer to the structure to free", file=f)
            print(" * \\sa ", self.class_name, "_new", sep='', file=f)
            print(" */", file=f)

            print("void ", self.class_name, "_free(", self.class_name, " *self);", sep='', file=f)

            print("void ", self.class_name, "_ref(", self.class_name, " *self);", sep='', file=f)
            print("void ", self.class_name, "_unref(", self.class_name, " *self);", sep='', file=f)
            if self.extends:
                print(self.prefix_class(self.extends), " *", self.class_name, "_get_base(", self.class_name, " *self);", sep='', file=f)
                print("int ", self.class_name, "_set_base(", self.class_name, " *self, ", self.prefix_class(self.extends), " *value);", sep='', file=f)

            if self.attributes:
                for a in self.attributes:
                    print("/** \\memberof ", self.class_name, sep='', file=f)
                    print(" * Get the value of the ", a['name'], " attribute", sep='', file=f)
                    print(" * \\param self - pointer to a ", self.class_name, sep='', file=f)
                    print(" * \\return A pointer to the attribute value", file=f)
                    print(" * \\sa ", self.class_name, "_set_", a['name'], sep='', file=f)
                    print(" */", file=f)

                    if a['type'] == 'type_string':
                        print("char *", self.class_name, "_get_", a['name'], "(", self.class_name, " *self);", sep='', file=f)
                    elif a['type'] == 'type_int':
                        print("int *", self.class_name, "_get_", a['name'], "(", self.class_name, " *self);", sep='', file=f)

                    print("/** \\memberof ", self.class_name, sep='', file=f)
                    print(" * Set the value of the ", a['name'], " attribute", sep='', file=f)
                    print(" * \\param self - pointer to a ", self.class_name, sep='', file=f)
                    print(" * \\param value - A pointer to the attribute value", file=f)
                    if a['type'] == 'type_string':
                        print(" * \\return 0 for success", file=f)
                    print(" * \\sa ", self.class_name, "_get_", a['name'], sep='', file=f)
                    print(" */", file=f)

                    if a['type'] == 'type_string':
                        print("int ", self.class_name, "_set_", a['name'], "(", self.class_name, " *self, char *value);", sep='', file=f)
                    elif a['type'] == 'type_int':
                        print("void ", self.class_name, "_set_", a['name'], "(", self.class_name, " *self, int *value);", sep='', file=f)

            if self.children:
                for e in self.children:
                    print("/** \\memberof ", self.class_name, sep='', file=f)
                    print(" * Get the", e['name'], "element", file=f)
                    print(" * \\param self - pointer to a ", self.class_name, sep='', file=f)
                    if e.get("array", False):
                        print(" * \\param number - An index to the specific element", file=f)
                    if e['type'] == "type_string":
                        print(" * \\return A pointer to the", e['name'], "string", file=f)
                    elif e['type'] == "type_real" or e['type'] == "type_int":
                        print(" * \\return A pointer to the value of", e['name'], "or NULL if no value is present.", file=f)
                    else:
                        print(" * \\return A pointer to the structure representing the", e['name'], "element", file=f)
                    print(" * \\sa ", self.class_name, "_set_", e['name'], sep='', file=f)
                    print(" */", file=f)

                    if e['type'] == "type_string":
                        return_type = "char *"
                    elif e['type'] == "type_real":
                        return_type = "double *"
                    elif e['type'] == "type_int":
                        return_type = "int *"
                    else:
                        return_type = self.prefix_class(e['type']) + " *"
                    print(return_type, self.class_name, "_get_", e['name'], "(", self.class_name, " *self", sep='', end='', file=f)
                    if e.get("array", False):
                        print(", int number", end='', file=f)
                    print(");", file=f)

                    # _get_number_of_ for arrays
                    if e.get('array', False):
                        print("/** \\memberof ", self.class_name, sep='', file=f)
                        print(" * Get the number of", e['name'], "currently contained in the", self.name, "structure", file=f)
                        print(" * \\param self - pointer to a ", self.class_name, sep='', file=f)
                        print(" * \\return The number of ", e['name'], "s", sep='', file=f)
                        print(" */", file=f)
                        print("int ", self.class_name, "_get_number_of_", e['name'], "(", self.class_name, " *self);", sep='', file=f)

                for e in self.children:
                    if not e.get('array', False):   # No setters for arrays
                        return_type = "void"
                        if e['type'] == "type_string":
                            man_type = "string"
                            param_type = "char *"
                            return_type = "int"
                        elif e['type'] == "type_real":
                            param_type = "double *"
                        elif e['type'] == "type_int":
                            param_type = "int *"
                        else:
                            man_type = self.prefix_class(e['type'])
                            param_type = man_type + " *"
                        print("/** \\memberof ", self.class_name, sep='', file=f)
                        print(" * Set the ", e['name'], " element", sep='', file=f)
                        print(" * \\param self - pointer to a ", self.class_name, sep='', file=f)
                        if e['type'] == "type_real" or e['type'] == "type_int":
                            print(" * \\param value - A pointer to the value to set or NULL to not include this element.", sep='', file=f)
                        else:
                            print(" * \\param value - A pointer to a \\a ", man_type, " to set.", sep='', file=f)
                        if return_type == "int":
                            print(" * \\return 0 for success", file=f)
                        print(" * \\sa ", self.class_name, "_get_", e['name'], sep='', file=f)
                        print(" */", file=f)

                        print(return_type, " ", self.class_name, "_set_", e['name'], "(", self.class_name, " *self, ", param_type, "value);", sep='', file=f)

                for e in self.children:
                    if e['type'] != "type_string" and e['type'] != "type_real" and e['type'] != "type_int":
                        print("/** \\memberof ", self.class_name, sep='', file=f)
                        print(" * Create a new ", e['name'], " element and insert it into the ", self.class_name, sep='', file=f)
                        print(" * \\param self - pointer to a ", self.class_name, sep='', file=f)
                        print(" * \\return A pointer to the newly created structure or NULL if memory allocation failed", file=f)
                        print(" */", file=f)

                        print(self.prefix_class(e['type']), " *", self.class_name, "_create_", e['name'], "(", self.class_name, " *self);", sep='', file=f)
                        if e.get('array', False):
                            print("int ", self.class_name, "_add_", e['name'], "(", self.class_name, " *self, ", self.prefix_class(e['type']), " *child);", sep='', file=f)
                            print("int ", self.class_name, "_remove_", e['name'], "(", self.class_name, " *self, int index);", sep='', file=f)


            print(file=f)
            print("#endif", file=f)

        os.chdir("private")
        with open(self.name + ".h", "w") as f:
            common.output_file = f
            common.print_license_string()
            print("#ifndef _", self.namespace.upper(), "_PRIVATE_", self.name.upper(), "_H", sep='', file=f)
            print("#define _", self.namespace.upper(), "_PRIVATE_", self.name.upper(), "_H", sep='', file=f)

            print(file=f)
            print("#include <libxml/xmlwriter.h>", file=f)
            print(file=f)

            included = [ 'type_string', 'type_real', 'type_int' ]
            if self.children:
                for e in self.children:
                    if e['type'] not in included:
                        included.append(e['type'])
                        print('#include <', self.namespaces[e['type']], '/private/', e['type'], '.h>', sep='', file=f)

            if self.extends and self.extends not in included:
                included.append(self.extends)
                print("#include <", self.namespaces[self.extends], '/private/', self.extends, ".h>", sep='', file=f)

            print(file=f)
            print("struct ", self.class_name, " {", sep='', file=f)

            if self.extends:
                print("\t", self.prefix_class(self.extends), " *base;", sep='', file=f)

            if self.attributes:
                for a in self.attributes:
                    if a['type'] == 'type_string':
                        print("\tchar *", a['name'], ";", sep='', file=f) 
                    elif a['type'] == 'type_int':
                        print("\tint *", a['name'], ";", sep='', file=f)
                        print("\tint ", a['name'], "_number;", sep='', file=f)

            if self.children:
                for e in self.children:
                    if e['type'] == 'type_string':
                        print("\tchar *", e['name'], ";", sep='', file=f)
                    elif e['type'] == 'type_real':
                        print("\tdouble *", e['name'], ";", sep='', file=f)
                        print("\tdouble ", e['name'], "_number;", sep='', file=f)
                    elif e['type'] == 'type_int':
                        print("\tint *", e['name'], ";", sep='', file=f)
                        print("\tint ", e['name'], "_number;", sep='', file=f)
                    else:
                        print("\t", self.prefix_class(e['type']), " *", sep='', end='', file=f)
                        if e.get('array', False):
                            print("*", end='', file=f)    
                        print(e['name'], ";", sep='', file=f)
                for e in self.children:
                    if e.get('array', False):
                        print("\tint num_", e['name'], ";", sep='', file=f)
                for e in self.children:
                    print("\tint in_", e['name'], ";", sep='', file=f)

            print("\tint reference_count;", file=f)
            if self.fields:
                for e in self.fields:
                    print("\t", e, sep='', file=f)
            print("};", file=f)
     
            print(file=f)
            print("int ", self.class_name, "_start_element(", self.class_name, " *self, const char *localname, int nb_attributes, const char **attributes);", sep='', file=f)
            print("void ", self.class_name, "_end_element(", self.class_name, " *self, const char *localname);", sep='', file=f)
            print("int ", self.class_name, "_characters(", self.class_name, " *self, const char *ch, int len);", sep='', file=f)
            if self.name in need_name:
                extra = ", char *element_name"
            else:
                extra = ""
            print("int ", self.class_name, "_xml(", self.class_name, " *self, xmlTextWriterPtr writer", extra, ");", sep='', file=f)
            if self.attributes:
                print("int ", self.class_name, "_init_attributes(", self.class_name, " *self, int nb_attributes, const char **attributes);", sep='', file=f)
            print(file=f)
            print("#endif", file=f)
        os.chdir("..")
