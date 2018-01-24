#stty raw
mpirun -np 8 -hostfile hosts -output-filename proc/logs.txt -timestamp-output ./ChangePointGA_MS
#stty cooked
