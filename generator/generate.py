# libsoc - Library to handle standardised output files
# Copyright (C) 2015 Rikard Nordgren
# 
# This file was autogenerated and should not be edited
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
        try:
            os.remove("../include/so/" + name + ".h")
            os.remove("../include/so/private/" + name + ".h")
        except:
            pass
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
