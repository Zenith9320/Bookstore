#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include "BlockList.hpp"
#include "BookSystem.hpp"
#include <stack>
#include <iostream>

using std::cout;
using std::string;
using std::stack;

struct Account {
  string UserID;
  string password;
  int privilege;
  string UserName;

  Account & operator = (const Account& other) {
    if (this != &other) {
      UserID = other.UserID;
      password = other.password;
      privilege = other.privilege;
      UserName = other.UserName;
    }
    return *this;
  }
  
  Account() : UserID(""), password(""), privilege(0), UserName("") {};

  Account(string ID, string pswd, string name) 
  : UserID(ID), password(pswd), privilege(1), UserName(name){};

  Account(string ID, string pswd, int priv, string name) 
  : UserID(ID), password(pswd), privilege(priv), UserName(name) {};
};

inline bool operator == (const Account& account1, const Account& account2) {
  return account1.UserID == account2.UserID;
}

inline bool operator > (const Account& account1, const Account& account2) {
  return account1.UserID > account2.UserID;
}

inline bool operator < (const Account& account1, const Account& account2) {
  return account1.UserID < account2.UserID;
}

class AccountSystem {
private:
  Blocklist<Account> AccountList;
  stack<Account> LogStack;
  string select_book;

public:
  AccountSystem() : AccountList("Account_", "index_file.dat", "value_file.dat") {
    select_book = "";
  }
  ~AccountSystem() {
    while (!LogStack.empty()) LogStack.pop();
    AccountList.clearall();
    select_book = "";
    LogStack = stack<Account>();
  };
  int get_current_privilege() {
    if (LogStack.empty()) return -1;
    return LogStack.top().privilege;
  }
  int get_privilege(const string& ID) {
    if (!AccountList.if_find(ID)) return -1;
    return AccountList.FindSingle(ID).privilege;
  }
  void Register(string ID, string pswd, string name) {
    Account New_member(ID, pswd, name);
    auto current_account = LogStack.top();
    if (current_account.privilege != 0 && current_account.privilege != 1) {
      cout << "Invalid\n";
      return;
    }
    if (AccountList.if_find(ID)) { cout << "Invalid\n"; return; }
    AccountList.Insert(ID, New_member);
  }
  void UserAdd(string ID, string pswd, int priv, string name) {
    if (AccountList.if_find(ID)) { cout << "Invalid\n"; return; }
    auto current_account = LogStack.top();
    if (priv > current_account.privilege) { cout << "Invalid\n"; return; } 
    Account New_member(ID, pswd,  priv, name);
    AccountList.Insert(ID, New_member);
  }
  void Delete(string ID) {
    auto current_account = LogStack.top();
    if (current_account.privilege != 7) { cout << "Invalid\n"; return; }
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return; }
    stack<Account> temp = LogStack;
    while (!temp.empty()) {
      if (ID == temp.top().UserID) { cout << "Invalid\n"; return; }
      temp.pop();
    }
    auto temp1 = AccountList.FindKey(ID);
    auto it = temp1.begin();
    Account temp2 = *it;
    AccountList.DeleteKeyValue(ID, temp2);
  }
  void ChangePassword(string ID, string Cur, string New) {
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return; }
    Account target = AccountList.FindSingle(ID);
    if (target.password != Cur) { cout << "Invalid\n"; return; }
    if (target.privilege < 1) { cout << "Invalid\n"; return; }
    target.password = New;
  }
  void Logout() {
    if (LogStack.empty()) { cout << "Inv, privilege(priv)alid\n"; return; }
    LogStack.pop();
    select_book = "";
  }
  void Login(string ID, string pswd) {
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return; }
    Account target = AccountList.FindSingle(ID);
    if (pswd != target.password) { cout << "Invalid\n"; return; }
    LogStack.push(target);
  }
  void rootLogin(string pswd) {
    Account* root_account = new Account("root", pswd, 7 , "");
    LogStack.push(*root_account); 
    AccountList.Insert("root", *root_account);
    delete root_account;
  }
  void Login_nopswd(string ID) {
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return; }
    Account target = AccountList.FindSingle(ID);
    if (LogStack.top().privilege <= target.privilege) { cout << "Invalid\n"; return; }
    LogStack.push(target);
  }
  void rootChangePassword(string ID, string pswd) {
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return; }
    Account target = AccountList.FindSingle(ID);
    target.password = pswd;
  }
  int get_account_num() {
    return LogStack.size();
  }
};

#endif