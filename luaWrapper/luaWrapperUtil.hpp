/**
 * Copyright (c) 2010-2013 Alexander Ames
 * Alexander.Ames@gmail.com
 * See Copyright Notice at the end of this file
 */

/** API Summary:
 *
 * This is a set of utility functions that add to the functionalit of
 * LuaWrapper. Over time I found myself repeating a few patterns, such as
 * writing many trvial getter and setter functions, and wanting pass ownership
 * of one object to another and have the Lua script properly handle that case.
 *
 * This file contains the additional functions that I've added but that do
 * not really belong in the core API.
 */
#pragma once

#include <luaWrapper/luaWrapper.hpp>

#include <type_traits>

#ifndef LUAW_STD
#define LUAW_STD std
#endif

/**
 * @brief This template removes reference and const qualifier from the type
 */
template <typename T> struct luaW_remove_cr {
	typedef typename std::remove_const<typename std::remove_reference<T>::type>::type type;
};

/**
 * A set of templated luaL_check and lua_push functions for use in the getters
 * and setters below
 *
 * It is often useful to override luaU_check, luaU_to and/or luaU_push to
 * operate on your own simple types rather than register your type with
 * LuaWrapper, especially with small objects.
 */

template<typename U> U luaU_check(lua_State* _L, int _index);
template<typename U> U luaU_to(lua_State* _L, int _index);
template<typename U> void luaU_push(lua_State* _L, const U& _value);

/**
 * This is slightly different than the previous three functions in that you
 * shouldn't need to write your own version of it, since it uses luaU_check
 * automatically.
 */
template<typename U> U luaU_opt(lua_State* _L, int _index, const U& _fallback = U()) {
	if (lua_isnil(_L, _index)) {
		return _fallback;
	} else {
		return luaU_check<U>(_L, _index);
	}
}

/**
 * These are just some functions I've always felt should exist
 */
template <typename U> inline U luaU_getfield(lua_State* _L, int _index, const char* _field) {
	static_assert(!LUAW_STD::is_same<U, const char*>::value,
		"luaU_getfield is not safe to use on const char*'s. (The string will be popped from the stack.)");
	lua_getfield(_L, _index, _field);
	U val = luaU_to<U>(_L, -1);
	lua_pop(_L, 1);
	return val;
}

template <typename U> inline U luaU_checkfield(lua_State* _L, int _index, const char* _field) {
	static_assert(!LUAW_STD::is_same<U, const char*>::value,
		"luaU_checkfield is not safe to use on const char*'s. (The string will be popped from the stack.)");
	lua_getfield(_L, _index, _field);
	U val = luaU_check<U>(_L, -1);
	lua_pop(_L, 1);
	return val;
}

template <typename U> inline U luaU_optfield(lua_State* _L, int _index, const char* _field, const U& _fallback = U()) {
	static_assert(!LUAW_STD::is_same<U, const char*>::value,
		"luaU_getfield is not safe to use on const char*'s. (The string will be popped from the stack.)");
	lua_getfield(_L, _index, _field);
	U val = luaU_opt<U>(_L, -1, _fallback);
	lua_pop(_L, 1);
	return val;
}

template <typename U> inline void luaU_setfield(lua_State* _L, int _index, const char* _field, U _val) {
	luaU_push<U>(_L, _val);
	lua_setfield(_L, luaW_correctindex(_L, _index, 1), _field);
}

/** A set of trivial getter and setter templates. These templates are designed
 * to call trivial getters or setters.
 *
 * There are four forms:
 *  1. Getting or setting a public member variable that is of a primitive type
 *  2. Getting or setting a public member variable that is a pointer to an
 *	 object
 *  3. Getting or setting a private member variable that is of a primitive type
 *	 through a getter or setter
 *  3. Getting or setting a private member variable that is is a pointer to an
 *	 object through a getter or setter
 *
 * The interface to all of them is the same however. In addition to plain
 * getter and setter functions, there is a getset which does both - if an
 * argument is supplied it attempts to set the value, and in either case it
 * returns the value. In your lua table declaration in C++ rather than write
 * individiual wrappers for each getter and setter you may do the following:
 *
 * static luaL_reg Foo_metatable[] =
 * {
 *	 { "GetBar", luaU_get<Foo, bool, &Widget::GetBar> },
 *	 { "SetBar", luaU_set<Foo, bool, &Widget::SetBar> },
 *	 { "Bar", luaU_getset<Foo, bool, &Widget::GetBar, &Widget::SetBar> },
 *	 { NULL, NULL }
 * }
 *
 * Getters and setters must have one of the following signatures:
 *	void T::Setter(U _value);
 *	void T::Setter(U* _value);
 *	void T::Setter(const U& _value);
 *	U Getter() const;
 *	U* Getter() const;
 *
 * In this example, the variable Foo::bar is private and so it is accessed
 * through getter and setter functions. If Foo::bar were public, it could be
 * accessed directly, like so:
 *
 * static luaL_reg Foo_metatable[] =
 * {
 *	 { "GetBar", luaU_get<Foo, bool, &Widget::bar> },
 *	 { "SetBar", luaU_set<Foo, bool, &Widget::bar> },
 *	 { "Bar", luaU_getset<Foo, bool, &Widget::bar> },
 * }
 *
 * In a Lua script, you can now use foo:GetBar(), foo:SetBar() and foo:Bar()
 */

