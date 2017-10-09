#ifndef EXAMPLE_HPP_
#define EXAMPLE_HPP_

#include <etk/String.hpp>
#include <iostream>

class BankAccount {
	public:
		BankAccount(const char* _owner, float _balance);
		const char* getOwnerName() const;
		void deposit(float _amount);
		void withdraw(float _amount);
		float checkBalance() const;
		static float checkTotalMoneyInBank();
	private:
		const char* m_owner;
		float m_balance;
		static float s_totalMoneyInBank;
};

#endif // EXAMPLE_HPP_
