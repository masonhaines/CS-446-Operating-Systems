All: looped_sum threaded_sum

looped_sum: looped_sum.o
	gcc -pthread -g -o looped_sum looped_sum.o

looped_sum.o: looped_sum.c 
	gcc  -pthread -g -c looped_sum.c

threaded_sum: threaded_sum.o
	gcc -g -o threaded_sum threaded_sum.o -pthread

threaded_sum.o: threaded_sum.c 
	gcc -g -c threaded_sum.c -pthread

clean:
	rm *.o looped_sum threaded_sum