template <typename T, typename U, U T::*Member> int luaU_get(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	luaU_push<U>(_L, obj.get()->*Member);
	return 1;
}

template <typename T, typename U, U* T::*Member> int luaU_get(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	luaW_push<U>(_L, obj.get()->*Member);
	return 1;
}

template <typename T, typename U, U (T::*Getter)() const> int luaU_get(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	luaU_push<U>(_L, (obj.get()->*Getter)());
	return 1;
}

template <typename T, typename U, const U& (T::*Getter)() const> int luaU_get(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	luaU_push<U>(_L, (obj.get()->*Getter)());
	return 1;
}

template <typename T, typename U, U* (T::*Getter)() const> int luaU_get(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	luaW_push<U>(_L, (obj.get()->*Getter)());
	return 1;
}

template <typename T, typename U, U T::*Member> int luaU_set(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (obj != null) {
		obj.get()->*Member = luaU_check<U>(_L, 2);
	}
	return 0;
}

template <typename T, typename U, U* T::*Member> int luaU_set(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (obj != null) {
		ememory::SharedPtr<U> member = luaW_opt<U>(_L, 2);
		obj.get()->*Member = member.get();
	}
	return 0;
}

template <typename T, typename U, const U* T::*Member> int luaU_set(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (obj != null) {
		ememory::SharedPtr<U> member = luaW_opt<U>(_L, 2);
		obj.get()->*Member = member.get();
	}
	return 0;
}

template <typename T, typename U, const U* T::*Member> int luaU_setandrelease(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (obj != null) {
		ememory::SharedPtr<U> member = luaW_opt<U>(_L, 2);
		obj.get()->*Member = member.get();
		if (member) {
			luaW_release<U>(_L, member);
		}
	}
	return 0;
}

template <typename T, typename U, void (T::*Setter)(U)> int luaU_set(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (obj != null) {
		(obj.get()->*Setter)(luaU_check<U>(_L, 2));
	}
	return 0;
}

template <typename T, typename U, void (T::*Setter)(const U&)> int luaU_set(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (obj != null) {
		(obj.get()->*Setter)(luaU_check<U>(_L, 2));
	}
	return 0;
}

template <typename T, typename U, void (T::*Setter)(U*)> int luaU_set(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (obj != null) {
		ememory::SharedPtr<U> member = luaW_opt<U>(_L, 2);
		(obj.get()->*Setter)(member.get());
	}
	return 0;
}

template <typename T, typename U, void (T::*Setter)(U*)> int luaU_setandrelease(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (obj != null) {
		ememory::SharedPtr<U> member = luaW_opt<U>(_L, 2);
		(obj.get()->*Setter)(member);
		if (member) {
			luaW_release<U>(_L, member);
		}
	}
	return 0;
}

template <typename T, typename U, U T::*Member> int luaU_getset(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (    obj != null
	     && lua_gettop(_L) >= 2) {
		obj.get()->*Member = luaU_check<U>(_L, 2);
		return 0;
	} else {
		luaU_push<U>(_L, obj.get()->*Member);
		return 1;
	}
}

template <typename T, typename U, U* T::*Member> int luaU_getset(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (    obj != null
	     && lua_gettop(_L) >= 2) {
		ememory::SharedPtr<U> member = luaW_opt<U>(_L, 2);
		obj.get()->*Member = member.get();
		return 0;
	} else {
		luaW_push<U>(_L, obj.get()->*Member);
		return 1;
	}
}

