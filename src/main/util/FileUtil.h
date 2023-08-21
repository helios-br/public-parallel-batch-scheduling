#pragma once

#include <fstream>
#include <string>
#include <cstdio>
#include "../util/StringUtil.h"

using namespace std;

inline string newCopy(string inputFileName) {
	string outputFilename = inputFileName + ".run";
	ifstream src(convertToChar(inputFileName), ios::binary);
	ofstream dst(convertToChar(outputFilename), ios::binary);
	dst << src.rdbuf();
	src.close();
	dst.close();
	return outputFilename;
}

inline void removeFile(string fileName) {
	remove(convertToChar(fileName));
}
