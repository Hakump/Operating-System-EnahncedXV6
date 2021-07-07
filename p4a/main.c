#include <stdio.h>
#include "mapreduce.h"
#include "mapreduce.c"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

void Map(char *file_name) {
    FILE *fp = fopen(file_name, "r");
    assert(fp != NULL);

    char *line = NULL;
    size_t size = 0;
    while (getline(&line, &size, fp) != -1) {
        char *token, *dummy = line;
        while ((token = strsep(&dummy, " \t\n\r")) != NULL) {
            MR_Emit(token, "1");
        }
    }
    free(line);
    fclose(fp);
}

void Reduce(char *key, Getter get_next, int partition_number) {
    int count = 0;
    char *value;
    while ((value = get_next(key, partition_number)) != NULL)
        count++;
    printf("%s %d\n", key, count);
}

int main() {
    MR_Run();
    return 0;
}

// backup methods
///**
// * free the value and return next one, used in get_next
// * @param v this node
// * @return next node
// */
//values* free_values(values* v){
//    values* next = v->next;
//    free(v->the_value); free(v);
//    return next;
//}
//
//// alternative free, which free the pair at get next
//char* getter(char *key, int partition_number){
//    // get the num of value key, delete the returned node
//    char* value; pair* pos;
//    //find the key position;
//    pos = bsearch(key,partition_tb[partition_number].sorted, (size_t)((unsigned) partition_tb[partition_number].num_list), sizeof(pair*),comparator);
//    if (pos==NULL || pos->curr_visit == NULL)
//        return NULL;
//    value = strdup(pos->curr_visit->the_value);
//    pos->curr_visit = free_values(pos->curr_visit);
//    return value;
//}
