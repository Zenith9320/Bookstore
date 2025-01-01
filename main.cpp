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
  std::ios::sync_with_stdio(false); 
  cin.tie(nullptr);          
  cout.tie(nullptr);
  LogSystem log_system;
  BookSystem book_system;
  AccountSystem account_system;
  string s;

  freopen("/home/entong/Bookstore/bookstore-testcases/basic/testcase3.in", "r", stdin);
  freopen("/home/entong/Bookstore/bookstore-testcases/basic/testcase3(1).out", "w", stdout);

  int cur_privilege = -1;
  while (getline(std::cin, s)) { 
    auto input = GetInput(s);
    bool flag = judge(input);
    if (!flag && s != "") cout << "Invalid\n";
    else {
      //退出程序
      if (input.size() == 0) continue;
      if (input[0] == "exit" || input[0] == "quit") {
        return 0;
      }

      //财务记录查询
      if (input.size() == 2 || input.size() == 3) {
        if (input[0] == "show" && input[1] == "finance") {
          if (cur_privilege != 7) {
            cout << "Invalid\n";
            continue;
          }
          if (input.size() == 2) {
            log_system.show_finance();
          } else {
            for (int i = 0; i < input[2].size(); i++) {
              if (!isdigit(input[2][i])) {
                cout << "Invalid\n";
                continue;
              }
              if (i == 0 && input[2][i] == '0') {
                cout << "Invalid\n";
                continue;
              }
            }
            int count = std::stoi(input[2]);
            log_system.show_finance(count);
          }
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
          book_system.select_books_add();
          cur_privilege = account_system.get_current_privilege();
          continue;
        } else if (input.size() == 3) {
          account_system.Login(input[1], input[2]);
          book_system.select_books_add();
          cur_privilege = account_system.get_current_privilege();
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
        book_system.select_books_pop();
        book_system.set_select_book();
        cur_privilege = account_system.get_current_privilege();
      }

      //注册
      if (input[0] == "register") {
        account_system.Register(input[1], input[2], input[3]);
      }

      //修改密码
      if (input[0] == "passwd") {
        if (input.size() == 3 && cur_privilege != 7) {
          cout << "Invalid\n";
          continue;
        }
        if (input.size() == 3) {
          account_system.rootChangePassword(input[1], input[2]);
        } else {
          account_system.ChangePassword(input[1], input[2], input[3]);
        }
      }

      //添加用户
      if (input[0] == "useradd") {
        if (std::stoi(input[3]) >= cur_privilege) {
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
        if (cur_privilege < 3) {
          cout << "Invalid\n";
          continue;
        }
        book_system.select(input[1]);
      }

      //检索图书
      if (input[0] == "show") {
        if (cur_privilege < 1) {
          cout << "Invalid\n";
          continue;
        }
        if (input.size() == 1) {
          book_system.showall();
        } else {
          book_system.show(input[1]);
        }
      }

      //修改图书信息
      if (input[0] == "modify") {
        if (cur_privilege < 3) {
          cout << "Invalid\n";
          continue;
        }
        book_system.modify(input);
      }

      //购买图书
      if (input[0] == "buy") {
        int origin_quantity = book_system.get_quantity(input[1]);
        book_system.buy(input[1], input[2]);
        if (origin_quantity >= std::stoi(input[2])) {
          log_system.record_income(book_system.get_book(input[1]).price * std::stoi(input[2]));
        }
      }

      //进货
      if (input[0] == "import") {
        if (cur_privilege < 3) {
          cout << "Invalid\n";
          continue;
        }
        book_system.import(input[1], input[2]);
        bool judge = book_system.check_select_book();
        if (judge) log_system.record_outcome(std::stod(input[2]));
      }
    }
  }
}