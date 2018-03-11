#ifndef STORAGEGRAPH_H_
#define STORAGEGRAPH_H_

#include <iostream>
using std::cout; using std::cin; using std::endl;

#include <string>
using std::string; using std::stoul;

#include <vector>
using std::vector;

#include <fstream>
using std::fstream; using std::getline;

#include <regex>
using std::regex; using std::regex_match;
using std::smatch; using std::ssub_match;

#include <stdexcept>

#include <QDebug>
#include <QFile>
#include <QProgressDialog>

#include "utils/LogClass/LogClass.h"
#include "utils/HashTable/HashTable.h"
// #include "utils/UIGadgets/UIGadgets.h"

/****** StorageGraph : Class Declaration and Definitioin ******/

class Storage {
public:
  MessageTable     *messages = nullptr;
  SenderTable      *senders  = nullptr;

public:

  inline Storage(size_t msgtable_size = 5000,
                 size_t sdrtable_size = 200) {
    messages = new MessageTable(msgtable_size);
    senders  = new SenderTable(sdrtable_size);
    return;
  }

  ~Storage() {
    // NOTE: Only have to clean `LogRecord`s, `Cell`s will be handled by
    //       `Table`s
    LogRecord *beg = this->messages->global_begin;
    LogRecord *to_free = nullptr;
    while (beg != nullptr) {
      to_free = beg;
      beg = beg->time_suc;
      delete to_free;
    }
    return;
  }

  LogRecord &_time_sequence_promised_add(const string &log) {
    // prepare `LogMessage` content
    auto msg = LogMessage();
    try {
      msg = LogMessage(log, /*message_only=*/false);
    } catch (const std::runtime_error &e) {
      // log passed in can't match regex
      throw e;
    }
    // prepare `LogRecord` content
    LogRecord *prec = nullptr;
    try {
      prec = new LogRecord(log, /*whole_log=*/true);
    } catch (const std::bad_alloc &e) {
      cout << "Low Memory! Space allocation failed!" << endl;
      throw e;
    }
    try {
      // NOTE: The following two lines may fail due to cell existing,
      //       but it actually won't make any difference! 'Cause it ensures you
      //       that the cells exist anyway, and it even finds the cell for
      //       you (returned as reference to cell)!
      //       Just pretend that every `insert()` or `link()` is the first
      //       call to that operand.
      auto &msg_cell = this->messages->insert(msg);
      // associate `LogRecord` and `LogMessage`
      prec->set_message(msg_cell.data);
      auto &sndr_cell = this->senders->link(*prec);
      // add `LogRecord` to `Storage`
      // NOTE: The following tow lines requires that `log` passed hasn't
      //       appeared before.
      this->messages->join_rec_to_end(prec);    // set `time_suc`
      msg_cell.join_rec_to_end(prec);           // set `msg_suc`, and inc `count`
      sndr_cell.join_rec_to_end(prec);          // set `sender_suc`
    } catch (const std::bad_alloc &e) { throw e; }
    return *prec;
  }

  size_t is_repeat(const string &line) {
    try {
      // sorry, using apple's ver. of `syslog`
      auto re = regex("^--- last message repeated ([0-9]+) time[s]? ---$");
      smatch matches;
      regex_match(line, matches, re);
      if (matches.size() != 2) {
        return 0;
      } else { return stoul(matches[1].str()); }
    } catch (const std::exception &e) { return 0; }
  }

  LogRecord &_repeat_last_rec() {
    // NOTE: Before calling this function, make sure storage is **NOT** empty!
    LogRecord *prec = nullptr;
    try {
      prec = new LogRecord(*this->messages->global_end);
    } catch (const std::bad_alloc &e) { throw e; }
    auto &msg = *prec->message;
    auto &msg_cell  = this->messages->insert(msg);  // won't insert, only find
    auto &sndr_cell = this->senders->link(*prec);
    this->messages->join_rec_to_end(prec);
    msg_cell.join_rec_to_end(prec);
    sndr_cell.join_rec_to_end(prec);
    return *prec;
  }

