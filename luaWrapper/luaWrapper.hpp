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
 *  luaWrapper::is<LUAW_TYPE>
 *  luaWrapper::to<LUAW_TYPE>
 *  luaWrapper::check<LUAW_TYPE>
 *  luaWrapper::push<LUAW_TYPE>
 *  luaWrapper::registerElement<LUAW_TYPE>
 *  luaWrapper::setfuncs<LUAW_TYPE>
 *  luaWrapper::extend<LUAW_TYPE, LUAW_TYPE2>
 *  luaWrapper::hold<LUAW_TYPE>
 *  luaWrapper::release<LUAW_TYPE>
 *
 * These functions allow you to manipulate arbitrary classes just like you
 * would the primitive types (e.g. numbers or strings). If you are familiar
 * with the normal Lua API the behavior of these functions should be very
 * intuative.
 *
 * For more information see the README and the comments below
 */
#pragma once

// If you are linking against Lua compiled in C++, define LUAW_NO_EXTERN_C
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include <ememory/memory.hpp>
#include <etk/typeInfo.hpp>
#include <etk/Allocator.hpp>

#include <luaWrapper/debug.hpp>

#define LUAW_POSTCTOR_KEY "__postctor"
#define LUAW_EXTENDS_KEY "__extends"
#define LUAW_STORAGE_KEY "storage"
#define LUAW_CACHE_KEY "cache"
#define LUAW_CACHE_METATABLE_KEY "cachemetatable"
#define LUAW_HOLDS_KEY "holds"
#define LUAW_WRAPPER_KEY "LuaWrapper"

namespace luaWrapper {
	/**
	 * @brief main interface of Lua engine.
	 */
	class Lua {
		private:
			lua_State* m_luaState = null;
		public:
			Lua() {
				m_luaState = luaL_newstate();
				if (m_luaState != null) {
					luaL_openlibs(m_luaState);
				}
			}
			~Lua() {
				if (m_luaState != null) {
					lua_close(m_luaState);
					m_luaState = null;
				}
			}
			void executeFile(const etk::String& _fileName) {
				if (luaL_dofile(m_luaState, _fileName.c_str())) {
					LUAW_PRINT(lua_tostring(m_luaState, -1));
				}
			}
			lua_State* getState() {
				return m_luaState;
			}
	};
	
	/**
	 * A simple utility function to adjust a given index
	 * Useful for when a parameter index needs to be adjusted
	 * after pushing or popping things off the stack
	 */
	inline int correctindex(lua_State* _luaState, int _index, int _correction) {
		return _index < 0 ? _index - _correction : _index;
	}
	
	/**
	 * These are the default allocator and deallocator. If you would prefer an
	 * alternative option, you may select a different function when registering
	 * your class.
	 */
	template <typename LUAW_TYPE>
	ememory::SharedPtr<LUAW_TYPE> defaultallocator(lua_State* _luaState) {
		return ememory::makeShared<LUAW_TYPE>();
	}
	
	/**
	 * The identifier function is responsible for pushing a value unique to each
	 * object on to the stack. Most of the time, this can simply be the address
	 * of the pointer, but sometimes that is not adaquate. For example, if you
	 * are using shared_ptr you would need to push the address of the object the
	 * shared_ptr represents, rather than the address of the shared_ptr itself.
	 */
	template <typename LUAW_TYPE>
	void defaultidentifier(lua_State* _luaState, ememory::SharedPtr<LUAW_TYPE> _obj) {
		lua_pushlightuserdata(_luaState, _obj.get());
	}
	
	/**
	 * This class is what is used by LuaWrapper to contain the userdata. data
	 * stores a pointer to the object itself, and cast is used to cast toward the
	 * base class if there is one and it is necessary. Rather than use RTTI and
	 * typid to compare types, I use the clever trick of using the cast to compare
	 * types. Because there is at most one cast per type, I can use it to identify
	 * when and object is the type I want. This is only used internally.
	 */
	struct Userdata {
		Userdata(ememory::SharedPtr<void> _vptr = null, size_t _typeId = 0) :
		  m_data(etk::move(_vptr)),
		  m_typeId(_typeId) {
			// nothing to do ...
		}
		Userdata(Userdata&& _obj) {
			etk::swap(m_data, _obj.m_data);
			etk::swap(m_typeId, _obj.m_typeId);
		}
		Userdata(const Userdata& _obj) {
			m_data = _obj.m_data;
			m_typeId = _obj.m_typeId;
		}
		Userdata& operator= (Userdata&& _obj) {
			etk::swap(m_data, _obj.m_data);
			etk::swap(m_typeId, _obj.m_typeId);
			return *this;
		}
		Userdata& operator= (const Userdata& _obj) {
			m_data = _obj.m_data;
			m_typeId = _obj.m_typeId;
			return *this;
		}
		ememory::SharedPtr<void> m_data;
		size_t m_typeId;
	};
	
