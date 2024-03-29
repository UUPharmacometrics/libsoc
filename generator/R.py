#!/usr/bin/python3

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

# Generate R bindings to libsoc
import os
import sys
import common
from structure import structure

functions = []

def get_R_type(so_type):
    if so_type == 'Table':
        is_type = 'data.frame'
    elif so_type == 'Matrix':
        is_type = 'matrix'
    elif so_type == 'type_string':
        is_type = 'character'
    elif so_type == 'type_real':
        is_type = 'numeric'
    elif so_type == 'type_int':
        is_type = 'integer'
    else:
        is_type = 'so_' + so_type
    return is_type

def print_new(name):
    func_name = "r_so_" + name + "_new"
    functions.append({'name' : func_name, 'numargs' : 0})
    print("SEXP ", func_name, "()", sep='')
    print("{")
    print("\tso_", name, " *obj = so_", name, "_new();", sep='')
    print("\tif (!obj) {")
    print("\t\terror(\"Failed to create so_", name, " object\");", sep='')
    print("\t}")
    print("\treturn R_MakeExternalPtr(obj, R_NilValue, R_NilValue);")
    print("}")

def print_copy(name):
    func_name = "r_so_" + name + "_copy"
    functions.append({'name' : func_name, 'numargs' : 1})
    print("SEXP ", func_name, "(SEXP self)", sep='')
    print("{")
    print("\tso_", name, " *obj = so_", name, "_copy(R_ExternalPtrAddr(self));", sep='')
    print("\tif (!obj) {")
    print("\t\terror(\"Failed to copy so_", name, " object\");", sep='')
    print("\t}")
    print("\treturn R_MakeExternalPtr(obj, R_NilValue, R_NilValue);")
    print("}")

def print_free(name):
    func_name = "r_so_" + name + "_free"
    functions.append({'name' : func_name, 'numargs' : 1})
    print("SEXP ", func_name, "(SEXP self)", sep='')
    print("{")
    print("\tso_", name, "_free(R_ExternalPtrAddr(self));", sep='')
    print("\treturn R_NilValue;")
    print("}")

def print_ref_unref(name):
    func_name = "r_so_" + name + "_ref"
    functions.append({'name' : func_name, 'numargs' : 1})
    print("SEXP ", func_name, "(SEXP self)", sep='')
    print("{")
    print("\tso_", name, "_ref(R_ExternalPtrAddr(self));", sep='')
    print("\treturn R_NilValue;")
    print("}")
    print()
    func_name = "r_so_" + name + "_unref"
    functions.append({'name' : func_name, 'numargs' : 1})
    print("SEXP ", func_name, "(SEXP self)", sep='')
    print("{")
    print("\tso_", name, "_unref(R_ExternalPtrAddr(self));", sep='')
    print("\treturn R_NilValue;")
    print("}")

def print_attribute_getter(name, attr):
    func_name = common.create_get_name(name, attr['name'], prefix="r_so")
    functions.append({'name' : func_name, 'numargs' : 1})
    print("SEXP ", func_name, "(SEXP self)", sep='')
    print("{")
    if attr['type'] == 'type_string':
        print("\tchar *value = ", common.create_get_name(name, attr['name'], prefix="so"), "(R_ExternalPtrAddr(self));", sep='')
    elif attr['type'] == 'type_int':
        print("\tint *value = ", common.create_get_name(name, attr['name'], prefix="so"), "(R_ExternalPtrAddr(self));", sep='')
    print()
    print("\tSEXP result;")
    print()
    print("\tif (value) {")
    if attr['type'] == 'type_string':
        print("\t\tresult = PROTECT(NEW_STRING(1));")
        print("\t\tSET_STRING_ELT(result, 0, mkChar(value));")
    elif attr['type'] == 'type_int':
        print("\t\tresult = PROTECT(NEW_INTEGER(1));")
        print("\t\tINTEGER(result)[0] = *value;")
    print()
    print("\t\tUNPROTECT(1);")
    print("\t} else {")
    print("\t\tresult = R_NilValue;")
    print("\t}")
    print("\treturn result;")
    print("}")