  LogRecord &add_after_rec(LogRecord *prec, const string &log_whole) {
    if (prec == nullptr) {
      throw std::invalid_argument("Storage::add_after_rec() Pointer to "
          "LogRecord should not be null!");
    }
    // build LogMessage
    auto msg = LogMessage();
    try {
      msg = LogMessage(log_whole, /*message_only=*/false);
    } catch (const std::runtime_error &e) { throw e; }
    // create LogRecord
    LogRecord *new_rec = nullptr;
    try {
      new_rec = new LogRecord(log_whole, /*whole=*/true);
    } catch (const std::bad_alloc &e) {
      cout << "Low Memory! Space allocation failed!" << endl;
      throw e;
    }
    // LogDate check
    if (new_rec->date < prec->date  // earlier than before?
          || (prec->time_suc        // or later than after?
              && new_rec->date > prec->time_suc->date)) {
      cout << "The log's date info does not fit here!" << endl;
      cout << "Should be in range [ " << prec->date.str() << ", ";
      if (prec->time_suc) { cout << prec->time_suc->date.str(); }
      else { cout << "+inf"; }
      cout << " ]!" << endl;
      delete new_rec;
      throw std::runtime_error("Storage::add_after_rec() New LogRecord's "
          "date info does not fit!");
    }
    // add it to Storage
    try {
      auto &msg_cell = this->messages->insert(msg);
      new_rec->set_message(msg_cell.data);
      auto &sndr_cell = this->senders->link(*new_rec);
      // link `new_rec` into time axis
      new_rec->time_suc = prec->time_suc;
      prec->time_suc = new_rec;
      // linking to end?
      if (this->messages->global_end == prec) {
        this->messages->global_end = new_rec;
      }
      // link `new_rec` in message / sender axis
      LogRecord *msg_pre  = nullptr;
      LogRecord *sndr_pre = nullptr;
      // iter over **ENTIRE** table to locate `msg_pre` and `sndr_pre`
      for (auto cur_rec = this->messages->global_begin;
           cur_rec && cur_rec != new_rec;
           cur_rec = cur_rec->time_suc) {   // NOTE: For records may have
                                            //       *identical* time stamps,
                                            //       stupidly go along time is
                                            //       the best u can get.
        // update `msg_pre`
        if (cur_rec->message == new_rec->message) { msg_pre = cur_rec; }
        // update `sndr_pre`
        if (cur_rec->get_sender() == new_rec->get_sender()) {
          sndr_pre = cur_rec;
        }
      }
      // linking into axes
      if (msg_pre == nullptr) {     // if is first log of this message
        msg_cell.entry = new_rec;
      } else {  // link into message chain
        new_rec->msg_suc = msg_pre->msg_suc;
        msg_pre->msg_suc = new_rec;
      }
      if (sndr_pre == nullptr) {    // if is first log of this sender
        sndr_cell.entry = new_rec;
      } else {  // link into sender chain
        new_rec->sender_suc = sndr_pre->sender_suc;
        sndr_pre->sender_suc = new_rec;
      }
      // if is last / first log of this message / sender
      if (msg_cell.end == msg_pre) {    // HACK: Including `msg_pre == nullptr`.
        msg_cell.end = new_rec;
      }
      if (sndr_cell.end == sndr_pre) {
        sndr_cell.end = new_rec;
      }
      // manually inc `occur` of this message
      msg_cell.inc_occur();
    } catch (const std::bad_alloc &e) {
      // caused by `HashCell` allocation
      throw e;
    }
    return *new_rec;
  }

