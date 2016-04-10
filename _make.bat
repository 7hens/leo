win_bison -yd -o   src\yacc.c   _.y
win_flex      -o   src\lex.c    _.l
gcc -o atc src\*.c
atc test\main.txt
pause
