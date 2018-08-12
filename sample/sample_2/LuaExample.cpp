#include <etk/String.hpp>

#include <lua/lua.h>

#include <luaWrapper/luaWrapper.hpp>
#include <luaWrapper/luaWrapperUtil.hpp>

#include "LuaCustomTypes.hpp"
#include "Example.hpp"
#include <test-debug/debug.hpp>

static int Example_PrintMe(lua_State* L) {
	ememory::SharedPtr<Example> ex = luaW_check<Example>(L, 1);
	TEST_PRINT( "Example=");
	TEST_PRINT( "    m_boolean=" << ex->m_boolean);
	TEST_PRINT( "    m_integer=" << ex->m_integer);
	TEST_PRINT( "    m_uinteger=" << ex->m_uinteger);
	TEST_PRINT( "    m_string='" << ex->m_cstring << "'");
	TEST_PRINT( "    m_cppstring='" << ex->m_cppstring << "'");
	TEST_PRINT( "    m_number=" << ex->m_number);
	TEST_PRINT( "    m_floatnumber=" << ex->m_floatnumber);
	TEST_PRINT( "    m_vec={x=" << ex->m_vec.x << ",y=" << ex->m_vec.y <<"}");
	return 0;
}

static luaL_Reg Example_metatable[] = {
	// This function is
	{ "printMe", Example_PrintMe },
	
	// In many cases, all you need are simple getter or setter functions This
	// was such a common pattern for me that I wrote a huge set of templates
	// that generate these getter and setter functions for me. This is an
	// example of how they can be used
	//
	// Because everything in the Example class is marked public, you can access
	// them directly with these templates.
	//
	//                       Class    | data type | class member
	{ "getBoolean", luaU_get<Example,   bool,       &Example::m_boolean> },
	{ "setBoolean", luaU_set<Example,   bool,       &Example::m_boolean> },
	{ "m_boolean", luaU_getset<Example,   bool,       &Example::m_boolean> },
	
	{ "getInteger", luaU_get<Example, int, &Example::m_integer> },
	{ "setInteger", luaU_set<Example, int, &Example::m_integer> },
	{ "m_integer", luaU_getset<Example, int, &Example::m_integer> },
	
	{ "getUInteger", luaU_get<Example, unsigned int, &Example::m_uinteger> },
	{ "setUInteger", luaU_set<Example, unsigned int, &Example::m_uinteger> },
	{ "m_uinteger", luaU_getset<Example, unsigned int, &Example::m_uinteger> },
	
	{ "getCString", luaU_get<Example, const char*, &Example::m_cstring> },
	{ "setCString", luaU_set<Example, const char*, &Example::m_cstring> },
	{ "m_string", luaU_getset<Example, const char*, &Example::m_cstring> },
	
	{ "getCPPString", luaU_get<Example, etk::String, &Example::m_cppstring> },
	{ "setCPPString", luaU_set<Example, etk::String, &Example::m_cppstring> },
	{ "m_cppstring", luaU_getset<Example, etk::String, &Example::m_cppstring> },
	
	{ "getVec", luaU_get<Example, Vector2D, &Example::m_vec> },
	{ "setVec", luaU_set<Example, Vector2D, &Example::m_vec> },
	{ "m_vec", luaU_getset<Example, Vector2D, &Example::m_vec> },
	
	{ "getNumber", luaU_get<Example, double, &Example::m_number> },
	{ "setNumber", luaU_set<Example, double, &Example::m_number> },
	{ "m_number", luaU_getset<Example, double, &Example::m_number> },
	
	{ "getFloatNumber", luaU_get<Example, float, &Example::m_floatnumber> },
	{ "setFloatNumber", luaU_set<Example, float, &Example::m_floatnumber> },
	{ "m_floatnumber", luaU_getset<Example, float, &Example::m_floatnumber> },
	
	{ "getPtr", luaU_get<Example, Example, &Example::m_ptr> },
	{ "setPtr", luaU_set<Example, Example, &Example::m_ptr> },
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
	{ "getBooleanFunc", luaU_get<Example,   bool, &Example::GetBoolean> },
	{ "setBooleanFunc", luaU_set<Example,   bool, &Example::SetBoolean> },
	{ "BooleanFunc", luaU_getset<Example,   bool, &Example::GetBoolean, &Example::SetBoolean> },
	
	{ "getIntegerFunc", luaU_get<Example, int, &Example::GetInteger> },
	{ "setIntegerFunc", luaU_set<Example, int, &Example::SetInteger> },
	{ "IntegerFunc", luaU_getset<Example, int, &Example::GetInteger, &Example::SetInteger> },
	
	{ "getUIntegerFunc", luaU_get<Example, unsigned int, &Example::GetUInteger> },
	{ "setUIntegerFunc", luaU_set<Example, unsigned int, &Example::SetUInteger> },
	{ "UIntegerFunc", luaU_getset<Example, unsigned int, &Example::GetUInteger, &Example::SetUInteger> },
	
	{ "getCStringFunc", luaU_get<Example, const char*, &Example::GetCString> },
	{ "setCStringFunc", luaU_set<Example, const char*, &Example::SetCString> },
	{ "CStringFunc", luaU_getset<Example, const char*, &Example::GetCString, &Example::SetCString> },
	
	{ "getNumberFunc", luaU_get<Example, double, &Example::GetNumber> },
	{ "setNumberFunc", luaU_set<Example, double, &Example::SetNumber> },
	{ "NumberFunc", luaU_getset<Example, double, &Example::GetNumber, &Example::SetNumber> },
	
	{ "getFloatNumberFunc", luaU_get<Example, float, &Example::GetFloatNumber> },
	{ "setFloatNumberFunc", luaU_set<Example, float, &Example::SetFloatNumber> },
	{ "FloatNumberFunc", luaU_getset<Example, float, &Example::GetFloatNumber, &Example::SetFloatNumber> },
	
	{ "getPtrFunc", luaU_get<Example, Example, &Example::GetPtr> },
	{ "setPtrFunc", luaU_set<Example, Example, &Example::SetPtr> },
	{ "PtrFunc", luaU_getset<Example, Example, &Example::GetPtr, &Example::SetPtr> },
	
	// In order to use luaU_get and luaU_set on non-primitive types, you must define luaU_to
	// and luaU_check for that type.
	// See LuaCustomTypes.hpp for an example involving etk::String and Vector2D
	{ "getCPPStringFunc", luaU_get<Example, etk::String, &Example::GetCPPString> },
	{ "setCPPStringFunc", luaU_set<Example, etk::String, &Example::SetCPPString> },
	{ "CPPStringFunc", luaU_getset<Example, etk::String, &Example::GetCPPString, &Example::SetCPPString> },
	
	{ "getVecFunc", luaU_get<Example, Vector2D, &Example::GetVec> },
	{ "setSetFunc", luaU_set<Example, Vector2D, &Example::SetVec> },
	{ "VecFunc", luaU_getset<Example, Vector2D, &Example::GetVec, &Example::SetVec> },
	
	{ "DoSomething", luaU_func(&Example::DoSomething) },
	{ "DoSomething2", luaU_func(&Example::DoSomething2) },
	
	//{ "DoSomethingElse1", luaU_funcsig(int, Example, DoSomethingElse, int, int) },
	//{ "DoSomethingElse2", luaU_funcsig(int, Example, DoSomethingElse, float) },
	{ NULL, NULL }
};

int luaopen_Example(lua_State* _L) {
    luaW_register<Example>(_L, "Example", NULL, Example_metatable);
    return 1;
}

