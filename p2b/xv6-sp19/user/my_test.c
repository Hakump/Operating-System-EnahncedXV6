#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"

int main(){
    int former = getopenedcount();

    for (int i = 0; i < 10; i++){
        open("console", O_RDWR);
    }
    int now = getopenedcount();
    printf(1,"Number of calls is %d\n", now - former);
    return 0;
}