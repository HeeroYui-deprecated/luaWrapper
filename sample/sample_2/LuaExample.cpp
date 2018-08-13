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
	{ "m_boolean", luaWrapper::utils::getSet<Example,   bool,       &Example::m_boolean> },
	
	{ "getInteger", luaWrapper::utils::get<Example, int, &Example::m_integer> },
	{ "setInteger", luaWrapper::utils::set<Example, int, &Example::m_integer> },
	{ "m_integer", luaWrapper::utils::getSet<Example, int, &Example::m_integer> },
	
	{ "getUInteger", luaWrapper::utils::get<Example, unsigned int, &Example::m_uinteger> },
	{ "setUInteger", luaWrapper::utils::set<Example, unsigned int, &Example::m_uinteger> },
	{ "m_uinteger", luaWrapper::utils::getSet<Example, unsigned int, &Example::m_uinteger> },
	
	{ "getCString", luaWrapper::utils::get<Example, const char*, &Example::m_cstring> },
	{ "setCString", luaWrapper::utils::set<Example, const char*, &Example::m_cstring> },
	{ "m_string", luaWrapper::utils::getSet<Example, const char*, &Example::m_cstring> },
	
	{ "getCPPString", luaWrapper::utils::get<Example, etk::String, &Example::m_cppstring> },
	{ "setCPPString", luaWrapper::utils::set<Example, etk::String, &Example::m_cppstring> },
	{ "m_cppstring", luaWrapper::utils::getSet<Example, etk::String, &Example::m_cppstring> },
	
	{ "getVec", luaWrapper::utils::get<Example, Vector2D, &Example::m_vec> },
	{ "setVec", luaWrapper::utils::set<Example, Vector2D, &Example::m_vec> },
	{ "m_vec", luaWrapper::utils::getSet<Example, Vector2D, &Example::m_vec> },
	
	{ "getNumber", luaWrapper::utils::get<Example, double, &Example::m_number> },
	{ "setNumber", luaWrapper::utils::set<Example, double, &Example::m_number> },
	{ "m_number", luaWrapper::utils::getSet<Example, double, &Example::m_number> },
	
	{ "getFloatNumber", luaWrapper::utils::get<Example, float, &Example::m_floatnumber> },
	{ "setFloatNumber", luaWrapper::utils::set<Example, float, &Example::m_floatnumber> },
	{ "m_floatnumber", luaWrapper::utils::getSet<Example, float, &Example::m_floatnumber> },
	
	{ "getPtr", luaWrapper::utils::get<Example, Example, &Example::m_ptr> },
	{ "setPtr", luaWrapper::utils::set<Example, Example, &Example::m_ptr> },
	{ "Ptr", luaWrapper::utils::getSet<Example, Example, &Example::m_ptr> },
	
	// The getters and setters above work on member variables directly, but
	// sometimes all you have are getter and setter functions instead of
	// variables. You can still automate the creation of getter and setter Lua
	// function wrappers.
	
	// Normally it would be silly to have getter and setter functions for both
	// the member variable AND the getter/setter function, I've included both
	// here as an example of how it works
	
	// Example of member access through getters and setters
	//                           Class    | data type | getter
	//                           Class    | data type | setter
	//                           Class    | data type | getter               | setter
	{ "getBooleanFunc", luaWrapper::utils::get<Example,   bool, &Example::getBoolean> },
	{ "setBooleanFunc", luaWrapper::utils::set<Example,   bool, &Example::setBoolean> },
	{ "BooleanFunc", luaWrapper::utils::getSet<Example,   bool, &Example::getBoolean, &Example::setBoolean> },
	
	{ "getIntegerFunc", luaWrapper::utils::get<Example, int, &Example::getInteger> },
	{ "setIntegerFunc", luaWrapper::utils::set<Example, int, &Example::setInteger> },
	{ "IntegerFunc", luaWrapper::utils::getSet<Example, int, &Example::getInteger, &Example::setInteger> },
	
	{ "getUIntegerFunc", luaWrapper::utils::get<Example, unsigned int, &Example::getUInteger> },
	{ "setUIntegerFunc", luaWrapper::utils::set<Example, unsigned int, &Example::setUInteger> },
	{ "UIntegerFunc", luaWrapper::utils::getSet<Example, unsigned int, &Example::getUInteger, &Example::setUInteger> },
	
	{ "getCStringFunc", luaWrapper::utils::get<Example, const char*, &Example::getCString> },
	{ "setCStringFunc", luaWrapper::utils::set<Example, const char*, &Example::setCString> },
	{ "CStringFunc", luaWrapper::utils::getSet<Example, const char*, &Example::getCString, &Example::setCString> },
	
	{ "getNumberFunc", luaWrapper::utils::get<Example, double, &Example::getNumber> },
	{ "setNumberFunc", luaWrapper::utils::set<Example, double, &Example::setNumber> },
	{ "NumberFunc", luaWrapper::utils::getSet<Example, double, &Example::getNumber, &Example::setNumber> },
	
	{ "getFloatNumberFunc", luaWrapper::utils::get<Example, float, &Example::getFloatNumber> },
	{ "setFloatNumberFunc", luaWrapper::utils::set<Example, float, &Example::setFloatNumber> },
	{ "FloatNumberFunc", luaWrapper::utils::getSet<Example, float, &Example::getFloatNumber, &Example::setFloatNumber> },
	
	{ "getPtrFunc", luaWrapper::utils::get<Example, Example, &Example::getPtr> },
	{ "setPtrFunc", luaWrapper::utils::set<Example, Example, &Example::setPtr> },
	{ "PtrFunc", luaWrapper::utils::getSet<Example, Example, &Example::getPtr, &Example::setPtr> },
	
	// In order to use luaWrapper::utils::get and luaWrapper::utils::set on non-primitive types, you must define luaWrapper::utils::to
	// and luaWrapper::utils::check for that type.
	// See LuaCustomTypes.hpp for an example involving etk::String and Vector2D
	{ "getCPPStringFunc", luaWrapper::utils::get<Example, etk::String, &Example::getCPPString> },
	{ "setCPPStringFunc", luaWrapper::utils::set<Example, etk::String, &Example::setCPPString> },
	{ "CPPStringFunc", luaWrapper::utils::getSet<Example, etk::String, &Example::getCPPString, &Example::setCPPString> },
	
	{ "getVecFunc", luaWrapper::utils::get<Example, Vector2D, &Example::getVec> },
	{ "setsetFunc", luaWrapper::utils::set<Example, Vector2D, &Example::setVec> },
	{ "VecFunc", luaWrapper::utils::getSet<Example, Vector2D, &Example::getVec, &Example::setVec> },
	
	{ "doSomething", luaWrapperUtils_func(&Example::doSomething) },
	{ "doSomething2", luaWrapperUtils_func(&Example::doSomething2) },
	
	//{ "DoSomethingElse1", luaWrapper::utils::funcsig(int, Example, DoSomethingElse, int, int) },
	//{ "DoSomethingElse2", luaWrapper::utils::funcsig(int, Example, DoSomethingElse, float) },
	{ NULL, NULL }
};

int luaopen_Example(luaWrapper:Lua& _lua) {
    _lua.registerElement<Example>("Example", NULL, Example_metatable);
    return 1;
}

