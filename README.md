# COP-6726-Spring2015

To run the main program - this is used to run queries againt the tpch-dbgen pipe-delimited text file.

	make main
	./bin/main

To run the test program - this is used to load, scan, and scan & filter heap DBFiles

	make test
	./bin/test

To run the unittests - this will run all the unittest written in googletest

	make unittest
	./bin/unittest
