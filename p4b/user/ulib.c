#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#define PGSIZE 4096

char*
strcpy(char *s, char *t)
{
    char *os;

    os = s;
    while((*s++ = *t++) != 0)
        ;
    return os;
}

int
strcmp(const char *p, const char *q)
{
    while(*p && *p == *q)
        p++, q++;
    return (uchar)*p - (uchar)*q;
}

uint
strlen(char *s)
{
    int n;

    for(n = 0; s[n]; n++)
        ;
    return n;
}

void*
memset(void *dst, int c, uint n)
{
    stosb(dst, c, n);
    return dst;
}

char*
strchr(const char *s, char c)
{
    for(; *s; s++)
        if(*s == c)
            return (char*)s;
    return 0;
}

char*
gets(char *buf, int max)
{
    int i, cc;
    char c;

    for(i=0; i+1 < max; ){
        cc = read(0, &c, 1);
        if(cc < 1)
            break;
        buf[i++] = c;
        if(c == '\n' || c == '\r')
            break;
    }
    buf[i] = '\0';
    return buf;
}

int
stat(char *n, struct stat *st)
{
    int fd;
    int r;

    fd = open(n, O_RDONLY);
    if(fd < 0)
        return -1;
    r = fstat(fd, st);
    close(fd);
    return r;
}

int
atoi(const char *s)
{
    int n;

    n = 0;
    while('0' <= *s && *s <= '9')
        n = n*10 + *s++ - '0';
    return n;
}

void*
memmove(void *vdst, void *vsrc, int n)
{
    char *dst, *src;

    dst = vdst;
    src = vsrc;
    while(n-- > 0)
        *dst++ = *src++;
    return vdst;
}
// NEW static lock

lock_t lock;

void* real_stack[64] = { NULL };
void* PGA_stack[64] = { NULL };
int loc = 0;
//int counter = 0;

void lock_init(lock_t *lock) {
    lock->ticketnumber = 0;
    lock->turn = 0;
}
void lock_acquire(lock_t *lock) {
    int myturn = xaddl(&lock->ticketnumber,1); //must be atomic, since many threads might ask for a lock at the same time
    while (lock->turn != myturn);
}

void lock_release(lock_t *lock ) {
    xaddl(&lock->turn,1);//this need not be atomic, since only the possessor of the lock will execute this
}

int thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2){
    lock_acquire(&lock);
    void *stack = malloc(PGSIZE*2);
    if (stack == NULL)
    {lock_release(&lock);
        return -1;
    }

    //todo: add for loop to find the next free loc
    real_stack[loc] = stack;
    void* PGA = (void*)(((uint)stack+PGSIZE-1)/PGSIZE*PGSIZE);
    PGA_stack[loc] = PGA;
    int temp = clone(start_routine,arg1,arg2,PGA);
    loc++;
    lock_release(&lock);
    return temp;
}

int thread_join(){
    void* stack;
    int TID = join(&stack);
    if(TID != -1)
    {   int i;
        for (i = 0; i < 64; ++i) {
            if(stack == PGA_stack[i]){
                loc--;
                //counter--;
                break;
            }
        }
        free(real_stack[i]);
        real_stack[i] = NULL; PGA_stack[i] = NULL;
    }
    return TID;
}
