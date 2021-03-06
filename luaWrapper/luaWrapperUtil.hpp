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

namespace luaWrapper {
	namespace utils {
		/**
		 * @brief This template removes reference and const qualifier from the type
		 */
		template <typename LUAW_TYPE> struct remove_cr {
			typedef typename std::remove_const<typename std::remove_reference<LUAW_TYPE>::type>::type type;
		};
		
		
		/**
		 * This is slightly different than the previous three functions in that you
		 * shouldn't need to write your own version of it, since it uses check
		 * automatically.
		 */
		template<typename U> U opt(lua_State* _luaState, int _index, const U& _fallback = U()) {
			if (lua_isnil(_luaState, _index)) {
				return _fallback;
			} else {
				return luaWrapper::utils::check<U>(_luaState, _index);
			}
		}
		
		/**
		 * These are just some functions I've always felt should exist
		 */
		template <typename U> inline U getfield(lua_State* _luaState, int _index, const char* _field) {
			static_assert(!LUAW_STD::is_same<U, const char*>::value,
				"luaWrapper::utils::getfield is not safe to use on const char*'s. (The string will be popped from the stack.)");
			lua_getfield(_luaState, _index, _field);
			U val = luaWrapper::utils::to<U>(_luaState, -1);
			lua_pop(_luaState, 1);
			return val;
		}
		
		template <typename U> inline U checkfield(lua_State* _luaState, int _index, const char* _field) {
			static_assert(!LUAW_STD::is_same<U, const char*>::value,
				"luaWrapper::utils::checkfield is not safe to use on const char*'s. (The string will be popped from the stack.)");
			lua_getfield(_luaState, _index, _field);
			U val = luaWrapper::utils::check<U>(_luaState, -1);
			lua_pop(_luaState, 1);
			return val;
		}
		
		template <typename U> inline U optfield(lua_State* _luaState, int _index, const char* _field, const U& _fallback = U()) {
			static_assert(!LUAW_STD::is_same<U, const char*>::value,
				"luaWrapper::utils::getfield is not safe to use on const char*'s. (The string will be popped from the stack.)");
			lua_getfield(_luaState, _index, _field);
			U val = luaWrapper::utils::opt<U>(_luaState, -1, _fallback);
			lua_pop(_luaState, 1);
			return val;
		}
		
