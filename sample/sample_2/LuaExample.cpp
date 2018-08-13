#include <etk/String.hpp>

#include <lua/lua.h>

#include <luaWrapper/luaWrapper.hpp>
#include <luaWrapper/luaWrapperUtil.hpp>

#include "LuaCustomTypes.hpp"
#include "Example.hpp"
#include <test-debug/debug.hpp>

static int Example_PrintMe(lua_State* L) {
	ememory::SharedPtr<Example> ex = luaWrapper::check<Example>(L, 1);
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
	{ "getBoolean", luaWrapper::utils::get<Example,   bool,       &Example::m_boolean> },
	{ "setBoolean", luaWrapper::utils::set<Example,   bool,       &Example::m_boolean> },
	{ "m_boolean", luaWrapper::utils::getset<Example,   bool,       &Example::m_boolean> },
	
	{ "getInteger", luaWrapper::utils::get<Example, int, &Example::m_integer> },
	{ "setInteger", luaWrapper::utils::set<Example, int, &Example::m_integer> },
	{ "m_integer", luaWrapper::utils::getset<Example, int, &Example::m_integer> },
	
	{ "getUInteger", luaWrapper::utils::get<Example, unsigned int, &Example::m_uinteger> },
	{ "setUInteger", luaWrapper::utils::set<Example, unsigned int, &Example::m_uinteger> },
	{ "m_uinteger", luaWrapper::utils::getset<Example, unsigned int, &Example::m_uinteger> },
	
	{ "getCString", luaWrapper::utils::get<Example, const char*, &Example::m_cstring> },
	{ "setCString", luaWrapper::utils::set<Example, const char*, &Example::m_cstring> },
	{ "m_string", luaWrapper::utils::getset<Example, const char*, &Example::m_cstring> },
	
	{ "getCPPString", luaWrapper::utils::get<Example, etk::String, &Example::m_cppstring> },
	{ "setCPPString", luaWrapper::utils::set<Example, etk::String, &Example::m_cppstring> },
	{ "m_cppstring", luaWrapper::utils::getset<Example, etk::String, &Example::m_cppstring> },
	
	{ "getVec", luaWrapper::utils::get<Example, Vector2D, &Example::m_vec> },
	{ "setVec", luaWrapper::utils::set<Example, Vector2D, &Example::m_vec> },
	{ "m_vec", luaWrapper::utils::getset<Example, Vector2D, &Example::m_vec> },
	
	{ "getNumber", luaWrapper::utils::get<Example, double, &Example::m_number> },
	{ "setNumber", luaWrapper::utils::set<Example, double, &Example::m_number> },
	{ "m_number", luaWrapper::utils::getset<Example, double, &Example::m_number> },
	
	{ "getFloatNumber", luaWrapper::utils::get<Example, float, &Example::m_floatnumber> },
	{ "setFloatNumber", luaWrapper::utils::set<Example, float, &Example::m_floatnumber> },
	{ "m_floatnumber", luaWrapper::utils::getset<Example, float, &Example::m_floatnumber> },
	
	{ "getPtr", luaWrapper::utils::get<Example, Example, &Example::m_ptr> },
	{ "setPtr", luaWrapper::utils::set<Example, Example, &Example::m_ptr> },
	{ "Ptr", luaWrapper::utils::getset<Example, Example, &Example::m_ptr> },
	
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
	{ "getBooleanFunc", luaWrapper::utils::get<Example,   bool, &Example::GetBoolean> },
	{ "setBooleanFunc", luaWrapper::utils::set<Example,   bool, &Example::SetBoolean> },
	{ "BooleanFunc", luaWrapper::utils::getset<Example,   bool, &Example::GetBoolean, &Example::SetBoolean> },
	
	{ "getIntegerFunc", luaWrapper::utils::get<Example, int, &Example::GetInteger> },
	{ "setIntegerFunc", luaWrapper::utils::set<Example, int, &Example::SetInteger> },
	{ "IntegerFunc", luaWrapper::utils::getset<Example, int, &Example::GetInteger, &Example::SetInteger> },
	
	{ "getUIntegerFunc", luaWrapper::utils::get<Example, unsigned int, &Example::GetUInteger> },
	{ "setUIntegerFunc", luaWrapper::utils::set<Example, unsigned int, &Example::SetUInteger> },
	{ "UIntegerFunc", luaWrapper::utils::getset<Example, unsigned int, &Example::GetUInteger, &Example::SetUInteger> },
	
	{ "getCStringFunc", luaWrapper::utils::get<Example, const char*, &Example::GetCString> },
	{ "setCStringFunc", luaWrapper::utils::set<Example, const char*, &Example::SetCString> },
	{ "CStringFunc", luaWrapper::utils::getset<Example, const char*, &Example::GetCString, &Example::SetCString> },
	
	{ "getNumberFunc", luaWrapper::utils::get<Example, double, &Example::GetNumber> },
	{ "setNumberFunc", luaWrapper::utils::set<Example, double, &Example::SetNumber> },
	{ "NumberFunc", luaWrapper::utils::getset<Example, double, &Example::GetNumber, &Example::SetNumber> },
	
	{ "getFloatNumberFunc", luaWrapper::utils::get<Example, float, &Example::GetFloatNumber> },
	{ "setFloatNumberFunc", luaWrapper::utils::set<Example, float, &Example::SetFloatNumber> },
	{ "FloatNumberFunc", luaWrapper::utils::getset<Example, float, &Example::GetFloatNumber, &Example::SetFloatNumber> },
	
	{ "getPtrFunc", luaWrapper::utils::get<Example, Example, &Example::GetPtr> },
	{ "setPtrFunc", luaWrapper::utils::set<Example, Example, &Example::SetPtr> },
	{ "PtrFunc", luaWrapper::utils::getset<Example, Example, &Example::GetPtr, &Example::SetPtr> },
	
	// In order to use luaWrapper::utils::get and luaWrapper::utils::set on non-primitive types, you must define luaWrapper::utils::to
	// and luaWrapper::utils::check for that type.
	// See LuaCustomTypes.hpp for an example involving etk::String and Vector2D
	{ "getCPPStringFunc", luaWrapper::utils::get<Example, etk::String, &Example::GetCPPString> },
	{ "setCPPStringFunc", luaWrapper::utils::set<Example, etk::String, &Example::SetCPPString> },
	{ "CPPStringFunc", luaWrapper::utils::getset<Example, etk::String, &Example::GetCPPString, &Example::SetCPPString> },
	
	{ "getVecFunc", luaWrapper::utils::get<Example, Vector2D, &Example::GetVec> },
	{ "setSetFunc", luaWrapper::utils::set<Example, Vector2D, &Example::SetVec> },
	{ "VecFunc", luaWrapper::utils::getset<Example, Vector2D, &Example::GetVec, &Example::SetVec> },
	
	{ "DoSomething", luaWrapperUtils_func(&Example::DoSomething) },
	{ "DoSomething2", luaWrapperUtils_func(&Example::DoSomething2) },
	
	//{ "DoSomethingElse1", luaWrapper::utils::funcsig(int, Example, DoSomethingElse, int, int) },
	//{ "DoSomethingElse2", luaWrapper::utils::funcsig(int, Example, DoSomethingElse, float) },
	{ NULL, NULL }
};

int luaopen_Example(lua_State* _L) {
    luaWrapper::registerElement<Example>(_L, "Example", NULL, Example_metatable);
    return 1;
}

