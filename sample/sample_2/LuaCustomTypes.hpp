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
Vector2D luaWrapper::utils::check<Vector2D>(lua_State* _L, int _index) {
	return Vector2D(luaWrapper::utils::getfield<float>(_L, _index, "x"),
	                luaWrapper::utils::getfield<float>(_L, _index, "y"));
}

template<>
Vector2D luaWrapper::utils::to<Vector2D>(lua_State* _L, int _index ) {
	return Vector2D(luaWrapper::utils::getfield<float>(_L, _index, "x"),
	                luaWrapper::utils::getfield<float>(_L, _index, "y"));
}

template<>
void luaWrapper::utils::push<Vector2D>(lua_State* _L, const Vector2D& _val) {
	lua_newtable(_L);
	luaWrapper::utils::setfield<float>(_L, -1, "x", _val.x);
	luaWrapper::utils::setfield<float>(_L, -1, "y", _val.y);
}

