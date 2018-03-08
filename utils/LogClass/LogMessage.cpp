#include <iostream>
using std::cout; using std::cin; using std::endl;

#include <string>
using std::string;

#include <regex>
using std::regex; using std::regex_match;
using std::smatch; using std::ssub_match;

#include <stdexcept>

#include <cstdint>

#include "utils/LogClass/LogClass.h"


/**** LogMessage ****/

void
LogMessage::_init_to_zero() {
  /* pass */
  this->host.erase();
  this->sender.erase();
  this->message.erase();
  return;
}


LogMessage::LogMessage() {
  _init_to_zero();
  return;
}


LogMessage::LogMessage(const string &str, const bool message_only) {
  _init_to_zero();
  try {
    regex re;
    if (message_only == false) {    // log string with date
      re = regex(RE_WHOLE);
    } else {
      message = str;
      return;
    }
    smatch matches;
    regex_match(str, matches, re);
    if (matches.size() != 5) {  // match failed
      throw std::runtime_error(string("LogMessage.cpp::LogMessage(str) `str` ")
          + "passed in was illegal: " + str);
    }
    // match success
    host = matches[2].str();
    sender = matches[3].str();
    message = matches[4].str();
  } catch (const std::regex_error &e) {
    throw e;
  }
  return;
}


LogMessage::LogMessage(const LogMessage &copy) {
  this->host = copy.host;
  this->sender = copy.sender;
  this->message = copy.message;
  return;
}


void
LogMessage::append_msg(const string &more) {
  // WHY:  Some apps (like google-chrome) will generate logs that span across
  //       multiple lines (usually xml-like), thus you will need to append
  //       the messages in the new lines to a previous `LogMessage` instance.
  //       Otherwise the log read into memory will be incomplete, lack unique
  //       informations and cause further problem reading following logs.
  message.append(string("\n") + more);
  return;
}

/**** DEPRECATED ****/
// uint64_t
// strhash(string str, size_t pool_size=HASH_SPACE,
//         size_t range=STRHASH_RANGE) {
//   // ALG:  For chars in message string, collect their ascii values,
//   //       times 6 (fill the 5000 hash space). After the for loop, subtract
//   //       the sum outcome by 10000 (expected sum average value).
//   //       Lastly mod the sum by `HASH_SPACE` (fit into static hash space).
//   //
//   // HACK: Since it's highly possible that log messages have their first
//   //       10-to-40-ish characters *IDENTICAL*, a better practice is that you
//   //       start your hash from the end.
//   //       Here I take the last `STRHASH_RANGE` (now 30) chars.
//   uint64_t ret = 0U;
//   auto curr = str.end(); --curr;    // `str.end()` is actually one-off-end
//   auto begin = str.begin();
//   for (size_t max_range = range;
//        curr != begin && max_range != 0; --curr, --max_range) {
//     ret += static_cast<uint8_t>(*curr) * 6; // NOTE: it's okay if overflow
//   }
//   return (ret - 10000U) % pool_size;
// }


/****** Test ******/

// int main(int argc, const char *argv[]) {
//   cout << "==== Test LogMessage ====" << endl;
//
//   auto sample = string("Jan  8 12:07:06 zhuxiaoguangs-MacBook-Air com.apple.x"
//       "pc.launchd[1] (com.apple.preference.displays.MirrorDisplays): Service "
//       "only ran for 0 seconds. Pushing respawn out by 10 seconds.");
//   auto info = LogMessage();
//   cout << "empty init passed" << '\n';
//   info = LogMessage(sample);
//   cout << "arg init / operator= passed" << '\n';
//   auto info_cpy = LogMessage(info);
//   cout << "copy init passed" << '\n';
//   if (info == LogMessage(sample)) {
//     cout << "operator== / operator!= passed" << '\n';
//   } else { cout << "operator== / operator!= ERROR!" << '\n'; }
//   if (info.notempty() && LogMessage().notempty() == false) {
//     cout << "notempty() passed" << '\n';
//   }
//   info.append_msg("some other texts...");
//   cout << info.message << '\n';
//
//   // for (size_t timer = 0; timer != 1E+4; ++timer) {
//   //   cout << "hashed message ==> " << strhash(info.message) << '\n';
//   // }
//   cout << "end of test" << endl;
//   return 0;
// }
