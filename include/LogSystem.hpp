#ifndef LOGSYSTEM_HPP
#define LOGSYSTEM_HPP

#include "Vector.hpp"
#include <iomanip>
#include <string>
#include <iostream>

using std::string;
using std::fixed;
using std::setprecision;

struct Finance {
  double income;
  double outcome;
};

class LogSystem {
private:
  //记录收入支出的记录
  Vector<Finance> FinanceList;

public:
  LogSystem() : FinanceList("Bookstore_FinanceList.dat") {};
  ~LogSystem() {}
  
  //输出所有记录
  void show_finance() {
    double total_income = 0;
    double total_outcome = 0;
    for (int i = 0; i < FinanceList.size(); i++) {
      Finance finance = FinanceList[i];
      total_income += finance.income;
      total_outcome += finance.outcome;
    }
    std::cout << "+ " << fixed << setprecision(2) << total_income << " - " << fixed << setprecision(2) << total_outcome;
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
    double total_outcome = 0;
    for (int i = FinanceList.size() - 1; i >= FinanceList.size() - count; i--) {
      Finance finance = FinanceList[i];
      total_income += finance.income;
      total_outcome += finance.outcome;
    }
    std::cout << "+ " << fixed << setprecision(2) << total_income << " - " << fixed << setprecision(2) << total_outcome;
  }

  //记录一笔收入
  void record_income(const double& income) {
    Finance finance;
    finance.income = income;
    finance.outcome = 0;
    FinanceList.push_back(finance);
  }

  //记录一笔支出
  void record_outcome(const double& outcome) {
    Finance finance;
    finance.income = 0;
    finance.outcome = outcome;
    FinanceList.push_back(finance);
  }
};

#endif