#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

char *strdup_printf(const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	int len = vsnprintf(NULL, 0, fmt, arg);
	va_end(arg);
	va_start(arg, fmt);
	char *buffer = (char*)malloc(len + 1);
	if (buffer) {
		vsnprintf(buffer, len + 1, fmt, arg);
	}
	va_end(arg);
	return buffer;
}

bool matchPartNumber(const char *pattern, const char *partNumber) {
	int len = strlen(pattern);
	for (int i = 0; i < len; i++) {
		if (pattern[i] == '.') {
			break;
		}
		if ((tolower(pattern[i]) != 'x') && (toupper(pattern[i]) != toupper(partNumber[i]))) {
			return false;
		}
	}
	return true;
}

char *searchSVD(const char *path, const char *mcuModel) {
	DIR *dir = opendir(path);
	if (dir == NULL) return NULL;
	struct dirent *entry;
	char *fileName = NULL;
	while (entry = readdir(dir)) {
		if (entry->d_name[0] != '.') {
			if (matchPartNumber(entry->d_name, mcuModel)) {
				char *newPath = strdup_printf("%s/%s", path, entry->d_name);
				struct stat s;
				if (stat(newPath, &s) == 0) {
					if (s.st_mode & S_IFDIR) {
						fileName = searchSVD(newPath, mcuModel);
						if (fileName) {
							free(newPath);
							break;
						}
					} else {
						fileName = newPath;
						break;
					}
				}
				free(newPath);
			}
		}
	}
	closedir(dir);
	return fileName;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s mcu-model\n", argv[0]);
		return 0;
	}
	int exitCode = 0;
	char *svdRootDir = strdup_printf("%s/../metadata/svd", dirname(argv[0]));
	const char *mcuModel = argv[1];
	char *svdFileName = searchSVD(svdRootDir, mcuModel);
	if (svdFileName) {
		printf("%s\n", svdFileName);
	} else {
		fprintf(stderr, "SVD file for %s not found\n", mcuModel);
	}
	free(svdFileName);
	free(svdRootDir);
	return exitCode;
}
