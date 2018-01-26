#stty raw
mpirun -np 2 -hostfile hosts ../ChangePointGA/Debug/ChangePointGA --preload-binary
#stty cooked
