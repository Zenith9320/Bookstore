#ifndef LOGSYSTEM_HPP
#define LOGSYSTEM_HPP

#include "Vector.hpp"
#include <iomanip>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

using std::string;
using std::fixed;
using std::setprecision;

struct Finance {
  double income;
  double expense;
};

struct Operation {
  int priv;
  char WorkerID[31];
  char operationType[50];
  char operation_content[500];
  bool if_success;
  Operation() : WorkerID(""), operationType(""), operation_content("") {}
  Operation(const string& WorkerID, const string& operationType, const string& operation_content, bool check, int priv) : priv(priv) {
    strcpy(this->WorkerID, WorkerID.c_str());
    strcpy(this->operationType, operationType.c_str());
    strcpy(this->operation_content, operation_content.c_str());
    if_success = check;
  }
  ~Operation() = default;
};

class LogSystem {
private:
  //记录收入支出的记录
  Vector<Finance> FinanceList;
  Vector<Operation> Log;//记录所有人的操作
  Vector<Operation> Log1;//记录员工的操作

public:
  LogSystem() : FinanceList("Bookstore_FinanceList.dat"), Log("Bookstore_Log.dat"), Log1("Bookstore_employee_Log.dat") {};
  ~LogSystem() {}
  
  //输出所有记录
  void show_finance() {
    double total_income = 0;
    double total_expense = 0;
    for (int i = 0; i < FinanceList.size(); i++) {
      Finance finance = FinanceList[i];
      total_income += finance.income;
      total_expense += finance.expense;
    }
    std::cout << "+ " << fixed << setprecision(2) << total_income << " - " << fixed << setprecision(2) << total_expense;
  }

  double get_total_income() {
    double total_income = 0;
    for (int i = 0; i < FinanceList.size(); i++) {
      Finance finance = FinanceList[i];
      total_income += finance.income;
    }
    return total_income;
  }

  double get_total_expense() {
    double total_expense = 0;
    for (int i = 0; i < FinanceList.size(); i++) {
      Finance finance = FinanceList[i];
      total_expense += finance.expense;
    }
    return total_expense;
  }

  //输出指定笔数的记录
  void show_finance(const int& count) {
    if (count == 0) {
      std::cout << "\n";
      return;
    }
    if (count > FinanceList.size() || count < 0) {
      std::cout << "Invalid\n";
      return;
    }
    if (count == FinanceList.size()) {
      show_finance();
      return;
    }
    double total_income = 0;
    double total_expense = 0;
    for (int i = FinanceList.size() - 1; i >= FinanceList.size() - count; i--) {
      Finance finance = FinanceList[i];
      total_income += finance.income;
      total_expense += finance.expense;
    }
    std::cout << "+ " << fixed << setprecision(2) << total_income << " - " << fixed << setprecision(2) << total_expense;
  }

  //记录一笔收入
  void record_income(const double& income) {
    Finance finance;
    finance.income = income;
    finance.expense = 0;
    FinanceList.push_back(finance);
  }

  //记录一笔支出
  void record_expense(const double& expense) {
    Finance finance;
    finance.income = 0;
    finance.expense = expense;
    FinanceList.push_back(finance);
  }

  //输出Log中的信息
  void show_operations() {
    for (int i = 0 ; i < 190; i++) std::cout << '-';
    std::cout << '\n';
    std::cout << std::left << std::setw(31) << "Worker ID" << std::setw(75) << "Operation Type" << std::setw(75) << "Operation Content" << std::setw(10) << "Status" << '\n';
    for (int i = 0 ; i < 190; i++) std::cout << '-';
    std::cout << '\n';
    for (int i = 0; i < Log.size(); i++) {
      Operation operation = Log[i];
      if (operation.operationType[0] != 'm') {
        std::cout << std::left << std::setw(31) << operation.WorkerID << std::setw(75) << operation.operationType << std::setw(75) << operation.operation_content;
        if (operation.if_success) std::cout << std::setw(6) << "SUCCESS\n";
        else std::cout << std::left << std::setw(6) << "FAILED\n";
      }
      else {
        std::vector <string> modify_types;
        std::vector <string> modify_contents;
        string temp = "";
        for (int i = 0; i < strlen(operation.operation_content); i++) {
          if (operation.operation_content[i] == '-') continue;
          if (operation.operation_content[i] == '=') {
            modify_types.push_back(operation.operationType + temp);
            temp = "";
            while (operation.operation_content[i] != ' ') i++;
          } else {
            temp += operation.operation_content[i];
          }
        }
        if (temp.size() != 0) modify_contents.push_back(temp);
        for (int i = 0; i < strlen(operation.operation_content); i++) {
          if (operation.operation_content[i] == '-') {
            continue;
          }
          if (operation.operation_content[i] == ' ') {
            modify_contents.push_back(temp);
            temp = "";
          } else {
            temp += operation.operation_content[i];
          }
        }
        if (temp.size() != 0) modify_contents.push_back(temp);
        std::cout << std::left << std::setw(31) << operation.WorkerID << std::setw(75) << modify_types[0] << std::setw(75) << "modify to: " + modify_contents[0];
        if (operation.if_success) std::cout << std::setw(6) << "SUCCESS\n";
        else std::cout << std::left << std::setw(6) << "FAILED\n";
        for (int i = 1; i < modify_types.size(); i++) {
          std::cout << std::left << std::setw(31) << " " << std::setw(75) << modify_types[i] << std::setw(75) << "modify to: " + modify_contents[i];
          if (operation.if_success) std::cout << std::setw(6) << "SUCCESS\n";
          else std::cout << std::left << std::setw(6) << "FAILED\n";
        }
      }
    }    
    for (int i = 0 ; i < 190; i++) std::cout << '-';
    std::cout << '\n';
  }

