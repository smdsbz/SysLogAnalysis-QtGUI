#ifndef FPTREE_H_
#define FPTREE_H_

#include <iostream>
using std::cout; using std::cin; using std::endl;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <stdexcept>

#include <QProgressDialog>

#include "utils/LogClass/LogClass.h"
#include "utils/HashTable/HashTable.h"
#include "utils/StorageGraph/StorageGraph.h"


class FPTree {
public:

  class _Node {
  public:
    _HashCell_LogMessage *entity = nullptr;
    size_t  occur  = 0;
    _Node  *child  = nullptr;
    _Node  *brother= nullptr;
    /* _Node  *next_occur = nullptr; */

  public:

    explicit _Node(_HashCell_LogMessage *p) {
      if (p == nullptr) {
        throw std::runtime_error("FPTree::_Node() Empty node!");
      }
      this->entity = p;
      return;
    }

    inline _Node &operator++() { this->occur += 1; return *this; }

    _Node *in_brothers(_HashCell_LogMessage *p) {
      _Node *pnode = this;
      for (; pnode != nullptr; pnode = pnode->brother) {
        if (pnode->entity == p) { break; }
      }
      return pnode; // NOTE: Including returning `nullptr`
    }

    _Node *in_children(_HashCell_LogMessage *p) {
      return this->child->in_brothers(p);
    }

    _Node *last_brother() {
      _Node *pnode = this;
      for (; pnode->brother != nullptr; pnode = pnode->brother) { ; }
      return pnode;
    }

  };

  class _Header {
  public:
    _HashCell_LogMessage *entity = nullptr;
    size_t  freq   = 0;
    /* _Node  *occur  = nullptr; */

  public:

    _Header(_HashCell_LogMessage *p, size_t f) {
      if (p == nullptr) {
        throw std::runtime_error("FPTree::_Header() Empty header!");
      }
      this->entity = p;
      this->freq   = f;
      return;
    }

    _Header &operator= (const _Header &other) {
      this->entity = other.entity;
      this->freq   = other.freq;
      return *this;
    }

    inline bool operator==(const _Header &other) {
      return this->entity == other.entity;
    }

    inline _HashCell_LogMessage *operator->() {
      if (this->entity == nullptr) {
        throw std::runtime_error("FPTree::_Header::operator-> `entity` field is empty!");
      }
      return this->entity;
    }

  };


  Storage         *storage = nullptr;
  vector<_Header>  headers;
  _Node           *nodes = nullptr;

public:

  explicit FPTree(Storage *s) {
    if (s == nullptr) {
      throw std::invalid_argument("FPTree::FPTree() Accompanied Storage "
          "cannot be null!");
    }
    this->storage = s;
  }

  ~FPTree() {
    vector<_Node *> node_stack;
    auto pnode = this->nodes;
    // get first branch
    while (pnode) {
      node_stack.push_back(pnode);
      pnode = pnode->child;
    }
    // pre-order traverse and free `_Node`s
    while (!node_stack.empty()) {
      pnode = node_stack.back();
      auto next_node = node_stack.back()->brother;
      node_stack.pop_back(); delete pnode;  // free end of current branch
      // get another branch
      while (next_node) {
        node_stack.push_back(next_node);
        next_node = next_node->child;
      }
    }   // pre-order traverse
    return;
  }

  _Header &operator[](_HashCell_LogMessage *p) {
    // check if `p` is in `headers`
    for (auto &each : this->headers) {
      if (each.entity->data == p->data) { return each; }
    }   // else - `p` not in `headers` yet
    throw std::overflow_error("FPTree::operator[LogMessage] Not found!");
  }

  _Header &_add_header(_HashCell_LogMessage *p, size_t f) {
    // first header
    if (this->headers.empty()) {
      this->headers.push_back(_Header(p, f));
      return this->headers[0];
    }
    // not first - consider insertion place
    // insert to tail
    if (f < this->headers.back().freq) {
      this->headers.push_back(_Header(p, f));
      return this->headers.back();
    }
    // insert at first / middle
    for (size_t idx = 0, range = this->headers.size();
         idx != range; ++idx) {
      if (f < this->headers[idx].freq) { continue; }
      this->headers.insert(this->headers.cbegin() + idx, _Header(p, f));
      return this->headers[idx];
    }
    throw std::runtime_error("FPTree::_add_header() Cannot find insert position");
  }

  FPTree &_first_run(size_t valve_freq=3) {
    if (this->storage == nullptr) {
      throw std::runtime_error("FPTree::_first_run() Attempting to run "
          "analysis on empty storage!");
    }
    auto &s = *(this->storage);
    // ProgressBar
    QProgressDialog progress(/*labelText=*/"First run:\n"
                                           "Gathering neccessary context...",
                             /*closeButtonText=*/"Stop",
                             /*minimum=*/0, /*maximum=*/s.messages->space);
    // cancel "close" button
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress.setCancelButton(nullptr);
    progress.setWindowModality(Qt::WindowModal);
    // do actual job - fp-tree first run
    for (size_t idx = 0, range = s.messages->space;
         idx != range; ++idx) {
      progress.setValue(idx); // set ProgressBar
      if (s.messages->table[idx].occupied()) {
        for (auto pcell = s.messages->table + idx;
             pcell; pcell = pcell->next) {
          if (pcell->count < valve_freq) { continue; }
          this->_add_header(pcell, pcell->count);
        }   // for all isotopes
      } // if has content
    }   // for all cell in table
    return *this;
  }

