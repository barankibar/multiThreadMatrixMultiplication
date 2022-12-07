#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<time.h>

#define MATRIX_SIZE 5

//Struct model for matrix
struct v {
	size_t row;
	size_t column;
};

//Defining matrices
short matrix_a[MATRIX_SIZE][MATRIX_SIZE];
short matrix_b[MATRIX_SIZE][MATRIX_SIZE];
short result[MATRIX_SIZE][MATRIX_SIZE];

//Function Declerations
void printMatrix(int row, int col, short matrix[row][col]);

void readMatrix (int row, int col, short arr[row][col], FILE *fp );

static void * multiplication(void *arg);

//MAIN
int main(int argc, char *argv[]){
	clock_t begin = clock();
	if(argc != 3) {
		printf("Usage: %s <file1> <file2> \n", argv[0]);
		return 1;
	}

	/* 
		Example of matrix file's schema:
		-------------------------
		a1  a2  a3  a4  a5
		a6  a7  a8  a9  a10
		a11 a12 a13 a14 a15
		a16 a17 a18 a19 a20
		a21 a22 a23 a24 a25
		-------------------------
	*/
	//Assing numbers from file1 to first matrix row and column
	FILE *fp = fopen(argv[1], "r");
	if(fp == NULL)
	{
		printf("Couldn't open file1 \n");
		return 1;
	}
	readMatrix(MATRIX_SIZE, MATRIX_SIZE, matrix_a, fp);
	fclose(fp);
	
	//Assing numbers from file2 to first matrix row and column
	fp = fopen(argv[2], "r");
	if(fp == NULL)
	{
		printf("Couldn't open file2 \n");
		return 1;
	}
	readMatrix(MATRIX_SIZE, MATRIX_SIZE, matrix_b, fp);
	fclose(fp);

	printMatrix(MATRIX_SIZE, MATRIX_SIZE, matrix_a);
	printMatrix(MATRIX_SIZE, MATRIX_SIZE, matrix_b);

	pthread_t threads[MATRIX_SIZE];
	size_t i, k;
	
	//Allocate memory for threads return values
	struct v **data;
	data = (struct v **)malloc(MATRIX_SIZE * sizeof(struct v));
	
	//Creating threads for each row 
	for(i=0; i < MATRIX_SIZE; i++) {
		data[i] = (struct v *)malloc(MATRIX_SIZE * sizeof(struct v));
		
		//Assign the rows and colums of result matrix for threads 
		for(k=0; k < MATRIX_SIZE; k++) {
			data[i][k].row = i;
			data[i][k].column = k;
		}
		//Create the threads for each row
		pthread_create(&threads[i], NULL, multiplication, data[i]);
	}

	//Collect each thread
	for(i=0; i < MATRIX_SIZE; i++) {
		pthread_join(threads[i], NULL);
	}
	
	//Print Result
	printMatrix(MATRIX_SIZE, MATRIX_SIZE, result);
	
	//Free redundant memory
	for(i=0; i < MATRIX_SIZE; i++) {
		free(data[i]);
	}
	free(data);

	clock_t end = clock();
	printf("The elapsed time for this all process: %f \n", (double)(end - begin) / CLOCKS_PER_SEC);
	return 0;
}

//Secondary Functions
void readMatrix (int row, int col, short arr[row][col], FILE *fp) {
	int i,j;

	for(i = 0; i < MATRIX_SIZE; i++) {
		for(j=0; j < MATRIX_SIZE; j++) {
			fscanf(fp, "%hu", &arr[i][j]);
		}
	}
}


void printMatrix(int row, int col, short matrix[row][col]) {
    int x = 0;
    int y = 0;

    for(x = 0 ; x < row ; x++) {
        printf(" (");
        for(y = 0 ; y < col ; y++){
            printf("  %d  ", matrix[x][y]);
        }
        printf(")\n");
    }
    printf("------------------------------------------\n");
}

static void * multiplication(void *arg) {
	// For find execution time of a thread
	clock_t begin = clock();

	struct v *data = (struct v *)arg;
	size_t l;
	
	for(l=0; l < MATRIX_SIZE; l++){
		//Assign the each row of matris into the i and  each column of matrix into the j
		size_t i = (data[l].row);
		size_t j = (data[l].column);
		
		int sum = 0;	
		size_t d;
		
		for(d = 0; d < MATRIX_SIZE; d++) {
			sum += matrix_a[i][d] * matrix_b[d][j];
		}
		
		result[i][j] = sum;
		sum = 0;
	}
	clock_t end = clock();
	printf("The elapsed time for this thread : %f \n", (double)(end - begin) / CLOCKS_PER_SEC);

	return 0;
}
