#!/usr/bin/python
import lutinModule as module
import lutinTools as tools

def get_desc():
	return "luwWrapper : simple Lua automatic wrapper"


def create(target):
	# module name is 'edn' and type binary.
	myModule = module.Module(__file__, 'luaWrapper', 'LIBRARY')

	# name of the dependency
	myModule.add_module_depend('lua')
	
	# add extra compilation flags :
	myModule.add_extra_compile_flags()
	# add the file to compile:
	myModule.add_src_file([
		'luaWrapper/luaWrapperStd.cpp'
		])
	
	myModule.add_export_path(tools.get_current_path(__file__))
	
	# add the currrent module at the 
	return myModule


