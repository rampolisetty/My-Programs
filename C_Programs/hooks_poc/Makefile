# My Makefile

all: shared_lib_usage
	@echo ""
	@echo ""
	@echo "After running make, make sure to set LD_LIBRARY_PATH"
	@echo ""

shared_object_1.o: shared_object_1.c
	@gcc -g -c -Wall -Werror -fpic shared_object_1.c

libshared_object_1.so: shared_object_1.o
	@gcc -shared -o libshared_object_1.so shared_object_1.o

shared_object_2.o: shared_object_2.c
	@gcc -g -c -Wall -Werror -fpic shared_object_2.c

libshared_object_2.so: shared_object_2.o
	@gcc -shared -o libshared_object_2.so shared_object_2.o

shared_object_3.o: shared_object_3.c
	@gcc -g -c -Wall -Werror -fpic shared_object_3.c

libshared_object_3.so: shared_object_3.o
	@gcc -shared -o libshared_object_3.so shared_object_3.o

static_object_1.o: static_object_1.c
	@gcc -g -c static_object_1.c -o static_object_1.o

libstatic_object_1.a: static_object_1.o
	@ar rcs libstatic_object_1.a static_object_1.o

static_object_2.o: static_object_2.c
	@gcc -g -c static_object_2.c -o static_object_2.o

libstatic_object_2.a: static_object_2.o
	@ar rcs libstatic_object_2.a static_object_2.o

shared_lib_usage: shared_lib_usage.c libshared_object_1.so \
    		  libshared_object_2.so libshared_object_3.so \
		  libstatic_object_1.a libstatic_object_2.a
	@gcc -g -L./ -Wall -o shared_lib_usage shared_lib_usage.c \
	  -lshared_object_3 -lshared_object_2 -lshared_object_1 \
	  -lstatic_object_1 -lstatic_object_2

clean: 
	@rm -f *.o
	@rm -f *.a
	@rm -f *.so
	@rm -f shared_lib_usage

