cd Win_FlexBison
win_bison -yd -o   ..\..\src\yacc.c   ..\..\src\_.y
win_flex      -o   ..\..\src\lex.c    ..\..\src\_.l

cd ..\..
gcc -o build\yasc src\*.c

yasc ..\test\main.txt
pause
