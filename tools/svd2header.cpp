#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <tinyxml.h>

struct FieldInfo {
	const char *name;
	const char *desc;
	const char *offset;
	const char *width;
};

struct RegInfo {
	const char *name;
	const char *desc;
	const char *offset;
	std::vector<FieldInfo> fields;
};

struct InterruptInfo {
	const char *name;
	const char *desc;
	const char *value;
};

struct PeriphInfo {
	const char *name;
	const char *groupName;
	const char *desc;
	const char *baseAddr;
	std::vector<RegInfo> regs;
	std::vector<InterruptInfo> interrupts;
};

int checkSvd(TiXmlDocument *xmlDoc) {
	TiXmlElement *rootNode = xmlDoc->FirstChildElement("device");
	if (rootNode) {
		TiXmlElement *peripheralsNode = rootNode->FirstChildElement("peripherals");
		if (peripheralsNode) {
			return 0;
		} else {
			fprintf(stderr, "Failed to find <peripherals> XML node\n");
			return -4;
		}
	} else {
		fprintf(stderr, "Failed to find <device> XML node\n");
		return -3;
	}
}

const char *getNodeAttr(TiXmlElement *node, const char *attrName, const char *defValue) {
	TiXmlElement *attrNode = node->FirstChildElement(attrName);
	if (attrNode) {
		return attrNode->GetText();
	} else {
		return defValue;
	}
}

int convertSvdToHeader(TiXmlDocument *xmlDoc, FILE *output) {
	TiXmlElement *rootNode = xmlDoc->FirstChildElement("device");
	TiXmlElement *peripheralsNode = rootNode->FirstChildElement("peripherals");
	TiXmlElement *peripheralNode = peripheralsNode->FirstChildElement("peripheral");
	std::map<std::string, PeriphInfo> periph;
	while (peripheralNode != NULL) {
		const char *periphParent = peripheralNode->Attribute("derivedFrom");
		const char *periphName = getNodeAttr(peripheralNode, "name", NULL);
		const char *periphGroupName = getNodeAttr(peripheralNode, "groupName", NULL);
		const char *periphDesc = getNodeAttr(peripheralNode, "description", NULL);
		const char *periphBaseAddr = getNodeAttr(peripheralNode, "baseAddress", NULL);
		if (periphName && periphBaseAddr) {
			PeriphInfo info = {};
			if (periphParent) {
				info = periph[std::string(periphParent)];
			}
			info.name = periphName;
			if (periphDesc) {
				info.desc = periphDesc;
			}
			info.baseAddr = periphBaseAddr;
			if (periphGroupName) {
				info.groupName = periphGroupName;
			}
			TiXmlElement *registersNode = peripheralNode->FirstChildElement("registers");
			if (registersNode) {
				TiXmlElement *registerNode = registersNode->FirstChildElement("register");
				while (registerNode != NULL) {
					const char *regName = getNodeAttr(registerNode, "name", NULL);
					const char *regDesc = getNodeAttr(registerNode, "description", NULL);
					const char *regOffset = getNodeAttr(registerNode, "addressOffset", NULL);
					if (regName && regOffset) {
						RegInfo regInfo = {};
						regInfo.name = regName;
						regInfo.desc = regDesc;
						regInfo.offset = regOffset;
						TiXmlElement *fieldsNode = registerNode->FirstChildElement("fields");
						if (fieldsNode) {
							TiXmlElement *fieldNode = fieldsNode->FirstChildElement("field");
							while (fieldNode != NULL) {
								const char *fieldName = getNodeAttr(fieldNode, "name", NULL);
								const char *fieldDesc = getNodeAttr(fieldNode, "description", NULL);
								const char *fieldOffset = getNodeAttr(fieldNode, "bitOffset", NULL);
								const char *fieldWidth = getNodeAttr(fieldNode, "bitWidth", NULL);
								if (fieldName && fieldOffset && fieldWidth) {
									FieldInfo fieldInfo = {};
									fieldInfo.name = fieldName;
									fieldInfo.desc = fieldDesc;
									fieldInfo.offset = fieldOffset;
									fieldInfo.width = fieldWidth;
									regInfo.fields.push_back(fieldInfo);
								} else {
									fprintf(stderr, "Warning: <field> node without <name>, <bitOffset> or <bitWidth>"
										" (in %s register of %s peripheral)\n", regName, periphName);
								}
								fieldNode = fieldNode->NextSiblingElement("field");
							}
						}
						info.regs.push_back(regInfo);
					} else {
						fprintf(stderr, "Warning: <register> node without <name> or <addressOffset> (in %s peripheral)\n", periphName);
					}
					registerNode = registerNode->NextSiblingElement("register");
				}
			}
			TiXmlElement *interruptNode = peripheralNode->FirstChildElement("interrupt");
			while (interruptNode != NULL) {
				const char *name = getNodeAttr(interruptNode, "name", NULL);
				const char *desc = getNodeAttr(interruptNode, "description", NULL);
				const char *value = getNodeAttr(interruptNode, "value", NULL);
				if (name && value) {
					InterruptInfo interruptInfo;
					interruptInfo.name = name;
					interruptInfo.desc = desc;
					interruptInfo.value = value;
					info.interrupts.push_back(interruptInfo);
				} else {
					fprintf(stderr, "Warning: <interrupt> node without <name> or <value> (in %s peripheral)\n", periphName);
				}
				interruptNode = interruptNode->NextSiblingElement("interrupt");
			}
			periph[periphName] = info;
		} else {
			fprintf(stderr, "Warning: <peripheral> node without <name> or <baseAddress>\n");
		}
		peripheralNode = peripheralNode->NextSiblingElement("peripheral");
	}
	const char *deviceName = getNodeAttr(rootNode, "name", NULL);
	if (deviceName != NULL) {
		fprintf(output, "#define %s\n\n", deviceName);
	} else {
		fprintf(stderr, "Warning: <device> node without <name>\n");
	}
	for (std::map<std::string, PeriphInfo>::iterator it = periph.begin(); it != periph.end(); ++it) {
		fprintf(output, "/* %s - %s */\n", it->second.name, it->second.desc);
		fprintf(output, "#define %s_BASE %s\n\n", it->second.name, it->second.baseAddr);
		for (std::vector<RegInfo>::iterator regIt = it->second.regs.begin(); regIt != it->second.regs.end(); ++regIt) {
			fprintf(output, "#define %s_%s (*(volatile uint32_t*)(%s_BASE + %s)) /* %s */\n", it->second.name, regIt->name, it->second.name,
				regIt->offset, regIt->desc);
		}
		if (it->second.interrupts.size() > 0) {
			fprintf(output, "\n");
			for (std::vector<InterruptInfo>::iterator intIt = it->second.interrupts.begin(); intIt != it->second.interrupts.end(); ++intIt) {
				int value = atoi(intIt->value);
				fprintf(output, "#define %s_interrupt %i /* %s */\n", intIt->name, value, intIt->desc);
			}
		}
		fprintf(output, "\n");
		for (std::vector<RegInfo>::iterator regIt = it->second.regs.begin(); regIt != it->second.regs.end(); ++regIt) {
			fprintf(output, "/* %s_%s fields */\n", it->second.name, regIt->name);
			for (std::vector<FieldInfo>::iterator fIt = regIt->fields.begin(); fIt != regIt->fields.end(); ++fIt) {
				int offset = atoi(fIt->offset);
				int width = atoi(fIt->width);
				if (width > 1) {
					fprintf(output, "#define %s_%s_%s_OFFSET %i /* %s */\n", it->second.name, regIt->name, fIt->name, offset, fIt->desc);
					fprintf(output, "#define %s_%s_%s_WIDTH %i\n", it->second.name, regIt->name, fIt->name, width);
					fprintf(output, "#define %s_%s_%s_MASK 0x%08llX\n", it->second.name, regIt->name, fIt->name,
						(((uint64_t)1 << width) - 1) << offset);
				} else {
					fprintf(output, "#define %s_%s_%s (1 << %i) /* %s */\n", it->second.name, regIt->name, fIt->name, offset, fIt->desc);
				}
			}
			fprintf(output, "\n");
		}
	}
	return 0;
}

