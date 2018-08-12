#include "Example.hpp"
#include <etk/String.hpp>
#include <etk/typeInfo.hpp>
#include <iostream>

ETK_DECLARE_TYPE(Example);

// These are just a bunch of generic getters and setters for
// values in the Example class

bool Example::GetBoolean() const {
	return m_boolean;
}

int Example::GetInteger() const {
	return m_integer;
}

unsigned int Example::GetUInteger() const {
	return m_uinteger;
}

const char* Example::GetCString() const {
	return m_cstring;
}

const etk::String& Example::GetCPPString() const {
	return m_cppstring;
}

const Vector2D& Example::GetVec() const {
	return m_vec;
}

double Example::GetNumber() const {
	return m_number;
}

float Example::GetFloatNumber() const {
	return m_floatnumber;
}

Example* Example::GetPtr() const {
	return m_ptr;
}

void Example::SetBoolean(bool _val) {
	m_boolean = _val;
}

void Example::SetInteger(int _val) {
	m_integer = _val;
}

void Example::SetUInteger(unsigned int _val) {
	m_uinteger = _val;
}

void Example::SetCString(const char* _val) {
	m_cstring = _val;
}

void Example::SetCPPString(const etk::String& _val) {
	m_cppstring = _val;
}

void Example::SetVec(const Vector2D& _val) {
	m_vec = _val;
}

void Example::SetNumber(double _val) {
	m_number = _val;
}

void Example::SetFloatNumber(float _val) {
	m_floatnumber = _val;
}

void Example::SetPtr(Example* _val) {
	m_ptr = _val;
}

int Example::DoSomething(bool _b) {
	std::cout << "b = " << _b << std::endl;
	return 0;
}

int Example::DoSomethingElse(int _i, int _j) {
	std::cout << "i = " << _i << ", j = " << _j << std::endl;
	return 0;
}

int Example::DoSomethingElse(float _f) {
	std::cout << "f = " << _f << std::endl;
	return 0;
}


