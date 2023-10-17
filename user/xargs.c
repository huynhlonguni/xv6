#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

char* createArgv(char *buf, int len) {
	len++; //Null Terminator
	char *argv = malloc(sizeof(char) * len);
	memcpy(argv, buf, sizeof(char) * len);
	argv[len] = '\0';
	return argv;
}

int main(int argc, char *argv[]) {
	char *newArgv[MAXARG];
	int newArgc = 0;
	char c;
	char buf[512];
	int bIdx = 0;
	const int skipArgc = 1; //skip argv[0] = "xargs"
	const int defaultArgc = argc - skipArgc;
	int i;

	if (argc < 2) {
		printf("xargs <command> [args..]\n");
		exit(0);
	}

	newArgc = defaultArgc;

	memcpy(newArgv, &argv[skipArgc], sizeof(char*) * newArgc);

	while (read(0, &c, sizeof(char)) == sizeof(char)) {
		if (c == '\n') {
			newArgv[newArgc++] = createArgv(buf, bIdx);
			bIdx = 0; //reset

			if (fork() == 0) { //in child
				if (exec(argv[1], newArgv) == -1)
					printf("Failed to run command %s\n", argv[1]);
			}
			else { //in parent
				wait(0);

				for (i = defaultArgc; i < newArgc; i++)
					free(newArgv[i]);

				newArgc = defaultArgc;
			}
		}
		else if (c == ' ') {
			newArgv[newArgc++] = createArgv(buf, bIdx);
			bIdx = 0; //reset
		}
		else {
			buf[bIdx++] = c;
		}
	}

	exit(0);
}
