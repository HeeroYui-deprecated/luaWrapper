/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/String.hpp>

#include <lua/lua.h>
#include <luaWrapper/luaWrapper.hpp>
#include <luaWrapper/luaWrapperUtil.hpp>
#include <luaWrapper/debug.hpp>

namespace luaWrapper {
	namespace utils {
		/**
		 * LuaWrapper knows about primitive types like ints and floats, but it doesn't
		 * know about things like etk::Strings or other more complicated types.
		 * Sometimes, rather than register the type with LuaWrapper, it's easier to
		 * be able to convert it to and from Lua's primitive types, like strings or
		 * tables.
		 *
		 * To do this, you must write luaWrapper::utils::check, luaWrapper::utils::to and luaWrapper::utils::push functions for
		 * your type. You don't always need all three, it depends on if you're pushing
		 * objects to Lua, getting objects from Lua, or both.
		 *
		 * This example uses etk::String, but if you have other custom string types it
		 * should be easy to write versions of those functions too
		 */
		template<> etk::String check<etk::String>(lua_State* _luaState, int _index) {
		    return etk::String(luaL_checkstring(_luaState, _index));
		}
		template<> etk::String to<etk::String>(lua_State* _luaState, int _index) {
		    return etk::String(lua_tostring(_luaState, _index));
		}
		template<> void push<etk::String>(lua_State* _luaState, const etk::String& _val) {
		    lua_pushstring(_luaState, _val.c_str());
		}
		
		template<> bool check<bool>(lua_State* _luaState, int _index) {
			return lua_toboolean(_luaState, _index) != 0;
		}
		template<> bool to<bool>(lua_State* _luaState, int _index) {
			return lua_toboolean(_luaState, _index) != 0;
		}
		template<> void push<bool>(lua_State* _luaState, const bool& _value) {
			lua_pushboolean(_luaState, _value);
		}
		
		template<> const char* check<const char*>(lua_State* _luaState, int _index) {
			return luaL_checkstring(_luaState, _index);
		}
		template<> const char* to<const char*>(lua_State* _luaState, int _index) {
			return lua_tostring(_luaState, _index);
		}
		template<> void push<const char*>(lua_State* _luaState, const char* const& _value) {
			lua_pushstring(_luaState, _value);
		}
		
		template<> const char* const check<const char* const>(lua_State* _luaState, int _index) {
			return luaL_checkstring(_luaState, _index);
		}
		template<> const char* const to<const char* const>(lua_State* _luaState, int _index) {
			return lua_tostring(_luaState, _index);
		}
		template<> void push<const char* const>(lua_State* _luaState, const char* const& _value) {
			lua_pushstring(_luaState, _value);
		}
		
		template<> unsigned int check<unsigned int>(lua_State* _luaState, int _index) {
			return static_cast<unsigned int>(luaL_checkinteger(_luaState, _index));
		}
		template<> unsigned int to<unsigned int>(lua_State* _luaState, int _index) {
			return static_cast<unsigned int>(lua_tointeger(_luaState, _index));
		}
		template<> void push<unsigned int>(lua_State* _luaState, const unsigned int& _value) {
			lua_pushinteger(_luaState, _value);
		}
		
		template<> int check<int>(lua_State* _luaState, int _index) {
			return static_cast<int>(luaL_checkinteger(_luaState, _index));
		}
		template<> int to<int>(lua_State* _luaState, int _index) {
			return static_cast<int>(lua_tointeger(_luaState, _index));
		}
		template<> void push<int>(lua_State* _luaState, const int& _value) {
			lua_pushinteger(_luaState, _value);
		}
		
		template<> unsigned char check<unsigned char>(lua_State* _luaState, int _index) {
			return static_cast<unsigned char>(luaL_checkinteger(_luaState, _index));
		}
		template<> unsigned char to<unsigned char>(lua_State* _luaState, int _index) {
			return static_cast<unsigned char>(lua_tointeger(_luaState, _index));
		}
		template<> void push<unsigned char>(lua_State* _luaState, const unsigned char& _value) {
			lua_pushinteger(_luaState, _value);
		}
		
		template<> char check<char>(lua_State* _luaState, int _index) {
			return static_cast<char>(luaL_checkinteger(_luaState, _index));
		}
		template<> char to<char>(lua_State* _luaState, int _index) {
			return static_cast<char>(lua_tointeger(_luaState, _index));
		}
		template<> void push<char>(lua_State* _luaState, const char& _value) {
			lua_pushinteger(_luaState, _value);
		}
		
		template<> float check<float>(lua_State* _luaState, int _index) {
			return static_cast<float>(luaL_checknumber(_luaState, _index));
		}
		template<> float to<float>(lua_State* _luaState, int _index) {
			return static_cast<float>(lua_tonumber(_luaState, _index));
		}
		template<> void push<float>(lua_State* _luaState, const float& _value) {
			lua_pushnumber(_luaState, _value);
		}
		
		template<> double check<double>(lua_State* _luaState, int _index) {
			return static_cast<double>(luaL_checknumber(_luaState, _index));
		}
		template<> double to<double>(lua_State* _luaState, int _index) {
			return static_cast<double>(lua_tonumber(_luaState, _index));
		}
		template<> void push<double>(lua_State* _luaState, const double& _value) {
			lua_pushnumber(_luaState, _value);
		}
	}
}

