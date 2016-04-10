
// if not == unless
if (cond) { a = b; } else { a = c; }
if (cond) a = b; else a = c;
a = ? cond { b } :: { c }
a = ? cond { b :: c }
a = (b :: c ? cond)

// while not == until
arr = while (cond) { a = b; }
arr = % cond { a = b }
arr = (a = b % cond)

// for, foreach
arr = for (i = 1; i < length; i += 1) { doSomething(); }
arr = % i => 1 ... length { doSomething() }
arr = doSomething() % i => 1 ... length
