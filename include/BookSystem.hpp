#ifndef BOOKSYSTEM_HPP
#define BOOKSYSTEM_HPP

#include "BlockList.hpp"
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>

using std::string;
using std::vector;
using std::set;
using std::cout;

struct Book {
  string ISBN;
  string name;
  string author;
  vector<string> keywords;
  double price;
  int quantity;

  Book() = default;
  Book(string ISBN, string name, string author, vector<string> keywords, double price, int quantity)
  : ISBN(ISBN), name(name), author(author), keywords(keywords), price(price), quantity(quantity) {};
  Book(string ISBN) : ISBN(ISBN) {};

  //按照格式输出图书信息
  void outputBook(const Book& output) const {
    cout << output.ISBN << '\t' << output.name << '\t' << output.author << '\t';
    for (int i = 0; i < keywords.size(); i++) {
      cout << keywords[i] << '|';
    }
    cout << '\t' << output.price << '\t' << output.quantity << '\n';
  }
};

inline bool operator < (const Book& book1, const Book& book2) {
  return book1.ISBN < book2.ISBN;
};

class BookSystem {
private:
  //从书名到ISBN的MAP
  Blocklist<string> name_ISBN_list;
  //从作者到ISBN的MAP
  Blocklist<string> author_ISBN_list;
  //从关键词到ISBN的MAP
  Blocklist<string> keywords_ISBN_list;
  //从单价到ISBN的MAP
  Blocklist<double> price_ISBN_list;
  //从ISBN到Book的MAP
  Blocklist<Book> ISBN_Book_list;
  //当前选中的书
  Book select_book;
public:
  BookSystem() : name_ISBN_list("name_ISBN_", "index_file.dat", "value_file.dat"),
                 author_ISBN_list("author_ISBN_", "index_file.dat", "value_file.dat"),
                 keywords_ISBN_list("keywords_ISBN_", "index_file.dat", "value_file.dat"),
                 price_ISBN_list("price_ISBN_", "index_file.dat", "value_file.dat"),
                 ISBN_Book_list("ISBN_Book_", "index_file.dat", "value_file.dat") {
    select_book = Book();
  }
  void clear_select_book() {
    select_book = Book();
  }
  void show(const string& order) {
    set<Book> show_books;
    string showtype = "";
    for (int i = 1; i < order.size(); i++) {
      if (order[i] == '=') break;
      showtype += order[i];
    }
    string content = "";
    int i = 1;
    while (order[i] != '=' && i < order.size()) {
      i++;
    }
    i++;
    while (i < order.size()) {
      content += order[i];
      i++;
    }
    if (showtype == "ISBN") {
      show_books = ISBN_Book_list.FindKey(content);
    }
    if (showtype == "name") {
      auto ISBN_set = name_ISBN_list.FindKey(content);
      for (auto ISBN : ISBN_set) {
        Book book;
        ISBN_Book_list.FindKey(ISBN);
        show_books.insert(book);
      }
    }
    if (showtype == "author") {
      auto ISBN_set = author_ISBN_list.FindKey(content);
      for (auto ISBN : ISBN_set) {
        Book book;
        ISBN_Book_list.FindKey(ISBN);
        show_books.insert(book);
      }
    }
    if (showtype == "keyword") {
      auto ISBN_set = keywords_ISBN_list.FindKey(content);
      for (auto ISBN : ISBN_set) {
        Book book;
        ISBN_Book_list.FindKey(ISBN);
        show_books.insert(book);
      }
    }
    int num = show_books.size();
    if (num == 0) {
      cout << "\n";
      return;
    }
    for (auto it = show_books.begin(); it != show_books.end(); it++) {
      it->outputBook(*it);
    }
  }
  void buy(const string& ISBN, const string& quantity) {
    if (!ISBN_Book_list.if_find(ISBN)) {
      cout << "Invalid\n";
      return;
    }
    Book book = ISBN_Book_list.FindSingle(ISBN);
    int num = std::stoi(quantity);
    if (num > book.quantity || num < 0) {
      cout << "Invalid\n";
      return;
    }
    book.quantity -= num;
    //重新插入更改后的图书
    ISBN_Book_list.DeleteKeyValue(ISBN, ISBN_Book_list.FindSingle(ISBN));
    ISBN_Book_list.Insert(ISBN, book);
  }  
  void select(const string& ISBN) {
    if (!ISBN_Book_list.if_find(ISBN)) {
      cout << "Invalid\n";
      return;
    }
    select_book = ISBN_Book_list.FindSingle(ISBN);
  }

