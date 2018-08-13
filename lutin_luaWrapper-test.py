#!/usr/bin/python
import lutin.debug as debug
import lutin.tools as tools


def get_type():
	return "BINARY"

def get_sub_type():
	return "TEST"

def get_name():
	return "lua-wrapper-test"

def get_desc():
	return "lua wrapper test application"

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
	    'test/test.cpp',
	    'test/testCCallLuaFunction.cpp',
	    ])
	my_module.add_depend([
	    'luaWrapper',
	    'etest',
	    'test-debug'
	    ])
	return True

