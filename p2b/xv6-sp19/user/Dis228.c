#include "types.h"
#include "stat.h"
#include "user.h"

//int
//main(int argc, char *argv[])
//{
//    unsigned int i = 0;
//    char* temp = "\0";
//    printf(1,"a[%d] is %s\n",i,temp[i]);
//    // instruction have diff length
//    exit();
//}

int
main(int argc, char *argv[])
{
    unsigned int i = 0;
    //stack
    char temp[16];
    //heap
    char* b = malloc(sizeof(char)*16);

    printf(1,"%p\n", temp);
    printf(1,"%p\n", temp[15]);
    printf(1,"%p\n", b);

    //printf(1,"temp[%d] is %s\n",i,temp[i]);
    // instruction have diff length
    exit();
}
