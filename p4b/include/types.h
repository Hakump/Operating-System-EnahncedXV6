#ifndef _TYPES_H_
#define _TYPES_H_

// Type definitions

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;
// NEW
typedef struct lock_t {
    uint ticketnumber;
    uint turn;
} lock_t;
#ifndef NULL
#define NULL (0)
#endif

#endif //_TYPES_H_
