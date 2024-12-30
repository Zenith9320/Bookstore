//对指令进行合法性判断，并且将指令拆分成一个个字符串放在一个vector中
#ifndef PROCESSLINE_HPP
#define PROCESSLINE_HPP

#include<string>
#include<vector>
#include<iostream>

using std::cin;
using std::vector;
using std::string;

//检验UserID,Password相关的输入合法性
//要求最大长度：30， 合法字符集为数字、字母、下划线
inline bool judgeaccount(const string &psd) {
  if (psd.size() > 30) return false;
  for (int i = 0; i < psd.length(); i++) {
    if (!((psd[i] >= '0' && psd[i] <= '9') 
    || (psd[i] >= 'a' && psd[i] <= 'z') 
    || (psd[i] >= 'A' && psd[i] <= 'Z') 
    || psd[i] == '_')) {
      return false;
    }
  }
  return true;
}

//检验Username合法性
//要求为除不可见字符外的ASCII字符,长度最大为30
inline bool judgeUsername(const string& s) {
  if (s.size() > 30) return false;
  for (char c : s) {
    if (c < 32 || c > 126) {
      return false;
    }
  }
  return true;
}

//获取当前的查询条件类型
//show指令包括ISBN，name,author,keyword
inline string get_type(const string &s) {
  bool flag = false;
  string temp = "";
  for (int i = 1; i < s.size(); i++) {
    if (s[i] == '=') {
      flag = true;
      break;
    }
    temp += s[i];
  }
  if (flag) {
    if (temp == "ISBN"
    || temp == "name"
    || temp == "author"
    || temp == "keyword") return temp;
  }
  return "";
}

//检验ISBN的合法性
//要求为除不可见字符外的ASCII字符,长度最大为20
inline bool judgeISBN(const string& s) {
  if (s.size() > 20) return false;
  for (char c : s) {
    if (c < 32 || c > 126) {
      return false;
    }
  }
  return true;
}

//检验书名、作者、关键词的合法性
//要求为除不可见字符和英文双引号以外的ASCII字符
inline bool judgeBookAutherKey(const string &s) {
  if (s.size() > 60) return false;
  if (s[0] != '"' || s[s.size() - 1] != '"') return false;
  for (int i = 1; i < s.size() - 1; i++) {
    char ch = s[i];
    if (!isprint(ch) || ch == '"') {
      return false;
    }
  }
  return true;
}

//检验购买数量和合法性
//要求为最大长度10的数字，并且数值不超过2147483647
inline bool judgequantity(const string& s) {
  for (int i = 0; i < s.size(); i++) {
    if (!isdigit(s[i])) return false;
  }
  const int MAX = 2147483647;
  int quantity = std::stoi(s);
  if (quantity > MAX) return false;
  if (s.size() > 10) return false;
  return true;
}

//检验和price有关的变量和合法性
inline bool judgeprice(const string& s) {
  if (s.size() > 13) return false;
  for (int i = 0; i < s.size(); i++) {
    if (!isdigit(s[i]) && s[i] != '.') return false;
    if (s[i] == '.' && i == 0) return false;
    if (s[i] == '.' && i == s.size() - 1) return false;
    if (s[i] == '.' && s.size() - i > 3) return false;
  }
  return true;
}

//检验show操作中查询条件的合法性
//要求为-XX=XX的格式，既要检验查询的主体，又要检验查询的内容
inline bool judge_show_query(const string& s) {
  string temp = "";
  if (s[0] != '-') return false;
  int pos = 1;
  while (s[pos] != '=' && pos < s.size()) {
    temp += s[pos];
    pos++;
  }
  if (pos == s.size()) {
    return false;
  }
  pos++;
  string temp1 = "";
  while (pos < s.size()) {
    temp1 += s[pos];
    pos++;
  }
  if (temp1.size() == 0) return false;
  if (temp == "ISBN" 
  && judgeISBN(temp1)) return true;
  if ((temp == "name" || temp == "author")
  && judgeBookAutherKey(temp1)) return true;
  if (temp == "keyword") {
    for (int i = 0; i < temp1.size(); i++) {
      if (temp1[i] == '|') return false;
    }
    return true;
  }
  return false;
}

