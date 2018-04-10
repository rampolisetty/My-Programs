/*
 **************************************************************************************** 
 *
 *       Filename:   sharedObject.c
 *       Author:     Ram Polisetty (Ram P), polisetty_ram@yahoo.com
 *       Created:    04/30/2012 04:49:56 PM
 *       Description:  
 *
 *       Version:    1.0
 *       Revision:   none
 *       Company:    Self
 *
 **************************************************************************************** 
 */
#include<unistd.h>
#include<stdio.h>

int a = 10;

static void initialize_hooks (void) __attribute__((constructor));

static void
initialize_hooks (void)
{
    a = 100;
    fprintf(stdout, "\nInitialize hooks called from pid %d\n",
            getpid());
}

int func (int data)
{
    printf("\nI am called from %d, and my value is %d and my address is %p\n", 
           getpid(), a, &a);

    return (data + a);
}