def print_attribute_setter(name, attr):
    func_name = common.create_set_name(name, attr['name'], prefix="r_so")
    functions.append({'name' : func_name, 'numargs' : 2})
    print("SEXP " + func_name + "(SEXP self, SEXP string)", sep='')
    print("{")
    if attr['type'] == 'type_string':
        print("\tchar *c_string = (char *) CHAR(STRING_ELT(string, 0));")
        print("\tint fail = ", common.create_set_name(name, attr['name'], prefix="so"), "(R_ExternalPtrAddr(self), c_string);", sep="")
        print("\tif (fail) {")
        print("\t\terror(\"", common.create_set_name(name, attr['name'], prefix="so"), " failed\");", sep='')
        print("\t}")
    elif attr['type'] == 'type_int':
        print("\tso_", name, "_set_", attr['name'], "(R_ExternalPtrAddr(self), INTEGER(string));", sep='')
    print()
    print("\treturn R_NilValue;")
    print("}")

def print_get_number_of(name, child, cls):
    func_name = "r_so_" + name + "_get_number_of_" + child
    functions.append({'name' : func_name, 'numargs' : 1})
    print("SEXP ", func_name, "(SEXP self)", sep='')
    print("{")
    print("\tint number = so_", name, "_get_number_of_", child, "(R_ExternalPtrAddr(self));", sep='')
    print("\tSEXP r_int = PROTECT(NEW_INTEGER(1));")
    print("\tINTEGER(r_int)[0] = number;")
    print("\tUNPROTECT(1);")
    print("\treturn r_int;")
    print("}")

def print_add_child(name, child):
    func_name = "r_so_" + name + "_add_" + child['name']
    functions.append({'name' : func_name, 'numargs' : 2})
    print("SEXP ", func_name, "(SEXP self, SEXP child)", sep='')
    print("{")
    if child['type'] == 'Table':
        print("\tint fail = so_", name, "_add_", child['name'], "(R_ExternalPtrAddr(self), df2table(child));", sep='')
    else:
        print("\tint fail = so_", name, "_add_", child['name'], "(R_ExternalPtrAddr(self), R_ExternalPtrAddr(child));", sep='')
    print("\tif (fail) {")
    print("\t\terror(\"Failed to add ", child['name'], " to ", name, "\");", sep='')
    print("\t}")
    print("\treturn(R_NilValue);")
    print("}")

def print_remove_child(name, child, cls):
    func_name = "r_so_" + name + "_remove_" + child
    functions.append({'name' : func_name, 'numargs' : 2})
    print("SEXP ", func_name, "(SEXP self, SEXP index)", sep='')
    print("{")
    print("\tint fail = so_", name, "_remove_", child, "(R_ExternalPtrAddr(self), INTEGER(index)[0]);", sep='')
    print("\tif (fail) {")
    print("\t\terror(\"Failed to remove ", child, " from ", name, "\");", sep='')
    print("\t}")
    print("\treturn(R_NilValue);")
    print("}")

