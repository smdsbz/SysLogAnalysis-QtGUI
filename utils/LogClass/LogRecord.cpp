#include <iostream>
using std::cout; using std::cin; using std::endl;

#include <string>
using std::string;

#include <stdexcept>

#include "utils/LogClass/LogClass.h"


/****** LogRecord ******/

LogRecord::LogRecord(const string &str, const bool whole) {
  // NOTE: Only set date, other should be done in Storage.
  try {
    if (whole == true) { date = LogDate(str.substr(0, 15)); }   // static hack
    else { date = LogDate(str); }
  } catch (const std::runtime_error &e) {
    throw std::runtime_error(string("LogRecord.cpp::LogRecord(str) failed ")
        + "to create LogRecord instance due to: " + e.what());
  }
  /* message = nullptr; time_suc = nullptr; sender_suc = nullptr; */
  return;
}


LogRecord::~LogRecord() {
  // HACK: Actually there's nothing you can do here, because you don't
  //       know the Graph context, so you've got no way of knowing how to
  //       re-chain related `LogRecord`s.
  /* pass */ ;
  return;
}

