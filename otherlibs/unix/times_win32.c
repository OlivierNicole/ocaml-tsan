/**************************************************************************/
/*                                                                        */
/*                                 OCaml                                  */
/*                                                                        */
/*                  File contributed by Josh Berdine                      */
/*                                                                        */
/*   Copyright 2011 Institut National de Recherche en Informatique et     */
/*     en Automatique.                                                    */
/*                                                                        */
/*   All rights reserved.  This file is distributed under the terms of    */
/*   the GNU Lesser General Public License version 2.1, with the          */
/*   special exception on linking described in the file LICENSE.          */
/*                                                                        */
/**************************************************************************/

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include "unixsupport.h"
#include <windows.h>


static double to_sec(FILETIME ft) {
#if defined(_MSC_VER) && _MSC_VER < 1300
  /* See gettimeofday.c - it is not possible for these values to be 64-bit, so
     there's no worry about using a signed struct in order to work around the
     lack of support for casting int64_t to double.
   */
  LARGE_INTEGER tmp;
#else
  ULARGE_INTEGER tmp;
#endif

  tmp.u.LowPart = ft.dwLowDateTime;
  tmp.u.HighPart = ft.dwHighDateTime;

  /* convert to seconds:
     GetProcessTimes returns number of 100-nanosecond intervals */
  return tmp.QuadPart / 1e7;
}


value caml_unix_times(value unit) {
  value res;
  FILETIME creation, exit, stime, utime;

  if (!(GetProcessTimes(GetCurrentProcess(), &creation, &exit, &stime,
                        &utime))) {
    caml_win32_maperr(GetLastError());
    caml_uerror("times", Nothing);
  }

  res = caml_alloc_small(4 * Double_wosize, Double_array_tag);
  Store_double_field(res, 0, to_sec(utime));
  Store_double_field(res, 1, to_sec(stime));
  Store_double_field(res, 2, 0);
  Store_double_field(res, 3, 0);
  return res;
}
