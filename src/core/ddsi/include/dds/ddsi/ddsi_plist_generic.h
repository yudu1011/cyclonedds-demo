/*
 * Copyright(c) 2019 ADLINK Technology Limited and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License
 * v. 1.0 which is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause
 */
#ifndef DDSI_PLIST_GENERIC_H
#define DDSI_PLIST_GENERIC_H

#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

#include "dds/export.h"

#include "dds/ddsrt/attributes.h"

#if defined (__cplusplus)
extern "C" {
#endif

/* Instructions for the generic serializer (&c) that handles most parameters.
   The "packed" attribute means single-byte instructions on GCC and Clang. */
enum pserop {
  XSTOP,
  XO, /* octet sequence */
  XS, /* string */
  XE1, XE2, XE3, /* enum 0..1, 0..2, 0..3 */
  Xi, Xix2, Xix3, Xix4, /* int32_t, 1 .. 4 in a row */
  Xu, Xux2, Xux3, Xux4, Xux5, /* uint32_t, 1 .. 5 in a row */
  XD, XDx2, /* duration, 1 .. 2 in a row */
  Xo, Xox2, /* octet, 1 .. 2 in a row */
  Xb, Xbx2, /* boolean, 1 .. 2 in a row */
  XbCOND, /* boolean: compare to ignore remainder if false (for use_... flags) */
  XbPROP, /* boolean: omit in serialized form; skip serialization if false; always true on deserialize */
  XG, /* GUID */
  XK, /* keyhash */
  XQ, /* arbitary non-nested sequence */
  Xopt, /* remainder is optional on deser, 0-init if not present */
} ddsrt_attribute_packed;

inline bool pserop_seralign_is_1 (enum pserop op) {
  /* NB: XbPROP is never serialized, so its alignment is irrelevant.  If ever there
     is a need to allow calling this function when op = XbPROP, it needs to be changed
     to taking the address of the pserop, and in that case inspect the following
     operator */
  assert (op != XbPROP && op != Xopt && op != XSTOP);
  return (op >= Xo && op <= XK);
}

DDS_EXPORT void plist_fini_generic (void * __restrict dst, const enum pserop *desc, bool aliased);
DDS_EXPORT dds_return_t plist_deser_generic (void * __restrict dst, const void * __restrict src, size_t srcsize, bool bswap, const enum pserop * __restrict desc);
DDS_EXPORT dds_return_t plist_ser_generic (void **dst, size_t *dstsize, const void *src, const enum pserop * __restrict desc);
DDS_EXPORT dds_return_t plist_unalias_generic (void * __restrict dst, const enum pserop * __restrict desc);
DDS_EXPORT bool plist_equal_generic (const void *srcx, const void *srcy, const enum pserop * __restrict desc);
DDS_EXPORT size_t plist_memsize_generic (const enum pserop * __restrict desc);

#if defined (__cplusplus)
}
#endif

#endif