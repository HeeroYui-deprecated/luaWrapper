#!/usr/bin/python
import lutinModule as module
import lutinTools as tools

def get_desc():
	return "luaWrapper example 2 : simple lua wrapper"

def create(target):
	# module name is 'edn' and type binary.
	myModule = module.Module(__file__, 'luaWrapperExample2', 'BINARY')
	# add extra compilation flags :
	myModule.add_extra_compile_flags()
	# add the file to compile:
	myModule.add_src_file([
		'Example.cpp',
		'LuaExample.cpp',
		'main.cpp'])
	
	# name of the dependency
	myModule.add_module_depend('luaWrapper')
	
	
	# add the currrent module at the 
	return myModule


