#include <gio/gio.h>

#if defined (__ELF__) && ( __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6))
# define SECTION __attribute__ ((section (".gresource.logo"), aligned (sizeof(void *) > 8 ? sizeof(void *) : 8)))
#else
# define SECTION
#endif

static const SECTION union { const guint8 data[1807]; const double alignment; void * const ptr;}  logo_resource_data = {
  "\107\126\141\162\151\141\156\164\000\000\000\000\000\000\000\000"
  "\030\000\000\000\164\000\000\000\000\000\000\050\003\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\324\207\244\163"
  "\001\000\000\000\164\000\000\000\017\000\114\000\204\000\000\000"
  "\210\000\000\000\324\265\002\000\377\377\377\377\210\000\000\000"
  "\001\000\114\000\214\000\000\000\220\000\000\000\103\145\122\020"
  "\000\000\000\000\220\000\000\000\013\000\166\000\240\000\000\000"
  "\016\007\000\000\104\145\144\165\160\145\040\105\156\164\162\151"
  "\145\163\057\000\002\000\000\000\057\000\000\000\000\000\000\000"
  "\144\145\144\165\160\145\145\056\160\156\147\000\000\000\000\000"
  "\136\006\000\000\000\000\000\000\211\120\116\107\015\012\032\012"
  "\000\000\000\015\111\110\104\122\000\000\000\135\000\000\000\112"
  "\010\006\000\000\000\331\242\130\340\000\000\001\175\151\103\103"
  "\120\111\103\103\040\160\162\157\146\151\154\145\000\000\050\221"
  "\175\221\075\110\102\121\030\206\237\173\055\214\060\034\162\210"
  "\150\270\203\065\325\122\041\215\145\201\004\006\142\006\152\015"
  "\335\037\065\301\173\223\173\257\264\070\006\255\101\103\077\113"
  "\126\103\113\163\255\015\255\101\020\364\003\321\334\320\124\324"
  "\022\161\343\250\240\104\366\055\347\341\075\337\373\161\276\367"
  "\200\134\053\351\246\323\065\015\246\345\332\311\130\124\111\147"
  "\262\212\377\005\077\103\004\231\040\242\352\116\171\046\221\210"
  "\323\261\076\357\220\304\171\073\046\146\165\356\373\263\372\214"
  "\234\243\203\244\000\323\172\331\166\101\132\005\042\033\156\131"
  "\360\036\020\322\327\124\003\244\063\140\324\116\147\262\040\075"
  "\010\135\153\360\253\340\102\235\145\061\063\144\247\222\263\040"
  "\207\000\245\320\306\132\033\353\153\266\011\362\044\020\066\114"
  "\313\000\071\335\140\103\160\125\260\131\252\350\315\167\212\015"
  "\003\071\153\151\121\350\300\020\061\346\131\040\201\202\106\205"
  "\042\045\134\306\050\142\241\340\220\044\106\264\203\177\260\356"
  "\117\120\101\243\104\021\035\205\071\326\061\121\353\176\304\037"
  "\374\316\326\311\117\214\067\046\005\242\320\375\354\171\357\303"
  "\340\337\201\357\155\317\373\072\362\274\357\143\360\075\301\245"
  "\325\362\257\327\140\352\003\174\333\055\055\174\010\301\115\070"
  "\277\152\151\332\056\134\154\301\300\143\131\265\325\272\344\003"
  "\344\174\036\336\116\241\057\003\375\067\320\273\334\310\255\171"
  "\317\311\075\244\252\020\277\206\375\003\030\051\100\160\245\303"
  "\336\075\355\271\375\333\323\314\357\007\040\353\162\354\311\346"
  "\311\045\000\000\000\006\142\113\107\104\000\000\000\000\000\000"
  "\371\103\273\177\000\000\000\011\160\110\131\163\000\000\056\043"
  "\000\000\056\043\001\170\245\077\166\000\000\000\007\164\111\115"
  "\105\007\350\007\034\017\064\041\142\063\243\354\000\000\000\031"
  "\164\105\130\164\103\157\155\155\145\156\164\000\103\162\145\141"
  "\164\145\144\040\167\151\164\150\040\107\111\115\120\127\201\016"
  "\027\000\000\004\075\111\104\101\124\170\332\355\233\115\110\062"
  "\135\024\200\217\037\137\362\202\144\077\226\375\150\321\037\022"
  "\006\256\012\004\063\044\002\151\065\201\233\026\355\332\266\157"
  "\023\021\204\073\153\123\321\302\240\155\213\204\134\104\104\045"
  "\201\101\213\050\312\300\040\044\246\322\032\045\251\064\065\021"
  "\347\274\253\040\336\067\165\262\031\277\276\074\017\334\325\075"
  "\327\173\175\234\271\163\347\334\253\014\021\021\210\222\362\017"
  "\051\040\351\044\235\040\351\044\235\040\351\044\235\040\351\044"
  "\235\040\351\044\235\244\023\044\235\244\023\044\375\377\306\277"
  "\345\366\205\063\231\014\004\203\101\010\205\102\160\173\173\013"
  "\034\307\001\307\161\020\217\307\041\235\116\103\115\115\015\324"
  "\326\326\102\135\135\035\164\165\165\101\173\173\073\150\265\132"
  "\250\250\250\020\157\020\130\006\274\276\276\342\311\311\011\072"
  "\034\016\324\351\164\010\000\237\052\026\213\005\135\056\027\106"
  "\243\121\121\306\363\243\245\207\102\041\164\072\235\150\060\030"
  "\076\055\372\243\242\323\351\320\343\361\040\317\363\044\375\117"
  "\156\156\156\320\341\160\210\042\372\243\342\160\070\060\223\311"
  "\220\364\367\154\157\157\113\046\374\255\054\056\056\142\066\233"
  "\045\351\157\244\122\051\264\130\054\222\213\367\172\275\044\375"
  "\075\133\133\133\222\113\067\233\315\370\362\362\362\351\261\311"
  "\176\352\021\214\124\052\005\126\253\025\274\136\157\316\030\206"
  "\141\300\154\066\203\112\245\202\252\252\052\340\171\036\042\221"
  "\010\354\355\355\201\313\345\022\324\317\301\301\001\230\114\046"
  "\132\062\276\261\271\271\371\327\325\071\075\075\215\207\207\207"
  "\370\360\360\220\263\035\317\363\170\166\166\206\203\203\203\005"
  "\257\166\273\335\116\323\313\173\222\311\044\032\215\106\354\353"
  "\353\303\365\365\365\117\257\263\131\226\055\050\335\152\265\222"
  "\364\077\011\004\002\230\110\044\212\156\077\071\071\131\120\374"
  "\147\227\217\077\076\015\320\331\331\371\245\366\035\035\035\005"
  "\143\170\236\057\155\302\353\342\342\002\144\062\131\311\313\334"
  "\334\134\111\176\264\150\064\232\267\336\140\060\200\134\056\247"
  "\054\243\130\244\323\151\130\133\133\313\033\323\337\337\117\251"
  "\135\061\331\331\331\001\237\317\227\067\146\150\150\210\244\213"
  "\305\371\371\071\214\217\217\027\214\353\355\355\045\351\142\160"
  "\164\164\004\043\043\043\020\211\104\362\306\055\054\054\100\113"
  "\113\013\155\142\174\205\154\066\013\156\267\033\154\066\133\301"
  "\330\341\341\141\030\033\033\373\157\166\216\124\052\225\340\127"
  "\146\061\321\150\064\242\176\136\074\036\207\371\371\171\230\231"
  "\231\051\030\253\323\351\140\171\171\031\252\253\253\151\347\250"
  "\130\256\256\256\220\141\030\101\111\056\223\311\204\201\100\200"
  "\066\061\276\202\317\347\103\255\126\053\110\370\350\350\050\336"
  "\335\335\321\166\335\127\360\373\375\250\126\253\005\011\267\333"
  "\355\105\245\161\111\372\073\036\037\037\005\145\021\001\000\335"
  "\156\167\321\273\104\222\344\136\222\311\044\334\337\337\227\374"
  "\101\252\120\050\240\261\261\261\350\366\033\033\033\340\361\170"
  "\362\306\230\114\046\130\131\131\201\356\356\156\161\007\057\306"
  "\055\052\365\016\115\256\315\341\142\111\044\022\005\347\161\206"
  "\141\220\343\070\111\356\262\262\174\071\142\131\026\202\301\140"
  "\316\172\265\132\015\113\113\113\320\320\320\000\164\254\116\044"
  "\362\011\007\000\230\235\235\025\375\075\240\354\245\307\343\361"
  "\274\365\172\275\036\350\000\251\004\051\333\102\017\151\222\016"
  "\242\057\036\362\326\313\144\062\111\373\377\362\021\214\130\054"
  "\006\054\313\226\134\234\122\251\204\266\266\266\242\332\206\303"
  "\141\170\172\172\312\131\337\324\324\004\112\245\362\373\112\047"
  "\150\172\041\351\004\111\007\372\373\313\067\340\371\371\031\130"
  "\226\205\130\054\006\162\271\034\064\032\015\150\265\332\222\054"
  "\237\312\216\144\062\211\116\247\023\053\053\053\377\312\271\114"
  "\114\114\340\365\365\265\244\375\227\235\164\236\347\161\152\152"
  "\052\157\262\313\140\060\140\070\034\046\351\142\161\171\171\051"
  "\050\213\271\272\272\052\331\030\312\356\101\032\010\004\004\305"
  "\355\357\357\323\352\105\054\142\261\230\240\070\277\337\117\322"
  "\305\342\327\257\137\202\342\232\233\233\111\272\130\264\266\266"
  "\012\212\033\030\030\040\351\142\321\323\323\003\014\303\200\220"
  "\023\134\264\116\027\221\140\060\210\066\233\355\303\125\213\132"
  "\255\306\335\335\135\111\373\057\333\054\143\072\235\206\343\343"
  "\143\070\075\075\005\216\343\100\241\120\200\136\257\007\243\321"
  "\010\365\365\365\360\255\363\351\004\315\351\044\235\040\351\044"
  "\235\244\023\044\235\244\023\044\235\244\023\044\235\244\023\071"
  "\371\015\255\254\236\345\313\201\335\225\000\000\000\000\111\105"
  "\116\104\256\102\140\202\000\000\050\165\165\141\171\051" };

