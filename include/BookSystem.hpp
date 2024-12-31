#ifndef BOOKSYSTEM_HPP
#define BOOKSYSTEM_HPP

#include "BlockList.hpp"
#include "LogSystem.hpp"
#include "Vector.hpp"
#include <vector>

using std::string;
using std::vector;
using std::set;
using std::cout;

struct String {
  char str_ISBN[21];
  String& operator = (const String& other) {
    if (this != &other) {
      strcpy(str_ISBN, other.str_ISBN);
    }
    return *this;
  }
  String() {
    memset(str_ISBN, 0, sizeof(str_ISBN));
  };
  String(string ISBN) {
    strcpy(this->str_ISBN, ISBN.c_str());
  };
};

inline bool operator < (const String& str1, const String& str2) {
  return strcmp(str1.str_ISBN, str2.str_ISBN) < 0;
};

inline bool operator > (const String& str1, const String& str2) {
  return strcmp(str1.str_ISBN, str2.str_ISBN) > 0;
};

inline bool operator == (const String& str1, const String& str2) {
  return strcmp(str1.str_ISBN, str2.str_ISBN) == 0;
};

struct Book {
  char ISBN[21];
  char name[61];
  char author[61];
  char keywords[61];
  double price;
  int quantity;
  Book& operator = (const Book& other) {
    if (this != &other) {
      strcpy(ISBN, other.ISBN);
      strcpy(name, other.name);
      strcpy(author, other.author);
      strcpy(keywords, other.keywords);
      price = other.price;
      quantity = other.quantity;
    }
    return *this;
  }

  Book() {
    string blank = "";
    strcpy(this->ISBN, blank.c_str());
    strcpy(this->name, blank.c_str());
    strcpy(this->author, blank.c_str());
    strcpy(this->keywords, blank.c_str());
    price = 0;
    quantity = 0;
  };

  Book(string ISBN, string name, string author, string keywords, double price, int quantity)
  : price(price), quantity(quantity) {
    strcpy(this->ISBN, ISBN.c_str());
    strcpy(this->name, name.c_str());
    strcpy(this->author, author.c_str());
    strcpy(this->keywords, keywords.c_str());
  };
  Book(string ISBN) {
    string blank = "";
    strcpy(this->ISBN, ISBN.c_str());
    strcpy(this->name, blank.c_str());
    strcpy(this->author, blank.c_str());
    strcpy(this->keywords, blank.c_str());
    price = 0;
    quantity = 0;
  };

};
//按照格式输出图书信息
inline void outputBook(const Book& output) {
  cout << output.ISBN << '\t' << output.name << '\t' << output.author << '\t' << output.keywords << '\t' 
  << fixed << setprecision(2) << output.price << '\t' << output.quantity << '\n';
}

inline bool operator < (const Book& book1, const Book& book2) {
  return strcmp(book1.ISBN, book2.ISBN) < 0;  
};

inline bool operator > (const Book& book1, const Book& book2) {
  return strcmp(book1.ISBN, book2.ISBN) > 0;
};

inline bool operator == (const Book& book1, const Book& book2) {
  return strcmp(book1.ISBN, book2.ISBN) == 0;
};

