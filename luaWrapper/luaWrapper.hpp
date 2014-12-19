/**
 * Copyright (c) 2010-2013 Alexander Ames
 * Alexander.Ames@gmail.com
 * See Copyright Notice at the end of this file
 */

/** API Summary:
 *
 * LuaWrapper is a library designed to help bridge the gab between Lua and
 * C++. It is designed to be small (a single header file), simple, fast,
 * and typesafe. It has no external dependencies, and does not need to be
 * precompiled; the header can simply be dropped into a project and used
 * immediately. It even supports class inheritance to a certain degree. Objects
 * can be created in either Lua or C++, and passed back and forth.
 *
 * The main functions of interest are the following:
 *  luaW_is<T>
 *  luaW_to<T>
 *  luaW_check<T>
 *  luaW_push<T>
 *  luaW_register<T>
 *  luaW_setfuncs<T>
 *  luaW_extend<T, U>
 *  luaW_hold<T>
 *  luaW_release<T>
 *
 * These functions allow you to manipulate arbitrary classes just like you
 * would the primitive types (e.g. numbers or strings). If you are familiar
 * with the normal Lua API the behavior of these functions should be very
 * intuative.
 *
 * For more information see the README and the comments below
 */
#ifndef __LUA_WRAPPER_H__
#define __LUA_WRAPPER_H__

// If you are linking against Lua compiled in C++, define LUAW_NO_EXTERN_C
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <memory>

#define LUAW_POSTCTOR_KEY "__postctor"
#define LUAW_EXTENDS_KEY "__extends"
#define LUAW_STORAGE_KEY "storage"
#define LUAW_CACHE_KEY "cache"
#define LUAW_CACHE_METATABLE_KEY "cachemetatable"
#define LUAW_HOLDS_KEY "holds"
#define LUAW_WRAPPER_KEY "LuaWrapper"
/**
 * A simple utility function to adjust a given index
 * Useful for when a parameter index needs to be adjusted
 * after pushing or popping things off the stack
 */
inline int luaW_correctindex(lua_State* _L, int _index, int _correction) {
	return _index < 0 ? _index - _correction : _index;
}

/**
 * These are the default allocator and deallocator. If you would prefer an
 * alternative option, you may select a different function when registering
 * your class.
 */
template <typename T> std::shared_ptr<T> luaW_defaultallocator(lua_State* _L) {
	return std::make_shared<T>();
}

/**
 * The identifier function is responsible for pushing a value unique to each
 * object on to the stack. Most of the time, this can simply be the address
 * of the pointer, but sometimes that is not adaquate. For example, if you
 * are using shared_ptr you would need to push the address of the object the
 * shared_ptr represents, rather than the address of the shared_ptr itself.
 */
template <typename T> void luaW_defaultidentifier(lua_State* _L, std::shared_ptr<T> _obj) {
	lua_pushlightuserdata(_L, _obj.get());
}

/**
 * This class is what is used by LuaWrapper to contain the userdata. data
 * stores a pointer to the object itself, and cast is used to cast toward the
 * base class if there is one and it is necessary. Rather than use RTTI and
 * typid to compare types, I use the clever trick of using the cast to compare
 * types. Because there is at most one cast per type, I can use it to identify
 * when and object is the type I want. This is only used internally.
 */
struct luaW_Userdata {
	luaW_Userdata(std::shared_ptr<void> vptr = NULL, luaW_Userdata (*udcast)(const luaW_Userdata&) = NULL) :
	  data(vptr),
	  cast(udcast) {
		// nothing to do ...
	}
	std::shared_ptr<void> data;
	luaW_Userdata (*cast)(const luaW_Userdata&);
};

/**
 * This class cannot actually to be instantiated. It is used only hold the
 * table name and other information.
 */
