/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <luaWrapper/luaWrapper.hpp>
#include <etest/etest.hpp>


TEST(TestCCallLuaFunctionn, basicCallReturnDouble) {
	luaWrapper::Lua lua;
	lua.executeString(R"#(
	function MyFunctionName(x, y)
		return x + y*2
	end
	)#");
	double ret = lua.call<float>("MyFunctionName", 43.9, 143.6);
	EXPECT_EQ(ret, float(43.9 + 143.6 * 2.0));
}

TEST(TestCCallLuaFunctionn, basicCallReturnFloat) {
	luaWrapper::Lua lua;
	lua.executeString(R"#(
	function MyFunctionName(x, y)
		return x + y*2
	end
	)#");
	float ret = lua.call<float>("MyFunctionName", 43.9f, 143.6f);
	EXPECT_EQ(ret, 43.9f + 143.6f * 2.0f);
}

TEST(TestCCallLuaFunctionn, basicCallReturnInt) {
	luaWrapper::Lua lua;
	lua.executeString(R"#(
	function MyFunctionName(x, y)
		return x + y*2
	end
	)#");
	int ret = lua.call<int>("MyFunctionName", 43, 76);
	EXPECT_EQ(ret, 43 + 76 * 2);
}

TEST(TestCCallLuaFunctionn, basicCallReturnBool) {
	luaWrapper::Lua lua;
	lua.executeString(R"#(
	function MyFunctionName(x, y)
		return x or y
	end
	)#");
	bool ret = lua.call<bool>("MyFunctionName", true, false);
	EXPECT_EQ(ret, true || false);
	ret = lua.call<bool>("MyFunctionName", false, false);
	EXPECT_EQ(ret, false || false);
	ret = lua.call<bool>("MyFunctionName", true, true);
	EXPECT_EQ(ret, true || true);
	ret = lua.call<bool>("MyFunctionName", false, true);
	EXPECT_EQ(ret, false || true);
}

TEST(TestCCallLuaFunctionn, basicCallReturnVoid) {
	luaWrapper::Lua lua;
	lua.executeString(R"#(
	function MyFunctionName(x, y)
		print("x or y")
	end
	)#");
	lua.callVoid("MyFunctionName", true, "TRESDF");
}

TEST(TestCCallLuaFunctionn, basicCallNotExist) {
	luaWrapper::Lua lua;
	lua.executeString(R"#(
	function MyFunctionName(x, y)
		print("x or y")
	end
	)#");
	EXPECT_THROW(lua.callVoid("MyFunctionNameThatDoesNotExit", true, "TRESDF"), etk::exception::RuntimeError);
}


