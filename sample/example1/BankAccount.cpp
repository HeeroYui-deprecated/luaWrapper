#include "BankAccount.hpp"

float BankAccount::s_totalMoneyInBank = 0;

BankAccount::BankAccount(const char* _owner, float _balance) :
  m_owner(_owner) ,
  m_balance(_balance) {
    s_totalMoneyInBank += _balance;
}

const char* BankAccount::getOwnerName() const {
    return m_owner;
}

void BankAccount::deposit(float _amount) {
    s_totalMoneyInBank += _amount;
    m_balance += _amount;
}

void BankAccount::withdraw(float _amount) {
    s_totalMoneyInBank -= _amount;
    m_balance -= _amount;
}

float BankAccount::checkBalance() const {
    return m_balance;
}

float BankAccount::checkTotalMoneyInBank() {
    return s_totalMoneyInBank;
}

