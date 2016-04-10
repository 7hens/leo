#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#if defined(DEBUG)
#define debug_assert(c) ((c)||printf("debug>> assertion failed: %s, #%d, "#c"\n",__FILE__,__LINE__))
#else
#define debug_assert(c) ((void)0)
#endif /* DEBUG */

Value AssocD_set (Assoc self, Value key, Value value);
Value AssocD_get (Assoc self, Value key);
Chars ValueD_toString (Value self);
void ExpD_init ();
Chars ExpD_toString (Exp self);
#endif

