#pragma once

#include <etk/String.hpp>

#include <lua/lua.h>
#include <luaWrapper/luaWrapper.hpp>
#include <luaWrapper/luaWrapperUtil.hpp>

#include "Vector2D.hpp"

/**
 * These two functions let me convert a simple Vector2D structure into a Lua
 * table holding the x and y values
 */
template<>
Vector2D luaU_check<Vector2D>(lua_State* _L, int _index) {
	return Vector2D(luaU_getfield<float>(_L, _index, "x"),
	                luaU_getfield<float>(_L, _index, "y"));
}

template<>
Vector2D luaU_to<Vector2D>(lua_State* _L, int _index ) {
	return Vector2D(luaU_getfield<float>(_L, _index, "x"),
	                luaU_getfield<float>(_L, _index, "y"));
}

template<>
void luaU_push<Vector2D>(lua_State* _L, const Vector2D& _val) {
	lua_newtable(_L);
	luaU_setfield<float>(_L, -1, "x", _val.x);
	luaU_setfield<float>(_L, -1, "y", _val.y);
}

