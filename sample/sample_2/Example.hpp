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
		
		bool getBoolean() const;
		void setBoolean(bool _val);
		
		int getInteger() const;
		void setInteger(int _val);
		
		unsigned int getUInteger() const;
		void setUInteger(unsigned int _val);
		
		const char* getCString() const;
		void setCString(const char* _val);
		
		const etk::String& getCPPString() const;
		void setCPPString(const etk::String& _val);
		
		double getNumber() const;
		void setNumber(double _val);
		
		float getFloatNumber() const;
		void setFloatNumber(float _val);
		
		Example* getPtr() const;
		void setPtr(Example* _val);
		
		const Vector2D& getVec() const;
		void setVec(const Vector2D& _Member);
		
		int DoSomething(bool _b);
		void DoSomething2(bool _b, int _p) {};
		
		int DoSomethingElse(int _i, int _j);
		int DoSomethingElse(float _f);
};