def print_get_child(name, child):
    func_name = common.create_get_name(name, child['name'], prefix="r_so")
    numargs = 1
    print("SEXP ", func_name, "(SEXP self", sep ='', end='')
    if child.get("array", False):
        numargs += 1
        print(", SEXP index", end='')
    print(")")
    functions.append({'name' : func_name, 'numargs' : numargs})
    print("{")
    if child['type'] == "type_string":
        this_type = "char"
    elif child['type'] == "type_real":
        this_type = "double"
    elif child['type'] == "type_int":
        this_type = "int"
    else:
        this_type = "so_" + child['type']
    print("\t", this_type, " *child = ", common.create_get_name(name, child['name'], prefix="so"), "(R_ExternalPtrAddr(self)", sep='', end='')
    if child.get("array", False):
        print(", INTEGER(index)[0]", end='')
    print(");")
    if child['type'] == 'Table':
        print("\tSEXP result = table2df(child);")
    elif child['type'] == 'type_string':
        print("\tif (!child) {")
        print("\t\treturn R_NilValue;")
        print("\t}")
        print("\tSEXP result;")
        print("\tPROTECT(result = NEW_STRING(1));")
        print("\tSET_STRING_ELT(result, 0, mkChar(child));")
        print("\tUNPROTECT(1);")
    elif child['type'] == 'type_real':
        print("\tif (!child) {")
        print("\t\treturn R_NilValue;")
        print("\t}")
        print("\tSEXP result;")
        print("\tPROTECT(result = NEW_NUMERIC(1));")
        print("\tREAL(result)[0] = *child;")
        print("\tUNPROTECT(1);")
    elif child['type'] == 'type_int':
        print("\tif (!child) {")
        print("\t\treturn R_NilValue;")
        print("\t}")
        print("\tSEXP result;")
        print("\tPROTECT(result = NEW_INTEGER(1));")
        print("\tINTEGER(result)[0] = *child;")
        print("\tUNPROTECT(1);")
    elif child['type'] == 'Matrix':
        print("\tSEXP result = matrix2Rmatrix(child);")
    else:
        print("\tSEXP result = R_MakeExternalPtr(child, R_NilValue, R_NilValue);")
    print("\treturn result;")
    print("}")

def print_set_child(name, child):
    func_name = common.create_set_name(name, child['name'], prefix="r_so")
    functions.append({'name' : func_name, 'numargs' : 2})
    print("SEXP ", func_name, "(SEXP self, SEXP child)", sep='')
    print("{")
    if child['type'] == "Table":
        print("\tso_Table *table = df2table(child);", sep='')
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), table);", sep='')
    elif child['type'] == 'type_string':
        print("\tint fail = so_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), (char *) CHAR(STRING_ELT(child, 0)));", sep='')
        print("\tif (fail) {")
        print("\t\terror(\"so_", name, "_set_", child['name'], " failed\");", sep='')
        print("\t}")
    elif child['type'] == 'type_real':
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), REAL(child));", sep='')
    elif child['type'] == 'type_int':
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), INTEGER(child));", sep='')
    elif child['type'] == 'Matrix':
        print("\tso_Matrix *matrix = Rmatrix2matrix(child);", sep='')
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), matrix);", sep='')
    else:
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), R_ExternalPtrAddr(child));", sep='')
    print("\treturn R_NilValue;")
    print("}")

def print_create_child(name, child, cls):
    if cls != 'type_string' and cls != 'type_real' and cls != 'type_int':
        func_name = "r_so_" + name + "_create_" + child
        functions.append({'name' : func_name, 'numargs' : 1})
        print("SEXP ", func_name, "(SEXP self)", sep='')
        print("{")
        print("\tso_", cls, " *child = so_", name, "_create_", child, "(R_ExternalPtrAddr(self));", sep='')
        print("\treturn R_MakeExternalPtr(child, R_NilValue, R_NilValue);")
        print("}")

def print_includes():
    print("#include <R.h>")
    print("#include <Rdefines.h>")
    print("#include <so.h>")
    print("#include \"soc.h\"")

