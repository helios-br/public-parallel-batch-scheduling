#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
#include "main/Application.cpp"
//#include "test/ApplicationTest.cpp"

using namespace std;

const string version = "1.4";

void handleException(exception_ptr exptr) {
	try {
		if (exptr) {
			rethrow_exception(exptr);
		}
	} catch (exception &e) {
		cout << "Unknown exception: " << e.what() << endl;
		throw runtime_error("Runtime error!");
	}
}

int main(int argc, char **argv) {
	try {
		string fileName = argv[1];
		if (fileName.compare("test") == 0) {
			//runTest();
		} else if (fileName.compare("version") == 0) {
			cout << "Version " << version << endl;
		} else {
			runApplication(argv[1], argv[2], argv[3]);
		}
	} catch (exception &e) {
		cout << endl << "Exception: " << e.what() << endl;
		throw runtime_error("Runtime error!");
	} catch (...) {
		cout << endl << "An exception was thrown!" << endl;
		handleException(current_exception());
	}
	return 0;
}
