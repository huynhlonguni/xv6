#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *name) {
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if ((fd = open(path, O_RDONLY)) < 0) {
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}

	if (fstat(fd, &st) < 0) {
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	if (st.type != T_DIR) {
		fprintf(2, "find: first agrument isn't a directory: %s\n", path);
		close(fd);
		return;
	}

	if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
		printf("find: path too long\n");
		close(fd);
		return;
	}

	strcpy(buf, path);
	p = buf+strlen(buf);
	*p++ = '/';

	while (read(fd, &de, sizeof(de)) == sizeof(de)) {
		if (de.inum == 0)
			continue;
		memmove(p, de.name, DIRSIZ);
		p[DIRSIZ] = 0;
		if (stat(buf, &st) < 0) {
			fprintf(2, "find: cannot stat %s\n", buf);
			continue;
		}
		switch (st.type) {
			case T_DEVICE:
			case T_FILE:
				if (strcmp(de.name, name) == 0)
					printf("%s\n", buf);
				break;

			case T_DIR:
				if (strcmp(de.name, ".") && strcmp(de.name, ".."))
					find(buf, name);
				break;
		}
	}
	close(fd);
}

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("find <search dir> <name>\n");
		exit(0);
	}

	find(argv[1], argv[2]);
	exit(0);
}
