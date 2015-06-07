#set output 'cactus_SEQ.ps'
#set output 'matmul_2.ps'
#set output 'matmul_3.ps'
set style data lines
set xlabel "number iteration" offset -1,0
set xrange [0 : 70]
set ylabel "id polychord" offset 0,-1
set yrange [0 : 70]
set key left box
plot \
"EG_seqPolychord_Loctopus.txt" t "Error Grid Simple",\
"HE_seqPolychord_Loctopus.txt" t "Error Grid With Half Edge",\
"WQ_seqPolychord_Loctopus.txt" t "Error With Queue Priority"
