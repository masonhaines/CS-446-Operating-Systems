all: looped_sum threaded_sum

looped_sum: looped_sum.o
    gcc -o looped_sum looped_sum.o

looped_sum.o: looped_sum.c 
    gcc -c looped_sum.c

threaded_sum: threaded_sum.o
    gcc -o threaded_sum threaded_sum.o -pthread

threaded_sum.o: threaded_sum.c 
    gcc -c threaded_sum.c

clean:
    rm -f *.o looped_sum threaded_sum
