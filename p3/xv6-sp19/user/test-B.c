#include "types.h"
#include "stat.h"
#include "user.h"

void
test_failed()
{
    printf(1, "TEST FAILED\n");
    exit();
}

void
test_passed()
{
    printf(1, "TEST PASSED\n");
    exit();
}

int
main(int argc, char *argv[])
{
    char *ptr;
    int i;
    char arr[6] = "CS537";


    int pid = fork();
    if (pid < 0) {
        printf(1,"F2\n");
        test_failed();
    }
    else if (pid == 0) {
        ptr = shmget(2);
        if (ptr == NULL) {
            printf(1,"F3\n");
            test_failed();
        }

        for (i = 0; i < 5; i++) {
            printf(1,"F5\n");
            *(ptr+i) = arr[i];
        }

        exit();
    }
    else {
        wait();

        ptr = shmget(2);
        if (ptr == NULL) {
            printf(1,"F6\n");
            test_failed();
        }

        for (i = 0; i < 5; i++) {
            if (*(ptr+i) != arr[i]) {
                printf(1,"F7:%s , %s\n",(ptr+i),arr);
                test_failed();
            }
        }
    }

    test_passed();
    exit();
}
