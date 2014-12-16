#ifndef LUA_CUSTOM_TYPES_H__
#define LUA_CUSTOM_TYPES_H__

#include <string>

#include <lua/lua.h>
#include <luaWrapper/luaWrapper.hpp>
#include <luaWrapper/luaWrapperUtil.hpp>

#include "Vector2D.hpp"

/**
 * LuaWrapper knows about primitive types like ints and floats, but it doesn't
 * know about things like std::strings or other more complicated types.
 * Sometimes, rather than register the type with LuaWrapper, it's easier to
 * be able to convert it to and from Lua's primitive types, like strings or
 * tables.
 *
 * To do this, you must write luaU_check, luaU_to and luaU_push functions for
 * your type. You don't always need all three, it depends on if you're pushing
 * objects to Lua, getting objects from Lua, or both.
 *
 * This example uses std::string, but if you have other custom string types it
 * should be easy to write versions of those functions too
 */
template<> struct luaU_Impl<std::string> {
    static std::string luaU_check(lua_State* _L, int _index) {
        return std::string(luaL_checkstring(_L, _index));
    }
    static std::string luaU_to(lua_State* _L, int _index) {
        return std::string(lua_tostring(_L, _index));
    }
    static void luaU_push(lua_State* _L, const std::string& _val) {
        lua_pushstring(_L, _val.c_str());
    }
};

/**
 * These two functions let me convert a simple Vector2D structure into a Lua
 * table holding the x and y values
 */
template<> struct luaU_Impl<Vector2D> {
    static Vector2D luaU_check(lua_State* _L, int _index) {
        return Vector2D(
            luaU_getfield<float>(_L, _index, "x"),
            luaU_getfield<float>(_L, _index, "y"));
    }
    static Vector2D luaU_to(lua_State* _L, int _index ) {
        return Vector2D(
            luaU_getfield<float>(_L, _index, "x"),
            luaU_getfield<float>(_L, _index, "y"));
    }
    static void luaU_push(lua_State* _L, const Vector2D& _val) {
        lua_newtable(_L);
        luaU_setfield<float>(_L, -1, "x", _val.x);
        luaU_setfield<float>(_L, -1, "y", _val.y);
    }
};

#endif