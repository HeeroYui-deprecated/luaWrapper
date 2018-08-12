#!/usr/bin/python
import lutin.debug as debug
import lutin.tools as tools

def get_type():
	return "BINARY"

def get_sub_type():
	return "SAMPLE"

def get_desc():
	return "lua sample 2"

def get_licence():
	return "MPL-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return ["Mr DUPIN Edouard <yui.heero@gmail.com>"]

def configure(target, my_module):
	my_module.add_src_file([
	    'sample/sample_2/Example.cpp',
	    'sample/sample_2/LuaExample.cpp',
	    'sample/sample_2/main.cpp'
	    ])
	my_module.add_depend([
	    'luaWrapper',
	    'test-debug'
	    ])
	my_module.copy_path('sample/sample_2/example2.lua')
	return True