	/**
	 * This class cannot actually to be instantiated. It is used only hold the
	 * table name and other information.
	 */
	template <typename LUAW_TYPE>
	class LuaWrapper {
		public:
			static const char* classname;
			static void (*identifier)(lua_State*, ememory::SharedPtr<LUAW_TYPE>);
			static ememory::SharedPtr<LUAW_TYPE> (*allocator)(lua_State*);
			static void (*postconstructorrecurse)(lua_State* _luaState, int numargs);
		private:
			LuaWrapper();
	};
	template <typename LUAW_TYPE> const char* LuaWrapper<LUAW_TYPE>::classname;
	template <typename LUAW_TYPE> void (*LuaWrapper<LUAW_TYPE>::identifier)(lua_State*, ememory::SharedPtr<LUAW_TYPE>);
	template <typename LUAW_TYPE> ememory::SharedPtr<LUAW_TYPE> (*LuaWrapper<LUAW_TYPE>::allocator)(lua_State*);
	template <typename LUAW_TYPE> void (*LuaWrapper<LUAW_TYPE>::postconstructorrecurse)(lua_State* _luaState, int _numargs);
	
	template <typename LUAW_TYPE, typename LUAW_TYPE2>
	void identify(lua_State* _luaState, LUAW_TYPE* _obj) {
		LuaWrapper<LUAW_TYPE2>::identifier(_luaState, static_cast<LUAW_TYPE2*>(_obj));
	}
	
	template <typename LUAW_TYPE>
	inline void wrapperfield(lua_State* _luaState, const char* _field) {
		lua_getfield(_luaState, LUA_REGISTRYINDEX, LUAW_WRAPPER_KEY); // ... LuaWrapper
		lua_getfield(_luaState, -1, _field); // ... LuaWrapper LuaWrapper.field
		lua_getfield(_luaState, -1, LuaWrapper<LUAW_TYPE>::classname); // ... LuaWrapper LuaWrapper.field LuaWrapper.field.class
		lua_replace(_luaState, -3); // ... LuaWrapper.field.class LuaWrapper.field
		lua_pop(_luaState, 1); // ... LuaWrapper.field.class
	}
	
	/**
	 * Analogous to lua_is(boolean|string|*)
	 *
	 * Returns 1 if the value at the given acceptable index is of type T (or if
	 * strict is false, convertable to type T) and 0 otherwise.
	 */
	template <typename LUAW_TYPE>
	bool is(lua_State *_luaState, int _index, bool _strict = false) {
		bool equal = false;// lua_isnil(_luaState, index);
		if (    equal == false
		     && lua_isuserdata(_luaState, _index)
		     && lua_getmetatable(_luaState, _index)) {
			// ... ud ... udmt
			luaL_getmetatable(_luaState, LuaWrapper<LUAW_TYPE>::classname); // ... ud ... udmt Tmt
			equal = lua_rawequal(_luaState, -1, -2) != 0;
			if (!equal && !_strict) {
				lua_getfield(_luaState, -2, LUAW_EXTENDS_KEY); // ... ud ... udmt Tmt udmt.extends
				for (lua_pushnil(_luaState); lua_next(_luaState, -2); lua_pop(_luaState, 1)) {
					// ... ud ... udmt Tmt udmt.extends k v
					equal = lua_rawequal(_luaState, -1, -4) != 0;
					if (equal) {
						lua_pop(_luaState, 2); // ... ud ... udmt Tmt udmt.extends
						break;
					}
				}
				lua_pop(_luaState, 1); // ... ud ... udmt Tmt
			}
			lua_pop(_luaState, 2); // ... ud ...
		}
		return equal;
	}
	
