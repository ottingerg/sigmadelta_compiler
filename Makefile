CC=gcc
CFLAGS=-c -Wall

all: sigmadelta_func_test sigmadelta_func_testresults.csv

sigmadelta_func_test: sigmadelta_func.c 
	$(CC) sigmadelta_func_test.c sigmadelta_func.c -o sigmadelta_func_test -lm

sigmadelta_compiler: 
	$(CC) sigmadelta_compiler.c -o sigmadelta_compiler

sigmadelta_func.c: sigmadelta_compiler
	./sigmadelta_compiler > sigmadelta_func.c

sigmadelta_func_testresults.csv: sigmadelta_func_test
	./sigmadelta_func_test > sigmadelta_func_testresults.csv 

clean:
	rm sigmadelta_compiler sigmadelta_func.c sigmadelta_func_test
