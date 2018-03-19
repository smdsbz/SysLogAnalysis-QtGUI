#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <iostream>
using std::cout; using std::cin; using std::endl;

#include <string>
using std::string;

#include <stdexcept>

#include "utils/LogClass/LogClass.h"
#include "utils/HashTable/HashCell.h"


/****** HashTable : Class Declaration and Definitioin ******/

class MessageTable {
public:
  _HashCell_LogMessage *table = nullptr;
  size_t    space = 5000;
  HashFunc  hash  = HashFunc(30, 5000, 0);

  LogRecord *global_begin = nullptr;
  LogRecord *global_end   = nullptr;

public:

  inline MessageTable(size_t table_size = 5000,
                      size_t hash_range = 30) {
    if (table_size == 0) {
      throw std::invalid_argument("MessageTable::MessageTable(...)::table_size "
          "Provide an appropriate size to init table!");
    }
    this->space = table_size;
    this->table = new _HashCell_LogMessage[this->space];
    this->hash  = HashFunc(hash_range, this->space, 0);
    this->global_begin = nullptr;
    this->global_end   = nullptr;
    return;
  }

  ~MessageTable() {
    // free cells in chain
    // for each HashCell
    for (size_t cell_cur = 0; cell_cur != this->space; ++cell_cur) {
      auto *pcell = table + cell_cur;
      // for each valid cell in isotopes
      // free first in isotopes, until there's no more isotope
      while (pcell->next != nullptr) {
        auto *to_free = pcell->next;
        pcell->next = to_free->next;
        // to_free->next = nullptr;
        delete to_free;
      }
    }
    // free chain heads in `this->table`
    delete [] this->table;
    return;
  }

  /* 函数名称：insert
   * 函数参数：要插入的 msg（LogMessage）
   * 函数功能：将 msg 添加到查找表中；若已存在，则返回相应位置
   * 返回值：  插入/查找成功返回插入位置的引用，否则抛出对应错误
   * 抛出异常：std::bad_alloc
   *               内存申请失败时抛出
   */
  _HashCell_LogMessage &insert(const LogMessage &msgobj) {
    // get index
    size_t idx = this->hash(msgobj.get_message());
    if (idx >= this->space) {
      throw std::range_error("MessageTable::insert() HashFunc miscalculated!");
    }
    // find place to insert
    auto &msg_cell = this->table[idx];
    if (msg_cell.occupied()) {
      if (msg_cell.strict_equal(msgobj)) {  // check if is header
        return msg_cell;    // HACK: It doesn't matter, just make sure
                            //       there *IS* one
      }
      auto pcell = &msg_cell;
      while (pcell->next != nullptr) {
        if (pcell->next->strict_equal(msgobj)) {  // check if is this isotope
          return *(pcell->next);
        }
        // not this one ==> next isotope
       pcell = pcell->next;
      } // end of while ==> not in isotopes
      try {
        pcell->next = new _HashCell_LogMessage();
      } catch (const std::bad_alloc &e) {
        cout << "Low Memory! Space allocation failed while inserting:\n"
             << msgobj.get_message() << endl;
        throw e;
      }
      // new `_HashCell_LogMessage` alloced at `pcell->next`
      pcell->next->reset_cell(msgobj);
      return *(pcell->next);
    } else {  // msg_cell.occupied() == false
      msg_cell.reset_cell(msgobj);
      return msg_cell;
    }
    throw std::logic_error("MessageTable::insert() Jumped out of if-else!");
  }

  /* 函数名称：join_rec_to_end
   * 函数参数：要注册的 `LogRecord` 对象的地址
   * 函数功能：注册 `LogRecord` 的时间继承关系
   * 返回值：  操作成功返回自身引用
   */
  inline MessageTable &join_rec_to_end(LogRecord *prec) {
    // Set time axis successor
    if (this->global_begin == nullptr) {    // first rec ever
      this->global_begin = prec;
    } else {    // not first rec - this->global_end != nullptr
      this->global_end->time_suc = prec;
    }
    this->global_end = prec;    // reset end
    return *this;
  }

  _HashCell_LogMessage &operator[](const LogMessage &msgobj) {
    // NOTE: `msgobj` should be guaranteed to be in `Storage` already!
    auto pcell = this->table + hash( msgobj.get_message() );
    if (!pcell->occupied()) {
      throw std::overflow_error(string("MessageTable::operator[LogMessage] "
          "No match found for: ") + msgobj.get_message());
    }
    while ( pcell && !pcell->strict_equal(msgobj) ) {
      pcell = pcell->next;
    }
    if (pcell == nullptr) {
      throw std::overflow_error(string("MessageTable::operator[LogMessage] "
          "No match found for: ") + msgobj.get_message());
    }
    // pcell != nullptr ==> match found!
    return *pcell;
  }