template <typename T, typename U, U* T::*Member> int luaU_getsetandrelease(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (    obj != null
	     && lua_gettop(_L) >= 2) {
		ememory::SharedPtr<U> member = luaW_opt<U>(_L, 2);
		obj.get()->*Member = member.get();
		if (member)
			luaW_release<U>(_L, member);
		return 0;
	} else {
		luaW_push<U>(_L, obj.get()->*Member);
		return 1;
	}
}

template <typename T, typename U, U (T::*Getter)() const, void (T::*Setter)(U)> int luaU_getset(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (    obj != null
	     && lua_gettop(_L) >= 2) {
		(obj.get()->*Setter)(luaU_check<U>(_L, 2));
		return 0;
	} else {
		luaU_push<U>(_L, (obj.get()->*Getter)());
		return 1;
	}
}

template <typename T, typename U, U (T::*Getter)() const, void (T::*Setter)(const U&)> int luaU_getset(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (    obj != null
	     && lua_gettop(_L) >= 2) {
		(obj.get()->*Setter)(luaU_check<U>(_L, 2));
		return 0;
	} else {
		luaU_push<U>(_L, (obj.get()->*Getter)());
		return 1;
	}
}

template <typename T, typename U, const U& (T::*Getter)() const, void (T::*Setter)(const U&)> int luaU_getset(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (    obj != null
	     && lua_gettop(_L) >= 2) {
		(obj.get()->*Setter)(luaU_check<U>(_L, 2));
		return 0;
	} else {
		luaU_push<U>(_L, (obj.get()->*Getter)());
		return 1;
	}
}

template <typename T, typename U, U* (T::*Getter)() const, void (T::*Setter)(U*)> int luaU_getset(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (    obj != null
	     && lua_gettop(_L) >= 2) {
		ememory::SharedPtr<U> member = luaW_opt<U>(_L, 2);
		(obj.get()->*Setter)(member.get());
		return 0;
	} else {
		luaW_push<U>(_L, (obj.get()->*Getter)());
		return 1;
	}
}

template <typename T, typename U, U* (T::*Getter)() const, void (T::*Setter)(U*)> int luaU_getsetandrelease(lua_State* _L) {
	ememory::SharedPtr<T> obj = luaW_check<T>(_L, 1);
	if (    obj != null
	     && lua_gettop(_L) >= 2) {
		ememory::SharedPtr<U> member = luaW_opt<U>(_L, 2);
		(obj.get()->*Setter)(member);
		if (member)
			luaW_release<U>(_L, member);
		return 0;
	} else {
		luaW_push<U>(_L, (obj.get()->*Getter)());
		return 1;
	}
}

/**
 * luaU_func is a special macro that expands into a simple function wrapper.
 * Unlike the getter setters above, you merely need to name the function you
 * would like to wrap.
 *
 * For example,
 *
 * struct Foo
 * {
 *	 int DoSomething(int, const char*);
 * };
 *
 * static luaL_reg Foo_metatable[] =
 * {
 *	 { "DoSomething", luaU_func(&Foo::DoSomething) },
 *	 { NULL, NULL }
 * }
 *
 * void RegisterFoo(lua_State* L)
 * {
 *	 luaW_register<Foo>(L, "Foo", NULL, Foo_metatable);
 * }
 *
 * This macro will expand based on the function signature of Foo::DoSomething
 * In this example, it would expand into the following wrapper:
 *
 *	 luaU_push(luaW_check<T>(L, 1)->DoSomething(luaU_check<int>(L, 2), luaU_check<const char*>(L, 3)));
 *	 return 1;
 *
 * In this example there is only one member function called DoSomething. In some
 * cases there may be multiple overrides for a function. For those cases, an
 * additional macro has been provided, luaU_funcsig, which takes the entire
 * function signature. The arguments to the macro reflect the order they would
 * appear in the function signature: return type, type name, function name, and
 * finally a list of all the argument types. For example:
 *
 * struct Foo
 * {
 *	 int DoSomething (const char*);
 *	 int DoSomething (const char*, int);
 * };
 *
 * const struct luaL_Reg Foo_metatable[] =
 * {
 *	 {"DoSomething1", luaU_funcsig(int, Foo, DoSomething, const char*)) },
 *	 {"DoSomething1", luaU_funcsig(int, Foo, DoSomething, const char*, int)) },
 *	 { NULL, NULL }
 * };
 *
 * There`s also support for static and freestanding functions. Macros luaU_staticfunc
 * and luaU_staticfuncsig work equally to luaU_func and luaU_funcsig, except for that
 * you need to provide a separate metatable for static functions.
 * For example,
 *
 * struct Foo
 * {
 *	 int DoSomething(int, const char*);
 *	 static void DoSomethingElse(int a, int b, float c);
 * };
 *
 * static luaL_reg Foo_metatable[] =
 * {
 *	 { "DoSomething", luaU_func(&Foo::DoSomething) },
 *	 { NULL, NULL }
 * };
 *
 * static luaL_reg Foo_metatable_static[] =
 * {
 *	 { "DoSomethingElse", luaU_staticfunc(&Foo::DoSomethingElse) },
 *	 { NULL, NULL }
 * };
 *
 * void RegisterFoo(lua_State* L)
 * {
 *	 luaW_register<Foo>(L, "Foo", Foo_metatable_static, Foo_metatable);
 * }
 *
 * After that you will be able to use Foo class from Lua like this:
 * local foo = Foo.new()
 * foo:DoSomething(42, 'The Ultimate Question of Life, the Universe, and Everything.') -- member function call
 * Foo:DoSomethingElse(30, 12, 3.1459) -- Static function call
 *
 * These macros and it's underlying templates are somewhat experimental and some
 * refinements are probably needed.  There are cases where it does not
 * currently work and I expect some changes can be made to refine its behavior.
 */

