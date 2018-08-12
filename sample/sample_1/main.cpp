#include <iostream>
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <test-debug/debug.hpp>
#include <etk/etk.hpp>


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
		TEST_ERROR("missing file...");
		usage();
	}
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_BankAccount(L);
	if (luaL_dofile(L, inputFileName.c_str())) {
		TEST_PRINT(lua_tostring(L, -1));
	}
	lua_close(L);
	TEST_PRINT("END SAMPLE 1");
	return 0;
}