  // TODO: Fails when containing cr/lf
  // TODO: Figure out why this works in Qt-GUI
  // _HashCell_LogMessage &operator[](const LogMessage *pmsg) {
  //   auto pcell = this->table + hash(pmsg->get_message());
  //   if (!pcell->occupied()) {
  //     throw std::overflow_error(string("MessageTable::operator[LogMessage *] "
  //         "No match found for: ") + pmsg->get_message());
  //   }
  //   while (pcell && !(&pcell->data == pmsg)) {
  //     pcell = pcell->next;
  //   }
  //   if (pcell == nullptr) {
  //     throw std::overflow_error(string("MessageTable::operator[LogMessage *] "
  //         "No match found for: ") + pmsg->get_message());
  //   }
  //   return *pcell;
  // }

  /*
  HACK: Current work-around for the problem stated above
  _HashCell_LogMessage &operator[](const LogMessage *pmsg) {
    auto pcell = this->table;
    for (size_t idx = 0, range = this->space; idx != range; ++idx) {
      pcell = this->table + idx;
      while (pcell->occupied() && pcell && (&pcell->data != pmsg) ) {
        pcell = pcell->next;
      }
    }
    if (pcell == nullptr) {
      throw std::overflow_error(string("MessageTable::operator[LogMessage *] "
          "No match found for: ") + pmsg->get_message());
    }
    return *pcell;
  }
  */

  _HashCell_LogMessage &operator[](const string &msg) {
    auto pcell = this->table + hash(msg);
    if (!pcell->occupied()) {
      throw std::overflow_error(string("MessageTable::operator[string] ")
          + "No match found for: " + msg);
    }
    while ( pcell && !pcell->value_equal(msg) ) {
      pcell = pcell->next;
    }
    if (pcell == nullptr) {
      throw std::overflow_error(string("MessageTable::operator[string] ")
          + "No match found for: " + msg);
    }
    // pcell != nullptr ==> match found!
    return *pcell;
  }

  _HashCell_LogMessage &operator[](const size_t idx) {
    if (idx >= this->space) {
      throw std::range_error(string("MessageTable::operator[index] ")
          + "index(" + std::to_string(idx) + ") is out of range!");
    }
    return this->table[idx];
  }

  inline void set_begin(LogRecord *beg) { this->global_begin = beg; }
  inline void set_end(LogRecord *end) { this->global_end = end; }

};



class SenderTable {
public:
  _HashCell_string     *table = nullptr;
  size_t    space = 200;
  HashFunc  hash = HashFunc(10, 200, 0);

public:

  inline SenderTable(size_t table_size = 200) {
    if (table_size == 0) {
      throw std::invalid_argument("SenderTable::SenderTable(...)::table_size "
          "Provide an appropriate size to init table!");
    }
    this->space = table_size;
    this->table = new _HashCell_string[this->space];
    this->hash  = HashFunc(10, this->space, 0);
    return;
  }

  inline ~SenderTable() { delete [] this->table; }

  /* 函数名称：link
   * 函数参数：已存在的 LogRecord 的引用
   * 函数功能：将该 LogRecord 添加至 SenderTable 的关系网中
   * 返回值：  对应该 LogRecord 的发送者在 SenderTable 中的存储位置
   */
  _HashCell_string &link(LogRecord &rec) {
    // make sure the message field is not empty
    if (rec.message == nullptr) {
      throw std::runtime_error("SenderTable::link() Invalid `massage` field!");
    }
    // get copy of sender_name
    string sender_name = rec.get_sender();
    // get insert position
    size_t idx = this->hash(sender_name);
    if (idx >= this->space) {
      throw std::range_error("SenderTable::insert() HashFunc miscalculated!");
    }
    // do linking
    auto &sndr_cell = this->table[idx];
    if (sndr_cell.occupied()) {
      if (sndr_cell == sender_name) {   // search chain head
        return sndr_cell;
      } // not chain head, continue searching
      auto pcell = &sndr_cell;
      while (pcell->next != nullptr) {
        if (*(pcell->next) == sender_name) {    // sender found
          // no need for new cell
          return *(pcell->next);
        }
        // sender not found *YET*, venture forth
        pcell = pcell->next;
      } // end of while
      // still not found? need new cell
      try {
        pcell->next = new _HashCell_string();
      } catch (const std::bad_alloc &e) {
        cout << "Low Memory! Space allocation failed while linking to "
             << sender_name << endl;
        throw e;
      }
      pcell->next->reset_cell(sender_name);
      return *(pcell->next);
    } else {    // not occupied
      sndr_cell.reset_cell(sender_name);
      return sndr_cell;
    }
    throw std::logic_error("SenderTable::link() Jumped out of if-else!");
  }

  _HashCell_string &operator[](const string &sender) {
    auto pcell = this->table + hash(sender);
    if (!pcell->occupied()) {
      throw std::overflow_error(string("SenderTable::operator[sender] ")
          + "No match found for: " + sender);
    }
    while (pcell && (pcell->data != sender)) { pcell = pcell->next; }
    if (pcell == nullptr) {
      throw std::overflow_error(string("SenderTable::operator[sender] ")
          + "No match found for: " + sender);
    }
    return *pcell;
  }

};

#endif