#define luaU_func(memberfunc) &luaU_MemberFuncWrapper<decltype(memberfunc),memberfunc>::call
#define luaU_funcsig(returntype, type, funcname, ...) luaU_func(static_cast<returntype (type::*)(__VA_ARGS__)>(&type::funcname))

#define luaU_staticfunc(func) &luaU_StaticFuncWrapper<decltype(func),func>::call
#define luaU_staticfuncsig(returntype, type, funcname, ...) luaU_staticfunc(static_cast<returntype (*)(__VA_ARGS__)>(&type::funcname))

template<int... ints> struct luaU_IntPack { };
template<int start, int count, int... tail> struct luaU_MakeIntRangeType {
	typedef typename luaU_MakeIntRangeType<start, count-1, start+count-1, tail...>::type type;
};
template<int start, int... tail> struct luaU_MakeIntRangeType<start, 0, tail...> {
	typedef luaU_IntPack<tail...> type;
};
template<int start, int count> inline typename luaU_MakeIntRangeType<start, count>::type luaU_makeIntRange() {
	return typename luaU_MakeIntRangeType<start, count>::type();
}

/**
 * Member function wrapper
 */
template<class MemFunPtrType, MemFunPtrType MemberFunc> struct luaU_MemberFuncWrapper;

template<class T, class ReturnType, class... Args, ReturnType(T::*MemberFunc)(Args...)> struct luaU_MemberFuncWrapper<ReturnType (T::*)(Args...), MemberFunc> {
	public:
		static int call(lua_State* _L) {
			return callImpl(_L, luaU_makeIntRange<2,sizeof...(Args)>());
		}
	private:
		template<int... indices> static int callImpl(lua_State* _L, luaU_IntPack<indices...>) {
			luaU_push<ReturnType>(_L, ((*luaW_check<T>(_L, 1)).*MemberFunc)(luaU_check<typename luaW_remove_cr<Args>::type>(_L, indices)...));
			return 1;
		}
};

template<class T, class... Args, void(T::*MemberFunc)(Args...)> struct luaU_MemberFuncWrapper<void(T::*)(Args...), MemberFunc> {
	public:
		static int call(lua_State* _L) {
			return callImpl(_L, luaU_makeIntRange<2, sizeof...(Args)>());
		}
	private:
		template<int... indices> static int callImpl(lua_State* _L, luaU_IntPack<indices...>) {
			((*luaW_check<T>(_L, 1)).*MemberFunc)(luaU_check<typename luaW_remove_cr<Args>::type>(_L, indices)...);
			return 0;
		}
};


/**
 * static function wrapper
 */

template<class FunPtrType, FunPtrType Func> struct luaU_StaticFuncWrapper;

