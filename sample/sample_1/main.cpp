#include <iostream>
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>


#include "LuaBankAccount.hpp"

using namespace std;

int main(int _argc, const char *_argv[]) {
    if (_argc == 2) {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        luaopen_BankAccount(L);
        if (luaL_dofile(L, _argv[1])) {
            std::cout << lua_tostring(L, -1) << std::endl;
        }
        lua_close(L);
    }
    return 0;
}