	/**
	 * Analogous to lua_to(boolean|string|*)
	 *
	 * Converts the given acceptable index to a T*. That value must be of (or
	 * convertable to) type T; otherwise, returns NULL.
	 */
	template <typename LUAW_TYPE>
	ememory::SharedPtr<LUAW_TYPE> to(lua_State* _luaState, int _index, bool _strict = false) {
		if (is<LUAW_TYPE>(_luaState, _index, _strict)) {
			Userdata* pud = static_cast<Userdata*>(lua_touserdata(_luaState, _index));
			return ememory::staticPointerCast<LUAW_TYPE>(pud->m_data);
		}
		return null;
	}
	
	/**
	 * Analogous to luaL_check(boolean|string|*)
	 *
	 * Converts the given acceptable index to a LUAW_TYPE*. That value must be of (or
	 * convertable to) type T; otherwise, an error is raised.
	 */
	template <typename LUAW_TYPE>
	ememory::SharedPtr<LUAW_TYPE> check(lua_State* _luaState,
	                                         int _index,
	                                         bool _strict = false) {
		ememory::SharedPtr<LUAW_TYPE> obj;
		if (is<LUAW_TYPE>(_luaState, _index, _strict)) {
			Userdata* pud = static_cast<Userdata*>(lua_touserdata(_luaState, _index));
			obj = ememory::staticPointerCast<LUAW_TYPE>(pud->m_data);
		} else {
			const char *msg = lua_pushfstring(_luaState, "%s expected, got %s", LuaWrapper<LUAW_TYPE>::classname, luaL_typename(_luaState, _index));
			luaL_argerror(_luaState, _index, msg);
		}
		return obj;
	}
	
	template <typename LUAW_TYPE>
	ememory::SharedPtr<LUAW_TYPE> opt(lua_State* _luaState,
	                                       int _index,
	                                       ememory::SharedPtr<LUAW_TYPE> _fallback = null,
	                                       bool _strict = false) {
		if (lua_isnil(_luaState, _index) == true) {
			return _fallback;
		} else {
			return check<LUAW_TYPE>(_luaState, _index, _strict);
		}
	}
	
	/**
	 * Analogous to lua_push(boolean|string|*)
	 *
	 * Pushes a userdata of type T onto the stack. If this object already exists in
	 * the Lua environment, it will assign the existing storage table to it.
	 * Otherwise, a new storage table will be created for it.
	 */
	template <typename LUAW_TYPE>
	void push(lua_State* _luaState,
	               ememory::SharedPtr<LUAW_TYPE> _obj) {
		if (_obj != null) {
			LuaWrapper<LUAW_TYPE>::identifier(_luaState, _obj); // ... id
			wrapperfield<LUAW_TYPE>(_luaState, LUAW_CACHE_KEY); // ... id cache
			lua_pushvalue(_luaState, -2); // ... id cache id
			lua_gettable(_luaState, -2); // ... id cache obj
			if (lua_isnil(_luaState, -1)) {
				// Create the new userdata and place it in the cache
				lua_pop(_luaState, 1); // ... id cache
				lua_insert(_luaState, -2); // ... cache id
				// placement new creation (need to initilaize the sructure:
				Userdata* ud = new ((char*)lua_newuserdata(_luaState, sizeof(Userdata))) Userdata(_obj, ETK_GET_TYPE_ID(LUAW_TYPE)); // ... cache id obj
				lua_pushvalue(_luaState, -1); // ... cache id obj obj
				lua_insert(_luaState, -4); // ... obj cache id obj
				lua_settable(_luaState, -3); // ... obj cache
				luaL_getmetatable(_luaState, LuaWrapper<LUAW_TYPE>::classname); // ... obj cache mt
				lua_setmetatable(_luaState, -3); // ... obj cache
				lua_pop(_luaState, 1); // ... obj
			} else {
				lua_replace(_luaState, -3); // ... obj cache
				lua_pop(_luaState, 1); // ... obj
			}
		} else {
			lua_pushnil(_luaState);
		}
	}
	
