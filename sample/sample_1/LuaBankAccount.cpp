#include <iostream>
#include <etk/String.hpp>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <luaWrapper/luaWrapper.hpp>

#include "BankAccount.hpp"

using namespace std;

ETK_DECLARE_TYPE(BankAccount);

/**
 * Allocator
 *
 * Types that do not have a default contructor require you to write an allocator function.
 * This function is passed to luaWrapper::register.
 */
ememory::SharedPtr<BankAccount> BankAccount_new(lua_State *_luaState) {
	const char* owner = luaL_checkstring(_luaState, 1);
	float balance = luaL_checknumber(_luaState, 2);
	return ememory::makeShared<BankAccount>(owner, balance);
}

/**
 * Static Functions
 *
 * These functions can be called directly from the BankAccount table in lua
 */

int BankAccount_checkTotalMoneyInBank(lua_State *_luaState) {
	lua_pushnumber(_luaState, BankAccount::checkTotalMoneyInBank());
	return 1;
}

/**
 * Member Functions
 *
 * These functions are stored on the BankAccount.metatable table.
 * All BankAccount objects in Lua have access to the functions defined there
 * by the use of special a __index metatmethod that is set up by LuaWrapper.
 */

int BankAccount_getOwnerName(lua_State *_luaState) {
	auto account = luaWrapper::check<BankAccount>(_luaState, 1);
	lua_pushstring(_luaState, account->getOwnerName());
	return 1;
}

int BankAccount_deposit(lua_State* _luaState) {
	auto account = luaWrapper::check<BankAccount>(_luaState, 1);
	float amount = luaL_checknumber(_luaState, 2);
	account->deposit(amount);
	return 0;
}

int BankAccount_withdraw(lua_State* _luaState) {
	auto account = luaWrapper::check<BankAccount>(_luaState, 1);
	float amount = luaL_checknumber(_luaState, 2);
	account->withdraw(amount);
	return 0;
}

int BankAccount_checkBalance(lua_State* _luaState) {
	auto account = luaWrapper::check<BankAccount>(_luaState, 1);
	lua_pushnumber(_luaState, account->checkBalance());
	return 1;
}

static luaL_Reg BankAccount_table[] = {
	{ "checkTotalMoneyInBank", BankAccount_checkTotalMoneyInBank },
	{ NULL, NULL }
};

static luaL_Reg BankAccount_metatable[] = {
	{ "getOwnerName", BankAccount_getOwnerName },
	{ "deposit", BankAccount_deposit },
	{ "withdraw", BankAccount_withdraw },
	{ "checkBalance", BankAccount_checkBalance },
	{ NULL, NULL }
};

int luaopen_BankAccount(luaWrapper:Lua& _lua) {
	_lua.registerElement<BankAccount>("BankAccount",
	                                  BankAccount_table,
	                                  BankAccount_metatable,
	                                  BankAccount_new // If your class has a default constructor you can omit this argument, LuaWrapper will generate a default allocator for you.
	                                  );
	return 1;
}

