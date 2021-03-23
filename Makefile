CC = gcc
CFLAG = -g -std=c99 -lm

oss: oss.o
	$(CC) $(CFLAG) $< -o $@

clean: 
	rm -f *.o oss
