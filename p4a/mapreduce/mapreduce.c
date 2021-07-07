#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mapreduce.h"
#include <search.h>
#include <pthread.h>
#include "time.h"

typedef struct _values_{
    struct _values_* next;
    char* the_value; // free
} values;

typedef struct _pair{
    struct _pair* next;
    char* key; // free
    values* value; // free
    char** sorted;
    int num;
    int visiting; // for iterating the list by order,
} pair;

typedef pair* keylist;

typedef struct _table{
    keylist partition_table[757]; // no free inside
    pthread_mutex_t table_lock[757];
    int numbers[757];
    pthread_mutex_t lock; // modify the num_list
    keylist* sorted; // to an array and sort, need free
    int visiting;
}partitions;

partitions* partition_tb;

Mapper map;
Reducer reducer;
Partitioner partitioner;

int num_partition, num_files, num_mapped;
char** files_path;
pthread_mutex_t map_task_lock;

pair* free_pair(pair* p);

void init(int argc, char *argv[], int reducer_num){
    num_files = argc - 1;
    num_mapped = 0;
    num_partition = reducer_num;
    files_path = argv + 1;
    partition_tb = malloc(sizeof(partitions)*num_partition);

    pthread_mutex_init(&map_task_lock,NULL);

    for (int i = 0; i < num_partition; ++i) {
        partition_tb[i].sorted = NULL;
        partition_tb[i].visiting = 0; // TODO: for getter_next, modify???
        pthread_mutex_init(&partition_tb[i].lock, NULL);
        for (int j = 0; j < 757; ++j) {
            partition_tb[i].numbers[j]=0;
            pthread_mutex_init(&partition_tb[i].table_lock[j], NULL);
            partition_tb[i].partition_table[j] = NULL;
        }
    }
}

int comparator(const void* p1, const void * p2){
    return strcmp((*(pair**)p1)->key,(*(pair**)p2)->key);
}
int com_val(const void* p1, const void * p2){
    return strcmp((*(char**)p1),(*(char**)p2));
}

char* getter_next(char* key, int partition_number){
    partitions now = partition_tb[partition_number];
    pair* pos = now.sorted[now.visiting];
    if (pos->visiting >= pos->num){ // value visited finished
        return NULL;
    }
    return pos->sorted[(pos->visiting)++];
}

void* map_thread(void *args){
    // note there is a map_lock s.t each mapper will only get a distinct file
    while (1){
        char* path;
        // lock the map_lock
        pthread_mutex_lock(&map_task_lock);
        if (num_mapped>=num_files){
            // unlock;
            pthread_mutex_unlock(&map_task_lock);
            return NULL;
        }
        // num of finished file++
        //unlock
        path = files_path[num_mapped++];
        pthread_mutex_unlock(&map_task_lock);
        (*map)(path);
    }
}

void* reduce_thread(void *args){
    int partition = *(int*)args; // which REDUCER to map to
    free(args); args = NULL;
    int size = 0;
    for (int k = 0; k < 757; ++k) {
        size+=partition_tb[partition].numbers[k];
    }
    if (size == 0){
        return NULL;
    }
    partition_tb[partition].sorted = malloc(sizeof(pair*)*size);
    pair* current;
    int num = 0; // should equal to num_list
    for (int i = 0; i < 757; i++){
        if (partition_tb[partition].partition_table[i] != NULL){
            current = partition_tb[partition].partition_table[i];
            while (current!=NULL){
                partition_tb[partition].sorted[num] = current;
                current = current->next;
                num++;
            }
        }
    }

    //printf("%d, %d\n",num,partition_tb[partition].num_list);
    qsort(partition_tb[partition].sorted, num, sizeof(pair*),comparator);

    for (int i = 0; i < num; i++) {
        // planA: qsort
        partition_tb[partition].sorted[i]->sorted = malloc(sizeof(char*) * partition_tb[partition].sorted[i]->num);
        values* this = partition_tb[partition].sorted[i]->value;
        for (int j = 0; j < partition_tb[partition].sorted[i]->num; ++j) {
            partition_tb[partition].sorted[i]->sorted[j] = this->the_value;
            this = this->next;
        }
        qsort(partition_tb[partition].sorted[i]->sorted,partition_tb[partition].sorted[i]->num, sizeof(char*),com_val);
        // planB: sort when inserting
        (*reducer)(partition_tb[partition].sorted[i]->key,getter_next,partition);
        partition_tb[partition].visiting = i + 1;
    }
    //printf("%s\n",partition_tb[partition].sorted[num-1]->key);
    // free everything
    for (int i = 0; i < size; ++i) { // free every pair in the partition
        free_pair(partition_tb[partition].sorted[i]);
    }
    //printf("FINISH\n");
    free(partition_tb[partition].sorted); // free the sorted array
    return NULL;
}

