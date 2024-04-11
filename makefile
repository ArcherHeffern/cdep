CC=gcc
CCFLAGS=-Wall

deployee: deployee.c strutil.c
	$(CC) $(CCFLAGS) -o deployee deployee.c strutil.c

debug: 
	$(CC) -g $(CCFLAGS) -o deployee deployee.c strutil.c

clean:
	rm deployee
