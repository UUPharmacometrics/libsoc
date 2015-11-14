#!/usr/bin/python3

# SO XML data binding for ISO C

import os
import sys
import shutil
import common
from structure import structure, need_name
from genclass import genclass

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

# Check for inheritance and add to need_name list
for name in structure:
    if 'extends' in structure[name]:
        need_name.append(name)

# Create code and headers
for name in structure:
    os.chdir("gen")
    genc = genclass(name, structure)
    genc.create_code()
    os.chdir("../include/so")
    genc.create_headers()
    os.chdir("../..")
