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
  LogSystem() : FinanceList("FinanceList.dat") {};
  ~LogSystem() {
    FinanceList.clear();
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
    double total_income = 0;
    double total_outcome = 0;
    for (int i = FinanceList.size() - 1; i >= FinanceList.size() - count; i--) {
      Finance finance = FinanceList[i];
      total_income += finance.income;
      total_outcome += finance.outcome;
    }
  }

  //输出所有记录
  void show_finance() {
    double total_income = 0;
    double total_outcome = 0;
    for (int i = 0; i < FinanceList.size(); i++) {
      Finance finance = FinanceList[i];
      total_income += finance.income;
      total_outcome += finance.outcome;
    }
    std::cout << fixed << setprecision(2) << total_income << ' ' << fixed << setprecision(2) << total_outcome << '\n';
  }
};

#endif