int main(int argc, char **argv) {
	int exitCode = 0;
	if (argc != 3) {
		fprintf(stderr, "Usage: %s input-file.svd output-file.h\n", argv[0]);
	} else {
		const char *inputFileName = argv[1];
		const char *outputFileName = argv[2];
		TiXmlDocument *xmlDoc = new TiXmlDocument(inputFileName);
		if (xmlDoc->LoadFile()) {
			exitCode = checkSvd(xmlDoc);
			if (exitCode == 0) {
				FILE *outputFile = fopen(outputFileName, "w");
				if (outputFile) {
					const char *baseName = basename(outputFileName);
					int len = strlen(baseName);
					char normalizedFileName[len + 1];
					for (int i = 0; i <= len; i++) {
						char chr = baseName[i];
						if (isdigit(chr) || isalpha(chr) || (chr == 0)) {
							normalizedFileName[i] = toupper(chr);
						} else {
							normalizedFileName[i] = '_';
						}
					}
					fprintf(outputFile, "#ifndef __%s__\n", normalizedFileName);
					fprintf(outputFile, "#define __%s__\n\n", normalizedFileName);
					exitCode = convertSvdToHeader(xmlDoc, outputFile);
					fprintf(outputFile, "\n#endif\n");
					fclose(outputFile);
				} else {
					fprintf(stderr, "Failed to open output file \"%s\" for writing\n", outputFileName);
					exitCode = -2;
				}
			}
		} else {
			fprintf(stderr, "Failed to load XML file \"%s\": %s\n", inputFileName, xmlDoc->ErrorDesc());
			exitCode = -1;
		}
		delete xmlDoc;
	}
	return exitCode;
}
