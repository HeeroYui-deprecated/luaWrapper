/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <luaWrapper/luaWrapper.hpp>
#include <etest/etest.hpp>


static etk::String refOutputBoolean1("{\n\t\"tmpElement\": true\n}\n");

TEST(TestCCallLuaFunctionn, basicCall) {
	luaWrapper::Lua lua;
	lua.executeString(R"#(
	function MyFunctionName(x, y)
		return x + y*2
	end
	)#");
	
	float ret = lua.call<float>("MyFunctionName", 43.9f, 43.6f);
	
	EXPECT_EQ(ret, 43.9f + 43.6f * 2.0f);
	
}