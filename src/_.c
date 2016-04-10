#include "_.h"

int main (int argc, Chars *argv) {
	Value_init();
	AT_init();
	ExpD_init();
	Interpreter_init();
	srand(time(0));
	char currentDir[256];
	Chars absolutePath = NULL;
	Value filePath = NULL;
	if (argc > 1) {
		getcwd(currentDir, 256);
		absolutePath = String_getAbsolutePath(currentDir, argv[1]);
		filePath = Value_string(absolutePath);
		//printf("main: %s\n", absolutePath);
		free(absolutePath);
	}
	Interpreter_eval(filePath);
	return 0;
}