template <typename T> class LuaWrapper {
	public:
		static const char* classname;
		static void (*identifier)(lua_State*, std::shared_ptr<T>);
		static std::shared_ptr<T> (*allocator)(lua_State*);
		static luaW_Userdata (*cast)(const luaW_Userdata&);
		static void (*postconstructorrecurse)(lua_State* _L, int numargs);
	private:
		LuaWrapper();
};
template <typename T> const char* LuaWrapper<T>::classname;
template <typename T> void (*LuaWrapper<T>::identifier)(lua_State*, std::shared_ptr<T>);
template <typename T> std::shared_ptr<T> (*LuaWrapper<T>::allocator)(lua_State*);
template <typename T> luaW_Userdata (*LuaWrapper<T>::cast)(const luaW_Userdata&);
template <typename T> void (*LuaWrapper<T>::postconstructorrecurse)(lua_State* _L, int _numargs);

/**
 * Cast from an object of type T to an object of type U. This template
 * function is instantiated by calling luaW_extend<T, U>(L). This is only used
 * internally.
 */
template <typename T, typename U> luaW_Userdata luaW_cast(const luaW_Userdata& _obj) {
	return luaW_Userdata(static_cast<U*>(std::static_pointer_cast<T>(_obj.data)), LuaWrapper<U>::cast);
}

template <typename T, typename U> void luaW_identify(lua_State* _L, T* _obj) {
	LuaWrapper<U>::identifier(_L, static_cast<U*>(_obj));
}

template <typename T> inline void luaW_wrapperfield(lua_State* _L, const char* _field) {
	lua_getfield(_L, LUA_REGISTRYINDEX, LUAW_WRAPPER_KEY); // ... LuaWrapper
	lua_getfield(_L, -1, _field); // ... LuaWrapper LuaWrapper.field
	lua_getfield(_L, -1, LuaWrapper<T>::classname); // ... LuaWrapper LuaWrapper.field LuaWrapper.field.class
	lua_replace(_L, -3); // ... LuaWrapper.field.class LuaWrapper.field
	lua_pop(_L, 1); // ... LuaWrapper.field.class
}

/**
 * Analogous to lua_is(boolean|string|*)
 *
 * Returns 1 if the value at the given acceptable index is of type T (or if
 * strict is false, convertable to type T) and 0 otherwise.
 */
template <typename T> bool luaW_is(lua_State *_L, int _index, bool _strict = false) {
	bool equal = false;// lua_isnil(_L, index);
	if (!equal && lua_isuserdata(_L, _index) && lua_getmetatable(_L, _index)) {
		// ... ud ... udmt
		luaL_getmetatable(_L, LuaWrapper<T>::classname); // ... ud ... udmt Tmt
		equal = lua_rawequal(_L, -1, -2) != 0;
		if (!equal && !_strict) {
			lua_getfield(_L, -2, LUAW_EXTENDS_KEY); // ... ud ... udmt Tmt udmt.extends
			for (lua_pushnil(_L); lua_next(_L, -2); lua_pop(_L, 1)) {
				// ... ud ... udmt Tmt udmt.extends k v
				equal = lua_rawequal(_L, -1, -4) != 0;
				if (equal) {
					lua_pop(_L, 2); // ... ud ... udmt Tmt udmt.extends
					break;
				}
			}
			lua_pop(_L, 1); // ... ud ... udmt Tmt
		}
		lua_pop(_L, 2); // ... ud ...
	}
	return equal;
}

/**
 * Analogous to lua_to(boolean|string|*)
 *
 * Converts the given acceptable index to a T*. That value must be of (or
 * convertable to) type T; otherwise, returns NULL.
 */
template <typename T> std::shared_ptr<T> luaW_to(lua_State* _L, int _index, bool _strict = false) {
	if (luaW_is<T>(_L, _index, _strict)) {
		luaW_Userdata* pud = static_cast<luaW_Userdata*>(lua_touserdata(_L, _index));
		luaW_Userdata ud;
		while (    !_strict
		        && LuaWrapper<T>::cast != pud->cast) {
			ud = pud->cast(*pud);
			pud = &ud;
		}
		return std::static_pointer_cast<T>(pud->data);
	}
	return NULL;
}

