#ifndef HASHCELL_H_
#define HASHCELL_H_

#include "utils/LogClass/LogClass.h"

#include <iostream>
using std::cout; using std::cin; using std::endl;

#include <string>
using std::string;

#include <stdexcept>


/****** _HashCell_LogMessage : Class Declaration and Definition ******/

class _HashCell_LogMessage {
public:
  LogMessage    data;
  LogRecord    *entry = nullptr;
  LogRecord    *end   = nullptr;
  _HashCell_LogMessage *next = nullptr;
  size_t        count = 0;

public:

  inline _HashCell_LogMessage() { return; }

  ~_HashCell_LogMessage() {
    if (entry != nullptr) {
      string error_str = ("_HashCell_LogMessage::~_HashCell_LogMessage() "
      "Dirty previous deletion!\n\t");
      error_str.append("LogRecord ");
      if (entry != nullptr) {
        error_str.append("NOT CLEAN!\n\t");
        cout << entry->get_message() << endl;
      }
      else { error_str.append("OK!\n\t"); }
      throw std::runtime_error(error_str);    // NOTE: *CANNOT* be caught!
      //       Process got terminated!
    }
    return;
  }

  inline _HashCell_LogMessage &reset_cell(const LogMessage &msgobj) {
    if (entry != nullptr || next != nullptr) {
      throw std::runtime_error("_HashCell_LogMessage::reset_cell() "
          "Dirty previous deletion! Will break chain relations!");
    }
    data = msgobj;  // value copy
    return *this;
  }

  inline _HashCell_LogMessage &join_rec_to_end(LogRecord *prec) {
    // Set message axis successor
    if (this->entry == nullptr) {   // first ever
      this->entry = prec;
    } else {    // not first
      this->end->msg_suc = prec;
    }
    this->end = prec;
    this->count += 1;
    return *this;
  }

  inline _HashCell_LogMessage &inc_occur() { this->count += 1; return *this; }

  inline bool strict_equal(const LogMessage &msgobj) {
    return this->data == msgobj;
  }

  inline bool value_equal(const LogMessage &msgobj) {
    return this->data.get_message() == msgobj.get_message();
  }

  inline bool value_equal(const string &msg_only) {
    return this->data.get_message() == msg_only;
  }

  // NOTE: Default comparison set to `value_equal()`!
  inline bool operator==(const string &msg_only) {
    return this->value_equal(msg_only);
  }

  inline _HashCell_LogMessage &set_entry(LogRecord *rec) {
    if (rec->get_message() != data.get_message()) {
      throw std::runtime_error("_HashCell_LogMessage::set_entry() "
          "Setting `LogRecord` entry to an irrelevant pos!");
    }
    entry = rec;
    return *this;
  }

  inline _HashCell_LogMessage &set_next(_HashCell_LogMessage *nxt_cell) {
    next = nxt_cell;
    return *this;
  }

  inline bool occupied() { return this->data.notempty(); }

  inline string get_message() { return this->data.get_message(); }
  inline string _repr() { return this->data.get_message(); }

};


/****** _HashCell_string : Class Declaration and Definitioin ******/

class _HashCell_string {
public:
  string      data;
  LogRecord  *entry = nullptr;
  LogRecord  *end   = nullptr;
  _HashCell_string *next = nullptr;

public:

  inline _HashCell_string() { return; }

  ~_HashCell_string() {
    if (entry != nullptr) {
      string error_str = ("_HashCell_string::~_HashCell_string() "
      "Dirty previous deletion!\n\t");
      error_str.append("LogRecord ");
      if (entry != nullptr) {
        error_str.append("NOT CLEAN!\n\t");
        cout << entry->get_message() << endl;
      }
      else { error_str.append("OK!\n\t"); }
      error_str.append("_HashCell_string ");
      if (next != nullptr) { error_str.append("NOT CLEAN\n\t"); }
      else { error_str.append("OK!"); }
      throw std::runtime_error(error_str);    // NOTE: *CANNOT* be caught!
                                              //       Process got terminated!
    }
    return;
  }

  inline _HashCell_string &join_rec_to_end(LogRecord *prec) {
    // Set sender axis successor
    if (this->entry == nullptr) {   // first record ever
      this->entry = prec;
      this->end = prec;
    }
    this->end->sender_suc = prec;
    this->end = prec;
    return *this;
  }

  inline _HashCell_string &reset_cell(const string &str) {
    if (entry != nullptr || next != nullptr) {
      throw std::runtime_error("_HashCell_string::reset_cell() "
          "Dirty previous deletion! Will break chain relations!");
    }
    data = str;
    return *this;
  }

  inline bool operator==(const string &o) { return this->data == o; }
  inline bool operator!=(const string &o) { return this->data != o; }

  inline _HashCell_string &set_entry(LogRecord *rec) {
    if (rec->get_sender() != data) {
      throw std::runtime_error("_HashCell_string::set_entry() "
          "Setting `LogRecord` entry to an irrelevant pos!");
    }
    entry = rec;
    return *this;
  }

  inline _HashCell_string &set_end(LogRecord *rec) {
    if (rec->get_sender() != data) {
      throw std::runtime_error("_HashCell_string::set_entry() "
          "Setting `LogRecord` end to an irrelevant pos!");
    }
    end = rec;
    return *this;
  }

  inline _HashCell_string &set_next(_HashCell_string *nxt_cell) {
    this->next = nxt_cell;
    return *this;
  }

  inline bool occupied() { return !this->data.empty(); }

  inline string _repr() { return this->data; }

};



#endif
