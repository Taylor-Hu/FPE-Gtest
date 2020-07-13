/*
 * memtester version 4
 *
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2012 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */

#define __version__ "4.3.0"

#include "gtest_gnuc.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "mem_types.h"
#include "mem_sizes.h"
#include "chdrv_include.h"
#include "mem_tests.h"
#include "mem_tester.h"

#define EXIT_FAIL_NONSTARTER    0x01
#define EXIT_FAIL_ADDRESSLINES  0x02
#define EXIT_FAIL_OTHERTEST     0x04



struct test tests[] = {
    { "Random Value", test_random_value },
    { "Compare XOR", test_xor_comparison },
    { "Compare SUB", test_sub_comparison },
    { "Compare MUL", test_mul_comparison },
    { "Compare DIV",test_div_comparison },
    { "Compare OR", test_or_comparison },
    { "Compare AND", test_and_comparison },
    { "Sequential Increment", test_seqinc_comparison },
    { "Solid Bits", test_solidbits_comparison },
    { "Block Sequential", test_blockseq_comparison },
    { "Checkerboard", test_checkerboard_comparison },
    { "Bit Spread", test_bitspread_comparison },
    { "Bit Flip", test_bitflip_comparison },
    { "Walking Ones", test_walkbits1_comparison },
    { "Walking Zeroes", test_walkbits0_comparison },
#ifdef TEST_NARROW_WRITES    
    { "8-bit Writes", test_8bit_wide_random },
    { "16-bit Writes", test_16bit_wide_random },
#endif
    { NULL, NULL }
};

/* Sanity checks and portability helper macros. */
//#if (defined(_SC_VERSION) )
#if ((!defined(CFG_CHSSA_CHIP_MSTAR7C123D)||defined(CFG_CHSSA_CHIP_MSTAR5C58))&&(defined(_SC_VERSION) ))  /*hl20171221*/
void check_posix_system(void) {
    if (sysconf(_SC_VERSION) < 198808L) {
        printf("A POSIX system is required.  Don't be surprised if "
            "this craps out.\n");
        printf("_SC_VERSION is %lu\n", sysconf(_SC_VERSION));
    }
}
#else
#define check_posix_system()
#endif
/*
#ifdef _SC_PAGE_SIZE
int memtester_pagesize(void) {
    int pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1) {
        perror("get page size failed");
        exit(EXIT_FAIL_NONSTARTER);
    }
    printf("pagesize is %ld\n", (long) pagesize);
    return pagesize;
}
#else
int memtester_pagesize(void) {
    printf("sysconf(_SC_PAGE_SIZE) not supported; using pagesize of 8192\n");
    return 8192;
}
#endif
*/

/* Some systems don't define MAP_LOCKED.  Define it to 0 here
   so it's just a no-op when ORed with other constants. */
#ifndef MAP_LOCKED
  #define MAP_LOCKED 0
#endif

/* Function declarations */
void usage(char *me);

/* Global vars - so tests have access to this information */
int use_phys = 1;
off_t physaddrbase = 0;

/* Function definitions */
void usage(char *me) {
    printf("\n"
            "Usage: %s [-p physaddrbase [-d device]] <mem>[B|K|M|G] [loops]\n",
            me);
    exit(EXIT_FAIL_NONSTARTER);
}

int test_DDR(size_t size, ul loops) {
    ul loop, i;
    loop = 1;
    size_t pagesize = 0,  bufsize = 0,
         halflen = 0, count = 0;
    
    ptrdiff_t pagesizemask = 0;
    void volatile *buf = NULL, *aligned = NULL;
    ulv *bufa = NULL, *bufb = NULL;
    int done_mem = 0;


    printf("memtester version " __version__ " (%d-bit)\n", UL_LEN);
    printf("Copyright (C) 2001-2012 Charles Cazabon.\n");
    printf("Licensed under the GNU General Public License version 2 (only).\n");
    printf("\n");
    
    pagesize = 4096;
    pagesizemask = (ptrdiff_t) ~(pagesize - 1);
    //buf = (void volatile *) malloc(size);

    while (!done_mem) {
        while (!buf && size) { 
            buf = (void volatile *) CH_RTOS_AllocateMemory(gh_APP_HEAP,(S32)size);
            if (!buf) size -= pagesize;        }
            bufsize = size;
            printf("got  %lluMB (%llu bytes)", (ull) size >> 20,
                (ull) size);
       
            if ((size_t) buf % pagesize) {
                /* printf("aligning to page -- was 0x%tx\n", buf); */
                aligned = (void volatile *) ((char*)((size_t) buf & pagesizemask) + pagesize);
                /* printf("  now 0x%tx -- lost %d bytes\n", aligned,                 *      (size_t) aligned - (size_t) buf);
                */                
                bufsize -= ((size_t) aligned - (size_t) buf);
                done_mem = 1;
                } 
            else {
                aligned = buf;
                }
                /* Try mlock */
}  


    halflen = bufsize / 2;
    count = halflen / sizeof(ul);
    bufa = (ulv *) aligned;
    bufb = (ulv *) ((size_t) aligned + halflen);

    for(loop=1; ((!loops) || loop <= loops); loop++)
    {
        printf("Loop %lu", loop);
        if (loops) {
            printf("/%lu", loops);
        }
        printf(":\n");
        printf("  %-20s: ", "Stuck Address");
       // fflush(stdout);
        if (!test_stuck_address((ulv*)aligned, bufsize / sizeof(ul))) {
             printf("ok\n");
        } else {
            
        }
        for (i=0;;i++) {
            if (!tests[i].name) break;
            /* If using a custom testmask, only run this test if the
               bit corresponding to this test was set by the user.
             */

            printf("  %-20s: ", tests[i].name);
            if (!tests[i].fp(bufa, bufb, count)) {
                printf("ok\n");
            } else {
                
            }
            //fflush(stdout);
        }
        printf("\n");
        //fflush(stdout);
        //break;
    }
    CH_RTOS_FreeMemory(gh_APP_HEAP,(void *)buf);
    printf("Done.\n");
   // fflush(stdout);
    return 0;
}
