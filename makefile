EXECS=brute
MPICC?=mpicc

all: ${EXECS}

brute: brute.c
	${MPICC} -o brute brute.c

clean:
	rm -f ${EXECS}
