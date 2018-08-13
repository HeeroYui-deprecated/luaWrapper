#include <test-debug/debug.hpp>
#include <etk/etk.hpp>
#include <luaWrapper/luaWrapper.hpp>


#include "LuaBankAccount.hpp"

static void usage() {
	TEST_PRINT("Help:");
	TEST_PRINT("    ./xxx [OPTIONS] ---");
	TEST_PRINT("        --file=XXX         File to execute");
	exit(0);
}

int main(int _argc, const char *_argv[]) {
	TEST_PRINT("START SAMPLE 1");
	etk::init(_argc, _argv);
	etk::String inputFileName;
	for (int32_t iii=0; iii<_argc ; ++iii) {
		etk::String data = _argv[iii];
		if (    data == "-h"
		     || data == "--help") {
			usage();
		} else if (data.startWith("--file=") == true) {
			inputFileName = etk::String(&_argv[iii][7]);
		}
	}
	if (inputFileName.empty() == true) {
		TEST_WARNING("missing file... ==> select default one");
		inputFileName = "DATA:example1.lua";
	}
	{
		luaWrapper::Lua lua;
		luaopen_BankAccount(lua);
		lua.executeFile(inputFileName);
	}
	TEST_PRINT("END SAMPLE 1");
	return 0;
}
