#include "include/AccountSystem.hpp"
#include "include/ProcessLine.hpp"
#include "include/MemoryRiver.hpp"
#include "include/BookSystem.hpp"
#include "include/LogSystem.hpp"
#include <iostream>

using std::string;
using std::cout;

int main() {
  LogSystem log_system;
  BookSystem book_system;
  AccountSystem account_system;
  string s;

  freopen("/home/entong/Bookstore/bookstore-testcases/basic/testcase6.in", "r", stdin);
  freopen("/home/entong/Bookstore/bookstore-testcases/basic/testcase6.out", "w", stdout);

  int cur_privilege = -1;
  while (getline(std::cin, s)) { 
    auto input = GetInput(s);
    bool flag = judge(input);
    if (!flag && s != "") cout << "Invalid: syntax error\n";
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
            Operation op(account_system.get_worker_ID(), "show finance", "show finance", false, account_system.get_privilege(account_system.get_worker_ID()));
            log_system.load_log(op);
            cout << "Invalid\n";
            continue;
          }
          if (input.size() == 2) {
            log_system.show_finance();
            Operation op("root", "show finance", "show finance", true, 7);
            log_system.load_log(op);
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
            Operation op("root", "show finance", "show finance: count=" + std::to_string(count), true, 7);
            log_system.load_log(op);
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
          bool check = account_system.Login_nopswd(input[1]);
          if (check) book_system.select_books_add();
          cur_privilege = account_system.get_current_privilege();
          Operation op(input[1], "login", input[1] + " login", check, account_system.get_privilege(input[1]));
          log_system.load_log(op);
          continue;
        } else if (input.size() == 3) {
          bool check = account_system.Login(input[1], input[2]);
          if (check) {
            book_system.select_books_add();
          }
          cur_privilege = account_system.get_current_privilege();
          Operation op(input[1], "login", input[1] + " login", check, account_system.get_privilege(input[1]));
          log_system.load_log(op);
          continue;
        } else {
          cout << "Invalid\n";
          continue;
        }
      }

      //退出登录
      if (input[0] == "logout") {
        string temp = account_system.get_worker_ID();
        bool check = account_system.Logout();
        Operation op(temp, "logout", temp + " logout", check, account_system.get_privilege(temp));
        log_system.load_log(op);
        book_system.clear_select_book();
        book_system.select_books_pop();
        book_system.set_select_book();
        cur_privilege = account_system.get_current_privilege();
      }

      //注册
      if (input[0] == "register") {
        bool check = account_system.Register(input[1], input[2], input[3]);
        Operation op(input[1], "register", "register " + input[1] + " password: " + input[2] + " name: " + input[3], check, account_system.get_privilege(input[1]));
        log_system.load_log(op);
      }

      //修改密码
      if (input[0] == "passwd") {
        if (input.size() == 3 && cur_privilege != 7) {
          cout << "Invalid\n";
          continue;
        }
        if (input.size() == 3) {
          bool judge = account_system.rootChangePassword(input[1], input[2]);
          Operation op(input[1], "change passwd", "change password to " + input[2], judge, account_system.get_privilege(input[1]));
          log_system.load_log(op);
        } else {
          bool judge = account_system.ChangePassword(input[1], input[2], input[3]);
          Operation op(input[1], "change passwd", "change password from " + input[2] + " to " + input[3], judge, account_system.get_privilege(input[1]));
          log_system.load_log(op);
        }
      }

      //添加用户
      if (input[0] == "useradd") {
        if (std::stoi(input[3]) >= cur_privilege) {
          Operation op(account_system.get_worker_ID(), "useradd", "add user: " + input[1], false, account_system.get_privilege(account_system.get_worker_ID()));
          log_system.load_log(op);
          cout << "Invalid\n";
          continue;
        }
        if (input[3] != "0" && input[3] != "1" && input[3] != "3") {
          Operation op(account_system.get_worker_ID(), "useradd", "add user: " + input[1], false, account_system.get_privilege(account_system.get_worker_ID()));
          log_system.load_log(op);
          cout << "Invalid\n";
          continue;
        }
        bool judge = account_system.UserAdd(input[1], input[2], std::stoi(input[3]), input[4]);
        Operation op(account_system.get_worker_ID(), "useradd", "add user: " + input[1], judge, account_system.get_privilege(account_system.get_worker_ID()));
        log_system.load_log(op);
      }

      //删除用户
      if (input[0] == "delete") {
        bool judge = account_system.Delete(input[1]);
        Operation op(account_system.get_worker_ID(), "delete", "delete user: " + input[1], judge, account_system.get_privilege(account_system.get_worker_ID()));
        log_system.load_log(op);
      }

      //选择图书
      if (input[0] == "select") {
        if (cur_privilege < 3) {
          Operation op(account_system.get_worker_ID(), "select", "select book by ISBN: " + input[1], false, account_system.get_privilege(account_system.get_worker_ID()));
          log_system.load_log(op);
          cout << "Invalid\n";
          continue;
        }
        bool judge = book_system.select(input[1]);
        Operation op(account_system.get_worker_ID(), "select", "select book by ISBN: " + input[1], judge, account_system.get_privilege(account_system.get_worker_ID()));
        log_system.load_log(op);
      }

      //检索图书
      if (input[0] == "show") {
        if (cur_privilege < 1) {
          Operation op(account_system.get_worker_ID(), "show", "show all books", false, account_system.get_privilege(account_system.get_worker_ID()));
          log_system.load_log(op);
          cout << "Invalid\n";
          continue;
        }
        if (input.size() == 1) {
          bool judge = book_system.showall();
          Operation op(account_system.get_worker_ID(), "show", "show all books", judge, account_system.get_privilege(account_system.get_worker_ID()));
          log_system.load_log(op);
        } else {
          bool judge = book_system.show(input[1]);
          Operation op(account_system.get_worker_ID(), "show", "show books by " + input[1], judge, account_system.get_privilege(account_system.get_worker_ID()));
          log_system.load_log(op);
        }
      }

      //修改图书信息
      if (input[0] == "modify") {
        string select_book_ISBN = book_system.get_select_book().ISBN;
        if (select_book_ISBN == "") {
          select_book_ISBN = "None";
        }
        if (cur_privilege < 3) {
          Operation op(account_system.get_worker_ID(), "modify " + select_book_ISBN + " ", "modify book", false, account_system.get_privilege(account_system.get_worker_ID()));
          log_system.load_log(op);
          cout << "Invalid\n";
          continue;
        }
        bool judge = book_system.modify(input);
        string x = "";
        for (int i = 1; i < input.size(); i++) {
          x += input[i];
          x += " ";
        }
        Operation op(account_system.get_worker_ID(), "modify " + select_book_ISBN + " ", x, judge, account_system.get_privilege(account_system.get_worker_ID()));
        log_system.load_log(op);
      }

      //购买图书
      if (input[0] == "buy") {
        int origin_quantity = book_system.get_quantity(input[1]);
        bool judge = book_system.buy(input[1], input[2]);
        if (origin_quantity >= std::stoi(input[2])) {
          log_system.record_income(book_system.get_book(input[1]).price * std::stoi(input[2]));
        }
        Operation op(account_system.get_worker_ID(), "buy", "buy books: ISBN: " + input[1] + " quantity: " + input[2], judge, account_system.get_privilege(account_system.get_worker_ID()));
        log_system.load_log(op);
      }

      //进货
      if (input[0] == "import") {
        if (cur_privilege < 3) {
          Operation op(account_system.get_worker_ID(), "import", "import books: ISBN: " + input[1] + " total cost: " + input[2], false, account_system.get_privilege(account_system.get_worker_ID()));
          log_system.load_log(op);
          cout << "Invalid\n";
          continue;
        }
        book_system.import(input[1], input[2]);
        bool judge = book_system.check_select_book();
        if (judge) log_system.record_expense(std::stod(input[2]));
        Operation op(account_system.get_worker_ID(), "import", "import books: ISBN: " + input[1] + " total cost: " + input[2], judge, account_system.get_privilege(account_system.get_worker_ID()));
        log_system.load_log(op);
      }
      if (input[0] == "log") {
        if (account_system.get_current_privilege() != 7) {
          cout << "Invalid\n";
          continue;
        }
        log_system.show_log();
      }
      if (input[0] == "report") {
        if (account_system.get_current_privilege() != 7) {
          cout << "Invalid\n";
          continue;
        }
        if (input[1] == "finance") {
          log_system.show_finance_statements();
        }
        if (input[1] == "employee") {
          log_system.report_imployee();
        }
      }
    }
  }
}