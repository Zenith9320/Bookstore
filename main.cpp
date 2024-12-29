#include "include/AccountSystem.hpp"
#include "include/ProcessLine.hpp"
#include "include/MemoryRiver.hpp"
#include "include/BookSystem.hpp"
#include "include/LogSystem.hpp"
#include <iostream>

using std::string;
using std::cout;

void deleteGeneratedFiles(const vector<string>& files) {
  for (const auto& file : files) {
    std::ofstream file1(file, std::ios::trunc);
    file1.close();
  }
}

int main() {
  LogSystem log_system;
  BookSystem book_system;
  AccountSystem account_system;
  
  std::vector <string> files = 
  {"Account_index_file.dat", 
  "Account_value_file.dat", 
  "FinanceList.dat", 
  "ISBN_Book_index_file.dat", 
  "ISBN_Book_value_file.dat", 
  "author_ISBN_index_file.dat", 
  "author_ISBN_value_file.dat", 
  "keywords_ISBN_index_file.dat", 
  "keywords_ISBN_value_file.dat", 
  "name_ISBN_index_file.dat", 
  "name_ISBN_value_file.dat", 
  "price_ISBN_index_file.dat", 
  "price_ISBN_value_file.dat"};

  while (true) {
    string s;
    getline(std::cin, s);
    auto input = GetInput(s);
    bool flag = judge(input);
    if (!flag && s != "") cout << "Invalid\n";
    else {
      //退出程序
      if (input.size() == 0) continue;
      if (input[0] == "exit" || input[0] == "quit") {
        remove("Account_index_file.dat");
        remove("Account_value_file.dat");
        remove("FinanceList.dat");
        remove("ISBN_Book_index_file.dat");
        remove("ISBN_Book_value_file.dat");
        remove("author_ISBN_index_file.dat");
        remove("author_ISBN_value_file.dat");
        remove("keywords_ISBN_index_file.dat");
        remove("keywords_ISBN_value_file.dat");
        remove("name_ISBN_index_file.dat");
        remove("name_ISBN_value_file.dat");
        remove("price_ISBN_index_file.dat");
        remove("price_ISBN_value_file.dat");
        return 0;
      }

      //财务记录查询
      if (input[0] == "show" && input[1] == "finance") {
        if (input.size() == 2) {
          log_system.show_finance();
        } else {
          int count = std::stoi(input[2]);
          log_system.show_finance(count);
        }
      }

      //登录
      if (input[0] == "su") {
        if (input.size() == 2 && account_system.get_account_num() == 0) {
          cout << "Invalid\n";
          continue;
        }
        if (input.size() == 2) {
          account_system.Login_nopswd(input[1]);
          continue;
        } else if (input.size() == 3) {
          account_system.Login(input[1], input[2]);
          continue;
        } else {
          cout << "Invalid\n";
          continue;
        }
      }

      //退出登录
      if (input[0] == "logout") {
        account_system.Logout();
        book_system.clear_select_book();
      }

      //注册
      if (input[0] == "register") {
        account_system.Register(input[1], input[2], input[3]);
      }

      //修改密码
      if (input[0] == "passwd") {
        if (input.size() == 2 && account_system.get_privilege(input[1]) != 7) {
          cout << "Invalid\n";
          continue;
        }
        if (input.size() == 2) {
          account_system.rootChangePassword(input[1], input[2]);
        } else {
          account_system.ChangePassword(input[1], input[2], input[3]);
        }
      }

      //添加用户
      if (input[0] == "useradd") {
        if (std::stoi(input[3]) >= account_system.get_current_privilege()) {
          cout << "Invalid\n";
          continue;
        }
        if (input[3] != "0" && input[3] != "1" && input[3] != "3") {
          cout << "Invalid\n";
          continue;
        }
        account_system.UserAdd(input[1], input[2], std::stoi(input[3]), input[4]);
      }

      //删除用户
      if (input[0] == "delete") {
        account_system.Delete(input[1]);
      }

      //选择图书
      if (input[0] == "select") {
        book_system.select(input[1]);
      }

      //检索图书
      if (input[0] == "show") {
        if (input.size() == 1) {
          book_system.showall();
        } else {
          book_system.show(input[1]);
        }
      }

      //修改图书信息
      if (input[0] == "modify") {
        book_system.modify(input);
      }

      //购买图书
      if (input[0] == "buy") {
        book_system.buy(input[1], input[2]);
        log_system.record_income(book_system.get_select_book().price * std::stoi(input[2]));
      }

      //进货
      if (input[0] == "import") {
        book_system.import(input[1], input[2]);
        log_system.record_outcome(std::stod(input[2]));
      }
    }
  }
}