/**
 * Analogous to luaL_check(boolean|string|*)
 *
 * Converts the given acceptable index to a T*. That value must be of (or
 * convertable to) type T; otherwise, an error is raised.
 */
template <typename T> std::shared_ptr<T> luaW_check(lua_State* _L, int _index, bool _strict = false) {
	std::shared_ptr<T> obj;
	if (luaW_is<T>(_L, _index, _strict)) {
		luaW_Userdata* pud = static_cast<luaW_Userdata*>(lua_touserdata(_L, _index));
		luaW_Userdata ud;
		while (!_strict && LuaWrapper<T>::cast != pud->cast) {
			ud = pud->cast(*pud);
			pud = &ud;
		}
		obj = std::static_pointer_cast<T>(pud->data);
	} else {
		const char *msg = lua_pushfstring(_L, "%s expected, got %s", LuaWrapper<T>::classname, luaL_typename(_L, _index));
		luaL_argerror(_L, _index, msg);
	}
	return obj;
}

template <typename T> std::shared_ptr<T> luaW_opt(lua_State* _L, int _index, std::shared_ptr<T> _fallback = nullptr, bool _strict = false) {
	if (lua_isnil(_L, _index)) {
		return _fallback;
	} else {
		return luaW_check<T>(_L, _index, _strict);
	}
}

/**
 * Analogous to lua_push(boolean|string|*)
 *
 * Pushes a userdata of type T onto the stack. If this object already exists in
 * the Lua environment, it will assign the existing storage table to it.
 * Otherwise, a new storage table will be created for it.
 */
template <typename T> void luaW_push(lua_State* _L, std::shared_ptr<T> _obj) {
	if (_obj) {
		LuaWrapper<T>::identifier(_L, _obj); // ... id
		luaW_wrapperfield<T>(_L, LUAW_CACHE_KEY); // ... id cache
		lua_pushvalue(_L, -2); // ... id cache id
		lua_gettable(_L, -2); // ... id cache obj
		if (lua_isnil(_L, -1)) {
			// Create the new luaW_userdata and place it in the cache
			lua_pop(_L, 1); // ... id cache
			lua_insert(_L, -2); // ... cache id
			luaW_Userdata* ud = static_cast<luaW_Userdata*>(lua_newuserdata(_L, sizeof(luaW_Userdata))); // ... cache id obj
			ud->data = _obj;
			ud->cast = LuaWrapper<T>::cast;
			lua_pushvalue(_L, -1); // ... cache id obj obj
			lua_insert(_L, -4); // ... obj cache id obj
			lua_settable(_L, -3); // ... obj cache
			luaL_getmetatable(_L, LuaWrapper<T>::classname); // ... obj cache mt
			lua_setmetatable(_L, -3); // ... obj cache
			lua_pop(_L, 1); // ... obj
		} else {
			lua_replace(_L, -3); // ... obj cache
			lua_pop(_L, 1); // ... obj
		}
	} else {
		lua_pushnil(_L);
	}
}

/**
 * Instructs LuaWrapper that it owns the userdata, and can manage its memory.
 * When all references to the object are removed, Lua is free to garbage
 * collect it and delete the object.
 *
 * Returns true if luaW_hold took hold of the object, and false if it was
 * already held
 */
template <typename T> bool luaW_hold(lua_State* _L, std::shared_ptr<T> _obj) {
	luaW_wrapperfield<T>(_L, LUAW_HOLDS_KEY); // ... holds
	LuaWrapper<T>::identifier(_L, _obj); // ... holds id
	lua_pushvalue(_L, -1); // ... holds id id
	lua_gettable(_L, -3); // ... holds id hold
	// If it's not held, hold it
	if (!lua_toboolean(_L, -1)) {
		// Apply hold boolean
		lua_pop(_L, 1); // ... holds id
		lua_pushboolean(_L, true); // ... holds id true
		lua_settable(_L, -3); // ... holds
		lua_pop(_L, 1); // ...
		return true;
	}
	lua_pop(_L, 3); // ...
	return false;
}