		template <typename U> inline void setfield(lua_State* _luaState, int _index, const char* _field, U _val) {
			luaWrapper::utils::push<U>(_luaState, _val);
			lua_setfield(_luaState, luaWrapper::correctindex(_luaState, _index, 1), _field);
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
		 * getter and setter functions, there is a getSet which does both - if an
		 * argument is supplied it attempts to set the value, and in either case it
		 * returns the value. In your lua table declaration in C++ rather than write
		 * individiual wrappers for each getter and setter you may do the following:
		 *
		 * static luaL_reg Foo_metatable[] =
		 * {
		 *	 { "GetBar", luaWrapper::utils::get<Foo, bool, &Widget::GetBar> },
		 *	 { "SetBar", luaWrapper::utils::set<Foo, bool, &Widget::SetBar> },
		 *	 { "Bar", luaWrapper::utils::getSet<Foo, bool, &Widget::GetBar, &Widget::SetBar> },
		 *	 { NULL, NULL }
		 * }
		 *
		 * Getters and setters must have one of the following signatures:
		 *	void LUAW_TYPE::Setter(U _value);
		 *	void LUAW_TYPE::Setter(U* _value);
		 *	void LUAW_TYPE::Setter(const U& _value);
		 *	U Getter() const;
		 *	U* Getter() const;
		 *
		 * In this example, the variable Foo::bar is private and so it is accessed
		 * through getter and setter functions. If Foo::bar were public, it could be
		 * accessed directly, like so:
		 *
		 * static luaL_reg Foo_metatable[] =
		 * {
		 *	 { "GetBar", luaWrapper::utils::get<Foo, bool, &Widget::bar> },
		 *	 { "SetBar", luaWrapper::utils::set<Foo, bool, &Widget::bar> },
		 *	 { "Bar", luaWrapper::utils::getSet<Foo, bool, &Widget::bar> },
		 * }
		 *
		 * In a Lua script, you can now use foo:GetBar(), foo:SetBar() and foo:Bar()
		 */
		
		template <typename LUAW_TYPE, typename U, U LUAW_TYPE::*Member> int get(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			luaWrapper::utils::push<U>(_luaState, obj.get()->*Member);
			return 1;
		}
		
		template <typename LUAW_TYPE, typename U, U* LUAW_TYPE::*Member> int get(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			luaWrapper::push<U>(_luaState, obj.get()->*Member);
			return 1;
		}
		
		template <typename LUAW_TYPE, typename U, U (LUAW_TYPE::*Getter)() const> int get(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			luaWrapper::utils::push<U>(_luaState, (obj.get()->*Getter)());
			return 1;
		}
		
		template <typename LUAW_TYPE, typename U, const U& (LUAW_TYPE::*Getter)() const> int get(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			luaWrapper::utils::push<U>(_luaState, (obj.get()->*Getter)());
			return 1;
		}
		
		template <typename LUAW_TYPE, typename U, U* (LUAW_TYPE::*Getter)() const> int get(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			luaWrapper::push<U>(_luaState, (obj.get()->*Getter)());
			return 1;
		}
		
		template <typename LUAW_TYPE, typename U, U LUAW_TYPE::*Member> int set(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (obj != null) {
				obj.get()->*Member = luaWrapper::utils::check<U>(_luaState, 2);
			}
			return 0;
		}
		
		template <typename LUAW_TYPE, typename U, U* LUAW_TYPE::*Member> int set(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (obj != null) {
				ememory::SharedPtr<U> member = luaWrapper::opt<U>(_luaState, 2);
				obj.get()->*Member = member.get();
			}
			return 0;
		}
		
		template <typename LUAW_TYPE, typename U, const U* LUAW_TYPE::*Member> int set(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (obj != null) {
				ememory::SharedPtr<U> member = luaWrapper::opt<U>(_luaState, 2);
				obj.get()->*Member = member.get();
			}
			return 0;
		}
		
		template <typename LUAW_TYPE, typename U, const U* LUAW_TYPE::*Member> int setAndRelease(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (obj != null) {
				ememory::SharedPtr<U> member = luaWrapper::opt<U>(_luaState, 2);
				obj.get()->*Member = member.get();
				if (member) {
					luaWrapper::release<U>(_luaState, member);
				}
			}
			return 0;
		}
		
		template <typename LUAW_TYPE, typename U, void (LUAW_TYPE::*Setter)(U)> int set(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (obj != null) {
				(obj.get()->*Setter)(luaWrapper::utils::check<U>(_luaState, 2));
			}
			return 0;
		}
		
		template <typename LUAW_TYPE, typename U, void (LUAW_TYPE::*Setter)(const U&)> int set(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (obj != null) {
				(obj.get()->*Setter)(luaWrapper::utils::check<U>(_luaState, 2));
			}
			return 0;
		}
		
		template <typename LUAW_TYPE, typename U, void (LUAW_TYPE::*Setter)(U*)> int set(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (obj != null) {
				ememory::SharedPtr<U> member = luaWrapper::opt<U>(_luaState, 2);
				(obj.get()->*Setter)(member.get());
			}
			return 0;
		}
		
		template <typename LUAW_TYPE, typename U, void (LUAW_TYPE::*Setter)(U*)> int setAndRelease(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (obj != null) {
				ememory::SharedPtr<U> member = luaWrapper::opt<U>(_luaState, 2);
				(obj.get()->*Setter)(member);
				if (member) {
					luaWrapper::release<U>(_luaState, member);
				}
			}
			return 0;
		}
		
		template <typename LUAW_TYPE, typename U, U LUAW_TYPE::*Member> int getSet(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (    obj != null
			     && lua_gettop(_luaState) >= 2) {
				obj.get()->*Member = luaWrapper::utils::check<U>(_luaState, 2);
				return 0;
			} else {
				luaWrapper::utils::push<U>(_luaState, obj.get()->*Member);
				return 1;
			}
		}
		
		template <typename LUAW_TYPE, typename U, U* LUAW_TYPE::*Member> int getSet(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (    obj != null
			     && lua_gettop(_luaState) >= 2) {
				ememory::SharedPtr<U> member = luaWrapper::opt<U>(_luaState, 2);
				obj.get()->*Member = member.get();
				return 0;
			} else {
				luaWrapper::push<U>(_luaState, obj.get()->*Member);
				return 1;
			}
		}
		
		template <typename LUAW_TYPE, typename U, U* LUAW_TYPE::*Member> int getSetAndRelease(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (    obj != null
			     && lua_gettop(_luaState) >= 2) {
				ememory::SharedPtr<U> member = luaWrapper::opt<U>(_luaState, 2);
				obj.get()->*Member = member.get();
				if (member)
					luaWrapper::release<U>(_luaState, member);
				return 0;
			} else {
				luaWrapper::push<U>(_luaState, obj.get()->*Member);
				return 1;
			}
		}
		
		template <typename LUAW_TYPE, typename U, U (LUAW_TYPE::*Getter)() const, void (LUAW_TYPE::*Setter)(U)> int getSet(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (    obj != null
			     && lua_gettop(_luaState) >= 2) {
				(obj.get()->*Setter)(luaWrapper::utils::check<U>(_luaState, 2));
				return 0;
			} else {
				luaWrapper::utils::push<U>(_luaState, (obj.get()->*Getter)());
				return 1;
			}
		}
		
		template <typename LUAW_TYPE, typename U, U (LUAW_TYPE::*Getter)() const, void (LUAW_TYPE::*Setter)(const U&)> int getSet(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (    obj != null
			     && lua_gettop(_luaState) >= 2) {
				(obj.get()->*Setter)(luaWrapper::utils::check<U>(_luaState, 2));
				return 0;
			} else {
				luaWrapper::utils::push<U>(_luaState, (obj.get()->*Getter)());
				return 1;
			}
		}
		
		template <typename LUAW_TYPE, typename U, const U& (LUAW_TYPE::*Getter)() const, void (LUAW_TYPE::*Setter)(const U&)> int getSet(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (    obj != null
			     && lua_gettop(_luaState) >= 2) {
				(obj.get()->*Setter)(luaWrapper::utils::check<U>(_luaState, 2));
				return 0;
			} else {
				luaWrapper::utils::push<U>(_luaState, (obj.get()->*Getter)());
				return 1;
			}
		}
		
		template <typename LUAW_TYPE, typename U, U* (LUAW_TYPE::*Getter)() const, void (LUAW_TYPE::*Setter)(U*)> int getSet(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (    obj != null
			     && lua_gettop(_luaState) >= 2) {
				ememory::SharedPtr<U> member = luaWrapper::opt<U>(_luaState, 2);
				(obj.get()->*Setter)(member.get());
				return 0;
			} else {
				luaWrapper::push<U>(_luaState, (obj.get()->*Getter)());
				return 1;
			}
		}
		
		template <typename LUAW_TYPE, typename U, U* (LUAW_TYPE::*Getter)() const, void (LUAW_TYPE::*Setter)(U*)> int getSetAndRelease(lua_State* _luaState) {
			ememory::SharedPtr<LUAW_TYPE> obj = luaWrapper::check<LUAW_TYPE>(_luaState, 1);
			if (    obj != null
			     && lua_gettop(_luaState) >= 2) {
				ememory::SharedPtr<U> member = luaWrapper::opt<U>(_luaState, 2);
				(obj.get()->*Setter)(member);
				if (member)
					luaWrapper::release<U>(_luaState, member);
				return 0;
			} else {
				luaWrapper::push<U>(_luaState, (obj.get()->*Getter)());
				return 1;
			}
		}
		
		/**
		 * luaWrapper::utils::func is a special macro that expands into a simple function wrapper.
		 * Unlike the getter setters above, you merely need to name the function you
		 * would like to wrap.
		 *
		 * For example,
		 *
		 * struct Foo
		 * {
		 *	 int doSomething(int, const char*);
		 * };
		 *
		 * static luaL_reg Foo_metatable[] =
		 * {
		 *	 { "doSomething", luaWrapper::utils::func(&Foo::doSomething) },
		 *	 { NULL, NULL }
		 * }
		 *
		 * void RegisterFoo(lua_State* L)
		 * {
		 *	 luaWrapper::registerElement<Foo>(L, "Foo", NULL, Foo_metatable);
		 * }
		 *
		 * This macro will expand based on the function signature of Foo::DoSomething
		 * In this example, it would expand into the following wrapper:
		 *
		 *	 luaWrapper::utils::push(luaWrapper::check<T>(L, 1)->doSomething(luaWrapper::utils::check<int>(L, 2), luaWrapper::utils::check<const char*>(L, 3)));
		 *	 return 1;
		 *
		 * In this example there is only one member function called DoSomething. In some
		 * cases there may be multiple overrides for a function. For those cases, an
		 * additional macro has been provided, luaWrapper::utils::funcsig, which takes the entire
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
		 *	 {"DoSomething1", luaWrapper::utils::funcsig(int, Foo, DoSomething, const char*)) },
		 *	 {"DoSomething1", luaWrapper::utils::funcsig(int, Foo, DoSomething, const char*, int)) },
		 *	 { NULL, NULL }
		 * };
		 *
		 * There`s also support for static and freestanding functions. Macros luaWrapper::utils::staticfunc
		 * and luaWrapper::utils::staticfuncsig work equally to luaWrapper::utils::func and luaWrapper::utils::funcsig, except for that
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
		 *	 { "DoSomething", luaWrapper::utils::func(&Foo::DoSomething) },
		 *	 { NULL, NULL }
		 * };
		 *
		 * static luaL_reg Foo_metatable_static[] =
		 * {
		 *	 { "DoSomethingElse", luaWrapper::utils::staticfunc(&Foo::DoSomethingElse) },
		 *	 { NULL, NULL }
		 * };
		 *
		 * void RegisterFoo(lua_State* L)
		 * {
		 *	 luaWrapper::registerElement<Foo>(L, "Foo", Foo_metatable_static, Foo_metatable);
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
		
		#define luaWrapperUtils_func(memberfunc) &luaWrapper::utils::MemberFuncWrapper<decltype(memberfunc),memberfunc>::call
		#define luaWrapperUtils_funcsig(returntype, type, funcname, ...) luaWrapperUtilsfunc(static_cast<returntype (type::*)(__VA_ARGS__)>(&type::funcname))
		
		#define luaWrapperUtils_staticfunc(func) &luaWrapper::utils::StaticFuncWrapper<decltype(func),func>::call
		#define luaWrapperUtils_staticfuncsig(returntype, type, funcname, ...) luaWrapperUtilsstaticfunc(static_cast<returntype (*)(__VA_ARGS__)>(&type::funcname))
		
		template<int... ints> struct IntPack { };
		template<int start, int count, int... tail> struct MakeIntRangeType {
			typedef typename MakeIntRangeType<start, count-1, start+count-1, tail...>::type type;
		};
		template<int start, int... tail> struct MakeIntRangeType<start, 0, tail...> {
			typedef IntPack<tail...> type;
		};
		template<int start, int count> inline typename MakeIntRangeType<start, count>::type makeIntRange() {
			return typename MakeIntRangeType<start, count>::type();
		}
		
		/**
		 * Member function wrapper
		 */
		template<class LUAW_MEMORY_FUNCTION_POINTER_TYPE, LUAW_MEMORY_FUNCTION_POINTER_TYPE MemberFunc>
		struct MemberFuncWrapper;
		
		template<class LUAW_TYPE, class ReturnType, class... Args, ReturnType(LUAW_TYPE::*MemberFunc)(Args...)>
		struct MemberFuncWrapper<ReturnType (LUAW_TYPE::*)(Args...), MemberFunc> {
			public:
				static int call(lua_State* _luaState) {
					return callImpl(_luaState, makeIntRange<2,sizeof...(Args)>());
				}
			private:
				template<int... indices> static int callImpl(lua_State* _luaState, IntPack<indices...>) {
					luaWrapper::utils::push<ReturnType>(_luaState, ((*luaWrapper::check<LUAW_TYPE>(_luaState, 1)).*MemberFunc)(luaWrapper::utils::check<typename luaWrapper::utils::remove_cr<Args>::type>(_luaState, indices)...));
					return 1;
				}
		};
		
		template<class LUAW_TYPE, class... Args, void(LUAW_TYPE::*MemberFunc)(Args...)>
		struct MemberFuncWrapper<void(LUAW_TYPE::*)(Args...), MemberFunc> {
			public:
				static int call(lua_State* _luaState) {
					return callImpl(_luaState, luaWrapper::utils::makeIntRange<2, sizeof...(Args)>());
				}
			private:
				template<int... indices>
				static int callImpl(lua_State* _luaState, IntPack<indices...>) {
					((*luaWrapper::check<LUAW_TYPE>(_luaState, 1)).*MemberFunc)(luaWrapper::utils::check<typename luaWrapper::utils::remove_cr<Args>::type>(_luaState, indices)...);
					return 0;
				}
		};
		
		
		/**
		 * static function wrapper
		 */
		
		template<class LUAW_FUNCTION_POINTER_TYPE, LUAW_FUNCTION_POINTER_TYPE LUAW_FUNCTION>
		struct StaticFuncWrapper;
		
		template<class ReturnType, class... Args, ReturnType(*LUAW_FUNCTION)(Args...)>
		struct StaticFuncWrapper<ReturnType(*)(Args...), LUAW_FUNCTION> {
			public:
				static int call(lua_State* _luaState) {
					return callImpl(_luaState, luaWrapper::utils::makeIntRange<2,sizeof...(Args)>());
				}
			private:
				template<int... indices> static int callImpl(lua_State* _luaState, IntPack<indices...>) {
					luaWrapper::utils::push<ReturnType>(_luaState, (*LUAW_FUNCTION)(luaWrapper::utils::check<typename luaWrapper::utils::remove_cr<Args>::type>(_luaState, indices)...));
					return 1;
				}
		};
		
		template<class... Args, void(*LUAW_FUNCTION)(Args...)>
		struct StaticFuncWrapper<void(*)(Args...), LUAW_FUNCTION> {
			public:
				static int call(lua_State* _luaState) {
					return callImpl(_luaState, luaWrapper::utils::makeIntRange<2, sizeof...(Args)>());
				}
			private:
				template<int... indices>
				static int callImpl(lua_State* _luaState, luaWrapper::utils::IntPack<indices...>) {
					(*LUAW_FUNCTION)(luaWrapper::utils::check<typename luaWrapper::utils::remove_cr<Args>::type>(_luaState, indices)...);
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
		 *	 { "clone", luaWrapper::utils::clone<Foo> },
		 *	 { NULL, NULL }
		 * };
		 *
		 * Lua:
		 * foo = Foo.new()
		 * foo2 = foo:clone()
		 */
		template <typename LUAW_TYPE> int clone(lua_State* _luaState) {
			LUAW_TYPE* obj = new LUAW_TYPE(*luaWrapper::check<LUAW_TYPE>(_luaState, 1));
			lua_remove(_luaState, 1); // ...
			int numargs = lua_gettop(_luaState);
			luaWrapper::push<LUAW_TYPE>(_luaState, obj); // ... clone
			luaWrapper::hold<LUAW_TYPE>(_luaState, obj);
			luaWrapper::postconstructor<LUAW_TYPE>(_luaState, numargs);
			return 1;
		}
		
		/**
		 * luaWrapper::utils::build is intended to be used to initialize many values by passing in a
		 * table. They keys of the table are used as function names, and values are
		 * used as arguments to the function. This is intended to be used on functions
		 * that are simple setters.
		 *
		 * For example, if luaWrapper::utils::build is set as the post constructor, you can
		 * initialize an object as so:
		 *
		 * f = Foo.new
		 * {
		 *	 X = 10;
		 *	 Y = 20;
		 * }
		 *
		 * After the object is constructed, luaWrapper::utils::build will do the equivalent of
		 * calling f:X(10) and f:Y(20).
		 */
		template <typename LUAW_TYPE> int build(lua_State* _luaState) {
			// obj {}
			lua_insert(_luaState, -2); // {} obj
			if (lua_type(_luaState, 1) == LUA_TTABLE) {
				for (lua_pushnil(_luaState); lua_next(_luaState, 1); lua_pop(_luaState, 1)) {
					// {} obj k v
					lua_pushvalue(_luaState, -2); // {} obj k v k
					lua_gettable(_luaState, -4); // {} obj k v ud[k]
					lua_pushvalue(_luaState, -4); // {} obj k v ud[k] ud
					lua_pushvalue(_luaState, -3); // {} obj k v ud[k] ud v
					lua_call(_luaState, 2, 0); // {} obj k v
				}
				// {} ud
			}
			return 0;
		}
		
		/**
		 * Takes the object of type T at the top of the stack and stores it in on a
		 * table with the name storagetable, on the table at the specified _index.
		 *
		 * You may manually call luaWrapper::hold and luaWrapper::release to handle pointer
		 * ownership, but it is often easier to simply store a Lua userdata on a table
		 * that is owned by its parent. This ensures that your object will not be
		 * prematurely freed, and that it can only be destoryed after its parent.
		 *
		 * e.g.
		 *
		 * Foo* parent = luaWrapper::check<Foo>(_luaState, 1);
		 * Bar* child = luaWrapper::check<Bar>(_luaState, 2);
		 * if (parent && child)
		 * {
		 *	 luaWrapper::utils::store<Bar>(_luaState, 1, "Children");
		 *	 parent->AddChild(child);
		 * }
		 */
		template <typename LUAW_TYPE> void store(lua_State* _luaState, int _index, const char* _storagetable, const char* _key = NULL) {
			// ... store ... obj store.storagetable
			lua_getfield(_luaState, _index, _storagetable);
			if (_key) {
				// ... store ... obj store.storagetable key
				lua_pushstring(_luaState, _key);
			} else {
				// ... store ... obj store.storagetable key
				LuaWrapper<LUAW_TYPE>::identifier(_luaState, luaWrapper::to<LUAW_TYPE>(_luaState, -2));
			}
			// ... store ... obj store.storagetable key obj
			lua_pushvalue(_luaState, -3);
			// ... store ... obj store.storagetable
			lua_settable(_luaState, -3);
			// ... store ... obj
			lua_pop(_luaState, 1);
		}
	}
}
