/**********************************************************************
 *
 * PostGIS - Spatial Types for PostgreSQL
 * http://postgis.net
 *
 * Copyright (C) 2014 Sandro Santilli <strk@keybit.net>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU General Public Licence. See the COPYING file.
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CUnit/Basic.h"

#include "liblwgeom_internal.h"
#include "cu_tester.h"

/*
** Global variable to hold hex VT strings
*/
char *s;

/*
** The suite initialization function.
** Create any re-used objects.
*/
static int init_out_vt_suite(void)
{
	s = NULL;
	return 0;
}

/*
** The suite cleanup function.
** Frees any global objects.
*/
static int clean_out_vt_suite(void)
{
	if (s) free(s);
	s = NULL;
	return 0;
}

/*
** Creating an output WKB from a vt string 
** 
** @param cfg Vector Tileconfiguration to use, or null for a default
**            being ipx=ipy=0, sfx=sfy=1
*/
static void cu_vt(const char *wkt, lw_vt_cfg *cfg)
{
	LWGEOM *g = lwgeom_from_wkt(wkt, LW_PARSER_CHECK_NONE);
  /* Default configuration */
  lw_vt_cfg default_cfg = {
    0, /* ipx */
    0, /* ipy */
    1, /* sfx */
    1  /* sfy */
  };
  size_t sz;
  uint8_t *vt;

  if ( ! cfg ) cfg = &default_cfg;
  vt = lwgeom_to_vt_geom(g, cfg, &sz);

  if ( s ) free(s);
	if ( vt ) {
    lwgeom_free(g);
    s = hexbytes_from_bytes(vt, sz);
    lwfree(vt);
  } else {
    s = strdup("DEADBEEF");
  }
}

#define do_test_vt_geom(wkt, cfg, expected_hex) { \
	cu_vt(wkt, cfg); \
  if ( strcmp(s, expected_hex) ) {{ \
	  printf(" %s:%d: Exp: %s\n", __FILE__, __LINE__, expected_hex); \
    printf(" %s:%d: Obt: %s\n", __FILE__, __LINE__, s); \
  }} \
	CU_ASSERT_STRING_EQUAL(s, expected_hex); \
}


static void test_vt_out_point(void)
{
  /* first byte is count=1 (1<<3) | cmd=moveTo (1) */
	do_test_vt_geom("POINT(0 0 0 0)", NULL, "090000");

  /* zigzag makes 01 become 02 */ 
	do_test_vt_geom("SRID=4;POINTM(1 2 1)", NULL, "090204");

	do_test_vt_geom("POINTZ(-1 -2 1)", NULL, "090103"); 
  /*
    encoding of Y value (-2):
    1111:1111 ... 1111:1110 - input (fffffffe)
    1111:1111 ... 1111:1100 - A: input << 1
    1111:1111 ... 1111:1111 - B: input >> 31
    0000:0000 ... 0000:0011 - zigzag (A xor B) == output
   */
}

static void test_vt_out_linestring(void)
{
	do_test_vt_geom("LINESTRING(0 0, 1 1, 0 3)",
      NULL,
      "0900001202020104"); 
/*
       ^^ 0000:1001 ( 1 moveTo )
         ^^^^ (0,0)
             ^^ 0001:0010 ( 2 lineTo )
               ^^^^ (+1,+1 -- zigzag'ed to 02,02)
                   ^^^^ (-1,+1 -- zigzag'ed to 01,04)
*/
}

/*
** Used by test harness to register the tests in this file.
*/

CU_TestInfo vt_out_tests[] =
{
	PG_TEST(test_vt_out_point),
	PG_TEST(test_vt_out_linestring),
	CU_TEST_INFO_NULL
};
CU_SuiteInfo out_vt_suite = {"vt",  init_out_vt_suite,  clean_out_vt_suite, vt_out_tests};