/**
 * Releases LuaWrapper's hold on an object. This allows the user to remove
 * all references to an object in Lua and ensure that Lua will not attempt to
 * garbage collect it.
 *
 * This function takes the index of the identifier for an object rather than
 * the object itself. This is because needs to be able to run after the object
 * has already been deallocated. A wrapper is provided for when it is more
 * convenient to pass in the object directly.
 */
template <typename T> void luaW_release(lua_State* _L, int _index) {
	luaW_wrapperfield<T>(_L, LUAW_HOLDS_KEY); // ... id ... holds
	lua_pushvalue(_L, luaW_correctindex(_L, _index, 1)); // ... id ... holds id
	lua_pushnil(_L); // ... id ... holds id nil
	lua_settable(_L, -3); // ... id ... holds
	lua_pop(_L, 1); // ... id ...
}

template <typename T> void luaW_release(lua_State* _L, T* _obj) {
	LuaWrapper<T>::identifier(_L, _obj); // ... id
	luaW_release<T>(_L, -1); // ... id
	lua_pop(_L, 1); // ...
}

template <typename T> void luaW_postconstructorinternal(lua_State* _L, int _numargs) {
	// ... ud args...
	if (LuaWrapper<T>::postconstructorrecurse) {
		LuaWrapper<T>::postconstructorrecurse(_L, _numargs);
	}
	luaL_getmetatable(_L, LuaWrapper<T>::classname); // ... ud args... mt
	lua_getfield(_L, -1, LUAW_POSTCTOR_KEY); // ... ud args... mt postctor
	if (lua_type(_L, -1) == LUA_TFUNCTION) {
		for (int i = 0; i < _numargs + 1; i++) {
			lua_pushvalue(_L, -3 - _numargs); // ... ud args... mt postctor ud args...
		}
		lua_call(_L, _numargs + 1, 0); // ... ud args... mt
		lua_pop(_L, 1); // ... ud args...
	} else {
		lua_pop(_L, 2); // ... ud args...
	}
}

/**
 * This function is called from Lua, not C++
 *
 * Calls the lua post-constructor (LUAW_POSTCTOR_KEY or "__postctor") on a
 * userdata. Assumes the userdata is on the stack and numargs arguments follow
 * it. This runs the LUAW_POSTCTOR_KEY function on T's metatable, using the
 * object as the first argument and whatever else is below it as the rest of the
 * arguments This exists to allow types to adjust values in thier storage table,
 * which can not be created until after the constructor is called.
 */
template <typename T> void luaW_postconstructor(lua_State* _L, int _numargs) {
	// ... ud args...
	luaW_postconstructorinternal<T>(_L, _numargs); // ... ud args...
	lua_pop(_L, _numargs); // ... ud
}

/**
 * This function is generally called from Lua, not C++
 *
 * Creates an object of type T using the constructor and subsequently calls the
 * post-constructor on it.
 */
template <typename T> inline int luaW_new(lua_State* _L, int _numargs) {
	// ... args...
	std::shared_ptr<T> obj = LuaWrapper<T>::allocator(_L);
	luaW_push<T>(_L, obj); // ... args... ud
	luaW_hold<T>(_L, obj);
	lua_insert(_L, -1 - _numargs); // ... ud args...
	luaW_postconstructor<T>(_L, _numargs); // ... ud
	return 1;
}

template <typename T> int luaW_new(lua_State* _L) {
	return luaW_new<T>(_L, lua_gettop(_L));
}

/**
 * This function is called from Lua, not C++
 *
 * The default metamethod to call when indexing into lua userdata representing
 * an object of type T. This will first check the userdata's environment table
 * and if it's not found there it will check the metatable. This is done so
 * individual userdata can be treated as a table, and can hold thier own
 * values.
 */
