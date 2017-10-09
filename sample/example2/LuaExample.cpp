#include <iostream>
#include <etk/String.hpp>

#include <lua/lua.h>

#include <luaWrapper/luaWrapper.hpp>
#include <luaWrapper/luaWrapperUtil.hpp>

#include "LuaCustomTypes.hpp"
#include "Example.hpp"

static int Example_PrintMe(lua_State* L) {
	Example* ex = luaW_check<Example>(L, 1);
	std::cout << "Example="
	          << "{m_boolean=" << ex->m_boolean
	          << ",m_integer=" << ex->m_integer
	          << ",m_uinteger=" << ex->m_uinteger
	          << ",m_string='" << ex->m_cstring << "'"
	          << ",m_cppstring='" << ex->m_cppstring << "'"
	          << ",m_number=" << ex->m_number
	          << ",m_floatnumber=" << ex->m_floatnumber
	          << ",m_vec={x=" << ex->m_vec.x << ",y=" << ex->m_vec.y <<"}}"  << std::endl;
	return 0;
}

static luaL_Reg Example_metatable[] = {
	// This function is
	{ "PrintMe", Example_PrintMe },
	
	// In many cases, all you need are simple getter or setter functions This
	// was such a common pattern for me that I wrote a huge set of templates
	// that generate these getter and setter functions for me. This is an
	// example of how they can be used
	//
	// Because everything in the Example class is marked public, you can access
	// them directly with these templates.
	//
	//                       Class    | data type | class member
	{ "GetBoolean", luaU_get<Example,   bool,       &Example::m_boolean> },
	{ "SetBoolean", luaU_set<Example,   bool,       &Example::m_boolean> },
	{ "Boolean", luaU_getset<Example,   bool,       &Example::m_boolean> },
	
	{ "GetInteger", luaU_get<Example, int, &Example::m_integer> },
	{ "SetInteger", luaU_set<Example, int, &Example::m_integer> },
	{ "Integer", luaU_getset<Example, int, &Example::m_integer> },
	
	{ "GetUInteger", luaU_get<Example, unsigned int, &Example::m_uinteger> },
	{ "SetUInteger", luaU_set<Example, unsigned int, &Example::m_uinteger> },
	{ "UInteger", luaU_getset<Example, unsigned int, &Example::m_uinteger> },
	
	{ "GetCString", luaU_get<Example, const char*, &Example::m_cstring> },
	{ "SetCString", luaU_set<Example, const char*, &Example::m_cstring> },
	{ "CString", luaU_getset<Example, const char*, &Example::m_cstring> },
	
	{ "GetCPPString", luaU_get<Example, etk::String, &Example::m_cppstring> },
	{ "SetCPPString", luaU_set<Example, etk::String, &Example::m_cppstring> },
	{ "CPPString", luaU_getset<Example, etk::String, &Example::m_cppstring> },
	
	{ "GetVec", luaU_get<Example, Vector2D, &Example::m_vec> },
	{ "SetVec", luaU_set<Example, Vector2D, &Example::m_vec> },
	{ "Vec", luaU_getset<Example, Vector2D, &Example::m_vec> },
	
	{ "GetNumber", luaU_get<Example, double, &Example::m_number> },
	{ "SetNumber", luaU_set<Example, double, &Example::m_number> },
	{ "Number", luaU_getset<Example, double, &Example::m_number> },
	
	{ "GetFloatNumber", luaU_get<Example, float, &Example::m_floatnumber> },
	{ "SetFloatNumber", luaU_set<Example, float, &Example::m_floatnumber> },
	{ "FloatNumber", luaU_getset<Example, float, &Example::m_floatnumber> },
	
	{ "GetPtr", luaU_get<Example, Example, &Example::m_ptr> },
	{ "SetPtr", luaU_set<Example, Example, &Example::m_ptr> },
	{ "Ptr", luaU_getset<Example, Example, &Example::m_ptr> },
	
	// The getters and setters above work on member variables directly, but
	// sometimes all you have are getter and setter functions instead of
	// variables. You can still automate the creation of Getter and Setter Lua
	// function wrappers.
	
	// Normally it would be silly to have getter and setter functions for both
	// the member variable AND the getter/setter function, I've included both
	// here as an example of how it works
	
	// Example of member access through getters and setters
	//                           Class    | data type | getter
	//                           Class    | data type | setter
	//                           Class    | data type | getter               | setter
	{ "GetBooleanFunc", luaU_get<Example,   bool, &Example::GetBoolean> },
	{ "SetBooleanFunc", luaU_set<Example,   bool, &Example::SetBoolean> },
	{ "BooleanFunc", luaU_getset<Example,   bool, &Example::GetBoolean, &Example::SetBoolean> },
	
	{ "GetIntegerFunc", luaU_get<Example, int, &Example::GetInteger> },
	{ "SetIntegerFunc", luaU_set<Example, int, &Example::SetInteger> },
	{ "IntegerFunc", luaU_getset<Example, int, &Example::GetInteger, &Example::SetInteger> },
	
	{ "GetUIntegerFunc", luaU_get<Example, unsigned int, &Example::GetUInteger> },
	{ "SetUIntegerFunc", luaU_set<Example, unsigned int, &Example::SetUInteger> },
	{ "UIntegerFunc", luaU_getset<Example, unsigned int, &Example::GetUInteger, &Example::SetUInteger> },
	
	{ "GetCStringFunc", luaU_get<Example, const char*, &Example::GetCString> },
	{ "SetCStringFunc", luaU_set<Example, const char*, &Example::SetCString> },
	{ "CStringFunc", luaU_getset<Example, const char*, &Example::GetCString, &Example::SetCString> },
	
	{ "GetNumberFunc", luaU_get<Example, double, &Example::GetNumber> },
	{ "SetNumberFunc", luaU_set<Example, double, &Example::SetNumber> },
	{ "NumberFunc", luaU_getset<Example, double, &Example::GetNumber, &Example::SetNumber> },
	
	{ "GetFloatNumberFunc", luaU_get<Example, float, &Example::GetFloatNumber> },
	{ "SetFloatNumberFunc", luaU_set<Example, float, &Example::SetFloatNumber> },
	{ "FloatNumberFunc", luaU_getset<Example, float, &Example::GetFloatNumber, &Example::SetFloatNumber> },
	
	{ "GetPtrFunc", luaU_get<Example, Example, &Example::GetPtr> },
	{ "SetPtrFunc", luaU_set<Example, Example, &Example::SetPtr> },
	{ "PtrFunc", luaU_getset<Example, Example, &Example::GetPtr, &Example::SetPtr> },
	
	// In order to use luaU_get and luaU_set on non-primitive types, you must define luaU_to
	// and luaU_check for that type.
	// See LuaCustomTypes.hpp for an example involving etk::String and Vector2D
	{ "GetCPPStringFunc", luaU_get<Example, etk::String, &Example::GetCPPString> },
	{ "SetCPPStringFunc", luaU_set<Example, etk::String, &Example::SetCPPString> },
	{ "CPPStringFunc", luaU_getset<Example, etk::String, &Example::GetCPPString, &Example::SetCPPString> },
	
	{ "GetVecFunc", luaU_get<Example, Vector2D, &Example::GetVec> },
	{ "SetSetFunc", luaU_set<Example, Vector2D, &Example::SetVec> },
	{ "VecFunc", luaU_getset<Example, Vector2D, &Example::GetVec, &Example::SetVec> },
	
	{ "DoSomething", luaU_func(&Example::DoSomething) },
	{ "DoSomething2", luaU_func(&Example::DoSomething2) },
	
	//{ "DoSomethingElse1", luaU_funcsig(int, Example, DoSomethingElse, int, int) },
	//{ "DoSomethingElse2", luaU_funcsig(int, Example, DoSomethingElse, float) },
	{ NULL, NULL }
};

int luaopen_Example(lua_State* _L)
{
    luaW_register<Example>(_L, "Example", NULL, Example_metatable);
    return 1;
}