def print_wrapper_functions(name, struct):
    print("so_", name, "_new <- function() {", sep='')
    print("\tobj = .Call(\"r_so_", name, "_new\")", sep='')
    print("}")
    print()
    print("so_", name, "_copy <- function(self) {", sep='')
    print("\t.Call(\"r_so_", name, "_copy\", self)", sep='')
    print("}")
    print()
    print("so_", name, "_free <- function(self) {", sep='')
    print("\t.Call(\"r_so_", name, "_free\", self)", sep='')
    print("}")
    print()
    print("so_", name, "_ref <- function(self) {", sep='')
    print("\t.Call(\"r_so_", name, "_ref\", self)", sep='')
    print("}")
    print()
    print("so_", name, "_unref <- function(self) {", sep='')
    print("\t.Call(\"r_so_", name, "_unref\", self)", sep='')
    print("}")
    print()

    for attr in struct.get('attributes', []):
        print("so_", name, "_get_", attr['name'], " <- function(self) {", sep='')
        print("\t.Call(\"r_so_", name, "_get_", attr['name'], "\", self)", sep='')
        print("}")
        print()
        print("so_", name, "_set_", attr['name'], " <- function(self, value) {", sep='')
        print("\t.Call(\"r_so_", name, "_set_", attr['name'], "\", self, value)", sep='')
        print("}")
        print()

    if 'extends' in struct:
        print("so_", name, "_get_base <- function(self) {", sep='')
        print("\t.Call(\"r_so_", name, "_get_base\", self)", sep='')
        print("}")
        print()
        print("so_", name, "_set_base <- function(self, value) {", sep='')
        print("\t.Call(\"r_so_", name, "_set_base\", self, value)", sep='')
        print("}")
        print()

    if 'children' in struct:
        for child in struct['children']:
            print("so_", name, "_get_", child['name'], " <- function(self", sep='', end='')
            if child.get('array', False):
                print(", number", end='')
            print(") {")
            print("\t.Call(\"r_so_", name, "_get_", child['name'], "\", self", sep='', end='')
            if child.get('array', False):
                print(", number", end='')
            print(")")
            print("}")
            print()
            if child.get('array', False):
                print("so_", name, "_get_number_of_", child['name'], " <- function(self) {", sep='')
                print("\t.Call(\"r_so_", name, "_get_number_of_", child['name'], "\", self)", sep='')
                print("}")
                print("so_", name, "_add_", child['name'], " <- function(self, value) {", sep='')
                print("\t.Call(\"r_so_", name, "_add_", child['name'], "\", self, value)", sep='')
                print("}")
                print("so_", name, "_remove_", child['name'], " <- function(self, index) {", sep='')
                print("\t.Call(\"r_so_", name, "_remove_", child['name'], "\", self, index)", sep='')
                print("}")
            else:
                print("so_", name, "_set_", child['name'], " <- function(self, value) {", sep='')
                print("\t.Call(\"r_so_", name, "_set_", child['name'], "\", self, value)", sep='')
                print("}")
            print()
            if child['type'] != "type_string" and child['type'] != "type_real" and child['type'] != "type_int":
                print("so_", name, "_create_", child['name'], " <- function(self) {", sep='')
                print("\t.Call(\"r_so_", name, "_create_", child['name'], "\", self)", sep='')
                print("}")
                print()

