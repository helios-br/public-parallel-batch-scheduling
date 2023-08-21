#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>

using namespace std;

inline char* convertToChar(string text) {
	char *charArray = new char(text.size());
	for (int i = 0; i < text.size(); i++) {
		charArray[i] = text[i];
	}
	return charArray;
}

inline vector<string> split(string text, char delimiter) {
	vector<string> tokens;
	string token;
	istringstream tokenStream(text);
	while (getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

inline string replace(string &originalString, string replacedText, string newText) {
	size_t initialPosition = originalString.find(replacedText);
	if (initialPosition != string::npos) {
		originalString.replace(initialPosition, replacedText.length(), newText);
	}
	return originalString;
}

inline string replaceAll(string &text, string replacedString, string newString) {
	size_t pos = text.find(replacedString);
	while (pos != std::string::npos) {
		text.replace(pos, replacedString.size(), newString);
		pos = text.find(replacedString, pos + newString.size());
	}
	return text;
}

inline string ltrim(const string &s) {
    return regex_replace(s, regex("^\\s+"), string(""));
}

inline string rtrim(const string &s) {
    return regex_replace(s, regex("\\s+$"), string(""));
}

inline string trim(const string &s) {
    return ltrim(rtrim(s));
}

inline string removeBlankSpaces(string &text) {
	text = replaceAll(text, "     ", " ");
	text = replaceAll(text, "    ", " ");
	text = replaceAll(text, "   ", " ");
	text = replaceAll(text, "  ", " ");
	return trim(text);
}