static GStaticResource static_resource = { logo_resource_data.data, sizeof (logo_resource_data.data) - 1 /* nul terminator */, NULL, NULL, NULL };

G_MODULE_EXPORT
GResource *logo_get_resource (void);
GResource *logo_get_resource (void)
{
  return g_static_resource_get_resource (&static_resource);
}
/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __G_CONSTRUCTOR_H__
#define __G_CONSTRUCTOR_H__

/*
  If G_HAS_CONSTRUCTORS is true then the compiler support *both* constructors and
  destructors, in a usable way, including e.g. on library unload. If not you're on
  your own.

  Some compilers need #pragma to handle this, which does not work with macros,
  so the way you need to use this is (for constructors):

  #ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
  #pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(my_constructor)
  #endif
  G_DEFINE_CONSTRUCTOR(my_constructor)
  static void my_constructor(void) {
   ...
  }

*/

#ifndef __GTK_DOC_IGNORE__

#if  __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR(_func) static void __attribute__((constructor)) _func (void);
#define G_DEFINE_DESTRUCTOR(_func) static void __attribute__((destructor)) _func (void);

#elif defined (_MSC_VER)

/*
 * Only try to include gslist.h if not already included via glib.h,
 * so that items using gconstructor.h outside of GLib (such as
 * GResources) continue to build properly.
 */