template<class ReturnType, class... Args, ReturnType(*Func)(Args...)> struct luaU_StaticFuncWrapper<ReturnType(*)(Args...), Func> {
	public:
		static int call(lua_State* _L) {
			return callImpl(_L, luaU_makeIntRange<2,sizeof...(Args)>());
		}
	private:
		template<int... indices> static int callImpl(lua_State* _L, luaU_IntPack<indices...>) {
			luaU_push<ReturnType>(_L, (*Func)(luaU_check<typename luaW_remove_cr<Args>::type>(_L, indices)...));
			return 1;
		}
};

template<class... Args, void(*Func)(Args...)> struct luaU_StaticFuncWrapper<void(*)(Args...), Func> {
	public:
		static int call(lua_State* _L) {
			return callImpl(_L, luaU_makeIntRange<2, sizeof...(Args)>());
		}
	private:
		template<int... indices>
		static int callImpl(lua_State* _L, luaU_IntPack<indices...>) {
			(*Func)(luaU_check<typename luaW_remove_cr<Args>::type>(_L, indices)...);
			return 0;
		}
};

/**
 * Calls the copy constructor for an object of type T.
 * Arguments may be passed in, in case they're needed for the postconstructor
 *
 * e.g.
 *
 * C++:
 * luaL_reg Foo_Metatable[] =
 * {
 *	 { "clone", luaU_clone<Foo> },
 *	 { NULL, NULL }
 * };
 *
 * Lua:
 * foo = Foo.new()
 * foo2 = foo:clone()
 */
template <typename T> int luaU_clone(lua_State* _L) {
	T* obj = new T(*luaW_check<T>(_L, 1));
	lua_remove(_L, 1); // ...
	int numargs = lua_gettop(_L);
	luaW_push<T>(_L, obj); // ... clone
	luaW_hold<T>(_L, obj);
	luaW_postconstructor<T>(_L, numargs);
	return 1;
}

/**
 * luaU_build is intended to be used to initialize many values by passing in a
 * table. They keys of the table are used as function names, and values are
 * used as arguments to the function. This is intended to be used on functions
 * that are simple setters.
 *
 * For example, if luaU_build is set as the post constructor, you can
 * initialize an object as so:
 *
 * f = Foo.new
 * {
 *	 X = 10;
 *	 Y = 20;
 * }
 *
 * After the object is constructed, luaU_build will do the equivalent of
 * calling f:X(10) and f:Y(20).
 */
template <typename T> int luaU_build(lua_State* _L) {
	// obj {}
	lua_insert(_L, -2); // {} obj
	if (lua_type(_L, 1) == LUA_TTABLE) {
		for (lua_pushnil(_L); lua_next(_L, 1); lua_pop(_L, 1)) {
			// {} obj k v
			lua_pushvalue(_L, -2); // {} obj k v k
			lua_gettable(_L, -4); // {} obj k v ud[k]
			lua_pushvalue(_L, -4); // {} obj k v ud[k] ud
			lua_pushvalue(_L, -3); // {} obj k v ud[k] ud v
			lua_call(_L, 2, 0); // {} obj k v
		}
		// {} ud
	}
	return 0;
}

/**
 * Takes the object of type T at the top of the stack and stores it in on a
 * table with the name storagetable, on the table at the specified _index.
 *
 * You may manually call luaW_hold and luaW_release to handle pointer
 * ownership, but it is often easier to simply store a Lua userdata on a table
 * that is owned by its parent. This ensures that your object will not be
 * prematurely freed, and that it can only be destoryed after its parent.
 *
 * e.g.
 *
 * Foo* parent = luaW_check<Foo>(_L, 1);
 * Bar* child = luaW_check<Bar>(_L, 2);
 * if (parent && child)
 * {
 *	 luaU_store<Bar>(_L, 1, "Children");
 *	 parent->AddChild(child);
 * }
 */
template <typename T> void luaU_store(lua_State* _L, int _index, const char* _storagetable, const char* _key = NULL) {
	// ... store ... obj store.storagetable
	lua_getfield(_L, _index, _storagetable);
	if (_key) {
		// ... store ... obj store.storagetable key
		lua_pushstring(_L, _key);
	} else {
		// ... store ... obj store.storagetable key
		LuaWrapper<T>::identifier(_L, luaW_to<T>(_L, -2));
	}
	// ... store ... obj store.storagetable key obj
	lua_pushvalue(_L, -3);
	// ... store ... obj store.storagetable
	lua_settable(_L, -3);
	// ... store ... obj
	lua_pop(_L, 1);
}

