#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <openssl/md5.h>

int BRUTEFORCE_DEPTH = 5;

//C style bool
typedef int bool;
#define true 1
#define false 0
#define die -1

//Constants
const uint CHAR_OFFSET = 32;
const uint CHARS = 95;

//MPI Globals
int tag = 1;
int interrupter = 0;
int flag = 0;
MPI_Request request, request2;

//Definitions
bool bruteforceLevel(char *source, char* str, int index, int maxLength, int world_size, int world_rank);
void bruteforce(char *source, int maxLen, int world_size, int world_rank);
bool compareMD5(char *str1, char *str2);
void getMD5(char *md5string, char *string);

bool compareMD5(char *str1, char *str2)
{
	if (strcmp(str1, str2) == false) {
		return true;
	} else {
		return false;
	} 
}

bool bruteforceLevel(char *source, char* str, int index, int maxLength, int world_size, int world_rank) {
	MPI_Irecv(&interrupter, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &request);
	MPI_Test(&request, &flag, MPI_STATUS_IGNORE);

	if (interrupter)
		return die;

	int my_first = (world_rank * CHARS / world_size) + CHAR_OFFSET;
	int my_last = ((world_rank + 1) * CHARS / world_size) + CHAR_OFFSET;

	bool res;
	char md5_buf[33];
	for (int i = my_first; i < my_last; ++i) {
		MPI_Irecv(&interrupter, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &request);
		MPI_Test(&request, &flag, MPI_STATUS_IGNORE);
		if (interrupter)
			return die;

		str[index] = (char)i;
		if (index == maxLength - 1) {
			//char md5_buf[33];
			getMD5(md5_buf, str);
			if (compareMD5(md5_buf, source) == true) {  
				return true;
			}
		}
		else { 
			res = bruteforceLevel(source, str, index + 1, maxLength, 1, 0);
			if (res == true) {
				return true;
			} else if (res == die) {
				// printf("Die1");
				return die;
			} 
		}
	}
}

void bruteforce(char *source, int maxLen, int world_size, int world_rank) {   
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;

	char *buf = malloc(maxLen + 1);
	for (int i = 1; i <= maxLen; ++i) {
		memset(buf, 0, maxLen + 1);
		int res = bruteforceLevel(source, buf, 0, i, world_size, world_rank);
		if (res == die) {
			// printf("Die2");
			break;
		}
		else if (res == true) { 
			MPI_Get_processor_name(processor_name, &name_len);
			printf("Processor %s, rank %d out of %d processors\n", processor_name, world_rank, world_size);
			printf("Decoded: %s => %s \n\n",source, buf);
			for (int rank = 0; rank < world_size; ++rank) {
				if (world_rank != rank) {
					interrupter = 1;
					MPI_Isend(&interrupter, 1, MPI_INT, rank, tag, MPI_COMM_WORLD, &request2);
				}
			}
			break;
		}
	}
	//printf("Die3");
	//MPI_Barrier(MPI_COMM_WORLD);
	free(buf);
}

void getMD5(char *md5string, char *string) {
	unsigned char result[MD5_DIGEST_LENGTH];
	MD5(string, strlen(string), result);
	for(int i = 0; i < 16; ++i)
		sprintf(&md5string[i*2], "%02x", (unsigned int)result[i]);
	// sprintf(&md5string[32], "\n");
	// printf("md5 digest: %s\n", md5string);
}

int main(int argc, char** argv) {
	FILE * fp;
	size_t len = 0;
	ssize_t read;
	char *line = (char *)malloc(33*sizeof(char));

	MPI_Init(&argc, &argv);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if (world_rank == 0) {
		fp = fopen("passwords", "r");
		if (fp == NULL)
			exit(EXIT_FAILURE);
	}
	bool run = true;
	while (run) {
		if (world_rank == 0) { 
			read = getline(&line, &len, fp);
			if (read == -1) {
				run = false;
				line[0] = '\0';
			}
			line[32] = '\0';
			if (strlen(line) >= 32)
				printf("line %s\n", line);
		}

		MPI_Bcast(line, 33, MPI_CHAR, 0, MPI_COMM_WORLD);
		if (line[0] == '\0') {
			// printf("\n1\n");
			break;
		}

		bruteforce(line, BRUTEFORCE_DEPTH, world_size, world_rank);
		// printf("Retrieved broadcast data - %s - rank %d\n", line, world_rank);
		// printf("flag %d, rank %d \n", flag, world_rank);
		MPI_Barrier(MPI_COMM_WORLD);
		interrupter = 0;
	}

	if (world_rank == 0) {
		fclose(fp);	
	}

	if (line)
		free(line);

	printf("\n");
	MPI_Finalize();  
}
