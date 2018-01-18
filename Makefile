OBJS = ga/generation.o ga/stategy.o ga/operators/mutation.o ga/operators/fitness.o ga/operators/crossover.o ga/operators/selection.o model/londontrack18.o model/escorpio17.o model/map.o model/simulation.o main.o 
EXNAME = ChangePointGA
CFLAGS = -fopenmp -O1 -std=gnu11
LDLIBS = -lm

.PHONY: depend clean cleanAll

compile: depend $(OBJS)
	gcc -o $(EXNAME) $(OBJS) $(CFLAGS) $(LDLIBS)

-include depend.txt

depend:
	gcc -MM *.c > depend.txt

clean:
	rm -rf *.o *~

cleanAll: clean
	rm -f $(EXNAME)

