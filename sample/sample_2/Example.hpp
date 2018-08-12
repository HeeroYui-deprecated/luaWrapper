#pragma once

#include <etk/String.hpp>

#include "Vector2D.hpp"

class Example {
	public:
		Example() { }
		
		bool m_boolean = false;
		int m_integer = -76543;
		unsigned int m_uinteger = 765432;
		const char* m_cstring = "default C string";
		etk::String m_cppstring = "default C++ string";
		double m_number = 0.12345;
		float m_floatnumber = 0.23456f;
		Example* m_ptr = null;
		Vector2D m_vec = Vector2D(4,6);
		
		bool GetBoolean() const;
		int GetInteger() const;
		unsigned int GetUInteger() const;
		const char* GetCString() const;
		const etk::String& GetCPPString() const;
		double GetNumber() const;
		float GetFloatNumber() const;
		Example* GetPtr() const;
		const Vector2D& GetVec() const;
		
		void SetBoolean(bool _val);
		void SetInteger(int _val);
		void SetUInteger(unsigned int _val);
		void SetCString(const char* _val);
		void SetCPPString(const etk::String& _val);
		void SetNumber(double _val);
		void SetFloatNumber(float _val);
		void SetPtr(Example* _val);
		void SetVec(const Vector2D& _Member);
		
		int DoSomething(bool _b);
		void DoSomething2(bool _b, int _p) {};
		
		int DoSomethingElse(int _i, int _j);
		int DoSomethingElse(float _f);
};

