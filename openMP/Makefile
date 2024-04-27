all: openMP.c
	gcc -o openMP10k -fopenmp openMP10k.c
	gcc -o openMP100k -fopenmp openMP100k.c
	gcc -o openMP500k -fopenmp openMP500k.c
	gcc -o openMP1m -fopenmp openMP1m.c
clean:
	$(RM) openMP10k
	$(RM) openMP100k
	$(RM) openMP500k
	$(RM) openMP1m