CC=gcc -g
CFLAGS=-I lib
INC= src/hcu_queue.o src/hcu_queue_test.o src/hcu_driver.o
LDFLAGS = -pthread 

hcu:$(INC)
	$(CC) $(LDFLAGS) -o hcu src/hcu_queue.o src/hcu_driver.o

test:$(INC)
	 $(CC) $(LDFLAGS) -o hcu_test src/hcu_queue.o src/hcu_queue_test.o   
 
clean:	rm -r hcu*
