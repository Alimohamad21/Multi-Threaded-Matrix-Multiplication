#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

void readMatricesFromFile(char *fileName);

typedef struct multiplyElementArgs multiplyElementArgs;
typedef struct multiplyRowArgs multiplyRowArgs;
struct multiplyElementArgs {
    int i;
    int j;
};  // struct to store arguments for multiplyElement function

struct multiplyRowArgs {
    int i;
}; // struct to store arguments for multiplyRow function

void multiplyMatricesByRow();    // multiplies two matrices with the computation of each row in a thread

void multiplyMatricesByElement(); // multiplies two matrices with the computation of each element in a thread

void *multiplyElement(void *args); // computes an element in the multiplied matrix

void *multiplyRow(void *args); // computes a row in the multiplied matrix

void multiply(); // function which ask for file name of input file and multiplies it's 2 matrices

int **matrix1, **matrix2;
int rowCount1, columnCount1, rowCount2, columnCount2;
int **outputMatrix;

int main() {
    multiply();
}

void multiply() {
    char fileName[20];
    printf("Please enter input file name:");
    scanf("%s", fileName);
    readMatricesFromFile(fileName);
    multiplyMatricesByRow();
    multiplyMatricesByElement();
}

void readMatricesFromFile(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    if (!fp) {
        printf("Error opening file or file doesn't exist");
        exit(0);
    }
    fscanf(fp, "%d", &rowCount1);
    fscanf(fp, "%d", &columnCount1);
    matrix1 = malloc(rowCount1 * sizeof(int));
    for (int i = 0; i < rowCount1; i++)
        matrix1[i] = malloc(columnCount1 * sizeof(int));
    for (int i = 0; i < rowCount1; i++) {
        for (int j = 0; j < columnCount1; j++)
            fscanf(fp, "%d", &matrix1[i][j]);
    }
    fscanf(fp, "%d", &rowCount2);
    fscanf(fp, "%d", &columnCount2);
    matrix2 = malloc(rowCount2 * sizeof(int));
    for (int i = 0; i < rowCount2; i++)
        matrix2[i] = malloc(columnCount2 * sizeof(int));
    for (int i = 0; i < rowCount2; i++) {
        for (int j = 0; j < columnCount2; j++)
            fscanf(fp, "%d", &matrix2[i][j]);
    }
}


void multiplyMatricesByRow() {
    if (columnCount1 != rowCount2) {
        printf("Multiplication of Matrices is not possible\n");
        exit(0);
    }
    outputMatrix = malloc(rowCount1 * sizeof(int));
    for (int i = 0; i < rowCount1; i++)
        outputMatrix[i] = malloc(columnCount2 * sizeof(int));
    clock_t begin = clock();
    int noOfThreads = rowCount1;
    pthread_t th[noOfThreads];
    multiplyRowArgs args[noOfThreads];  // had to store arguments in an array so they don't overwrite each other
    for (int i = 0; i < rowCount1; i++) {
        args[i].i = i;
        pthread_create(&th[i], NULL, &multiplyRow, &args[i]);
    }
    for (int i = 0; i < noOfThreads; i++)
        pthread_join(th[i], NULL);
    printf("\nBy Row:\n");
    for (int i = 0; i < rowCount1; i++) {
        for (int j = 0; j < columnCount2; j++)
            printf("%d ", outputMatrix[i][j]);
        printf("\n");
    }
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("Time : %f s\n", time_spent);
}

void multiplyMatricesByElement() {
    if (columnCount1 != rowCount2) {
        printf("Multiplication of Matrices is not possible\n");
        exit(0);
    }
    outputMatrix = malloc(rowCount1 * sizeof(int));
    for (int i = 0; i < rowCount1; i++)
        outputMatrix[i] = malloc(columnCount2 * sizeof(int));
    clock_t begin = clock();
    int noOfThreads = rowCount1 * columnCount2;
    pthread_t th[noOfThreads];
    multiplyElementArgs args[noOfThreads];  // had to store arguments in an array so they don't overwrite each other
    for (int i = 0; i < rowCount1; i++) {
        for (int j = 0; j < columnCount2; j++) {
            outputMatrix[i][j] = 0;
            args[i * (columnCount2) + j].i = i;    // equation to know which iteration we're at
            args[i * (columnCount2) + j].j = j;
            pthread_create(&th[i * (columnCount2) + j], NULL, &multiplyElement, &args[i * (columnCount2) + j]);
        }
    }
    for (int i = 0; i < noOfThreads; i++)
        pthread_join(th[i], NULL);
    printf("\nBy Element:\n");
    for (int i = 0; i < rowCount1; i++) {
        for (int j = 0; j < columnCount2; j++)
            printf("%d ", outputMatrix[i][j]);
        printf("\n");
    }
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("Time : %f s\n", time_spent);
}

void *multiplyElement(void *args) {
    multiplyElementArgs *elementArgs = args;
    int i = elementArgs->i, j = elementArgs->j;
    for (int k = 0; k < columnCount1; k++) {
        outputMatrix[i][j] += matrix1[i][k] * matrix2[k][j];
    }
    return NULL;
}

void *multiplyRow(void *args) {
    multiplyRowArgs *rowArgs = args;
    int i = rowArgs->i;
    for (int j = 0; j < columnCount2; j++) {
        outputMatrix[i][j] = 0;
        for (int k = 0; k < columnCount1; k++) {
            outputMatrix[i][j] += matrix1[i][k] * matrix2[k][j];
        }
    }
    return NULL;
}