def print_accessors(name, struct):
    # attributes
    for attr in struct.get('attributes', []):
        print(attr['name'], "_acc <- function(value)", sep='')
        print("{")
        print("\tif (!isnull(.self$.cobj)) {")
        print("\t\tif (missing(value)) {")
        print("\t\t\tso_", name, "_get_", attr['name'], "(.self$.cobj)", sep='')
        print("\t\t} else {")

        if attr['type'] == 'type_string':
            print("\t\t\tstopifnot(is.character(value), length(value) == 1)")
        elif attr['type'] == 'type_int':
            print("\t\t\tstopifnot(length(value) == 1)")
            print("\t\t\tvalue = as.integer(value)")

        print("\t\t\tso_", name, "_set_", attr['name'], "(.self$.cobj, value)", sep='')
        print("\t\t}")
        print("\t}")
        print("}")
        print()

    # base
    if 'extends' in struct:
        print("base_acc <- function(value)", sep='')
        print("{")
        print("\tif (!isnull(.self$.cobj)) {")
        print("\t\tif (missing(value)) {")
        print("\t\t\tso_", name, "_get_base(.self$.cobj)", sep='')
        print("\t\t} else {")
        print("\t\t\tif (!is(value, \"", struct['extends'], "\")) {", sep='')
        print("\t\t\t\tstop(\"base object must be a '", struct['extends'], "' object\")", sep='')
        print("\t\t\t}")
        print("\t\t\tso_", name, "_set_base(.self$.cobj, value)", sep='')
        print("\t\t\tso_", struct['extends'], "_ref(value$.cobj)", sep='')
        print("\t\t}")
        print("\t}")
        print("}")
        print()


    # children
    if 'children' in struct:
        for child in struct['children']:
            print(child['name'], "_acc <- function(value)", sep='')
            print("{")
            print("\tif (!isnull(.self$.cobj)) {")
            print("\t\tif (missing(value)) {")

            if child.get('array', False):
                print("\t\t\tn = so_", name, "_get_number_of_", child['name'], "(.self$.cobj)", sep='')
                print("\t\t\tif (n > 0) {")
                print("\t\t\t\ta = list()")
                print("\t\t\t\tfor (i in seq.int(1, n)) {")
                print("\t\t\t\t\tchild = so_", name, "_get_", child['name'], "(.self$.cobj, i - 1L)", sep='')
                if child['type'] == 'Table':
                    print("\t\t\t\t\ta[[i]] = child")
                else:
                    print("\t\t\t\t\ta[[i]] = so_", child['type'], "$new(cobj=child)", sep='')
                    print("\t\t\t\t\tso_", child['type'], "_ref(child)", sep='')
                print("\t\t\t\t}")
                print("\t\t\t\treturn(a)")
                print("\t\t\t}")
            elif child['type'] == 'Table' or child['type'] == 'type_string' or child['type'] == 'type_real' or child['type'] == 'type_int' or child['type'] == 'Matrix':
                print("\t\t\tso_", name, "_get_", child['name'], "(.self$.cobj)", sep='')
            else:
                print("\t\t\tchild = so_", name, "_get_", child['name'], "(.self$.cobj)", sep='')
                print("\t\t\tif (!isnull(child)) {")
                print("\t\t\t\tso_", child['type'], "_ref(child)", sep='')
                print("\t\t\t\tso_", child['type'], "$new(cobj=child)", sep='')
                print("\t\t\t}")

            print("\t\t} else {")

            R_type = get_R_type(child['type'])
            if child.get('array', False):
                pass
            elif child['type'] == 'Table' or child['type'] == 'Matrix' or child['type'] == 'type_string' or child['type'] == 'type_real' or child['type'] == 'type_int':
                print("\t\t\tif (!is(value, \"", R_type,"\")) {", sep='')
                print("\t\t\t\tstop(\"object must be of type '", R_type, "'\")", sep='')
                print("\t\t\t}")
                print("\t\t\tso_", name, "_set_", child['name'], "(.self$.cobj, value)", sep='')
            else:
                print("\t\t\tif (!is(value, \"", R_type,"\")) {", sep='')
                print("\t\t\t\tstop(\"object must be of type '", R_type, "'\")", sep='')
                print("\t\t\t}")
                print("\t\t\tso_", name, "_set_", child['name'], "(.self$.cobj, value$.cobj)", sep='')
                print("\t\t\tso_", child['type'], "_ref(value$.cobj)", sep='')

            print("\t\t}")
            print("\t}")
            print("}")
            print()

