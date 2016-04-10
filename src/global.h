#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#define AT_VALUES at_True, at_Number, at_String, at_Array, at_Function, at_Func, at_Accessor

void AT_init ();
// ================================================================================================
// String
// ================================================================================================
void StringBuffer_add (int letter);
void StringBuffer_cat (Chars str);
Value StringBuffer_close ();
Chars r2s (real value);
word String_hash (Chars key);
Chars String_getCopy (Chars from);
Chars String_cat (Chars a, Chars b);
Chars String_format (Chars format, ...);
Chars String_charAt (Chars self, int index);
Chars String_upperCase (Chars from);
Chars String_lowerCase (Chars from);
Chars String_getDir (Chars filePath);
Chars String_getAbsolutePath (Chars currentDir, Chars filePath);

#endif /* MEMORY_H_INCLUDED */


