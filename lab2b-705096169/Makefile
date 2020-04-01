#NAME: HENRY MACARTHUR
#EMAIL: HENRYMAC16@GMAIL.COM
#ID: 705096169
default: 
	gcc -lpthread -g -lprofiler -Wall -Wextra SortedList.c lab2_list.c -o lab2_list

profile:
	#LD_PRELOAD=/u/cs/ugrad/macarthu/lib/lib/libprofiler.so.0
	CPUPROFILE=./output_data.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./output_data.gperf > profile.out
	pprof --list=helperfunc ./lab2_list ./output_data.gperf >> profile.out
 
 dist:	graphs profile
	tar -czvf lab2b-705096169.tar.gz *.png *.c *.csv Makefile README *.out *.h lab2b_list.gp

 clean:
	rm -rf lab2_list
	rm -rf lab2b-705096169.tar.gz
	rm -rf *.gperf

tests:
	./lab2_list --iterations=1000 --threads=1 --sync=s > lab2b_list.csv
	./lab2_list --iterations=1000 --threads=2 --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=4 --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=8 --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=16 --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=24 --sync=s >> lab2b_list.csv

	./lab2_list --iterations=1000 --threads=1 --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=2 --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=4 --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=8 --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=16 --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=24 --sync=m >> lab2b_list.csv

	####plot 1 


	####plot 2

	###3 
	-./lab2_list --threads=1 --iterations=1  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=2  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=4  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=8  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=16  --lists=4 --yield=id  >> lab2b_list.csv

	-./lab2_list --threads=4 --iterations=1  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=2  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=4  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=8  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=16  --lists=4 --yield=id  >> lab2b_list.csv

	-./lab2_list --threads=8 --iterations=1  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=2  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=4  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=8  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=16  --lists=4 --yield=id  >> lab2b_list.csv

	-./lab2_list --threads=12 --iterations=1  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --lists=4 --yield=id  >> lab2b_list.csv

	-./lab2_list --threads=16 --iterations=1  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2  --lists=4 --yield=id >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8  --lists=4 --yield=id  >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16  --lists=4 --yield=id  >> lab2b_list.csv

	./lab2_list --threads=1 --iterations=10  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=20  --lists=4 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=40  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=80  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	#./lab2_list --threads=1 --iterations=16  --lists=4 --yield=id  --sync=s >> lab2b_list.csv

	./lab2_list --threads=4 --iterations=10  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=20  --lists=4 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=40  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=80  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	#./lab2_list --threads=4 --iterations=16  --lists=4 --yield=id --sync=s  >> lab2b_list.csv

	./lab2_list --threads=8 --iterations=10  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=20  --lists=4 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=40  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=80  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	#./lab2_list --threads=8 --iterations=16  --lists=4 --yield=id  --sync=s >> lab2b_list.csv

	./lab2_list --threads=12 --iterations=10  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20  --lists=4 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	#./lab2_list --threads=12 --iterations=16  --lists=4 --yield=id  --sync=s >> lab2b_list.csv

	./lab2_list --threads=16 --iterations=10  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20  --lists=4 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80  --lists=4 --yield=id  --sync=s>> lab2b_list.csv


	./lab2_list --threads=1 --iterations=10  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=20  --lists=4 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=40  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=80  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	#./lab2_list --threads=1 --iterations=16  --lists=4 --yield=id  --sync=m >> lab2b_list.csv

	./lab2_list --threads=4 --iterations=10  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=20  --lists=4 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=40  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=80  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	#./lab2_list --threads=4 --iterations=16  --lists=4 --yield=id --sync=m  >> lab2b_list.csv

	./lab2_list --threads=8 --iterations=10  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=20  --lists=4 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=40  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=80  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	#./lab2_list --threads=8 --iterations=16  --lists=4 --yield=id  --sync=m >> lab2b_list.csv

	./lab2_list --threads=12 --iterations=10  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20  --lists=4 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	#./lab2_list --threads=12 --iterations=16  --lists=4 --yield=id  --sync=m >> lab2b_list.csv

	./lab2_list --threads=16 --iterations=10  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20  --lists=4 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80  --lists=4 --yield=id  --sync=m >> lab2b_list.csv

	# ./lab2_list --iterations=1000 --sync=m --lists=1 --threads=1 >>lab2b_list.csv
	# ./lab2_list --iterations=1000 --sync=m --lists=1 --threads=2 >>lab2b_list.csv
	# ./lab2_list --iterations=1000 --sync=m --lists=1 --threads=4 >>lab2b_list.csv
	# ./lab2_list --iterations=1000 --sync=m --lists=1 --threads=8 >>lab2b_list.csv
	# ./lab2_list --iterations=1000 --sync=m --lists=1 --threads=12 >>lab2b_list.csv

	./lab2_list --iterations=1000 --sync=m --lists=2 --threads=1 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=2 --threads=2 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=2 --threads=4 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=2 --threads=8 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=2 --threads=12 >>lab2b_list.csv

	./lab2_list --iterations=1000 --sync=m --lists=4 --threads=1 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=4 --threads=2 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=4 --threads=4 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=4 --threads=8 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=4 --threads=12 >>lab2b_list.csv

	./lab2_list --iterations=1000 --sync=m --lists=8 --threads=1 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=8 --threads=2 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=8 --threads=4 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=8 --threads=8 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=8 --threads=12 >>lab2b_list.csv

	./lab2_list --iterations=1000 --sync=m --lists=16 --threads=1 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=16 --threads=2 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=16 --threads=4 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=16 --threads=8 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=16 --threads=12 >>lab2b_list.csv

	# ./lab2_list --iterations=1000 --sync=s --lists=1 --threads=1 >>lab2b_list.csv
	# ./lab2_list --iterations=1000 --sync=s --lists=1 --threads=2 >>lab2b_list.csv
	# ./lab2_list --iterations=1000 --sync=s --lists=1 --threads=4 >>lab2b_list.csv
	# ./lab2_list --iterations=1000 --sync=s --lists=1 --threads=8 >>lab2b_list.csv
	# ./lab2_list --iterations=1000 --sync=s --lists=1 --threads=12 >>lab2b_list.csv

	./lab2_list --iterations=1000 --sync=s --lists=2 --threads=1 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=2 --threads=2 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=2 --threads=4 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=2 --threads=8 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=2 --threads=12 >>lab2b_list.csv

	./lab2_list --iterations=1000 --sync=s --lists=4 --threads=1 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=4 --threads=2 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=4 --threads=4 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=4 --threads=8 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=4 --threads=12 >>lab2b_list.csv

	./lab2_list --iterations=1000 --sync=s --lists=8 --threads=1 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=8 --threads=2 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=8 --threads=4 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=8 --threads=8 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=8 --threads=12 >>lab2b_list.csv

	./lab2_list --iterations=1000 --sync=s --lists=16 --threads=1 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=16 --threads=2 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=16 --threads=4 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=16 --threads=8 >>lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=16 --threads=12 >>lab2b_list.csv

 graphs: default tests 
	

	# ./lab2_list --threads=1 --iterations=1  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=1 --iterations=2  --lists=4 --yield=id --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=1 --iterations=4  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=1 --iterations=8  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=1 --iterations=16  --lists=4 --yield=id  --sync=m >> lab2b_list.csv

	# ./lab2_list --threads=4 --iterations=1  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=4 --iterations=2  --lists=4 --yield=id --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=4 --iterations=4  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=4 --iterations=8  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=4 --iterations=16  --lists=4 --yield=id --sync=m  >> lab2b_list.csv

	# ./lab2_list --threads=8 --iterations=1  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=8 --iterations=2  --lists=4 --yield=id --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=8 --iterations=4  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=8 --iterations=8  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=8 --iterations=16  --lists=4 --yield=id  --sync=m >> lab2b_list.csv

	# ./lab2_list --threads=12 --iterations=1  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=12 --iterations=2  --lists=4 --yield=id --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=12 --iterations=4  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=12 --iterations=8  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=12 --iterations=16  --lists=4 --yield=id  --sync=m >> lab2b_list.csv

	# ./lab2_list --threads=16 --iterations=1  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=16 --iterations=2  --lists=4 --yield=id --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=16 --iterations=4  --lists=4 --yield=id  --sync=m >> lab2b_list.csv
	# ./lab2_list --threads=16 --iterations=16  --lists=4 --yield=id  --sync=m >> lab2b_list.csv

	# ./lab2_list --threads=1 --iterations=1  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=1 --iterations=2  --lists=4 --yield=id --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=1 --iterations=4  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=1 --iterations=8  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=1 --iterations=16  --lists=4 --yield=id  --sync=s >> lab2b_list.csv

	# ./lab2_list --threads=4 --iterations=1  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=4 --iterations=2  --lists=4 --yield=id --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=4 --iterations=4  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=4 --iterations=8  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=4 --iterations=16  --lists=4 --yield=id --sync=s  >> lab2b_list.csv

	# ./lab2_list --threads=8 --iterations=1  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=8 --iterations=2  --lists=4 --yield=id --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=8 --iterations=4  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=8 --iterations=8  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=8 --iterations=16  --lists=4 --yield=id  --sync=s >> lab2b_list.csv

	# ./lab2_list --threads=12 --iterations=1  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=12 --iterations=2  --lists=4 --yield=id --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=12 --iterations=4  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=12 --iterations=8  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=12 --iterations=16  --lists=4 --yield=id  --sync=s >> lab2b_list.csv

	# ./lab2_list --threads=16 --iterations=1  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=16 --iterations=2  --lists=4 --yield=id --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=16 --iterations=4  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	# ./lab2_list --threads=16 --iterations=16  --lists=4 --yield=id  --sync=s >> lab2b_list.csv
	


	./lab2b_list.gp
	


