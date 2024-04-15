all: pthread.c
	gcc -o pthread10k -pthread pthread10k.c
	gcc -o pthread100k -pthread pthread100k.c
	gcc -o pthread500k -pthread pthread500k.c
	gcc -o pthread1m -pthread pthread1m.c
clean:
	$(RM) pthread10k
	$(RM) pthread100k
	$(RM) pthread500k
	$(RM) pthread1m