#include <iostream>
using std::cout; using std::cin; using std::endl;

#include <string>
using std::string;

#include <stdexcept>

#include "utils/LogClass/LogClass.h"
#include "utils/HashTable/HashTable.h"


/****** Class Definitions ******/

LogRecord::iterator &
LogRecord::iterator::operator++() {
  // already reached end
  if (pRec == nullptr) {
    throw std::runtime_error("LogRecord::iterator::operator++() Already "
        "reached end!");
  }
  // venture forth
  switch (axis) {
    case TIME  : { /* one_prev = pRec; */ pRec = pRec->time_suc  ; break; }
    case SENDER: { /* one_prev = pRec; */ pRec = pRec->sender_suc; break; }
    case UNSPECIFIED: {
      throw std::runtime_error("LogRecord::iterator::operator++() Unspecified "
          "iteration axis!");
      break;
    }
    default: {
      throw std::runtime_error("LogRecord::iterator::operator++() Unknown "
          "iteration axis!");
      break;
    }
  }
  return *this;
}


/****** Test ******/

// int main(int argc, const char *argv[]) {
//   string sample = ("Jan  8 12:07:06 zhuxiaoguangs-MacBook-Air com.apple.x"
//       "pc.launchd[1] (com.apple.preference.displays.MirrorDisplays): Service "
//       "only ran for 0 seconds. Pushing respawn out by 10 seconds.");
//
//   auto message = LogMessage(sample);
//   LogRecord records[10] = { };
//   for (size_t idx = 0; idx != 10; ++idx) {
//     records[idx] = LogRecord(sample, true);
//     records[idx].set_message(message);
//     if (idx != 9) { records[idx].set_time_successor(records[idx+1]); }
//   }
//
//   for (auto each_record : records) {
//     cout << each_record.get_message() << endl;
//   }
//
//   return 0;
// }