  FPTree &_add_pattern(vector<_Header> pat) {
    // HACK: Performance trade-off, more code for less conparison
    if (pat.empty()) { return *this; }  // sent empty pattern
    if (this->nodes == nullptr) {   // first pattern
      // simply convert vector to link-list
      try {
        this->nodes = new _Node(pat[0].entity);   // add first
      } catch (const std::bad_alloc &e) {
        cout << "Space allocation failed!" << endl;
        throw e;
      }
      auto prev_node = *(this->nodes);
      ++prev_node;  // inc `occur`
      for (size_t idx = 1, range = pat.size();
           idx != range;
           ++idx, prev_node = *prev_node.child) {
        try {
          prev_node.child = new _Node(pat[idx].entity);
        } catch (const std::bad_alloc &e) {
          cout << "Space allocation failed!" << endl;
          throw e;
        }
        ++prev_node;    // inc `occur`
      }
      return *this;
    } else {    // not first pattern
      _Node *current_layer = this->nodes;   // (first node in) first layer
                                            // promised not `nullptr`
      _Node *cursor_node = nullptr;
      for (auto &each : pat) {
        if (current_layer == nullptr) { // current_layer does not exist,
                                        // i.e. new pattern
          // directly append rest in `pat` to `cursor_node->child`
          try {
            cursor_node->child = new _Node(each.entity);
          } catch (const std::bad_alloc &e) {
            cout << "Space allocation failed!" << endl;
            throw e;
          }
          cursor_node = cursor_node->child;
          ++(*cursor_node); // injc `occur`
          /* current_layer = cursor_node->child;   // next layer, still `nullptr` */
          continue; // do *ONE* at a time
        }
        // find ref in current layer
        cursor_node = current_layer->in_brothers(each.entity);
        if (cursor_node == nullptr) {   // node not in current layer yet
          // append one to last in layer
          cursor_node = current_layer->last_brother();
          try {
            cursor_node->brother = new _Node(each.entity);
          } catch (const std::bad_alloc &e) {
            cout << "Space allocation failed!" << endl;
            throw e;
          }
          cursor_node = cursor_node->brother;
        } else {    // already in current layer
          /* pass */ ;
        }
        ++(*cursor_node);   // inc `occur`
        // go to next layer
        current_layer = cursor_node->child; // NOTE: Could be `nullptr`!
                                            //       Handled at the begining.
      } // for each in pat, i.e. each layer
    }
    return *this;
  }

  vector<_Header> _sieve_frequent(vector<LogRecord *> context) {
    // HACK: Kinda like bucket-sort
    vector<_Header> ret;
    for (auto &each_header : this->headers) {   // for each message,
                                                // with freq descend
      for (auto &each_prec : context) {     // for each record in context
        // append message if it occurs in context
        if (each_header.entity->strict_equal(*each_prec->message)) {
          ret.push_back(each_header);
          break;
        }
      } // for each record in context
    }   // for each message, with freq descend
    return ret;
  }

  FPTree &_second_run(size_t valve_delay=5) {
    QProgressDialog progress(/*labelText=*/"Second run:\n"
                                           "Mining frequent patterns...",
                             /*closeButtonText=*/"Stop",
                             /*minimum=*/0, /*maximum=*/this->headers.size());
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress.setCancelButton(nullptr);
    progress.setWindowModality(Qt::WindowModal);
    size_t cnt = 0;
    for (auto &header : this->headers) {    // for each frequent message
      progress.setValue(cnt++);
      for (auto prec = header.entity->entry;
           prec != nullptr;
           prec = prec->msg_suc) {  // for each occurance of message
        this->_add_pattern(
              _sieve_frequent(prec->peek(valve_delay)) );
      } // for each occurance of message
    }   // for each frequent message
    return *this;
  }

  FPTree &run(size_t hp_min_freq=3, size_t hp_max_delay=5) {
    this->_first_run(hp_min_freq);
    this->_second_run(hp_max_delay);
    return *this;
  }

  vector<_Node *> &show_pattern(vector<_Node *> &pat) {
    if (pat.size() == 0) {
      cout << "Empty pattern!" << endl;
    } else {
      // show pattern
      for (auto &each : pat) {
        cout << each->entity->get_message() << endl;
      }
      cout << "---- " << pat.size() << " message(s) in pattern" << endl;
      cout << "     occured " << pat.back()->occur << " time(s) ----" << endl;
    }
    return pat;
  }

  FPTree &show_result(size_t valve_freq=5) {
    vector<_Node *> node_stack;
    auto pnode = this->nodes;
    if (pnode == nullptr) {
      cout << "No patterns are extracted! Try with a larger log dump!" << endl;
      return *this;
    }
    // first pattern
    while (pnode && pnode->occur >= valve_freq) {
      node_stack.push_back(pnode);
      pnode = pnode->child;
    }
    this->show_pattern(node_stack);
    if (cin.get() == 'q') { return *this; } // type 'q' to end and return
    // more patterns
    while (!node_stack.empty()) {
      auto prev_pat_occur = node_stack.back()->occur;
      auto next_node = node_stack.back()->brother;  // change route
      node_stack.pop_back();    // quit current pattern, *ONE* level at a time
      if (next_node && next_node->occur >= valve_freq) {    // if different route exists
        while (next_node && next_node->occur >= valve_freq) {   // venture deep
          node_stack.push_back(next_node);
          next_node = next_node->child;
        }
        // reached another pattern, at max depth
        this->show_pattern(node_stack);
        if (cin.get() == 'q') { return *this; }
      } else if (next_node == nullptr) {  // different route does not exist,
                                          // i.e. still on previous pattern
        // NOTE: Pattern occurance changed, could be viewed as a
        //       different pattern.
        if (node_stack.back()->occur != prev_pat_occur) {
          this->show_pattern(node_stack);
          if (cin.get() == 'q') { return *this; }
        }
      } else { // different route *DOSE* exist,
               // but its occurance is less than `valve_freq`
        /* pass */ ;
      }
    }   // stack now empty
    return *this;
  }

};

#endif