  Storage &read_from_file(const string &filename) {
    QFile file(filename.c_str());
    // get file size
    auto filesize = file.size();
    qDebug() << "Size of " << filename.c_str() << " is " << filesize;
    // show progress bar as modal window
    QProgressDialog progress(/*labelText=*/"Reading from log dump...",
                             /*closeButtonText=*/"Stop",
                             /*miminum=*/0, /*maximum=*/filesize);
    // cancel "close" button
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress.setCancelButton(nullptr);
    progress.setWindowModality(Qt::WindowModal);
    // read content
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      // NOTE: Some logs may span across multiple lines. So you have to store
      //       the previous `LogRecord`, in case you have to append to it later.
      string line;
      LogRecord *prec = nullptr;
      LogMessage msgbuf;
      size_t char_cnt = 0;
      while ((line = file.readLine().constData(),
              !line.empty())) {
        char_cnt += line.size();
        if (char_cnt % 50 == 0) {
          progress.setValue(char_cnt);
        }
        try {
          msgbuf = LogMessage(line);
        } catch (const std::runtime_error &e) {
          // cannot match regex for LogMessage:
          //   1. false file
          //   2. last log spanned
          //   3. --- last message repeated ... ----
          if (prec == nullptr) {    // case 1: false file
            throw std::runtime_error("Storage::read_from_file() Input file is "
                "not standard log text file!");
          } else {
            // case 3: --- last message repeated ... ---
            size_t repeat_times = is_repeat(line);
            if (repeat_times != 0) {
              for (size_t t = 0; t != repeat_times; ++t) {
                prec = &(this->_repeat_last_rec());
              }
              continue;
            }
            // case 2: last log spanned
            prec->message->append_msg(line);
            continue;
          }
        }
        // legit log! added to msgbuf
        prec = &(this->_time_sequence_promised_add(line));
      } // end of input while-loop
      file.close();
      /* cout << log_cnt << " logs read!" << endl; */
    } else {    // failed to open
      throw std::runtime_error(string("Storage::read_from_file() Failed to ")
          + "open file \"" + filename + "\"!");
    }
    qDebug() << "Finished reading " << filename.c_str();
    return *this;
  }

  Storage &delete_rec(LogRecord *&prec) {
    // find `prec`'s predecessor on all dimentions
    // - find on time axis
    LogRecord *time_pre = nullptr;
    if (this->messages->global_begin == prec) { /* pass */ ; }
    else {  // not first
      time_pre = this->messages->global_begin;
      while (time_pre && time_pre->time_suc != prec) {
        time_pre = time_pre->time_suc;
      }
      if (time_pre == nullptr) {
        throw std::runtime_error("Storage::delete_rec() Time predecessor not "
            "found!");
      }
    }
    // - find on message axis
    // TODO: Fails when message contains cr/lf
    auto &msg_cell = (*this->messages)[*prec->message];
    LogRecord *msg_pre = nullptr;
    if (msg_cell.entry == prec) { /* pass */ ; }
    else {
      msg_pre = msg_cell.entry;
      while (msg_pre && msg_pre->msg_suc != prec) {
        msg_pre = msg_pre->msg_suc;
      }
      if (msg_pre == nullptr) {
        throw std::runtime_error("Storage::delete_rec() Message predecessor "
            "not found!");
      }
    }
    // - find on sender axis
    auto &sndr_cell = (*this->senders)[prec->get_sender()];
    LogRecord *sndr_pre = nullptr;
    if (sndr_cell.entry == prec) { /* pass */ ; }
    else {
      sndr_pre = sndr_cell.entry;
      while (sndr_pre && sndr_pre->sender_suc != prec) {
        sndr_pre = sndr_pre->sender_suc;
      }
      if (sndr_pre == nullptr) {
        throw std::runtime_error("Storage::delete_rec() Sender predecessor "
            "not found!");
      }
    }
    // re-chain
    // `prec` is first / not first
    if (time_pre == nullptr) {
      this->messages->global_begin = prec->time_suc;
    } else {
      time_pre->time_suc = prec->time_suc;
    }
    if (msg_pre == nullptr) {
      msg_cell.entry = prec->msg_suc;
    } else {
      msg_pre->msg_suc = prec->msg_suc;
    }
    if (sndr_pre == nullptr) {
      sndr_cell.entry = prec->sender_suc;
    } else {
      sndr_pre->sender_suc = prec->sender_suc;
    }
    // check if `prec` is end of chain
    if (this->messages->global_end == prec) {
      this->messages->global_end = time_pre;
    }
    if (msg_cell.end == prec) {
      msg_cell.end = msg_pre;
    }
    if (sndr_cell.end == prec) {
      sndr_cell.end = sndr_pre;
    }
    // free `prec`
    delete prec;
    prec = nullptr;
    // decrease occurance
    msg_cell.occur -= 1;
    return *this;
  }

  bool isEmpty() { return this->messages->global_begin == nullptr; }

  vector<_HashCell_LogMessage *> query_on_message(const string &in,
                                                  const bool fuzzy=false) {
    auto retvec = vector<_HashCell_LogMessage *>();
    if (fuzzy == true) {    // fuzzy search: only the sub-string is needed
                            // have to search the entire table!
      for (size_t spi = 0, space = this->messages->space;
           spi != space; ++spi) {
        // do job if not empty slot
        if (this->messages->table[spi].occupied()) {
          // iter through cell chain
          for (auto pcell = this->messages->table + spi;
               pcell != nullptr;
               pcell = pcell->next) {
            // append to retvec if find matched patter
            if (pcell->_repr().find(in) != string::npos
                  && pcell->entry != nullptr) {
              retvec.push_back(pcell);
            }
          } // iter through cell chain
        }   // else - empty `HashCell` slot, do *NOTHING*
      } // for-loop over `MessageTable` headers
    } else {    // no fuzzy
      try {
        auto pcell = &((*(this->messages))[in]);
        for (; pcell != nullptr; pcell = pcell->next) {
          if (pcell->_repr() == in) {
            retvec.push_back(pcell);
          }
        }   // for on cell chain
      } catch (const std::overflow_error &e) {
        return retvec;
      }
    }
    return retvec;
  }

  vector<_HashCell_string *> query_on_sender(const string &in,
                                             const bool fuzzy=false) {
    auto retvec = vector<_HashCell_string *>();
    if (fuzzy == true) {    // fuzzy search: only the sub-string is needed
                            // have to search the entire table!
      for (size_t spi = 0, space = this->senders->space;
           spi != space; ++spi) {
        // do job if not empty slot
        if (this->senders->table[spi].occupied()) {
          // iter through cell chain
          for (auto pcell = this->senders->table + spi;
               pcell != nullptr;
               pcell = pcell->next) {
            // append to retvec if find matched patter
            if (pcell->_repr().find(in) != string::npos
                  && pcell->entry != nullptr) {
              retvec.push_back(pcell);
            }
          } // iter through cell chain
        }   // else - empty `HashCell` slot, do *NOTHING*
      } // for-loop over `MessageTable` headers
    } else {    // no fuzzy
      try {
        auto pcell = &((*(this->senders))[in]);
        for (; pcell != nullptr; pcell = pcell->next) {
          if (pcell->_repr() == in) {
            retvec.push_back(pcell);
          }
        }   // for on cell chain
      } catch (const std::overflow_error &e) {
        return retvec;
      }
    }
    return retvec;
  }

  std::vector<LogRecord *> query(const std::string &content,
                                 const bool fuzzy=false,
                                 const std::string &axis="message") {
    auto ret = std::vector<LogRecord *>();
    if (this->isEmpty()) { return ret; }  // no query on empty storage
    /*** querying on time ***/
    if (axis == "time") {
      auto tgt_date = LogDate(content);   // NOTE: Legality checked before passed in!
      for (auto &each : *this->messages->global_begin) {
        if (each.date == tgt_date) {
          qDebug() << "Adding" << each.get_date().c_str();
          // NOTE: Comparing Dec last year and Jan this year
          ret.push_back(&each);
        }
      } // end of traverse
      return ret;
    }
    /*** querying on message ***/
    // NOTE: Only add *Cell.entry to ret vector, same with senders query
    if (axis == "message") {
      auto message_cells = query_on_message(content, fuzzy);
      for (auto &each : message_cells) { ret.push_back(each->entry); }
      return ret;
    }
    /*** querying on sender ***/
    if (axis == "sender") {
      auto sender_cells = query_on_sender(content, fuzzy);
      for (auto &each : sender_cells) { ret.push_back(each->entry); }
      return ret;
    }
    throw std::runtime_error("StorageGraph::query() Unrecognized query "
                             "axis: " + axis);
  }

};


#endif