def print_classes(name, struct):
    print("so_", name, " = setRefClass(\"so_", name, "\",", sep='')
    print("\tfields=list(")
    for attr in struct.get('attributes', []):
        print("\t\t", attr['name'], " = ", attr['name'], "_acc,", sep='')
    if 'children' in struct:
        for child in struct['children']:
            print("\t\t", child['name'], " = ", child['name'], "_acc,", sep='')
    if 'extends' in struct:
        print("\t\tbase = base_acc,")
    print("\t\t.cobj = \"externalptr\"")
    print("\t),")
    print("\tmethods=list(")
    print("\t\tcopy = function() {")
    print("\t\t\tcopy = so_", name, "_copy(.self$.cobj)", sep='')
    print("\t\t\tso_", name, "$new(cobj=copy)", sep='')
    print("\t\t},")
    print("\t\tinitialize = function(cobj) {")
    print("\t\t\tif (missing(cobj)) {")
    print("\t\t\t\t.cobj <<- so_", name, "_new()", sep='')
    print("\t\t\t} else {")
    print("\t\t\t\t.cobj <<- cobj")
    print("\t\t\t}")
    print("\t\t},")
    print("\t\tfinalize = function() {")
    print("\t\t\tso_", name, "_unref(.self$.cobj)", sep='')
    print("\t\t}", end='')
    if 'children' in struct:
        for child in struct['children']:
            if child.get('array', False):
                print(",")
                print("\t\tadd_", child['name'], " = function(value) {", sep='')
                if child['type'] == 'Table':
                    print("\t\t\tinvisible(so_", name, "_add_", child['name'], "(.self$.cobj, value))", sep='')
                else:
                    print("\t\t\tso_", name, "_add_", child['name'], "(.self$.cobj, value$.cobj)", sep='')
                    print("\t\t\tinvisible(so_", child['type'], "_ref(value$.cobj))", sep='')
                print("\t\t},")
                print("\t\tremove_", child['name'], " = function(value, index) {", sep='')
                print("\t\t\tinvisible(so_", name, "_remove_", child['name'], "(.self$.cobj, index))", sep='')
                print("\t\t}", end='')
    print()
    print("\t)")
    print(")")
    ns = open("../NAMESPACE", "a")
    print("export(so_", name, ")", sep='', file=ns)

def print_documentation(name, struct):
    print("\\name{so_", name, "}", sep='')
    print("\\alias{so_", name, "}", sep='')
    print("\\title{so_", name, " reference class}", sep='')     # Special for "abstract" class (extends)
    print("\\description{")
    print("\tReference Class for the ", struct['xpath'], " element of a PharmML-SO data structure", sep='')
    print("}")
    print("\\section{Methods}{")
    print("so_", name, "$new() - Create a new empty so_", name, " object\\cr", sep='')
    if name == 'SO':        # Special case for write method
        print("so_", name, "$write(filename, pretty=TRUE) - Write an SO to file. Set pretty to FALSE to not get pretty printed xml\\cr", sep='')
    if 'children' in struct:
        for child in struct['children']:
            if child.get('array', False):
                print("so_", name, "$add_", child['name'], "(object) - Add a ", child['name'], "\\cr", sep='')
                print("so_", name, "$remove_", child['name'], "(object, i) - Remove the ", child['name'], " having index i\\cr", sep='')
    print("}")
    if 'children' in struct or 'attributes' in struct:
        print("\\section{Fields}{")
        if 'children' in struct:
            for child in struct['children']:
                print("$", child['name'], " - ", sep='', end='')
                if child['type'] == 'Table':
                    if child.get('array', False):
                        print("A list of data.frames\\cr")
                    else:
                        print("A data.frame\\cr")
                elif child['type'] == 'Matrix':
                    if child.get('array', False):
                        print("A list of matrices\\cr")
                    else:
                        print("A matrix\\cr")
                elif child['type'] == 'type_string':
                    if child.get('array', False):
                        print("A list of character strings\\cr")
                    else:
                        print("A character string\\cr")
                elif child['type'] == 'type_real':
                    if child.get('array', False):
                        print("A list of numerics\\cr")
                    else:
                        print("A numeric\\cr")
                elif child['type'] == 'type_int':
                    if child.get('array', False):
                        print("A list of integers\\cr")
                    else:
                        print("An integer\\cr")
                else:
                    if child.get('array', False):
                        print("A list of \link{so_", child['type'], "} objects\\cr", sep='')
                    else:
                        print("A \link{so_", child['type'], "} object\\cr", sep='')
        if 'attributes' in struct:
            for attr in struct['attributes']:
                if attr['type'] == 'type_string':
                    print("$", attr['name'], " - A character string attribute\\cr", sep='')
                elif attr['type'] == 'type_int':
                    print("$", attr['name'], " - An integer attribute\\cr", sep='')

        print("}")
    print("\\keyword{so_", name, "}", sep='')