template <typename T> int luaW_index(lua_State* _L) {
	// obj key
	std::shared_ptr<T> obj = luaW_to<T>(_L, 1);
	luaW_wrapperfield<T>(_L, LUAW_STORAGE_KEY); // obj key storage
	LuaWrapper<T>::identifier(_L, obj); // obj key storage id
	lua_gettable(_L, -2); // obj key storage store
	// Check if storage table exists
	if (!lua_isnil(_L, -1)) {
		lua_pushvalue(_L, -3); // obj key storage store key
		lua_gettable(_L, -2); // obj key storage store store[k]
	}
	// If either there is no storage table or the key wasn't found
	// then fall back to the metatable
	if (lua_isnil(_L, -1)) {
		lua_settop(_L, 2); // obj key
		lua_getmetatable(_L, -2); // obj key mt
		lua_pushvalue(_L, -2); // obj key mt k
		lua_gettable(_L, -2); // obj key mt mt[k]
	}
	return 1;
}

/**
 * This function is called from Lua, not C++
 *
 * The default metamethod to call when creating a new index on lua userdata
 * representing an object of type T. This will index into the the userdata's
 * environment table that it keeps for personal storage. This is done so
 * individual userdata can be treated as a table, and can hold thier own
 * values.
 */
template <typename T> int luaW_newindex(lua_State* _L) {
	// obj key value
	std::shared_ptr<T> obj = luaW_check<T>(_L, 1);
	luaW_wrapperfield<T>(_L, LUAW_STORAGE_KEY); // obj key value storage
	LuaWrapper<T>::identifier(_L, obj); // obj key value storage id
	lua_pushvalue(_L, -1); // obj key value storage id id
	lua_gettable(_L, -3); // obj key value storage id store
	// Add the storage table if there isn't one already
	if (lua_isnil(_L, -1)) {
		lua_pop(_L, 1); // obj key value storage id
		lua_newtable(_L); // obj key value storage id store
		lua_pushvalue(_L, -1); // obj key value storage id store store
		lua_insert(_L, -3); // obj key value storage store id store
		lua_settable(_L, -4); // obj key value storage store
	}
	lua_pushvalue(_L, 2); // obj key value ... store key
	lua_pushvalue(_L, 3); // obj key value ... store key value
	lua_settable(_L, -3); // obj key value ... store
	return 0;
}

/**
 * This function is called from Lua, not C++
 *
 * The __gc metamethod handles cleaning up userdata. The userdata's reference
 * count is decremented and if this is the final reference to the userdata its
 * environment table is nil'd and pointer deleted with the destructor callback.
 */
template <typename T> int luaW_gc(lua_State* _L) {
	// obj
	std::shared_ptr<T> obj = luaW_to<T>(_L, 1);
	LuaWrapper<T>::identifier(_L, obj); // obj key value storage id
	luaW_wrapperfield<T>(_L, LUAW_HOLDS_KEY); // obj id counts count holds
	lua_pushvalue(_L, 2); // obj id counts count holds id
	lua_gettable(_L, -2); // obj id counts count holds hold
	if (lua_toboolean(_L, -1)) {
		obj.reset();
	}
	luaW_wrapperfield<T>(_L, LUAW_STORAGE_KEY); // obj id counts count holds hold storage
	lua_pushvalue(_L, 2); // obj id counts count holds hold storage id
	lua_pushnil(_L); // obj id counts count holds hold storage id nil
	lua_settable(_L, -3); // obj id counts count holds hold storage
	luaW_release<T>(_L, 2);
	return 0;
}

/**
 * Thakes two tables and registers them with Lua to the table on the top of the
 * stack. 
 *
 * This function is only called from LuaWrapper internally. 
 */
inline void luaW_registerfuncs(lua_State* _L, const luaL_Reg _defaulttable[], const luaL_Reg _table[]) {
	// ... T
#if LUA_VERSION_NUM == 502
	if (_defaulttable) {
		luaL_setfuncs(_L, _defaulttable, 0); // ... T
	}
	if (_table) {
		luaL_setfuncs(_L, _table, 0); // ... T
	}
#else
	if (_defaulttable) {
		luaL_register(_L, NULL, _defaulttable); // ... T
	}
	if (_table) {
		luaL_register(_L, NULL, _table); // ... T
	}
#endif
}