//检验modify操作中查询条件的合法性
//只是比show操作多了price
inline bool judge_modify_query(const string& s) {
  string temp = "";
  if (s[0] != '-') return false;
  int pos = 1;
  while (s[pos] != '=' && pos < s.size()) {
    temp += s[pos];
    pos++;
  }
  if (pos == s.size()) {
    return false;
  }
  pos++;
  string temp1 = "";
  string no_quotation;
  while (pos < s.size()) {
    temp1 += s[pos];
    pos++;
  }
  if (temp == "ISBN" 
  && judgeISBN(temp1)) return true;
  if ((temp == "name" || temp == "author" || temp == "keyword")
  && judgeBookAutherKey(temp1)) return true;
  if (temp =="price" && judgeprice(temp1)) return true;
  return false;
}

//进行指令长度和前面的关键词的正确性检验
inline bool judge(const vector<string> &input) {
  if (input.size() == 0) return false;
  string command = input[0];
  size_t length = input.size();
  if (input.size() == 1) {
    if (command == "exit" 
    || command == "quit" 
    || command == "log" 
    || command == "logout" 
    || command == "show") return true;
    else return false;
  }
  else {
    if (command == "su") {
      if (length == 2) {
        if (judgeaccount(input[1])) return true;
        return false;
      }
      if (length == 3) {
        if (judgeaccount(input[1]) && judgeaccount(input[2])) return true;
        return false;
      }
      return false;
    }
    if (command == "register") {
      if (length == 4) {
        if (judgeaccount(input[1]) 
        && judgeaccount(input[2])
        && judgeUsername(input[3])) return true;
        return false;
      }
      else return false;
    }
    if (command == "passwd") {
      if ((length == 4 || length == 3) 
      && judgeaccount(input[1]) 
      && judgeaccount(input[2]) 
      && judgeaccount(input[length == 3 ? 2 : 3])) {
        return true;
      }
      else return false;
    }
    if (command == "useradd") {
      if (length == 5) {
        if (judgeaccount(input[1])
        && judgeaccount(input[2])
        && judgeUsername(input[4])
        && (input[3]=="0" || input[3]=="1" || input[3]=="3" || input[3]=="7")) return true;
        return false;
      }
      else return false;
    }
    if (command == "delete") {
      if (length == 2) {
        if (judgeISBN(input[1])) return true;
        return false;
      }
      else return false;
    }
    if (command == "show") {
      if ((length == 2 && judge_show_query(input[1])) 
      || length == 1 || (length == 2 && input[1] == "finance") 
      || (length == 3 && input[1] == "finance")) return true;
      return false;
    }
    if (command == "buy") {
      if (length != 3) return false;
      else {
        if (judgeISBN(input[1]) && judgequantity(input[2])) return true;
      }
    }
    if (command == "select") {
      if (length != 2) return false;
      else {
        if (judgeISBN(input[1])) return true;
      }
      return false;
    }
    if (command == "modify") {
      if (length < 2 || length > 6) return false;
      else {
        for (int i = 1; i < input.size(); i++) {
          if (!judge_modify_query(input[i])) return false;
        }
      }
      return true;
    }
    if (command == "import") {
      if (length != 3) return false;
      else {
        if (judgequantity(input[1]) && judgeprice(input[2])) return true;
      }
      return false;
    }
    if (command == "show" && input[1] == "finance") {
      if (length != 2 && length != 3) return false;
      else {
        return true;
      }
    }
    if (command == "report") {
      if (length != 2) return false;
      if (input[1] == "finance" || input[1] == "employee") {
        return true;
      }
      return false;
    } 
  }
  return false;
}

inline vector<string> GetInput(const string &input) {
  vector<string> result;
  string temp = "";
  for (int i = 0; i < input.size(); i++) {
    if (input[i] == ' ') {
      result.push_back(temp);
      temp = "";
    }
    else {
      temp += input[i];
    }
  }
  if (temp.size() > 0) result.push_back(temp);
  return result;
}
#endif