all: mpi.c
	mpicc mpi10k.c -o mpi10k
	mpicc mpi100k.c -o mpi100k
	mpicc mpi500k.c -o mpi500k
	mpicc mpi1m.c -o mpi1m
clean:
	$(RM) mpi10k
	$(RM) mpi100k
	$(RM) mpi500k
	$(RM) mpi1m