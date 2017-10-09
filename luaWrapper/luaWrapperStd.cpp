/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#ifndef LUA_WRAPPER_STD_H__
#define LUA_WRAPPER_STD_H__

#include <etk/String.hpp>

#include <lua/lua.h>
#include <luaWrapper/luaWrapper.hpp>
#include <luaWrapper/luaWrapperUtil.hpp>

/**
 * LuaWrapper knows about primitive types like ints and floats, but it doesn't
 * know about things like etk::Strings or other more complicated types.
 * Sometimes, rather than register the type with LuaWrapper, it's easier to
 * be able to convert it to and from Lua's primitive types, like strings or
 * tables.
 *
 * To do this, you must write luaU_check, luaU_to and luaU_push functions for
 * your type. You don't always need all three, it depends on if you're pushing
 * objects to Lua, getting objects from Lua, or both.
 *
 * This example uses etk::String, but if you have other custom string types it
 * should be easy to write versions of those functions too
 */
template<> etk::String luaU_check<etk::String>(lua_State* _L, int _index) {
    return etk::String(luaL_checkstring(_L, _index));
}
template<> etk::String luaU_to<etk::String>(lua_State* _L, int _index) {
    return etk::String(lua_tostring(_L, _index));
}
template<> void luaU_push<etk::String>(lua_State* _L, const etk::String& _val) {
    lua_pushstring(_L, _val.c_str());
}

template<> bool luaU_check<bool>(lua_State* _L, int _index) {
	return lua_toboolean(_L, _index) != 0;
}
template<> bool luaU_to<bool>(lua_State* _L, int _index) {
	return lua_toboolean(_L, _index) != 0;
}
template<> void luaU_push<bool>(lua_State* _L, const bool& _value) {
	lua_pushboolean(_L, _value);
}

template<> const char* luaU_check<const char*>(lua_State* _L, int _index) {
	return luaL_checkstring(_L, _index);
}
template<> const char* luaU_to<const char*>(lua_State* _L, int _index) {
	return lua_tostring(_L, _index);
}
template<> void luaU_push<const char*>(lua_State* _L, const char* const& _value) {
	lua_pushstring(_L, _value);
}

template<> const char* const luaU_check<const char* const>(lua_State* _L, int _index) {
	return luaL_checkstring(_L, _index);
}
template<> const char* const luaU_to<const char* const>(lua_State* _L, int _index) {
	return lua_tostring(_L, _index);
}
template<> void luaU_push<const char* const>(lua_State* _L, const char* const& _value) {
	lua_pushstring(_L, _value);
}

template<> unsigned int luaU_check<unsigned int>(lua_State* _L, int _index) {
	return static_cast<unsigned int>(luaL_checkinteger(_L, _index));
}
template<> unsigned int luaU_to<unsigned int>(lua_State* _L, int _index) {
	return static_cast<unsigned int>(lua_tointeger(_L, _index));
}
template<> void luaU_push<unsigned int>(lua_State* _L, const unsigned int& _value) {
	lua_pushinteger(_L, _value);
}

template<> int luaU_check<int>(lua_State* _L, int _index) {
	return static_cast<int>(luaL_checkinteger(_L, _index));
}
template<> int luaU_to<int>(lua_State* _L, int _index) {
	return static_cast<int>(lua_tointeger(_L, _index));
}
template<> void luaU_push<int>(lua_State* _L, const int& _value) {
	lua_pushinteger(_L, _value);
}

template<> unsigned char luaU_check<unsigned char>(lua_State* _L, int _index) {
	return static_cast<unsigned char>(luaL_checkinteger(_L, _index));
}
template<> unsigned char luaU_to<unsigned char>(lua_State* _L, int _index) {
	return static_cast<unsigned char>(lua_tointeger(_L, _index));
}
template<> void luaU_push<unsigned char>(lua_State* _L, const unsigned char& _value) {
	lua_pushinteger(_L, _value);
}

template<> char luaU_check<char>(lua_State* _L, int _index) {
	return static_cast<char>(luaL_checkinteger(_L, _index));
}
template<> char luaU_to<char>(lua_State* _L, int _index) {
	return static_cast<char>(lua_tointeger(_L, _index));
}
template<> void luaU_push<char>(lua_State* _L, const char& _value) {
	lua_pushinteger(_L, _value);
}

template<> float luaU_check<float>(lua_State* _L, int _index) {
	return static_cast<float>(luaL_checknumber(_L, _index));
}
template<> float luaU_to<float>(lua_State* _L, int _index) {
	return static_cast<float>(lua_tonumber(_L, _index));
}
template<> void luaU_push<float>(lua_State* _L, const float& _value) {
	lua_pushnumber(_L, _value);
}

template<> double luaU_check<double>(lua_State* _L, int _index) {
	return static_cast<double>(luaL_checknumber(_L, _index));
}
template<> double luaU_to<double>(lua_State* _L, int _index) {
	return static_cast<double>(lua_tonumber(_L, _index));
}
template<> void luaU_push<double>(lua_State* _L, const double& _value) {
	lua_pushnumber(_L, _value);
}


#endif
