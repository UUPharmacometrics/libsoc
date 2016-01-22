GEN_SRCS := $(wildcard gen/*.c)
OBJS := $(GEN_SRCS:.c=.o)
OBJS_NODIR := $(notdir $(OBJS))
CODE := soext.c SOBlock_ext.c Table.c column.c common_types.c Matrix.c string.c
CODEOBJS := $(CODE:.c=.o)

CC := gcc
CFLAGS := -std=c99 -Wall -pedantic -c -g -fpic -I. -Iinclude `xml2-config --cflags`
#CFLAGS := -std=c99 -pedantic -c -g -fpic -I. -Iinclude
#CC := x86_64-w64-mingw32-gcc
LIBS := -lxml2 

VPATH := gen

libsoc.so: $(OBJS_NODIR) $(CODEOBJS)
	$(CC) -shared -o libsoc.so $(OBJS_NODIR) soext.o SOBlock_ext.o Table.o column.o common_types.o Matrix.o string.o $(LIBS) -std=c99 -pedantic


soext.o: src/soext.c include/so/soext.h 
	$(CC) $(CFLAGS) src/soext.c

SOBlock_ext.o: src/SOBlock_ext.c include/so/SOBlock_ext.h 
	$(CC) $(CFLAGS) src/SOBlock_ext.c

Table.o: src/Table.c include/so/Table.h include/so/private/Table.h 
	$(CC) $(CFLAGS) src/Table.c

column.o: src/column.c include/so/private/column.h 
	$(CC) $(CFLAGS) src/column.c

common_types.o: src/common_types.c include/pharmml/common_types.h 
	$(CC) $(CFLAGS) src/common_types.c

string.o: src/string.c include/pharmml/string.h 
	$(CC) $(CFLAGS) src/string.c

Matrix.o: src/Matrix.c include/so/Matrix.h include/so/private/Matrix.h 
	$(CC) $(CFLAGS) src/Matrix.c

gen/%.o: gen/%.c include/so/%.h include/so/private/%.h
	$(CC) $(CFLAGS) $<

#%.h: generator/generate.py
#	cd generator; python3 generate.py

.PHONY: doc
doc:
	doxygen

.PHONY: utils
utils:
	cd utils; $(CC) sotool.c -osotool `xml2-config --libs --cflags`

.PHONY: install_utils
install_utils:
	cp utils/sotool /usr/bin
	mkdir -p /usr/share/libsoc
	cp -rf utils/schema/* /usr/share/libsoc

.PHONY: clean
clean:
	rm -f libsoc.so
	rm -f *.o
	rm -rf R/src/include/
	rm -f R/src/static-*
	rm -f R/src/gen-*
	rm -f R/R/gen-*
	rm -rf soc.Rcheck
	rm -f R/NAMESPACE
	rm -rf html/
	rm -rf latex/
	rm -f *.tar.gz
	rm -f doxygen_sqlite3.db
	cd generator; python3 generate.py clean
	rm -rf generator/__pycache__
	rm -f fortran/libsoc.mod
	rm -f fortran/libsoc.f03
	rm -f fortran/test
	rm -f utils/sotool

.PHONY: R
R:
	cd generator; python3 R.py
	cp -r include R/src/
	for FILE in $$(find src/ -name "*.c"); do cp "$$FILE" R/src/static-$$(basename "$$FILE"); done
	for FILE in $$(find gen/ -name "*.c"); do cp "$$FILE" R/src/static-$$(basename "$$FILE"); done
	R CMD build R

#Fetch and compile R package dependencies for windows
.PHONY: R-windep
R-windep:
	rm -rf R-windep
	mkdir -p R-windep
	mkdir -p R-windep/include
	mkdir -p R-windep/lib
	wget -P R-windep http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz
	cd R-windep; tar xvfz libiconv-1.14.tar.gz
	cd R-windep/libiconv-1.14/;./configure --host=x86_64-w64-mingw32;make
	cp R-windep/libiconv-1.14/include/iconv.h R-windep/include
	cp R-windep/libiconv-1.14/lib/.libs/libiconv-2.dll R-windep/lib
	cp R-windep/libiconv-1.14/lib/.libs/libiconv.dll.a R-windep/lib
	cp R-windep/libiconv-1.14/lib/libcharset.dll.a R-windep/lib
	cp R-windep/libiconv-1.14/libcharset/lib/.libs/libcharset-1.dll R-windep/lib
	wget -P R-windep http://zlib.net/zlib-1.2.8.tar.gz
	cd R-windep; tar xvfz zlib-1.2.8.tar.gz
	cd R-windep/zlib-1.2.8/win32; sed -i 's/PREFIX =/PREFIX = x86_64-w64-mingw32-/' Makefile.gcc
	cd R-windep/zlib-1.2.8; make -f win32/Makefile.gcc
	cp R-windep/zlib-1.2.8/zconf.h R-windep/include
	cp R-windep/zlib-1.2.8/zlib.h R-windep/include
	cp R-windep/zlib-1.2.8/zlib1.dll R-windep/lib
	cp R-windep/zlib-1.2.8/libz.dll.a R-windep/lib
	wget -P R-windep ftp://xmlsoft.org/libxml2/libxml2-2.9.3.tar.gz
	cd R-windep; tar xvfz libxml2-2.9.3.tar.gz
	cd R-windep/libxml2-2.9.3; ./configure --host=x86_64-w64-mingw32 --without-python --without-docbook --without-ftp --without-http --without-schematron --with-lzma=no --with-zlib=/home/rikard/moose/soc/R-windep --with-iconv=/home/rikard/moose/soc/R-windep; make 
#	--without-html --without-legacy --without-regexps --without-sax1 --without-schemas --without-valid --without-xpath 
	cp -r R-windep/libxml2-2.9.3/include/libxml R-windep/include
	cp R-windep/libxml2-2.9.3/.libs/libxml2-2.dll R-windep/lib
	cp R-windep/libxml2-2.9.3/.libs/libxml2.dll.a R-windep/lib
	mkdir -p R/libs/x64
	cp R-windep/lib/* R/libs/x64
	cp -r R-windep/include R/src/wininclude

#generate:
#	python3 generate.py
