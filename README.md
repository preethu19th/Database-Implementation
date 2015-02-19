# COP-6726-Spring2015
[![Build Status](https://magnum.travis-ci.com/preethu19th/COP-6726-Spring2015.svg?token=kpbq8bmL5S9Txs63cGxQ&branch=master)](https://magnum.travis-ci.com/preethu19th/COP-6726-Spring2015)

To run the main program - this is used to run queries againt the tpch-dbgen pipe-delimited text file.

	make main
	./bin/main

To clean - this deletes the following folder bin, a1test

	make clean

To run the autotest program - This loads the tpch data into DBFile heap file, scans the lines in the DBFile and apply the cnf on the respective tables using static\_test\_data/cnf file, comparing the output with the expected output in static\_test\_data/expop\* files

	make autotest
	./bin/autotest

To run the a2-test program - This is used to test the BigQ class

	make test2
	./bin/test2

To run the a1-test program - this is used to load, scan, and scan & filter heap DBFiles interactively

	make a1-test
	./bin/a1-test

To run the unittests - this will run all the unittest written in googletest

	make unittest
	./bin/unittest

---------------------------------------------------------------------------------------
012408-1606P
--updated Lexer.l

readme 
=========
Test driver for Assignment 1 heap DBFile (spring 2011 dbi) 

This test driver gives a menu-based interface to three options that allows you to test your code:
	1. load (read a tpch file and write it out a heap DBFile)
	2. scan (read records from an existing heap DBFile)
	3. scan & filter (read records and filter using a CNF predicate)

Note that the driver only works with the tpch files (generated using the dbgen program). 

To compile the driver, type
	make test.out

To run the driver, type
	test.out
and follow the on-screen instructions.

Using the driver:
==================

1. SETTINGS: The following variables control the various file locations and they are declared in test.cc (just after the #include header declarations):
	o dbfile_dir -- this is where the created heap db-files will be stored. By default, this is set to "" (thus all the heap dbfiles will be created in the current directory).
	o tpch_dir -- this stores the directory path where the tpch-files can be found. The "/cise/tmp/dbi_sp15/DATA/" directory already has the required table files generated by TA and the path should be accessible by your program. There are two tpch-files: 10M and 1G. You can select any one to test. By default, tpch_dir is set to "/cise/tmp/dbi_sp15/DATA/10M". If you are running this driver from CISE linux machine, you donot have to change this setting. 
	o catalog_path -- this stores the catalog file path. By default this is set to "". Again, if you are running the driver from CISE linux machines you donot have to change this setting.

2. Next replace the stub files DBFile.h and DBFile.cc with your own code. If you are using additional headers, modify the Makefile to include them as well. Then compile and execute the driver. Select the load option and convert the tpch-files into heap dbfiles. By default, the heap file is written in the same directory as the executable and have the extension ".bin". 

3. Once load of a file has been selected, you can select option 2 or 3 to scan/filter all the records from the heap DBfile.  If option 3 is selected, a CNF should be supplied. Some example CNF's are given below. They are numbered q1,q2..q12. Use the table below to identify the tpch file associated with each CNF.
     	table    |   CNF
 ---------------------------------------
        region    |  q1 q2   
        nation    |  q3   
        supplier  |  q4 q5
        customer  |  q6
        part      |  q7   
        partsupp  |  q8 q9
        orders    |  q10                
        lineitem  |  q11 q12 

The expected output for these CNF's can be found in the file "output.log"

Example CNF's
================

q1 
(r_name = 'EUROPE')

q2 
(r_name < 'middle east') AND
(r_regionkey > 1)

q3 
(n_regionkey = 3) AND
(n_nationkey > 10) AND
(n_name > 'japan')

q4 
(s_suppkey < 10)

q5
(s_nationkey = 18) AND
(s_acctbal > 1000) AND
(s_suppkey < 400)

q6
(c_nationkey = 23) AND
(c_mktsegment = 'FURNITURE') AND
(c_acctbal > 7023.99) AND
(c_acctbal < 7110.83)


q7 
(p_brand = 'Brand#13') AND
(p_retailprice > 500) AND
(p_retailprice < 930) AND
(p_size > 28) AND
(p_size < 1000000)

q8 
(ps_supplycost > 999.98)

q9 
(ps_availqty < 10)
(ps_supplycost > 100) AND
(ps_suppkey < 300) AND

q10 
(o_orderpriority = '1-URGENT') AND
(o_orderstatus = '0') AND
(o_shippriority = 0) AND
(o_totalprice > 1015.68) AND
(o_totalprice < 1051.89)

q11
(l_shipdate > '1994-01-01') AND
(l_shipdate < '1994-01-07') AND
(l_discount > 0.05) AND
(l_discount < 0.06) AND
(l_quantity = 4) 


q12
(l_orderkey > 100) AND
(l_orderkey < 1000) AND
(l_partkey > 100) AND
(l_partkey < 5000) AND
(l_shipmode = 'AIR') AND
(l_linestatus = 'F') AND
(l_tax < 0.07)
