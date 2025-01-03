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
  char UserID[31];
  char password[31];
  int privilege;
  char UserName[31];

  Account & operator = (const Account& other) {
    if (this != &other) {
      strcpy(UserID, other.UserID);
      strcpy(password, other.password);
      privilege = other.privilege;
      strcpy(UserName, other.UserName);
    }
    return *this;
  }
  
  Account() : privilege(0) {
    memset(UserID, 0, sizeof(UserID));
    memset(password, 0, sizeof(password));
    memset(UserName, 0, sizeof(UserName));
  };

  Account(string ID, string pswd, string name) 
  : privilege(1) {
    strcpy(UserID, ID.c_str());
    strcpy(password, pswd.c_str());
    strcpy(UserName, name.c_str());
    string blank = "";
  };

  Account(string ID, string pswd, int priv, string name) 
  : privilege(priv) {
    strcpy(UserID, ID.c_str());
    strcpy(password, pswd.c_str());
    strcpy(UserName, name.c_str());
    string blank = "";
  };
};

inline bool operator == (const Account& account1, const Account& account2) {
  return (strcmp(account1.UserID, account2.UserID) == 0);
}

inline bool operator > (const Account& account1, const Account& account2) {
  return (strcmp(account1.UserID, account2.UserID) > 0);
}

inline bool operator < (const Account& account1, const Account& account2) {
  return (strcmp(account1.UserID, account2.UserID) < 0);
}

class AccountSystem {
private:
  Blocklist<Account> AccountList;
  stack<Account> LogStack;

public:
  AccountSystem() : AccountList("Bookstore_Account_", "index_file.dat", "value_file.dat") {
    Account root_account("root", "sjtu", 7, "root");
    string temp = "root";
    AccountList.Insert(temp.c_str(), root_account);
  }
  ~AccountSystem() {
    while (!LogStack.empty()) {
      LogStack.pop();
    }
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
  bool Register(string ID, string pswd, string name) {
    Account New_member(ID, pswd, name);
    if (AccountList.if_find(ID)) { cout << "Invalid\n"; return false; }
    AccountList.Insert(ID, New_member);
    return true;
  }
  bool UserAdd(string ID, string pswd, int priv, string name) {
    if (AccountList.if_find(ID)) { cout << "Invalid\n"; return false; }
    auto current_account = LogStack.top();
    if (priv > current_account.privilege) { cout << "Invalid\n"; return false; } 
    Account New_member(ID, pswd,  priv, name);
    AccountList.Insert(ID, New_member);
    return true;
  }
  bool Delete(string ID) {
    auto current_account = LogStack.top();
    if (current_account.privilege != 7) { cout << "Invalid\n"; return false; }
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return false; }
    stack<Account> temp = LogStack;
    while (!temp.empty()) {
      if (ID == temp.top().UserID) { cout << "Invalid\n"; return false; }
      temp.pop();
    }
    auto temp1 = AccountList.FindKey(ID);
    AccountList.DeleteKeyValue(ID, *temp1.begin());
    return true;
  }
  bool rootChangePassword(string ID, string pswd) {
    if (!AccountList.if_find(ID)) {
        cout << "Invalid\n";
        return false;
    }
    Account temp = AccountList.FindSingle(ID);
    while (AccountList.if_find(ID)) AccountList.DeleteKeyValue(ID, AccountList.FindSingle(ID)); 
    strcpy(temp.password, pswd.c_str());
    AccountList.Insert(ID, temp);
    Account temp2 = AccountList.FindSingle(ID);
    return true;
  }
  bool ChangePassword(string ID, string Cur, string New) {
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return false; }
    Account target = AccountList.FindSingle(ID);
    if (target.password != Cur) { cout << "Invalid\n"; return false; }
    if (ID == "root" && strcmp(Cur.c_str(), AccountList.FindSingle(ID).password) == 0) {
      rootChangePassword(ID, New);
      return false;
    }
    while (AccountList.if_find(ID)) AccountList.DeleteKeyValue(ID, AccountList.FindSingle(ID)); 
    strcpy(target.password, New.c_str());
    AccountList.Insert(ID, target);
    return true;
  }
  bool Logout() {
    if (LogStack.empty()) { cout << "Invalid\n"; return false; }
    string temp = "";
    LogStack.pop();
    return true;
  }
  bool Login(string ID, string pswd) {
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return false; }
    Account target = AccountList.FindSingle(ID);
    if (pswd != target.password) { cout << "Invalid\n"; return false; }
    LogStack.push(target);
    return true;
  }
  bool Login_nopswd(string ID) {
    if (!AccountList.if_find(ID)) { cout << "Invalid\n"; return false; }
    Account target = AccountList.FindSingle(ID);
    if (LogStack.top().privilege <= target.privilege) { cout << "Invalid\n"; return false; }
    LogStack.push(target);
    return true;
  }
  int get_account_num() {
    return LogStack.size();
  }
  bool check_pswd(const string& ID, const string& pswd) {
    return AccountList.FindSingle(ID).password == pswd;
  }
  string get_worker_ID() {
    if (LogStack.empty()) return "None";
    return LogStack.top().UserID;
  }
};

#endif