/**
 * Initializes the LuaWrapper tables used to track internal state. 
 *
 * This function is only called from LuaWrapper internally. 
 */
inline void luaW_initialize(lua_State* _L) {
	// Ensure that the LuaWrapper table is set up
	lua_getfield(_L, LUA_REGISTRYINDEX, LUAW_WRAPPER_KEY); // ... LuaWrapper
	if (lua_isnil(_L, -1)) {
		lua_newtable(_L); // ... nil {}
		lua_pushvalue(_L, -1); // ... nil {} {}
		lua_setfield(_L, LUA_REGISTRYINDEX, LUAW_WRAPPER_KEY); // ... nil LuaWrapper
		// Create a storage table
		lua_newtable(_L); // ... LuaWrapper nil {}
		lua_setfield(_L, -2, LUAW_STORAGE_KEY); // ... nil LuaWrapper
		// Create a holds table
		lua_newtable(_L); // ... LuaWrapper {}
		lua_setfield(_L, -2, LUAW_HOLDS_KEY); // ... nil LuaWrapper
		// Create a cache table, with weak values so that the userdata will not
		// be ref counted
		lua_newtable(_L); // ... nil LuaWrapper {}
		lua_setfield(_L, -2, LUAW_CACHE_KEY); // ... nil LuaWrapper
		lua_newtable(_L); // ... nil LuaWrapper {}
		lua_pushstring(_L, "v"); // ... nil LuaWrapper {} "v"
		lua_setfield(_L, -2, "__mode"); // ... nil LuaWrapper {}
		lua_setfield(_L, -2, LUAW_CACHE_METATABLE_KEY); // ... nil LuaWrapper
		lua_pop(_L, 1); // ... nil
	}
	lua_pop(_L, 1); // ...
}

/**
 * Run luaW_register or luaW_setfuncs to create a table and metatable for your
 * class.  These functions create a table with filled with the function from
 * the table argument in addition to the functions new and build (This is
 * generally for things you think of as static methods in C++). The given
 * metatable argument becomes a metatable for each object of your class. These
 * can be thought of as member functions or methods.
 *
 * You may also supply constructors and destructors for classes that do not
 * have a default constructor or that require special set up or tear down. You
 * may specify NULL as the constructor, which means that you will not be able
 * to call the new function on your class table. You will need to manually push
 * objects from C++. By default, the default constructor is used to create
 * objects and a simple call to delete is used to destroy them.
 *
 * By default LuaWrapper uses the address of C++ object to identify unique
 * objects. In some cases this is not desired, such as in the case of
 * shared_ptrs. Two shared_ptrs may themselves have unique locations in memory
 * but still represent the same object. For cases like that, you may specify an
 * identifier function which is responsible for pushing a key representing your
 * object on to the stack.
 *
 * luaW_register will set table as the new value of the global of the given
 * name. luaW_setfuncs is identical to luaW_register, but it does not set the
 * table globally.  As with luaL_register and luaL_setfuncs, both funcstions
 * leave the new table on the top of the stack.
 */
