***NOTE: This package is no longer maintained. It will no longer be available on CRAN. Please open an issue if you are interested in taking on the maintenance***

The standard output c library (libsoc) is a portable c library to create, write, read and manipulate PharmML-SO files from C. 

libsoc uses a SAX parser to parse SO-files and creates an object tree structure in memory to be able to manipulate the data. All manipulations of
the data structure are done through function calls so writing bindings to other languages should be easy.

Dependencies:
libsoc uses libxml2 to read and write XML. To build the libxml2-dev must be installed.


Bindings to R, in the form of an R package, are available.

To build:
``` 
cd generator
python3 generate.py
cd ..
make
```

To build documentation:
```
make doc
```

To build R source package:
```
make R
```
