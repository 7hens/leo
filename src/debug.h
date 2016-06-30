#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

Value AssocD_set (Assoc self, Value key, Value value);
Value AssocD_get (Assoc self, Value key);
Chars ValueD_toString (Value self);
void ExpD_init ();
Chars ExpD_toString (Exp self);
#endif