template <typename T> void luaW_setfuncs(lua_State* _L,
                                         const char* _classname,
                                         const luaL_Reg* _table,
                                         const luaL_Reg* _metatable,
                                         std::shared_ptr<T> (*_allocator)(lua_State*),
                                         void (*_identifier)(lua_State*, std::shared_ptr<T>)) {
	luaW_initialize(_L);
	LuaWrapper<T>::classname = _classname;
	LuaWrapper<T>::identifier = _identifier;
	LuaWrapper<T>::allocator = _allocator;
	const luaL_Reg defaulttable[] = {
		{ "new", luaW_new<T> },
		{ NULL, NULL }
	};
	const luaL_Reg defaultmetatable[] =  {
		{ "__index", luaW_index<T> }, 
		{ "__newindex", luaW_newindex<T> },
		{ "__gc", luaW_gc<T> }, 
		{ NULL, NULL }
	};
	// Set up per-type tables
	lua_getfield(_L, LUA_REGISTRYINDEX, LUAW_WRAPPER_KEY); // ... LuaWrapper
	lua_getfield(_L, -1, LUAW_STORAGE_KEY); // ... LuaWrapper LuaWrapper.storage
	lua_newtable(_L); // ... LuaWrapper LuaWrapper.storage {}
	lua_setfield(_L, -2, LuaWrapper<T>::classname); // ... LuaWrapper LuaWrapper.storage
	lua_pop(_L, 1); // ... LuaWrapper
	lua_getfield(_L, -1, LUAW_HOLDS_KEY); // ... LuaWrapper LuaWrapper.holds
	lua_newtable(_L); // ... LuaWrapper LuaWrapper.holds {}
	lua_setfield(_L, -2, LuaWrapper<T>::classname); // ... LuaWrapper LuaWrapper.holds
	lua_pop(_L, 1); // ... LuaWrapper
	lua_getfield(_L, -1, LUAW_CACHE_KEY); // ... LuaWrapper LuaWrapper.cache
	lua_newtable(_L); // ... LuaWrapper LuaWrapper.cache {}
	luaW_wrapperfield<T>(_L, LUAW_CACHE_METATABLE_KEY); // ... LuaWrapper LuaWrapper.cache {} cmt
	lua_setmetatable(_L, -2); // ... LuaWrapper LuaWrapper.cache {}
	lua_setfield(_L, -2, LuaWrapper<T>::classname); // ... LuaWrapper LuaWrapper.cache
	lua_pop(_L, 2); // ...
	// Open table
	lua_newtable(_L); // ... T
	luaW_registerfuncs(_L, _allocator ? defaulttable : NULL, _table); // ... T
	// Open metatable, set up extends table
	luaL_newmetatable(_L, _classname); // ... T mt
	lua_newtable(_L); // ... T mt {}
	lua_setfield(_L, -2, LUAW_EXTENDS_KEY); // ... T mt
	luaW_registerfuncs(_L, defaultmetatable, _metatable); // ... T mt
	lua_setfield(_L, -2, "metatable"); // ... T
}
template <typename T> void luaW_setfuncs(lua_State* _L,
                                         const char* _classname,
                                         const luaL_Reg* _table,
                                         const luaL_Reg* _metatable,
                                         std::shared_ptr<T> (*_allocator)(lua_State*)) {
	luaW_setfuncs(_L, _classname, _table, _metatable, _allocator, luaW_defaultidentifier<T>);
}
template <typename T> void luaW_setfuncs(lua_State* _L,
                                         const char* _classname,
                                         const luaL_Reg* _table,
                                         const luaL_Reg* _metatable) {
	luaW_setfuncs(_L, _classname, _table, _metatable, luaW_defaultallocator<T>, luaW_defaultidentifier<T>);
}

template <typename T> void luaW_register(lua_State* _L,
                                         const char* _classname,
                                         const luaL_Reg* _table,
                                         const luaL_Reg* _metatable,
                                         std::shared_ptr<T> (*_allocator)(lua_State*),
                                         void (*_identifier)(lua_State*, std::shared_ptr<T>)) {
	luaW_setfuncs(_L, _classname, _table, _metatable, _allocator, _identifier); // ... T
	lua_pushvalue(_L, -1); // ... T T
	lua_setglobal(_L, _classname); // ... T
}

template <typename T> void luaW_register(lua_State* _L,
                                         const char* _classname,
                                         const luaL_Reg* _table,
                                         const luaL_Reg* _metatable,
                                         std::shared_ptr<T> (*_allocator)(lua_State*)) {
	luaW_setfuncs(_L, _classname, _table, _metatable, _allocator, luaW_defaultidentifier<T>);
	lua_pushvalue(_L, -1); // ... T T
	lua_setglobal(_L, _classname); // ... T
}

template <typename T> void luaW_register(lua_State* _L,
                                         const char* _classname,
                                         const luaL_Reg* _table,
                                         const luaL_Reg* _metatable) {
	luaW_setfuncs(_L, _classname, _table, _metatable, luaW_defaultallocator<T>, luaW_defaultidentifier<T>); // ... T
	lua_pushvalue(_L, -1); // ... T T
	lua_setglobal(_L, _classname); // ... T
}

