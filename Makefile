GEN_SRCS := $(wildcard gen/*.c)
OBJS := $(GEN_SRCS:.c=.o)
OBJS_NODIR := $(notdir $(OBJS))
CODE := soext.c util.c Table.c column.c common_types.c Matrix.c estring.c
CODEOBJS := $(CODE:.c=.o)

CC := gcc
#CC := x86_64-w64-mingw32-gcc
CFLAGS := -std=c99 -pedantic -c -g -fpic -I. -Iinclude `xml2-config --cflags`
LIBS := -lxml2 

VPATH := gen

libsoc.so: $(OBJS_NODIR) $(CODEOBJS)
	$(CC) -shared -o libsoc.so $(OBJS_NODIR) soext.o util.o Table.o column.o common_types.o Matrix.o estring.o $(LIBS) -std=c99 -pedantic


soext.o: src/soext.c include/so/soext.h 
	$(CC) $(CFLAGS) src/soext.c

util.o: src/util.c include/so/private/util.h 
	$(CC) $(CFLAGS) src/util.c

Table.o: src/Table.c include/so/Table.h include/so/private/Table.h 
	$(CC) $(CFLAGS) src/Table.c

column.o: src/column.c include/so/private/column.h 
	$(CC) $(CFLAGS) src/column.c

common_types.o: src/common_types.c include/pharmml/common_types.h 
	$(CC) $(CFLAGS) src/common_types.c

Matrix.o: src/Matrix.c include/so/Matrix.h include/so/private/Matrix.h 
	$(CC) $(CFLAGS) src/Matrix.c

estring.o: src/estring.c include/so/estring.h include/so/private/estring.h 
	$(CC) $(CFLAGS) src/estring.c

gen/%.o: gen/%.c include/so/%.h include/so/private/%.h
	$(CC) $(CFLAGS) $<

#%.h: generator/generate.py
#	cd generator; python3 generate.py

.PHONY: clean

clean:
	rm -f libsoc.so
	rm -f *.o
	rm -rf R/src/include/
	rm -f R/src/static-*
	rm -f R/src/gen-*
	rm -f R/R/gen-*
	rm -f R/NAMESPACE
	rm -rf html/
	rm -rf latex/
	rm -f *.tar.gz
	rm -f doxygen_sqlite3.db
	cd generator; python3 generate.py clean

.PHONY: R

R:
	cd generator; python3 R.py
	cp -r include R/src/
	for FILE in $$(find src/ -name "*.c"); do cp "$$FILE" R/src/static-$$(basename "$$FILE"); done
	for FILE in $$(find gen/ -name "*.c"); do cp "$$FILE" R/src/static-$$(basename "$$FILE"); done
	R CMD build R

#generate:
#	python3 generate.py