#ifndef __G_LIB_H__
#include "gslist.h"
#endif

#include <stdlib.h>

#define G_HAS_CONSTRUCTORS 1

/* We do some weird things to avoid the constructors being optimized
 * away on VS2015 if WholeProgramOptimization is enabled. First we
 * make a reference to the array from the wrapper to make sure its
 * references. Then we use a pragma to make sure the wrapper function
 * symbol is always included at the link stage. Also, the symbols
 * need to be extern (but not dllexport), even though they are not
 * really used from another object file.
 */

/* We need to account for differences between the mangling of symbols
 * for x86 and x64/ARM/ARM64 programs, as symbols on x86 are prefixed
 * with an underscore but symbols on x64/ARM/ARM64 are not.
 */
#ifdef _M_IX86
#define G_MSVC_SYMBOL_PREFIX "_"
#else
#define G_MSVC_SYMBOL_PREFIX ""
#endif

#define G_DEFINE_CONSTRUCTOR(_func) G_MSVC_CTOR (_func, G_MSVC_SYMBOL_PREFIX)
#define G_DEFINE_DESTRUCTOR(_func) G_MSVC_DTOR (_func, G_MSVC_SYMBOL_PREFIX)

#define G_MSVC_CTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _wrapper(void);              \
  int _func ## _wrapper(void) { _func(); g_slist_find (NULL,  _array ## _func); return 0; } \
  __pragma(comment(linker,"/include:" _sym_prefix # _func "_wrapper")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _wrapper;

#define G_MSVC_DTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _constructor(void);              \
  int _func ## _constructor(void) { atexit (_func); g_slist_find (NULL,  _array ## _func); return 0; } \
   __pragma(comment(linker,"/include:" _sym_prefix # _func "_constructor")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _constructor;

#elif defined(__SUNPRO_C)

/* This is not tested, but i believe it should work, based on:
 * http://opensource.apple.com/source/OpenSSL098/OpenSSL098-35/src/fips/fips_premain.c
 */

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  init(_func)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void);

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  fini(_func)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void);

#else

/* constructors not supported for this compiler */

#endif

#endif /* __GTK_DOC_IGNORE__ */
#endif /* __G_CONSTRUCTOR_H__ */

#ifdef G_HAS_CONSTRUCTORS

#ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(logoresource_constructor)
#endif
G_DEFINE_CONSTRUCTOR(logoresource_constructor)
#ifdef G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(logoresource_destructor)
#endif
G_DEFINE_DESTRUCTOR(logoresource_destructor)

#else
#warning "Constructor not supported on this compiler, linking in resources will not work"
#endif

static void logoresource_constructor (void)
{
  g_static_resource_init (&static_resource);
}

static void logoresource_destructor (void)
{
  g_static_resource_fini (&static_resource);
}
