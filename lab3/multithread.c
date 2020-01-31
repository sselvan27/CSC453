#include "main.h"

int *in_array = NULL;
int *in_array2 = NULL; //[MIN_ARRAY_SIZE];
int *out_array = NULL; //[MIN_ARRAY_SIZE];
int idx;

int main(int argc, char *argv[])
{
    FILE *in;
    char temp[LINE_SIZE];
    int *temp_ptr = NULL;
    int i, number, array_size = MIN_ARRAY_SIZE;
    pthread_t tid[N_THREADS];
    int fake_ids[] = {0, 1, 2};

    struct timespec start, finish;
    double elapsed;

    printf("array_size: %d\n", array_size);

    if(argc != 2) {
        return -1;
    }

    in_array = calloc(array_size, sizeof(int));
    if(in_array == NULL) {
        perror("calloc in_array");
        return -1;
    }

    in = fopen(argv[1],"r");
    if(in == NULL) {
        perror("fopen in");
        free(in_array);
        return -1;
    }

    idx = 0;
    
    while (fgets(temp, LINE_SIZE, in) != NULL) {

        if (idx >= array_size) {
            array_size = array_size * 1.5;
            if ((temp_ptr=realloc(in_array, array_size * sizeof(int))) == NULL) {
                perror("realloc in_array");
                free(in_array);
                return -1;
            }

            in_array = temp_ptr;
            temp_ptr = NULL;
            //array_size = sizeof(in_array)/sizeof(int);
        }

        number = atoi(temp);

        in_array[idx++] = number;
    }

    
    idx--; //ignore the last entry (EOF)

    out_array = calloc(idx+1, sizeof(int));
    if(out_array == NULL) {
        perror("calloc out_array");
        free(in_array);
        return -1;
    }

    fclose(in);


    /* create a new copy of the input array to sort and compare times */
    in_array2 = calloc(array_size, sizeof(int));
    if(in_array2 == NULL) {
        perror("calloc out_array");
        free(in_array);
        free(out_array);
        return -1;
    }

    printf("array_size: %d\n", array_size);
    //printf("size array: %ld\n", sizeof(in_array)/sizeof(int));
    //printf("size array2: %ld\n", sizeof(in_array2)/sizeof(int));

    for (i=0; i<array_size; i++) {
        in_array2[i] = in_array[i];
    }

    //delete this line!!!!!!
    //in_array2 = in_array;



    /*this is where we should call the threads, everyhting before this should be put into a seperate function*/
    /*create two threads to sort, use exact same model from lecture*/
    /*after joining both threads, make new thread to merge*/

    /*This isn't the right way to do this be if we get this to work it should be easier to fix at the end than now*/

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = 0; i < N_THREADS-1; i++) {
        pthread_create(&tid[i], NULL, sort_runner, &fake_ids[i]);
    }

    for (i = 0; i < N_THREADS-1; i++) {
        pthread_join(tid[i], NULL);
    }
    
    
    

    for (i = N_THREADS-1; i < N_THREADS; i++) {
        pthread_create(&tid[i], NULL, merge_runner, &fake_ids[i]);
    }

    for (i = 0; i < N_THREADS-1; i++) {
        pthread_join(tid[i], NULL);
    }

    
    /* ... */
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Time Elapsed (Parallel): %f\n", elapsed);



    // time non-thread sorting
    clock_gettime(CLOCK_MONOTONIC, &start);

    standard_sort();

    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Time Elapsed (Linear): %f\n", elapsed);


    printf("Sorted Array:\n");
    for (i=0; i <= idx; i++) {
        printf("%d ", out_array[i]);
    }
    printf("\n");

    free(in_array);
    free(out_array);
    free(in_array2);

    return 0;
}


void *sort_runner(void *param) {
    //int idx,
    int tid = *((int *) param);
    //int result_local = 0;
    int chunk = (idx+2) / 2; //.........50
    int lower = tid*chunk;  // lower = 0
                            // lower = 50
                            
    // 0..50
    // 51..100
                            
    //int upper = lower + chunk;
    //int *in_array;

    //int i;

    if(lower + chunk >= idx) {
        chunk = idx + 1 - lower;
    }

    //printf("lower: %d chunk: %d tid: %d\n", lower, chunk, tid);
    qsort(in_array+lower, chunk, sizeof(int), comparator);
    
    
    /*for (i=0; i<chunk; i++) {
        printf("\tvalue: %d\n", in_array[lower+i]);
    }
    printf("end sort\n");
    //inplace_sort(in_array, lower, upper);
    */

    return NULL;

    /*insert the sorting algorithm */
}

void *merge_runner(void *param) {
    //int idx;//, tid = (int) *((int *) param); //don't think we need this yet but we will later if we combine the runners
    //int *in_array, *out_array;
    int middle = (idx+1) / 2;
    
    /*insert the merge algo */
    //merge_list_halves(in_array, out_array, 0, middle, middle+1, idx);

    mergeLists(in_array, 0, middle, idx+1, out_array );

    return NULL;
}

/*sorts inplace, just provide the idx range*/
int inplace_sort(int arr[], int min_idx, int max_idx) {
    return 0;
}

// [s1, e1)
void mergeLists( int in[], int s, int m, int e, int outArr[] )
{
    int i = 0, s1 = s, e1 = m, s2 = m, e2 = e;
    while( s1 < e1 && s2 < e2 ) {
        if ( in[ s1 ] < in[ s2 ] ) {
            outArr[ i++ ] = in[ s1++ ];
        } else {
            outArr[ i++ ] = in[ s2++ ];
        }
    }
    while( s1 < e1 ) {
        outArr[ i++ ] = in[ s1++ ];
    }
    while( s2 < e2 ) {
        outArr[ i++ ] = in[ s2++ ];
    }
}


/*use basic merge sort to combine two halves of one list into a second */


int comparator(const void *first, const void *second) {
    int f,s;
    f = *((int *) first);
    s = *((int *) second);
    return f-s;
}

void standard_sort() {
    int chunk = (idx+2) / 2;

    //sort first half
    qsort(in_array2, chunk, sizeof(int), comparator);

    if(chunk*2 >= idx) {
        chunk = idx + 1 - chunk;
    }

    //sort second half
    qsort(in_array2+chunk, chunk, sizeof(int), comparator);

    // merge
    int middle = (idx+1) / 2;
   
    //merge_list_halves(in_array2, out_array, 0, middle, middle+1, idx);
    
    mergeLists(in_array2, 0, middle, idx+1, out_array);

    return;
}

void print_array(int *arr, int length) {
    int i;
    for(i=0; i<length; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

