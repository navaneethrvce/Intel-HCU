CC=gcc -g
CFLAGS=-I lib
INC= src/hcu_queue.o src/hcu_queue_test.o src/hcu_driver.o src/hcu_hash.o src/hcu_hash_test.o
LDFLAGS = -pthread
LDLIBS = -lssl -lcrypto

hcu:$(INC)
	$(CC) $(LDFLAGS) -o hcu src/hcu_queue.o src/hcu_driver.o src/hcu_hash.o $(LDLIBS) 

queue_test:$(INC)
	 $(CC) $(LDFLAGS) -o hcu_queue_test src/hcu_queue.o src/hcu_queue_test.o

hash_test:$(INC)
	 $(CC) $(LDFLAGS) -o hcu_hash_test src/hcu_hash.o src/hcu_hash_test.o $(LDLIBS)

clean:
	rm -rf hcu*
	rm -rf src/*.o	