  //更改图书的单个信息
  void modify_single(const string& order) {
    string modifytype = "";
    for (int i = 1; i < order.size(); i++) {
      if (order[i] == '=') break;
      modifytype += order[i];
    }
    string content = "";
    int i = 1;
    while (order[i] != '=' && i < order.size()) {
      i++;
    }
    i++;
    while (i < order.size()) {
      content += order[i];
      i++;
    }
    if (modifytype == "ISBN") {
      if (!ISBN_Book_list.if_find(content)) {
        cout << "Invalid\n";
        return;
      }
      Book new_book = select_book;
      if (content == select_book.ISBN) {
        cout << "Invalid\n";
        return;
      }
      new_book.ISBN = content;
      ISBN_Book_list.DeleteKeyValue(select_book.ISBN, select_book);
      ISBN_Book_list.Insert(content, new_book);
      select_book = new_book;
    }
    if (modifytype == "name") {
      Book new_book = select_book;
      new_book.name = content;
      ISBN_Book_list.DeleteKeyValue(select_book.ISBN, select_book);
      ISBN_Book_list.Insert(select_book.ISBN, new_book);
      select_book = new_book;
    }
    if (modifytype == "author") {
      Book new_book = select_book;
      new_book.author = content;
      ISBN_Book_list.DeleteKeyValue(select_book.ISBN, select_book);
      ISBN_Book_list.Insert(select_book.ISBN, new_book);
      select_book = new_book;
    }
    if (modifytype == "keyword") {
      vector<string> keywords;
      string temp = "";
      for (int i = 0; i < content.size(); i++) {
        if (content[i] == '|') {
          if (find(keywords.begin(), keywords.end(), temp) != keywords.end()) {
            cout << "Invalid\n";
            return;
          }
          keywords.push_back(temp);
          temp = "";
        } else {
          temp += content[i];
        }
      }
      Book new_book = select_book;
      new_book.keywords = keywords;
      ISBN_Book_list.DeleteKeyValue(select_book.ISBN, select_book);
      ISBN_Book_list.Insert(select_book.ISBN, new_book);
      select_book = new_book;
    }
    if (modifytype == "price") {
      double price = std::stod(content);
      Book new_book = select_book;
      new_book.price = price;
      ISBN_Book_list.DeleteKeyValue(select_book.ISBN, select_book);
      ISBN_Book_list.Insert(select_book.ISBN, new_book);
    }
  }

  void modify(const vector<string>& orders) {
    if (select_book.ISBN == "") {
      cout << "Invalid\n";
      return;
    }
    std::set<string> modifytypes;
    for (int i = 1; i < orders.size(); i++) {
      string order = orders[i];
      string modifytype = "";
      for (int i = 1; i < order.size(); i++) {
        if (order[i] == '=') break;
        modifytype += order[i];
      }
      if (modifytypes.find(modifytype) != modifytypes.end()) {
        cout << "Invalid\n";
        return;
      }
      modifytypes.insert(modifytype);
    }
    for (int i = 1; i < orders.size(); i++) {
      modify_single(orders[i]);
    }
  }

  void import(const string& quantity, const string& TotalCost) {
    if (select_book.ISBN == "") {
      cout << "Invalid\n";
      return;
    }
    if (std::stoi(quantity) < 0) {
      cout << "Invalid\n";
      return;
    }
    if (std::stod(TotalCost) < 0) {
      cout << "Invalid\n";
      return;
    }
    int num = std::stoi(quantity);
    double p = std::stod(TotalCost);
    select_book.quantity += num;
    ISBN_Book_list.DeleteKeyValue(select_book.ISBN, select_book);
    ISBN_Book_list.Insert(select_book.ISBN, select_book);
  }
};
#endif