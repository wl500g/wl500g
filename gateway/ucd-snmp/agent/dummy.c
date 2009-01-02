/*
 * Dummy functions to build minimal agent
 *
 * Copyright (c) 2005 Oleg I. Vdovikin
 *
 * Licensed under GPL version 2
 *
 */

typedef unsigned char oid;

int read_objid (char *input, oid *output, int *out_len)
{
    *out_len = 0;

    return 0;
}

void snmp_set_quick_print (int val)
{
}

char *sprint_objid (char *buf, oid *name, int name_len)
{
    *buf = 0;
    return buf;
}
