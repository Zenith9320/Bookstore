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
  string passward;
  int privilege;
  string UserName;

  Account() : UserID(""), passward(""), privilege(-1), UserName("") {};
  Account(string ID, string pswd, string name) 
  : UserID(ID), passward(pswd), privilege(1), UserName(name){};
  Account(string ID, string pswd, string name, int priv) 
  : UserID(ID), passward(pswd), UserName(name), privilege(priv) {};
};

class AccountSystem {
private:
  Blocklist<Account> AccountList;
  stack<Account> LogStack;
  string select_book;
  BookSystem* bookSystem;

public:
  AccountSystem() : AccountList("Account_", "index_file.dat", "value_file.dat") {
    while (!LogStack.empty()) LogStack.pop();
    select_book = "";
    Account root;
    root.UserID = "root";
    root.passward = "";
    root.privilege = 7;
    root.UserName = "";
    AccountList.Insert(root.UserID, root);
  }
  ~AccountSystem() {
    while (!LogStack.empty()) LogStack.pop();
    delete bookSystem;
    AccountList.clearall();
    select_book = "";
  };
  int get_current_privilege() {
    if (LogStack.empty()) return -1;
    return LogStack.top().privilege;
  }
  int get_privilege(const string& ID) {
    if (!AccountList.if_find(ID)) return -1;
    return AccountList.FindSingle(ID).privilege;
  }
  AccountSystem(BookSystem books) : AccountList("Account_", "index_file.dat", "value_file.dat") {
    while (!LogStack.empty()) LogStack.pop();
    select_book = "";
    Account root;
    root.UserID = "root";
    root.passward = "";
    root.privilege = 7;
    root.UserName = "";
    AccountList.Insert(root.UserID, root);
    bookSystem = &books;
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
    Account New_member(ID, pswd, name, priv);
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
    if (target.passward != Cur) { cout << "Invalid\n"; return; }
    if (target.privilege < 1) { cout << "Invalid\n"; return; }
    target.passward = New;
  }
  void Logout() {
    if (LogStack.empty()) { cout << "Invalid\n"; return; }
    LogStack.pop();
    select_book = "";
    bookSystem->clear_select_book();
  }
  void Login(string ID, string pswd) {
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return; }
    Account target = AccountList.FindSingle(ID);
    if (pswd != target.passward) { cout << "Invalid\n"; return; }
    if (LogStack.top().privilege <= target.privilege) { cout << "Invalid\n"; return; }
    LogStack.push(target);
  }
  void rootLogin(string ID, string name) {
    if (ID != "root") { cout << "Invalid\n"; return; }
    Account root = Account("root", "", name, 7) ;
    LogStack.push(root);
  }
  void rootChangePassword(string ID, string pswd) {
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return; }
    Account target = AccountList.FindSingle(ID);
    target.passward = pswd;
  }
};

#endif