# Generate C part of binding
os.chdir("../R/src")

for name in structure:
    if structure[name]['namespace'] == 'so':
        with open("gen-" + name + ".c", "w") as f:

            sys.stdout = f

            common.output_file = f
            common.print_license_string()
            print()
            print_includes()
            print()
            print_new(name)
            print()
            print_copy(name)
            print()
            print_free(name)
            print()
            print_ref_unref(name)
            print()

            if 'extends' in structure[name]:
                child = { 'name' : 'base', 'type' : structure[name]['extends'] }
                print_get_child(name, child)
                print_set_child(name, child)
            if 'children' in structure[name]:
                for child in structure[name]['children']:
                    print_get_child(name, child)
                    print()
                    if child.get('array', False):
                        print_get_number_of(name, child['name'], child['type'])
                        print_add_child(name, child)
                        print_remove_child(name, child['name'], child['type'])
                    else:
                        print_set_child(name, child)
                        print()
                    print_create_child(name, child['name'], child['type'])
                    print()

            for attr in structure[name].get('attributes', []):
                print_attribute_getter(name, attr)
                print()
                print_attribute_setter(name, attr)
                print()


# Generate R part of binding
os.chdir("../R")

with open("../NAMESPACE", "w") as ns:
    print("useDynLib(libsoc, .registration=TRUE)", file=ns)
    print("import(methods)", file=ns)
    print("export(so_SO_read)", file=ns)
    print("export(id_column)", file=ns)
    print("export(id_column_name)", file=ns)
    print("export(idv_column)", file=ns)
    print("export(idv_column_name)", file=ns)
    print("export(dv_column)", file=ns)
    print("export(dv_column_name)", file=ns)

with open("../src/libsoc_init.c", "w") as init:
    print("#include <R.h>", file=init)
    print("#include <Rinternals.h>", file=init)
    print("#include <R_ext/Rdynload.h>", file=init)
    print(file=init)
    for f in functions:
        print("SEXP ", f['name'], "(", file=init, sep='', end='')
        arguments = []
        for i in range(0, f['numargs']):
            arguments.append("SEXP arg" + str(i))
        if not arguments:
            arguments.append("void")
        print(",".join(arguments), ");", sep='', file=init)
    print(file=init)
    print("static const R_CallMethodDef c_symbols[] = {", file=init)
    for f in functions:
        print('\t{ "', f['name'], '", (DL_FUNC) &', f['name'], ', ', f['numargs'], ' },', file=init, sep='')
    print("\t{ NULL, NULL, 0 }", file=init)
    print("};", file=init)
    print(file=init)
    print("void R_init_libsoc(DllInfo *info)", file=init)
    print("{", file=init)
    print("\tR_registerRoutines(info, NULL, c_symbols, NULL, NULL);", file=init)
    print("\tR_useDynamicSymbols(info, TRUE);", file=init)
    print("}", file=init)

for name in structure:
    if structure[name]['namespace'] == 'so':
        with open("gen-" + name + ".R", "w") as f:
            sys.stdout = f

            common.output_file = f
            common.print_license_string(language="R")
            print()
            print_wrapper_functions(name, structure[name])
            print_accessors(name, structure[name])
            print_classes(name, structure[name])

# Print the class documentation
os.chdir("../man")
for name in structure:
    if structure[name]['namespace'] == 'so':
        if name != "SO":        # Fixed documentation for SO
            with open(name + ".Rd", "w") as f:
                sys.stdout = f
                print_documentation(name, structure[name])