/**
 * luaW_extend is used to declare that class T inherits from class U. All
 * functions in the base class will be available to the derived class (except
 * when they share a function name, in which case the derived class's function
 * wins). This also allows luaW_to<T> to cast your object apropriately, as
 * casts straight through a void pointer do not work.
 */
template <typename T, typename U> void luaW_extend(lua_State* _L) {
	if(!LuaWrapper<T>::classname) {
		luaL_error(_L, "attempting to call extend on a type that has not been registered");
	}
	if(!LuaWrapper<U>::classname) {
		luaL_error(_L, "attempting to extend %s by a type that has not been registered", LuaWrapper<T>::classname);
	}
	LuaWrapper<T>::cast = luaW_cast<T, U>;
	LuaWrapper<T>::identifier = luaW_identify<T, U>;
	LuaWrapper<T>::postconstructorrecurse = luaW_postconstructorinternal<U>;
	luaL_getmetatable(_L, LuaWrapper<T>::classname); // mt
	luaL_getmetatable(_L, LuaWrapper<U>::classname); // mt emt
	// Point T's metatable __index at U's metatable for inheritance
	lua_newtable(_L); // mt emt {}
	lua_pushvalue(_L, -2); // mt emt {} emt
	lua_setfield(_L, -2, "__index"); // mt emt {}
	lua_setmetatable(_L, -3); // mt emt
	// Set up per-type tables to point at parent type
	lua_getfield(_L, LUA_REGISTRYINDEX, LUAW_WRAPPER_KEY); // ... LuaWrapper
	lua_getfield(_L, -1, LUAW_STORAGE_KEY); // ... LuaWrapper LuaWrapper.storage
	lua_getfield(_L, -1, LuaWrapper<U>::classname); // ... LuaWrapper LuaWrapper.storage U
	lua_setfield(_L, -2, LuaWrapper<T>::classname); // ... LuaWrapper LuaWrapper.storage
	lua_pop(_L, 1); // ... LuaWrapper
	lua_getfield(_L, -1, LUAW_HOLDS_KEY); // ... LuaWrapper LuaWrapper.holds
	lua_getfield(_L, -1, LuaWrapper<U>::classname); // ... LuaWrapper LuaWrapper.holds U
	lua_setfield(_L, -2, LuaWrapper<T>::classname); // ... LuaWrapper LuaWrapper.holds
	lua_pop(_L, 1); // ... LuaWrapper
	lua_getfield(_L, -1, LUAW_CACHE_KEY); // ... LuaWrapper LuaWrapper.cache
	lua_getfield(_L, -1, LuaWrapper<U>::classname); // ... LuaWrapper LuaWrapper.cache U
	lua_setfield(_L, -2, LuaWrapper<T>::classname); // ... LuaWrapper LuaWrapper.cache
	lua_pop(_L, 2); // ...
	// Make a list of all types that inherit from U, for type checking
	lua_getfield(_L, -2, LUAW_EXTENDS_KEY); // mt emt mt.extends
	lua_pushvalue(_L, -2); // mt emt mt.extends emt
	lua_setfield(_L, -2, LuaWrapper<U>::classname); // mt emt mt.extends
	lua_getfield(_L, -2, LUAW_EXTENDS_KEY); // mt emt mt.extends emt.extends
	for (lua_pushnil(_L); lua_next(_L, -2); lua_pop(_L, 1)) {
		// mt emt mt.extends emt.extends k v
		lua_pushvalue(_L, -2); // mt emt mt.extends emt.extends k v k
		lua_pushvalue(_L, -2); // mt emt mt.extends emt.extends k v k
		lua_rawset(_L, -6); // mt emt mt.extends emt.extends k v
	}
	lua_pop(_L, 4); // mt emt
}

#endif

#include <luaWrapper/luaWrapperUtil.hpp>
//#include <luaWrapper/luaWrapperStd.hpp>