	/**
	 * Instructs LuaWrapper that it owns the userdata, and can manage its memory.
	 * When all references to the object are removed, Lua is free to garbage
	 * collect it and delete the object.
	 *
	 * Returns true if hold took hold of the object, and false if it was
	 * already held
	 */
	template <typename LUAW_TYPE>
	bool hold(lua_State* _luaState,
	               ememory::SharedPtr<LUAW_TYPE> _obj) {
		wrapperfield<LUAW_TYPE>(_luaState, LUAW_HOLDS_KEY); // ... holds
		LuaWrapper<LUAW_TYPE>::identifier(_luaState, _obj); // ... holds id
		lua_pushvalue(_luaState, -1); // ... holds id id
		lua_gettable(_luaState, -3); // ... holds id hold
		// If it's not held, hold it
		if (!lua_toboolean(_luaState, -1)) {
			// Apply hold boolean
			lua_pop(_luaState, 1); // ... holds id
			lua_pushboolean(_luaState, true); // ... holds id true
			lua_settable(_luaState, -3); // ... holds
			lua_pop(_luaState, 1); // ...
			return true;
		}
		lua_pop(_luaState, 3); // ...
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
	template <typename LUAW_TYPE>
	void release(lua_State* _luaState,
	                  int _index) {
		wrapperfield<LUAW_TYPE>(_luaState, LUAW_HOLDS_KEY); // ... id ... holds
		lua_pushvalue(_luaState, correctindex(_luaState, _index, 1)); // ... id ... holds id
		lua_pushnil(_luaState); // ... id ... holds id nil
		lua_settable(_luaState, -3); // ... id ... holds
		lua_pop(_luaState, 1); // ... id ...
	}
	
	template <typename LUAW_TYPE>
	void release(lua_State* _luaState,
	                  LUAW_TYPE* _obj) {
		LuaWrapper<LUAW_TYPE>::identifier(_luaState, _obj); // ... id
		release<LUAW_TYPE>(_luaState, -1); // ... id
		lua_pop(_luaState, 1); // ...
	}
	
	template <typename LUAW_TYPE>
	void postconstructorinternal(lua_State* _luaState,
	                                  int _numargs) {
		// ... ud args...
		if (LuaWrapper<LUAW_TYPE>::postconstructorrecurse) {
			LuaWrapper<LUAW_TYPE>::postconstructorrecurse(_luaState, _numargs);
		}
		luaL_getmetatable(_luaState, LuaWrapper<LUAW_TYPE>::classname); // ... ud args... mt
		lua_getfield(_luaState, -1, LUAW_POSTCTOR_KEY); // ... ud args... mt postctor
		if (lua_type(_luaState, -1) == LUA_TFUNCTION) {
			for (int i = 0; i < _numargs + 1; i++) {
				lua_pushvalue(_luaState, -3 - _numargs); // ... ud args... mt postctor ud args...
			}
			lua_call(_luaState, _numargs + 1, 0); // ... ud args... mt
			lua_pop(_luaState, 1); // ... ud args...
		} else {
			lua_pop(_luaState, 2); // ... ud args...
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
	template <typename LUAW_TYPE>
	void postconstructor(lua_State* _luaState,
	                          int _numargs) {
		// ... ud args...
		postconstructorinternal<LUAW_TYPE>(_luaState, _numargs); // ... ud args...
		lua_pop(_luaState, _numargs); // ... ud
	}
	
	/**
	 * This function is generally called from Lua, not C++
	 *
	 * Creates an object of type T using the constructor and subsequently calls the
	 * post-constructor on it.
	 */
	template <typename LUAW_TYPE>
	inline int create(lua_State* _luaState, int _numargs) {
		// ... args...
		ememory::SharedPtr<LUAW_TYPE> obj = LuaWrapper<LUAW_TYPE>::allocator(_luaState);
		push<LUAW_TYPE>(_luaState, obj); // ... args... ud
		hold<LUAW_TYPE>(_luaState, obj);
		lua_insert(_luaState, -1 - _numargs); // ... ud args...
		postconstructor<LUAW_TYPE>(_luaState, _numargs); // ... ud
		return 1;
	}
	
	template <typename LUAW_TYPE>
	int create(lua_State* _luaState) {
		return create<LUAW_TYPE>(_luaState, lua_gettop(_luaState));
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
	template <typename LUAW_TYPE>
	int index(lua_State* _luaState) {
		// obj key
		ememory::SharedPtr<LUAW_TYPE> obj = to<LUAW_TYPE>(_luaState, 1);
		wrapperfield<LUAW_TYPE>(_luaState, LUAW_STORAGE_KEY); // obj key storage
		LuaWrapper<LUAW_TYPE>::identifier(_luaState, obj); // obj key storage id
		lua_gettable(_luaState, -2); // obj key storage store
		// Check if storage table exists
		if (!lua_isnil(_luaState, -1)) {
			lua_pushvalue(_luaState, -3); // obj key storage store key
			lua_gettable(_luaState, -2); // obj key storage store store[k]
		}
		// If either there is no storage table or the key wasn't found
		// then fall back to the metatable
		if (lua_isnil(_luaState, -1)) {
			lua_settop(_luaState, 2); // obj key
			lua_getmetatable(_luaState, -2); // obj key mt
			lua_pushvalue(_luaState, -2); // obj key mt k
			lua_gettable(_luaState, -2); // obj key mt mt[k]
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
	template <typename LUAW_TYPE>
	int newindex(lua_State* _luaState) {
		// obj key value
		ememory::SharedPtr<LUAW_TYPE> obj = check<LUAW_TYPE>(_luaState, 1);
		wrapperfield<LUAW_TYPE>(_luaState, LUAW_STORAGE_KEY); // obj key value storage
		LuaWrapper<LUAW_TYPE>::identifier(_luaState, obj); // obj key value storage id
		lua_pushvalue(_luaState, -1); // obj key value storage id id
		lua_gettable(_luaState, -3); // obj key value storage id store
		// Add the storage table if there isn't one already
		if (lua_isnil(_luaState, -1)) {
			lua_pop(_luaState, 1); // obj key value storage id
			lua_newtable(_luaState); // obj key value storage id store
			lua_pushvalue(_luaState, -1); // obj key value storage id store store
			lua_insert(_luaState, -3); // obj key value storage store id store
			lua_settable(_luaState, -4); // obj key value storage store
		}
		lua_pushvalue(_luaState, 2); // obj key value ... store key
		lua_pushvalue(_luaState, 3); // obj key value ... store key value
		lua_settable(_luaState, -3); // obj key value ... store
		return 0;
	}
	
	/**
	 * This function is called from Lua, not C++
	 *
	 * The __gc metamethod handles cleaning up userdata. The userdata's reference
	 * count is decremented and if this is the final reference to the userdata its
	 * environment table is nil'd and pointer deleted with the destructor callback.
	 */
	template <typename LUAW_TYPE>
	int gc(lua_State* _luaState) {
		// obj
		/*
		ememory::SharedPtr<LUAW_TYPE> obj = to<LUAW_TYPE>(_luaState, 1);
		LuaWrapper<LUAW_TYPE>::identifier(_luaState, obj); // obj key value storage id
		wrapperfield<LUAW_TYPE>(_luaState, LUAW_HOLDS_KEY); // obj id counts count holds
		lua_pushvalue(_luaState, 2); // obj id counts count holds id
		lua_gettable(_luaState, -2); // obj id counts count holds hold
		if (lua_toboolean(_luaState, -1)) {
			obj.reset();
		}
		wrapperfield<LUAW_TYPE>(_luaState, LUAW_STORAGE_KEY); // obj id counts count holds hold storage
		lua_pushvalue(_luaState, 2); // obj id counts count holds hold storage id
		lua_pushnil(_luaState); // obj id counts count holds hold storage id nil
		lua_settable(_luaState, -3); // obj id counts count holds hold storage
		release<LUAW_TYPE>(_luaState, 2);
		*/
		Userdata *object = static_cast<Userdata*>( lua_touserdata( _luaState, 1 ) );
		object->~Userdata();
		lua_pop( _luaState, 1 );
		return 0;
	}
	/*
	    static int gc( lua_State* const L )
	    {
	        
	        return 0;
	    }
	*/
	/**
	 * Thakes two tables and registers them with Lua to the table on the top of the
	 * stack. 
	 *
	 * This function is only called from LuaWrapper internally. 
	 */
	inline void registerfuncs(lua_State* _luaState, const luaL_Reg _defaulttable[], const luaL_Reg _table[]) {
		// ... T
	#if LUA_VERSION_NUM == 502
		if (_defaulttable) {
			luaL_setfuncs(_luaState, _defaulttable, 0); // ... T
		}
		if (_table) {
			luaL_setfuncs(_luaState, _table, 0); // ... T
		}
	#else
		if (_defaulttable) {
			luaL_register(_luaState, NULL, _defaulttable); // ... T
		}
		if (_table) {
			luaL_register(_luaState, NULL, _table); // ... T
		}
	#endif
	}
	
	/**
	 * Initializes the LuaWrapper tables used to track internal state. 
	 *
	 * This function is only called from LuaWrapper internally. 
	 */
	inline void initialize(lua_State* _luaState) {
		// Ensure that the LuaWrapper table is set up
		lua_getfield(_luaState, LUA_REGISTRYINDEX, LUAW_WRAPPER_KEY); // ... LuaWrapper
		if (lua_isnil(_luaState, -1)) {
			lua_newtable(_luaState); // ... nil {}
			lua_pushvalue(_luaState, -1); // ... nil {} {}
			lua_setfield(_luaState, LUA_REGISTRYINDEX, LUAW_WRAPPER_KEY); // ... nil LuaWrapper
			// Create a storage table
			lua_newtable(_luaState); // ... LuaWrapper nil {}
			lua_setfield(_luaState, -2, LUAW_STORAGE_KEY); // ... nil LuaWrapper
			// Create a holds table
			lua_newtable(_luaState); // ... LuaWrapper {}
			lua_setfield(_luaState, -2, LUAW_HOLDS_KEY); // ... nil LuaWrapper
			// Create a cache table, with weak values so that the userdata will not
			// be ref counted
			lua_newtable(_luaState); // ... nil LuaWrapper {}
			lua_setfield(_luaState, -2, LUAW_CACHE_KEY); // ... nil LuaWrapper
			lua_newtable(_luaState); // ... nil LuaWrapper {}
			lua_pushstring(_luaState, "v"); // ... nil LuaWrapper {} "v"
			lua_setfield(_luaState, -2, "__mode"); // ... nil LuaWrapper {}
			lua_setfield(_luaState, -2, LUAW_CACHE_METATABLE_KEY); // ... nil LuaWrapper
			lua_pop(_luaState, 1); // ... nil
		}
		lua_pop(_luaState, 1); // ...
	}
	
	/**
	 * Run register or setfuncs to create a table and metatable for your
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
	 * register will set table as the new value of the global of the given
	 * name. setfuncs is identical to register, but it does not set the
	 * table globally.  As with luaL_register and luaL_setfuncs, both funcstions
	 * leave the new table on the top of the stack.
	 */
	template <typename LUAW_TYPE>
	void setfuncs(lua_State* _luaState,
	                   const char* _classname,
	                   const luaL_Reg* _table,
	                   const luaL_Reg* _metatable,
	                   ememory::SharedPtr<LUAW_TYPE> (*_allocator)(lua_State*),
	                   void (*_identifier)(lua_State*, ememory::SharedPtr<LUAW_TYPE>)) {
		initialize(_luaState);
		LuaWrapper<LUAW_TYPE>::classname = _classname;
		LuaWrapper<LUAW_TYPE>::identifier = _identifier;
		LuaWrapper<LUAW_TYPE>::allocator = _allocator;
		const luaL_Reg defaulttable[] = {
			{ "new", create<LUAW_TYPE> },
			{ NULL, NULL }
		};
		const luaL_Reg defaultmetatable[] =  {
			{ "__index", index<LUAW_TYPE> }, 
			{ "__newindex", newindex<LUAW_TYPE> },
			{ "__gc", gc<LUAW_TYPE> }, 
			{ NULL, NULL }
		};
		// Set up per-type tables
		lua_getfield(_luaState, LUA_REGISTRYINDEX, LUAW_WRAPPER_KEY); // ... LuaWrapper
		lua_getfield(_luaState, -1, LUAW_STORAGE_KEY); // ... LuaWrapper LuaWrapper.storage
		lua_newtable(_luaState); // ... LuaWrapper LuaWrapper.storage {}
		lua_setfield(_luaState, -2, LuaWrapper<LUAW_TYPE>::classname); // ... LuaWrapper LuaWrapper.storage
		lua_pop(_luaState, 1); // ... LuaWrapper
		lua_getfield(_luaState, -1, LUAW_HOLDS_KEY); // ... LuaWrapper LuaWrapper.holds
		lua_newtable(_luaState); // ... LuaWrapper LuaWrapper.holds {}
		lua_setfield(_luaState, -2, LuaWrapper<LUAW_TYPE>::classname); // ... LuaWrapper LuaWrapper.holds
		lua_pop(_luaState, 1); // ... LuaWrapper
		lua_getfield(_luaState, -1, LUAW_CACHE_KEY); // ... LuaWrapper LuaWrapper.cache
		lua_newtable(_luaState); // ... LuaWrapper LuaWrapper.cache {}
		wrapperfield<LUAW_TYPE>(_luaState, LUAW_CACHE_METATABLE_KEY); // ... LuaWrapper LuaWrapper.cache {} cmt
		lua_setmetatable(_luaState, -2); // ... LuaWrapper LuaWrapper.cache {}
		lua_setfield(_luaState, -2, LuaWrapper<LUAW_TYPE>::classname); // ... LuaWrapper LuaWrapper.cache
		lua_pop(_luaState, 2); // ...
		// Open table
		lua_newtable(_luaState); // ... T
		registerfuncs(_luaState, _allocator ? defaulttable : NULL, _table); // ... T
		// Open metatable, set up extends table
		luaL_newmetatable(_luaState, _classname); // ... T mt
		lua_newtable(_luaState); // ... T mt {}
		lua_setfield(_luaState, -2, LUAW_EXTENDS_KEY); // ... T mt
		registerfuncs(_luaState, defaultmetatable, _metatable); // ... T mt
		lua_setfield(_luaState, -2, "metatable"); // ... T
	}
	template <typename LUAW_TYPE>
	void setfuncs(lua_State* _luaState,
	                   const char* _classname,
	                   const luaL_Reg* _table,
	                   const luaL_Reg* _metatable,
	                   ememory::SharedPtr<LUAW_TYPE> (*_allocator)(lua_State*)) {
		setfuncs(_luaState, _classname, _table, _metatable, _allocator, defaultidentifier<LUAW_TYPE>);
	}
	template <typename LUAW_TYPE>
	void setfuncs(lua_State* _luaState,
	                   const char* _classname,
	                   const luaL_Reg* _table,
	                   const luaL_Reg* _metatable) {
		setfuncs(_luaState, _classname, _table, _metatable, defaultallocator<LUAW_TYPE>, defaultidentifier<LUAW_TYPE>);
	}
	
	template <typename LUAW_TYPE>
	void registerElement(lua_State* _luaState,
	                   const char* _classname,
	                   const luaL_Reg* _table,
	                   const luaL_Reg* _metatable,
	                   ememory::SharedPtr<LUAW_TYPE> (*_allocator)(lua_State*),
	                   void (*_identifier)(lua_State*, ememory::SharedPtr<LUAW_TYPE>)) {
		setfuncs(_luaState, _classname, _table, _metatable, _allocator, _identifier); // ... T
		lua_pushvalue(_luaState, -1); // ... LUAW_TYPE LUAW_TYPE
		lua_setglobal(_luaState, _classname); // ... LUAW_TYPE
	}
	
	template <typename LUAW_TYPE>
	void registerElement(lua_State* _luaState,
	                   const char* _classname,
	                   const luaL_Reg* _table,
	                   const luaL_Reg* _metatable,
	                   ememory::SharedPtr<LUAW_TYPE> (*_allocator)(lua_State*)) {
		setfuncs(_luaState, _classname, _table, _metatable, _allocator, defaultidentifier<LUAW_TYPE>);
		lua_pushvalue(_luaState, -1); // ... LUAW_TYPE LUAW_TYPE
		lua_setglobal(_luaState, _classname); // ... LUAW_TYPE
	}
	
	template <typename LUAW_TYPE>
	void registerElement(lua_State* _luaState,
	                   const char* _classname,
	                   const luaL_Reg* _table,
	                   const luaL_Reg* _metatable) {
		setfuncs(_luaState, _classname, _table, _metatable, defaultallocator<LUAW_TYPE>, defaultidentifier<LUAW_TYPE>); // ... T
		lua_pushvalue(_luaState, -1); // ... T T
		lua_setglobal(_luaState, _classname); // ... T
	}
	
	/**
	 * extend is used to declare that class T inherits from class U. All
	 * functions in the base class will be available to the derived class (except
	 * when they share a function name, in which case the derived class's function
	 * wins). This also allows to<LUAW_TYPE> to cast your object apropriately, as
	 * casts straight through a void pointer do not work.
	 */
	template <typename LUAW_TYPE, typename LUAW_TYPE2>
	void extend(lua_State* _luaState) {
		if(!LuaWrapper<LUAW_TYPE>::classname) {
			luaL_error(_luaState, "attempting to call extend on a type that has not been registered");
		}
		if(!LuaWrapper<LUAW_TYPE2>::classname) {
			luaL_error(_luaState, "attempting to extend %s by a type that has not been registered", LuaWrapper<LUAW_TYPE>::classname);
		}
		//LuaWrapper<LUAW_TYPE>::cast = cast<LUAW_TYPE, LUAW_TYPE2>;
		LuaWrapper<LUAW_TYPE>::identifier = identify<LUAW_TYPE, LUAW_TYPE2>;
		LuaWrapper<LUAW_TYPE>::postconstructorrecurse = postconstructorinternal<LUAW_TYPE2>;
		luaL_getmetatable(_luaState, LuaWrapper<LUAW_TYPE>::classname); // mt
		luaL_getmetatable(_luaState, LuaWrapper<LUAW_TYPE2>::classname); // mt emt
		// Point T's metatable __index at U's metatable for inheritance
		lua_newtable(_luaState); // mt emt {}
		lua_pushvalue(_luaState, -2); // mt emt {} emt
		lua_setfield(_luaState, -2, "__index"); // mt emt {}
		lua_setmetatable(_luaState, -3); // mt emt
		// Set up per-type tables to point at parent type
		lua_getfield(_luaState, LUA_REGISTRYINDEX, LUAW_WRAPPER_KEY); // ... LuaWrapper
		lua_getfield(_luaState, -1, LUAW_STORAGE_KEY); // ... LuaWrapper LuaWrapper.storage
		lua_getfield(_luaState, -1, LuaWrapper<LUAW_TYPE2>::classname); // ... LuaWrapper LuaWrapper.storage U
		lua_setfield(_luaState, -2, LuaWrapper<LUAW_TYPE>::classname); // ... LuaWrapper LuaWrapper.storage
		lua_pop(_luaState, 1); // ... LuaWrapper
		lua_getfield(_luaState, -1, LUAW_HOLDS_KEY); // ... LuaWrapper LuaWrapper.holds
		lua_getfield(_luaState, -1, LuaWrapper<LUAW_TYPE2>::classname); // ... LuaWrapper LuaWrapper.holds U
		lua_setfield(_luaState, -2, LuaWrapper<LUAW_TYPE>::classname); // ... LuaWrapper LuaWrapper.holds
		lua_pop(_luaState, 1); // ... LuaWrapper
		lua_getfield(_luaState, -1, LUAW_CACHE_KEY); // ... LuaWrapper LuaWrapper.cache
		lua_getfield(_luaState, -1, LuaWrapper<LUAW_TYPE2>::classname); // ... LuaWrapper LuaWrapper.cache U
		lua_setfield(_luaState, -2, LuaWrapper<LUAW_TYPE>::classname); // ... LuaWrapper LuaWrapper.cache
		lua_pop(_luaState, 2); // ...
		// Make a list of all types that inherit from U, for type checking
		lua_getfield(_luaState, -2, LUAW_EXTENDS_KEY); // mt emt mt.extends
		lua_pushvalue(_luaState, -2); // mt emt mt.extends emt
		lua_setfield(_luaState, -2, LuaWrapper<LUAW_TYPE2>::classname); // mt emt mt.extends
		lua_getfield(_luaState, -2, LUAW_EXTENDS_KEY); // mt emt mt.extends emt.extends
		for (lua_pushnil(_luaState); lua_next(_luaState, -2); lua_pop(_luaState, 1)) {
			// mt emt mt.extends emt.extends k v
			lua_pushvalue(_luaState, -2); // mt emt mt.extends emt.extends k v k
			lua_pushvalue(_luaState, -2); // mt emt mt.extends emt.extends k v k
			lua_rawset(_luaState, -6); // mt emt mt.extends emt.extends k v
		}
		lua_pop(_luaState, 4); // mt emt
	}

}


#include <luaWrapper/luaWrapperUtil.hpp>