void MR_Run(int argc, char *argv[],
            Mapper mapper, int num_mappers,
            Reducer reduce, int num_reducers,
            Partitioner partition){
    //clock_t start = clock();
    init(argc,argv,num_reducers);
    map = mapper;
    reducer = reduce;
    partitioner = partition;
    pthread_t mappers[num_mappers];
    clock_t point_init = clock();

    for (int i = 0; i < num_mappers; ++i) {
        //printf("create %d\n", i);
        pthread_create(&mappers[i],NULL,map_thread,NULL);
    }
    clock_t map_create = clock(); printf("%f\n",(double)(map_create - point_init)/CLOCKS_PER_SEC);
    for (int j = 0; j < num_mappers; ++j) {
        //printf("join %d\n", j);
        pthread_join(mappers[j],NULL);
    }
    clock_t map_join = clock(); printf("%f\n",(double)(map_join - map_create)/CLOCKS_PER_SEC);
    pthread_t reducers[num_reducers];
    for (int i = 0; i < num_reducers; ++i) {
        //printf("creater %d\n", i);
        void* temp = malloc(sizeof(void*));
        *(int*)temp = i;
        pthread_create(&reducers[i],NULL,reduce_thread,temp);
    }
    clock_t reducer_create =clock(); printf("%f\n",(double)(reducer_create - map_join)/CLOCKS_PER_SEC);
    for (int j = 0; j < num_reducers; ++j) {
        //printf("joinr %d\n", j);
        pthread_join(reducers[j], NULL);
    }
    clock_t reducer_join = clock(); printf("%f\n",(double)(reducer_join - reducer_create)/CLOCKS_PER_SEC);
    free(partition_tb);
}

void MR_Emit(char *key, char *value){
    int partition_num = (int)(*partitioner)(key, num_partition); // give where to put MR_DefaultHashPartition
    int hash_num = (int)MR_DefaultHashPartition(key,757);

    values* new_value = malloc(sizeof(values)); char* new = strdup(value); new_value->the_value = new;
    //printf("SUCCESS %d\n",num_mapped);
    pthread_mutex_lock(&partition_tb[partition_num].table_lock[hash_num]);
    pair* temp = partition_tb[partition_num].partition_table[hash_num]; // [0] correct?
    while (temp!=NULL) {
        if (strcmp(key, temp->key) == 0)
            break;
        temp = temp->next;
    }
    //printf("SUCCESS-2 %d\n",num_mapped);
    if(temp!=NULL){ // add new value to the front of key list
        new_value->next = temp->value;
        temp->value = new_value;
        temp->num++;
        pthread_mutex_unlock(&partition_tb[partition_num].table_lock[hash_num]);
    } else { // create a new list
        //printf("SUCCESS-3 %d\n",num_mapped);
        pair* new_pair = malloc(sizeof(pair));
        new_pair->next = partition_tb[partition_num].partition_table[hash_num];
        partition_tb[partition_num].partition_table[hash_num] = new_pair;
        partition_tb[partition_num].numbers[hash_num]++;
        new_pair->num = 1;
        new_pair->key = strdup(key); // need free
        new_pair->value = new_value; // need free
        pthread_mutex_unlock(&partition_tb[partition_num].table_lock[hash_num]);
        new_pair->visiting = 0;
    }

}

/**
 * free a pair and return the next pair, may be NULL
 * @param p the pair that is going to be freed
 * @return next pair to be freed
 */
pair* free_pair(pair* p){  // TODO: change the freeing process
    free(p->key); // free the key inside the p
    // planA
    for (int i = 0; i < p->num; ++i) {
        free(p->sorted[i]);
    }
    free(p->sorted);
    // end planA
    pair* next = p->next; free(p); // free the pair struct
    return next;
}

unsigned long MR_DefaultHashPartition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0')
        hash = hash * 33 + c;
    return hash % num_partitions;
}


