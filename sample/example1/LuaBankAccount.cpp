#include <iostream>
#include <etk/String.hpp>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <luaWrapper/luaWrapper.hpp>

#include "BankAccount.hpp"

using namespace std;

/**
 * Allocator
 *
 * Types that do not have a default contructor require you to write an allocator function.
 * This function is passed to luaW_register.
 */

BankAccount* BankAccount_new(lua_State *_L) {
    const char* owner = luaL_checkstring(_L, 1);
    float balance = luaL_checknumber(_L, 2);
    return new BankAccount(owner, balance);
}

/**
 * Static Functions
 *
 * These functions can be called directly from the BankAccount table in lua
 */

int BankAccount_checkTotalMoneyInBank(lua_State *_L) {
    lua_pushnumber(_L, BankAccount::checkTotalMoneyInBank());
    return 1;
}

/**
 * Member Functions
 *
 * These functions are stored on the BankAccount.metatable table.
 * All BankAccount objects in Lua have access to the functions defined there
 * by the use of special a __index metatmethod that is set up by LuaWrapper.
 */

int BankAccount_getOwnerName(lua_State *_L) {
    BankAccount* account = luaW_check<BankAccount>(_L, 1);
    lua_pushstring(_L, account->getOwnerName());
    return 1;
}

int BankAccount_deposit(lua_State* _L) {
    BankAccount* account = luaW_check<BankAccount>(_L, 1);
    float amount = luaL_checknumber(_L, 2);
    account->deposit(amount);
    return 0;
}

int BankAccount_withdraw(lua_State* _L) {
    BankAccount* account = luaW_check<BankAccount>(_L, 1);
    float amount = luaL_checknumber(_L, 2);
    account->withdraw(amount);
    return 0;
}

int BankAccount_checkBalance(lua_State* _L) {
    BankAccount* account = luaW_check<BankAccount>(_L, 1);
    lua_pushnumber(_L, account->checkBalance());
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

int luaopen_BankAccount(lua_State* _L) {
    luaW_register<BankAccount>(_L,
        "BankAccount",
        BankAccount_table,
        BankAccount_metatable,
        BankAccount_new // If your class has a default constructor you can omit this argument,
                        // LuaWrapper will generate a default allocator for you.
    );
    return 1;
}

