/*
# This file is Copyright 2009 Dean Hall.
#
# This file is part of the Python-on-a-Chip program.
# Python-on-a-Chip is free software: you can redistribute it and/or modify
# it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.
#
# Python-on-a-Chip is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1
# is seen in the file COPYING up one directory from this.
*/

/**
 * System Test 213
 * Add support for Python 2.6 bytecodes
 */

#include "pm.h"


#define HEAP_SIZE 0x4000

extern unsigned char usrlib_img[];


int main(void)
{
    uint8_t heap[HEAP_SIZE];
    PmReturn_t retval;

    retval = pm_init(heap, HEAP_SIZE, MEMSPACE_PROG, usrlib_img);
    PM_RETURN_IF_ERROR(retval);

    retval = pm_run((uint8_t *)"t213");
    return (int)retval;
}
