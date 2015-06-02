#set output 'cactus_SEQ.ps'
#set output 'matmul_2.ps'
#set output 'matmul_3.ps'
set style data lines
set xlabel "number iteration" offset -1,0
set xrange [0 : 30]
set ylabel "performance (microsec.)" offset 0,-1
set yrange [0 : 25378967]
set key left box
plot \
"EG_timePolychord_cactus.txt" t "Error Grid Simple",\
"HE_timePolychord_cactus.txt" t "Error Grid With Half Edge",\
"WQ_timePolychord_cactus.txt" t "Error With Queue Priority"