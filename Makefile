SOC_GENSRCS := Bayesian.c Bayesian_PPE.c DiagnosticIndividualParams.c DiagnosticStructuralModel.c Estimates.c Estimation.c ExternalFile.c IndividualEstimates.c \
	InformationCriteria.c Message.c MissingData.c MLE.c ModelDiagnostic.c OFMeasures.c OptimalDesignBlock.c OptimalDesign.c OtherMethod.c \
   	OtherMethod_PPE.c PharmMLRef.c PopulationEstimates.c PrecisionIndividualEstimates.c PrecisionPopulationEstimates.c RandomEffects_IE.c \
   	RawResults.c Residuals.c SimulationBlock.c Simulation.c SimulationSubType.c SOBlock.c SO.c TargetToolMessages.c TaskInformation.c ToolSettings.c
SOC_GENOBJS := $(SOC_GENSRCS:.c=.o)

SOC_SRCS := soext.c SOBlock_ext.c Table.c column.c common_types.c Matrix.c string.c
SOC_OBJS := $(SOC_SRCS:.c=.o)

CC := gcc
CFLAGS := -std=c99 -Wall -pedantic -c -g -fpic -I. -Iinclude `xml2-config --cflags`
#CFLAGS := -std=c99 -pedantic -c -g -fpic -I. -Iinclude
#CC := x86_64-w64-mingw32-gcc
LIBS := -lxml2 

VPATH := gen

libsoc.so: $(SOC_GENOBJS) $(SOC_OBJS)
	$(CC) -shared -o libsoc.so $(SOC_GENOBJS) $(SOC_OBJS) $(LIBS) -std=c99 -pedantic

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
	rm -rf libsoc.Rcheck
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
	for FILE in $(addprefix gen/, $(SOC_GENSRCS)); do cp "$$FILE" R/src/static-$$(basename "$$FILE"); done
	for FILE in $(addprefix src/, $(SOC_SRCS)); do cp "$$FILE" R/src/static-$$(basename "$$FILE"); done
	R CMD build R

#Fetch and compile dependencies for windows
.PHONY: windep
windep:
	rm -rf windep
	mkdir -p windep
	mkdir -p windep/include
	mkdir -p windep/lib
	wget -P windep http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz
	cd windep; tar xvfz libiconv-1.14.tar.gz
	cd windep/libiconv-1.14/;./configure --host=x86_64-w64-mingw32;make
	cp windep/libiconv-1.14/include/iconv.h windep/include
	cp windep/libiconv-1.14/lib/.libs/libiconv-2.dll windep/lib
	cp windep/libiconv-1.14/lib/.libs/libiconv.dll.a windep/lib
	cp windep/libiconv-1.14/lib/libcharset.dll.a windep/lib
	cp windep/libiconv-1.14/libcharset/lib/.libs/libcharset-1.dll windep/lib
	wget -P windep http://zlib.net/zlib-1.2.8.tar.gz
	cd windep; tar xvfz zlib-1.2.8.tar.gz
	cd windep/zlib-1.2.8/win32; sed -i 's/PREFIX =/PREFIX = x86_64-w64-mingw32-/' Makefile.gcc
	cd windep/zlib-1.2.8; make -f win32/Makefile.gcc
	cp windep/zlib-1.2.8/zconf.h windep/include
	cp windep/zlib-1.2.8/zlib.h windep/include
	cp windep/zlib-1.2.8/zlib1.dll windep/lib
	cp windep/zlib-1.2.8/libz.dll.a windep/lib
	wget -P windep ftp://xmlsoft.org/libxml2/libxml2-2.9.3.tar.gz
	cd windep; tar xvfz libxml2-2.9.3.tar.gz
	cd windep/libxml2-2.9.3; ./configure --host=x86_64-w64-mingw32 --without-python --without-docbook --without-ftp --without-http --without-schematron --with-lzma=no --with-zlib=/home/rikard/moose/soc/windep --with-iconv=/home/rikard/moose/soc/windep; make 
#	--without-html --without-legacy --without-regexps --without-sax1 --without-schemas --without-valid --without-xpath 
	cp -r windep/libxml2-2.9.3/include/libxml windep/include
	cp windep/libxml2-2.9.3/.libs/libxml2-2.dll windep/lib
	cp windep/libxml2-2.9.3/.libs/libxml2.dll.a windep/lib

#generate:
#	python3 generate.py
