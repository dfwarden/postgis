/**********************************************************************
 * $Id: cu_list.h 5674 2010-06-03 02:04:15Z mleslie $
 *
 * PostGIS - Spatial Types for PostgreSQL
 * http://postgis.refractions.net
 * Copyright 2010 LISAsoft Pty Ltd
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU General Public Licence. See the COPYING file.
 *
 **********************************************************************/

#ifndef __cu_list_h__
#define __cu_list_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CUnit/Basic.h"

/***********************************************************************
** for Computational Geometry Suite
*/

/* Admin functions */
int init_list_suite(void);
int clean_list_suite(void);

#endif /* __cu_list_h__ */
