#set output 'cactus_SEQ.ps'
#set output 'matmul_2.ps'
#set output 'matmul_3.ps'
set style data lines
set xlabel "number iteration" offset -1,0
set xrange [0 : 30]
set ylabel "performance (microsec.)" offset 0,-1
set yrange [0 : 9271962]
set key left box
plot \
"EG_timePolychord_DINO3.txt" t "Error Grid Simple",\
"HE_timePolychord_DINO3.txt" t "Error Grid With Half Edge",\
"WQ_timePolychord_DINO3.txt" t "Error With Queue Priority"