class BookSystem {
private:
  //从书名到ISBN的MAP
  Blocklist<String> name_ISBN_list;
  //从作者到ISBN的MAP
  Blocklist<String> author_ISBN_list;
  //从关键词到ISBN的MAP
  Blocklist<String> keywords_ISBN_list;
  //从单价到ISBN的MAP
  Blocklist<String> price_ISBN_list;
  //从ISBN到Book的MAP
  Blocklist<Book> ISBN_Book_list;
  //当前选中的书
  string select_book_ISBN;
  //登录栈对应的选中的书构成的栈
  Vector<String> select_books;
public:
  BookSystem() : name_ISBN_list("Bookstore_name_ISBN_", "index_file.dat", "value_file.dat"),
                 author_ISBN_list("Bookstore_author_ISBN_", "index_file.dat", "value_file.dat"),
                 keywords_ISBN_list("Bookstore_keywords_ISBN_", "index_file.dat", "value_file.dat"),
                 price_ISBN_list("Bookstore_price_ISBN_", "index_file.dat", "value_file.dat"),
                 ISBN_Book_list("Bookstore_ISBN_Book_", "index_file.dat", "value_file.dat"),
                 select_books("Bookstore_select_books.dat")
                 {
    select_book_ISBN = "";
  }
  ~BookSystem() {
    ofstream tempfile("Bookstore_select_books.dat", std::ios::trunc);
    tempfile.close();
    clear_select_book();
  }
  void clear_select_book() {
    select_book_ISBN = "";
  }
  void show(const string& order) {
    if (select_books.size() == 0) {
      std::cout << "Invalid\n";
      return;
    }
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
      if (order[i] != '\"') {
        content += order[i];
      }
      i++;
    }
    if (showtype == "ISBN") {
      if (!ISBN_Book_list.if_find(content)) {
        cout << '\n';
        return;
      }
      auto ans = ISBN_Book_list.FindSingle(content);
      if (ans.ISBN[0] == '\0') {
        cout << '\n';
        return;
      }
      outputBook(ans);
      return;
    }
    if (showtype == "name") {
      auto ISBN_set = name_ISBN_list.FindKey(content);
      for (auto ISBN : ISBN_set) {
        string temp = ISBN.str_ISBN;
        auto result = ISBN_Book_list.FindKey(temp);
        for (auto it = result.begin(); it != result.end(); it++) show_books.insert(*it);
      }
    }
    if (showtype == "author") {
      auto ISBN_set = author_ISBN_list.FindKey(content);
      for (auto ISBN : ISBN_set) {
        string temp = ISBN.str_ISBN;
        auto result = ISBN_Book_list.FindKey(temp);
        for (auto it = result.begin(); it != result.end(); it++) show_books.insert(*it);
      }
    }
    if (showtype == "keyword") {
      auto ISBN_set = keywords_ISBN_list.FindKey(content);
      for (auto ISBN : ISBN_set) {
        string temp = ISBN.str_ISBN;
        auto qualified_books = ISBN_Book_list.FindKey(temp);
        for (auto book : qualified_books) {
          show_books.insert(book);
        }
      }
    }
    int num = show_books.size();
    if (num == 0) {
      cout << "\n";
      return;
    }
    for (auto it = show_books.begin(); it != show_books.end(); it++) {
      outputBook(*it);
    }
  }
  void showall() {
    if (select_books.size() == 0) {
      std::cout << "Invalid\n";
      return;
    }
    int index_pos = 0;
    int index_number = 0;
    ISBN_Book_list.index_file.get_info(index_number, 1); 
    ISBN_Book_list.index_file.get_info(index_pos, 2);    
    for (int i = 0; i < index_number; ++i) {
      Index index;
      ISBN_Book_list.index_file.read(index, index_pos); 
      int block_pos = index.offset;
      Block<Book> block;
      ISBN_Book_list.block_file.read(block, block_pos); 
      for (int j = 0; j < block.KeyValue_num; ++j) {
        const Key_Value<Book>& kv = block.KeyValues[j];
        if constexpr (std::is_same<Book, Book>::value) {
          outputBook(kv.value);
        }
      }
      index_pos = index.next_offset;
    }
  }
  void buy(const string& ISBN, const string& quantity) {
    if (!ISBN_Book_list.if_find(ISBN)) {
      cout << "Invalid\n";
      return;
    }
    Book book = ISBN_Book_list.FindSingle(ISBN);
    int num = std::stoi(quantity);
    if (num > book.quantity || num <= 0) {
      cout << "Invalid\n";
      return;
    }
    book.quantity -= num;
    //重新插入更改后的图书
    ISBN_Book_list.DeleteKeyValue(ISBN, ISBN_Book_list.FindSingle(ISBN));
    ISBN_Book_list.Insert(ISBN, book);
    std::cout << fixed << setprecision(2) << book.price * std::stod(quantity) << '\n';
  }  
  void select(const string& ISBN1) {
    if (!ISBN_Book_list.if_find(ISBN1)) {
      Book new_book(ISBN1);
      ISBN_Book_list.Insert(ISBN1, new_book);
    }
    select_book_ISBN = ISBN1;
    String temp(ISBN1);
    select_books[select_books.size() - 1] = temp;
  }

  //修改图书信息
  void modify(const vector<string>& orders) {
    if (select_books.size() == 0) {
      std::cout << "Invalid\n";
      return;
    }
    Book select_book = ISBN_Book_list.FindSingle(select_book_ISBN);
    if (select_book.ISBN[0] == '\0') {
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
    Book new_book = select_book;
    for (int j = 1; j < orders.size(); j++) {
      string modifytype = "";
      string order = orders[j];
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
        if (strcmp(new_book.ISBN, content.c_str()) == 0) {
          cout << "Invalid\n";
          return;
        } 
        if (ISBN_Book_list.if_find(content)) {
          cout << "Invalid\n";
          return;
        }
        string name1 = select_book.name;
        string author1 = select_book.author;
        string keywords1 = select_book.keywords;      
        String temp1(select_book.ISBN);
        if (select_book.name[0] != '\0') name_ISBN_list.DeleteKeyValue(name1, temp1);
        if (select_book.author[0] != '\0') author_ISBN_list.DeleteKeyValue(author1, temp1);
        if (select_book.keywords[0] != '\0') keywords_ISBN_list.DeleteKeyValue(keywords1, temp1);
        if (std::to_string(select_book.price).size() != 0) price_ISBN_list.DeleteKeyValue(std::to_string(select_book.price), temp1);
        string key = select_book.ISBN;
        ISBN_Book_list.DeleteKeyValue(key, select_book);
        strcpy(new_book.ISBN, content.c_str());
        key = content;
        ISBN_Book_list.Insert(key, new_book);
        select_book = new_book;
        string ISBN1 = new_book.ISBN;
        name1 = new_book.name;
        author1 = new_book.author;
        keywords1 = new_book.keywords;
        if (select_book.name[0] != '\0') name_ISBN_list.Insert(name1, ISBN1);
        if (select_book.author[0] != '\0') author_ISBN_list.Insert(author1, ISBN1);
        if (select_book.keywords[0] != '\0') keywords_ISBN_list.Insert(keywords1, ISBN1);
        if (std::to_string(select_book.price).size() != 0) price_ISBN_list.Insert(std::to_string(new_book.price), ISBN1);
        select_book_ISBN = content;
      }
      if (modifytype == "name") {
        content = content.substr(1, content.size() - 2);
        strcpy(new_book.name, content.c_str());
        String temp1(select_book.ISBN);
        String temp2(new_book.ISBN);
        name_ISBN_list.DeleteKeyValue(select_book.name, temp1);
        name_ISBN_list.Insert(new_book.name, temp1);
      }
      if (modifytype == "author") {
        content = content.substr(1, content.size() - 2);
        strcpy(new_book.author, content.c_str());
        String temp1(select_book.ISBN);
        String temp2(new_book.ISBN);
        author_ISBN_list.DeleteKeyValue(select_book.author, temp1);
        author_ISBN_list.Insert(new_book.author, temp1);
      }
      if (modifytype == "keyword") {
        content = content.substr(1, content.size() - 2);
        strcpy(new_book.keywords, content.c_str());
        String temp1(select_book.ISBN);
        String temp2(new_book.ISBN);
        string origin_keywords = select_book.keywords;
        string now_keywords = new_book.keywords;
        string temp = "";
        for (int i = 0; i < origin_keywords.size(); i++) {
          if (origin_keywords[i] == '|') {
            keywords_ISBN_list.DeleteKeyValue(temp, temp1);
            temp = "";
          } else {
            temp += origin_keywords[i];
          }
        }
        if (temp.size() != 0) keywords_ISBN_list.DeleteKeyValue(temp, temp1);
        temp = "";
        for (int i = 0; i < now_keywords.size(); i++) {
          if (now_keywords[i] == '|') {
            keywords_ISBN_list.Insert(temp, temp1);
            temp = "";
          } else {
            temp += now_keywords[i];
          }
        }
        if (temp.size() != 0) keywords_ISBN_list.Insert(temp, temp1);
      }
      if (modifytype == "price") {
        double price = std::stod(content);
        new_book.price = price;
        String temp1(select_book.ISBN);
        price_ISBN_list.DeleteKeyValue(std::to_string(select_book.price), temp1);
        price_ISBN_list.Insert(std::to_string(new_book.price), temp1);
      }
    }
    ISBN_Book_list.DeleteKeyValue(select_book.ISBN, select_book);
    ISBN_Book_list.Insert(new_book.ISBN, new_book);
  }

  void import(const string& quantity, const string& TotalCost) {
    if (select_books.size() == 0) {
      std::cout << "Invalid\n";
      return;
    }
    Book select_book = ISBN_Book_list.FindSingle(select_book_ISBN);
    if (select_book.ISBN[0] == '\0') {
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
  inline auto get_select_book() {
    return ISBN_Book_list.FindSingle(select_book_ISBN);
  }
  inline auto get_book(const string& ISBN1) {
    return ISBN_Book_list.FindSingle(ISBN1);
  }
  inline void set_select_book(const string& target) {    
    select_book_ISBN = target;
  }  
  inline bool check_select_book() {
    Book select_book = ISBN_Book_list.FindSingle(select_book_ISBN);
    if (select_book.ISBN[0] == '\0' || !ISBN_Book_list.if_find(select_book.ISBN)) return false;
    return true;
  }
  inline int get_quantity(const string& target) {
    return ISBN_Book_list.FindSingle(target).quantity;
  }
  inline void select_books_add() {
    String temp;
    select_books.push_back(temp);
  }
  inline void select_books_pop() {
    if (select_books.size() == 0) return;
    select_books.pop_back();
  }
  inline void set_select_book() {
    string temp = select_books[select_books.size() - 1].str_ISBN;
    select_book_ISBN = temp;
  }
};
#endif