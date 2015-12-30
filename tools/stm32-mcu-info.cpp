#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <libgen.h>
#include <string>
#include <fstream>
#include <json/json.h>

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

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s mcu-model\n", argv[0]);
		return 0;
	}
	int exitCode = 0;
	char *mcuModel = argv[1];
	for (int i = 0; mcuModel[i]; i++) {
		mcuModel[i] = tolower(mcuModel[i]);
	}
	char *catalogFileName = strdup_printf("%s/../metadata/stm32%c%c_catalog.json", dirname(argv[0]), mcuModel[5], mcuModel[6]);
	for (int i = 0; mcuModel[i]; i++) {
		mcuModel[i] = toupper(mcuModel[i]);
	}
	std::ifstream ifs(catalogFileName, std::ifstream::in);
	Json::Reader reader;
	Json::Value root;
	reader.parse(ifs, root, false);
	Json::Value fields = root["metaData"]["fields"];
	Json::Value records = root["records"];
	const char *partNumberName = NULL;
	const char *flashSizeName = NULL;
	const char *ramSizeName = NULL;
	const char *coreName = NULL;
	for (Json::Value::iterator it = fields.begin(); it != fields.end(); ++it) {
		const char *header = (*it)["header"].asCString();
		const char *name = (*it)["name"].asCString();
		if (strcmp(header, "Part Number") == 0) {
			partNumberName = name;
		} else if (strcmp(header, "FLASH Size (Prog)(kB)") == 0) {
			flashSizeName = name;
		} else if (strcmp(header, "Internal RAM Size(kB)") == 0) {
			ramSizeName = name;
		} else if (strcmp(header, "Core") == 0) {
			coreName = name;
		}
	}
	if (partNumberName && flashSizeName && ramSizeName && coreName) {
		Json::Value::iterator it;
		for (it = records.begin(); it != records.end(); ++it) {
			const char *partNumber = (*it)[partNumberName].asCString();
			const char *flashSize = (*it)[flashSizeName].asCString();
			const char *ramSize = (*it)[ramSizeName].asCString();
			const char *core = (*it)[coreName].asCString();
			if (strcmp(partNumber, mcuModel) == 0) {
				const char *tmp = strstr(core, "Cortex");
				if (tmp) {
					char *tmp2 = strdup(tmp);
					for (int i = 0; tmp2[i]; i++) {
						tmp2[i] = tolower(tmp2[i]);
					}
					char *tmp3 = strdup(mcuModel);
					tmp3[7] = 0;
					printf("-mcpu=%s -D_ROM=0x%x -D_RAM=0x%x -D_ROM_OFF=0x08000000 -D_RAM_OFF=0x20000000 -D%s -mthumb\n", tmp2,
					atoi(flashSize) * 1024, atoi(ramSize) * 1024, tmp3);
					free(tmp3);
					free(tmp2);
					break;
				}
			}
		}
		if (it == records.end()) {
			fprintf(stderr, "%s not found in catalog\n", mcuModel);
			exitCode = -2;
		}
	} else {
		fprintf(stderr, "File \"%s\" corrupted\n", catalogFileName);
		exitCode = -1;
	}
	free(catalogFileName);
	return exitCode;
}
