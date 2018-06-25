#include <iostream>

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include "LuaExample.hpp"

int main(int argc, const char *argv[]) {
    if (argc == 2) {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        luaopen_Example(L);
        if (luaL_dofile(L, argv[1])) {
            std::cout << lua_tostring(L, -1) << std::endl;
        }
        lua_close(L);
    }
    return 0;
}
