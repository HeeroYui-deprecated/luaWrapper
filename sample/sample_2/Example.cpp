#include "Example.hpp"
#include <etk/String.hpp>
#include <etk/typeInfo.hpp>
#include <iostream>
#include <test-debug/debug.hpp>

ETK_DECLARE_TYPE(Example);

// These are just a bunch of generic getters and setters for
// values in the Example class

bool Example::getBoolean() const {
	return m_boolean;
}

int Example::getInteger() const {
	return m_integer;
}

unsigned int Example::getUInteger() const {
	return m_uinteger;
}

const char* Example::getCString() const {
	return m_cstring;
}

const etk::String& Example::getCPPString() const {
	return m_cppstring;
}

const Vector2D& Example::getVec() const {
	return m_vec;
}

double Example::getNumber() const {
	return m_number;
}

float Example::getFloatNumber() const {
	return m_floatnumber;
}

Example* Example::getPtr() const {
	return m_ptr;
}

void Example::setBoolean(bool _val) {
	m_boolean = _val;
}

void Example::setInteger(int _val) {
	m_integer = _val;
}

void Example::setUInteger(unsigned int _val) {
	m_uinteger = _val;
}

void Example::setCString(const char* _val) {
	m_cstring = _val;
}

void Example::setCPPString(const etk::String& _val) {
	m_cppstring = _val;
}

void Example::setVec(const Vector2D& _val) {
	m_vec = _val;
}

void Example::setNumber(double _val) {
	m_number = _val;
}

void Example::setFloatNumber(float _val) {
	m_floatnumber = _val;
}

void Example::setPtr(Example* _val) {
	m_ptr = _val;
}

int Example::DoSomething(bool _b) {
	TEST_PRINT("b = " << _b);
	return 0;
}

int Example::DoSomethingElse(int _i, int _j) {
	TEST_PRINT("i = " << _i << ", j = " << _j);
	return 0;
}

int Example::DoSomethingElse(float _f) {
	TEST_PRINT("f = " << _f);
	return 0;
}