  void show_log() {
    show_operations();
    show_all_finance();
  }

  //记录一次操作
  void load_log(const Operation& operation) {
    Log.push_back(operation);
  }

  void show_all_finance() {
    std::cout << "\nFinance List:\n";
    for (int i = 0 ; i < 160; i++) std::cout << '-';
    std::cout << '\n';
    std::cout << std::left << std::setw(31) << "WorkerID" << std::setw(75) << "Operation Content" << std::setw(20) << "Income/Expense" << '\n';
    for (int i = 0 ; i < 160; i++) std::cout << '-';
    std::cout << '\n';
    int k = 0;
    for (int i = 0; i < Log.size(); i++) {
      Operation operation = Log[i];
      if ((operation.operationType[0] == 'b' || operation.operationType[0] == 'i') && operation.if_success) {
        std::cout << std::left << std::setw(31) << operation.WorkerID << std::setw(75) << operation.operation_content;
        if (operation.operationType[0] == 'b') {
          while (FinanceList[k].income == 0) {
            k++;
            if (k == FinanceList.size()) break;
          }
          std::cout << std::left << std::setw(20) << "Income: " + std::to_string(FinanceList[k].income) << '\n';
          k++;
        } else {
          while (FinanceList[k].expense == 0) {
            k++;
            if (k == FinanceList.size()) break;
          }
          std::cout << std::left << std::setw(20) << "Expense: " + std::to_string(FinanceList[k].expense) << '\n';
          k++;
        }
      }
    }
    for (int i = 0 ; i < 160; i++) std::cout << '-';
    std::cout << '\n';
    std::cout << "Total Income: " << get_total_income() << '\n';
    std::cout << "Total Expense: " << get_total_expense() << '\n';
  }

  void report_imployee() {
    std::cout << "\nEmployee Log:\n";
    for (int i = 0 ; i < 190; i++) std::cout << '-';
    std::cout << '\n';
    std::cout << std::left << std::setw(31) << "Worker ID" << std::setw(75) << "Operation Type" << std::setw(75) << "Operation Content" << std::setw(10) << "Status" << '\n';
    for (int i = 0 ; i < 190; i++) std::cout << '-';
    std::cout << '\n';
    for (int i = 0; i < Log.size(); i++) {
      Operation operation = Log[i];
      if (operation.priv != 3) {
        continue;
      }
      if (operation.operationType[0] != 'm') {
        std::cout << std::left << std::setw(31) << operation.WorkerID << std::setw(75) << operation.operationType << std::setw(75) << operation.operation_content;
        if (operation.if_success) std::cout << std::setw(6) << "SUCCESS\n";
        else std::cout << std::left << std::setw(6) << "FAILED\n";
      }
      else {
        std::vector <string> modify_types;
        std::vector <string> modify_contents;
        string temp = "";
        for (int i = 0; i < strlen(operation.operation_content); i++) {
          if (operation.operation_content[i] == '-') continue;
          if (operation.operation_content[i] == '=') {
            modify_types.push_back(operation.operationType + temp);
            temp = "";
            while (operation.operation_content[i] != ' ') i++;
          } else {
            temp += operation.operation_content[i];
          }
        }
        if (temp.size() != 0) modify_contents.push_back(temp);
        for (int i = 0; i < strlen(operation.operation_content); i++) {
          if (operation.operation_content[i] == '-') {
            continue;
          }
          if (operation.operation_content[i] == ' ') {
            modify_contents.push_back(temp);
            temp = "";
          } else {
            temp += operation.operation_content[i];
          }
        }
        if (temp.size() != 0) modify_contents.push_back(temp);
        std::cout << std::left << std::setw(31) << operation.WorkerID << std::setw(75) << modify_types[0] << std::setw(75) << "modify to: " + modify_contents[0];
        if (operation.if_success) std::cout << std::setw(6) << "SUCCESS\n";
        else std::cout << std::left << std::setw(6) << "FAILED\n";
        for (int i = 1; i < modify_types.size(); i++) {
          std::cout << std::left << std::setw(31) << " " << std::setw(75) << modify_types[i] << std::setw(75) << "modify to: " + modify_contents[i];
          if (operation.if_success) std::cout << std::setw(6) << "SUCCESS\n";
          else std::cout << std::left << std::setw(6) << "FAILED\n";
        }
      }
    }    
    for (int i = 0 ; i < 190; i++) std::cout << '-';
    std::cout << '\n';
  }

  double get_total_cash() {
    double total_income = 0;
    for (int i = 0; i < FinanceList.size(); i++) {
      Finance finance = FinanceList[i];
      total_income += finance.income + finance.expense;
    }
    return total_income;
  }

  double get_profit() {
    return get_total_income() - get_total_expense();
  }

  void show_finance_statements() {
    show_all_finance();
    std::cout << "Total Cash: " << get_total_cash() << '\n';
    std::cout << "Profit: " << get_profit() << '\n';
  }
};

#endif