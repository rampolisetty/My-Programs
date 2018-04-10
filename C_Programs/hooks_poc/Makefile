# My Makefile

all: shared_lib_usage

shared_object_1.o: shared_object_1.c
	gcc -g -c -Wall -Werror -fpic shared_object_1.c

libshared_object_1.so: shared_object_1.o
	gcc -shared -o libshared_object_1.so shared_object_1.o

shared_object_2.o: shared_object_2.c
	gcc -g -c -Wall -Werror -fpic shared_object_2.c

libshared_object_2.so: shared_object_2.o
	gcc -shared -o libshared_object_2.so shared_object_2.o

shared_object_3.o: shared_object_3.c
	gcc -g -c -Wall -Werror -fpic shared_object_3.c

libshared_object_3.so: shared_object_3.o
	gcc -shared -o libshared_object_3.so shared_object_3.o

shared_lib_usage: shared_lib_usage.c libshared_object_1.so libshared_object_2.so libshared_object_3.so
	gcc -L./ -Wall -o shared_lib_usage shared_lib_usage.c -lshared_object_3 -lshared_object_2 -lshared_object_1

clean: 
	rm -f *.o
	rm -f *.so
	rm -f shared_lib_usage
