#! /usr/local/cs/bin/gnuplot
#
set terminal png
set datafile separator ","

unset xtics
set xtics

set title "Throughput of synchronization mechanisms"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Throughput of program(op/s)"
set logscale y
set output 'lab2b_1.png'
set key left top

plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'list using spin locks' with linespoints lc rgb 'purple', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'list using mutrex locks' with linespoints lc rgb 'green'



 set title "Wait time for mutex locks and average operation timing"
set xlabel "Threads"
set logscale x 10
set ylabel "Time (ns)"
set logscale y 10
set output 'lab2b_2.png'
set key left top

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'average operation time' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'wait for lock time' with linespoints lc rgb 'blue'

set title "successful runs of unprotected and protected threads using multiple lists"

set logscale x 2
set xrange [0.75:]
set xlabel "# Threads"
set ylabel "# Iterations"
set logscale y 5

set output 'lab2b_3.png'
plot \
     "< grep 'list-id-m,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'mutex' with points lc rgb 'red', \
     "< grep 'list-id-s,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'spin' with points lc rgb 'blue', \
     "< grep 'list-id-none,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'unprotected' with points lc rgb 'green'

#####
# set title "Throughput of Mutex Lists w/ Partitions"
# set xlabel "# threads"
# set logscale x 2
# set ylabel "operation per second"
# set logscale y 10
# set output 'lab2b_4.png'

# # grep out only single threaded, un-protected, non-yield results
# plot \
#      "< grep 'list-none-m,[0-9]*,1000,1' lab2b_list.csv" using ($2):(1000000000/($7)) \
# 	title 'mutexes with 1 list' with linespoints lc rgb 'purple', \
     # "< grep 'list-none-m,[0-9]*,1000,1' lab2b_list.csv" using ($2):(1000000000/($7)) \
	# title 'list using mutrex locks' with linespoints lc rgb 'green'

set title "Throughput of Mutex Lists w/ Partitions"
set xlabel "# Threads"

set xrange [0.75:]
set ylabel "operations/seccond"
set logscale y 10
set key right top
set output 'lab2b_4.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'blue', \
	 "< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'green', \
	 "< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'black', \
     "< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'purple'

set title "Throughput of Sync Lists w/ Partitions"
set xlabel "# Threads"

set xrange [0.75:]
set ylabel "operations/seccond"
set logscale y 10
set key right top
set output 'lab2b_5.png'

plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'blue', \
	 "< grep 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'green', \
	 "< grep 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'black', \
     "< grep 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'purple'    