#ifndef STRING_UTIL_TEST_H
#define STRING_UTIL_TEST_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <assert.h>
#include <algorithm>
#include "../../main/util/StringUtil.h"

using namespace std;

inline void testSplitWithEmptyString() {
	string text = "";
	vector<string> tokens = split(text, ';');
	assert(tokens.size() == 0);
}

inline void testSplitWithString() {
	string text = "10;11;12";
	vector<string> tokens = split(text, ';');
	assert(tokens.size() == 3);
	assert(tokens[0] == "10");
	assert(tokens[1] == "11");
	assert(tokens[2] == "12");
}

inline void testReplace() {
	string text = "Brazil, Argentina, Mexico, Canada";
	assert(replaceAll(text, "Brazil", "Australia").compare("Australia, Argentina, Mexico, Canada") == 0);
	assert(replaceAll(text, ",", "").compare("Australia Argentina Mexico Canada") == 0);
}

inline void testTrim() {
	assert(trim("  ") == "");
	assert(trim(" \n ") == "");
	assert(trim(" 1 2 ") == "1 2");
	assert(trim("  19   58 0.73") == "19   58 0.73");
	assert(ltrim(" 1 2 ") == "1 2 ");
	assert(rtrim(" 1 2 ") == " 1 2");
}

inline void testRemoveBlankSpaces() {
	string text = " 19   58 0.73";
	assert(removeBlankSpaces(text) == "19 58 0.73");
}

inline void runStringUtilTest() {
	cout << "# Running StringUtilTest.h" << endl << endl;

	cout << ".... testSplitWithEmptyString [STARTED]" << endl;
	testSplitWithEmptyString();
	cout << ".... testSplitWithString [STARTED]" << endl;
	testSplitWithString();
	cout << ".... testReplace [STARTED]" << endl;
	testReplace();
	cout << ".... testTrim [STARTED]" << endl;
	testTrim();
	cout << ".... removeBlankSpaces [STARTED]" << endl;
	testRemoveBlankSpaces();

	cout << endl << "[OK] StringUtilTest.h" << endl;
}

#endif
