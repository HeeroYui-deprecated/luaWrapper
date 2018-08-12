#pragma once

#include <etk/String.hpp>

#include "Vector2D.hpp"

class Example {
	public:
		Example() :
		  m_boolean(),
		  m_integer(),
		  m_uinteger(),
		  m_cstring(""),
		  m_cppstring(""),
		  m_number(),
		  m_floatnumber(),
		  m_ptr(),
		  m_vec() {
			
		}
		
		bool m_boolean;
		int m_integer;
		unsigned int m_uinteger;
		const char* m_cstring;
		etk::String m_cppstring;
		double m_number;
		float m_floatnumber;
		Example* m_ptr;
		Vector2D m_vec;
		
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

