#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug
import os

def get_type():
	return "LIBRARY"

def get_name():
	return "luwWrapper"

def get_desc():
	return "luwWrapper : simple Lua automatic wrapper"

def get_licence():
	return "MPL-2"

def get_compagny_type():
	return "org"

def get_compagny_name():
	return "Edouard DUPIN"

def get_maintainer():
	return ["Mr DUPIN Edouard <yui.heero@gmail.com>"]

def get_version():
	return [0,1,"dev"]


def configure(target, my_module):
	my_module.add_extra_flags()
	my_module.add_depend('lua')
	my_module.add_src_file([
		'luaWrapper/luaWrapperStd.cpp'
		])
	my_module.add_header_file([
		'luaWrapper/luaWrapper.hpp'
		'luaWrapper/luaWrapperUtils.hpp'
		])
	return my_module


