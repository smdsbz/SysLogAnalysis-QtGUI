#include <iostream>
using std::cout; using std::cin; using std::endl;

#include <string>
using std::string; using std::stoul;

#include <cstring>
using std::memset;

#include <regex>
using std::regex; using std::regex_match;
using std::smatch; using std::ssub_match;

#include <stdexcept>

#include "utils/LogClass/LogClass.h"


/****** LogDate ******/

/* 函数名称：_month_str_to_ushort
 * 函数参数：月份英文简写（string）
 * 函数功能：将月份简写转化成对应的月份数字码
 * 返回值：  月份数字码（unsigned short）
 * 抛出异常：std::invalid_argument
 *               当传入的不是标准的月份英文简写时抛出
 */
unsigned short
_month_str_to_ushort(string str) {
  if (str == "Jan") { return  1; }
  if (str == "Feb") { return  2; }
  if (str == "Mar") { return  3; }
  if (str == "Apr") { return  4; }
  if (str == "May") { return  5; }
  if (str == "Jun") { return  6; }
  if (str == "Jul") { return  7; }
  if (str == "Aug") { return  8; }
  if (str == "Sep") { return  9; }
  if (str == "Oct") { return 10; }
  if (str == "Nov") { return 11; }
  if (str == "Dec") { return 12; }
  else {
    throw std::invalid_argument("LogDate.cpp::_month_str_to_ushort() illegal "
        "month string");
  }
}

/* 函数名称：_month_ushort_to_str
 * 函数参数：月份数字码（unsigned short)
 * 函数功能：将月份数字码转化成对应的英文简写
 * 返回值：  月份英文简写（string）
 * 抛出异常：std::invalid_argument
 *               当传入的月份数字码不在 1～12 的范围内时抛出
 */
string
_month_ushort_to_str(unsigned short mon) {
  if (mon == 0 || mon > 12) {
    throw std::invalid_argument("LogDate.cpp::_month_ushort_to_str() illegal "
        "month number!");
  }
  return months[mon - 1];
}

/* 函数名称：_ushort_to_dualdigit
 * 函数参数：两位数数字（unsigned short）
 *           输出前导字符（string，默认为 "0"）
 * 函数功能：将两位数数字转化成标准格式字符串
 * 返回值：  数字字符串（string）
 * 抛出异常：std::invalid_argument
 *               当传入的前导字符为空时抛出
 */
string
_ushort_to_dualdigit(unsigned short us, const string filler="0") {
  if (filler.empty()) {
    throw std::invalid_argument("LogDate.cpp:_ushort_to_dualdigit() "
        "`filler` should not be empty!");
  }
  string ret;
  if (us < 10) { ret.push_back(filler[0]); }
  else { ret.push_back(static_cast<char>('0' + us / 10)); }
  ret.push_back(static_cast<char>('0' + us % 10));
  return ret;
}


void
LogDate::_init_to_zero() {
  memset(&time, 0U, 4); // all to zero
  return;
}


LogDate::LogDate() {
  _init_to_zero();
  return;
}


LogDate::LogDate(const string &str) {
  _init_to_zero();
  try {
    regex re(RE_DATE + "[^\n]*");  // regex
    smatch matches;     // store matched strings
    regex_match(str, matches, re); // do match
    if (matches.size() != 6) {  // match failed
      throw std::runtime_error(string("LogDate.cpp::LogDate(str) `str` passed")
          + " in was illegal: " + std::to_string(matches.size()) + " found");
    }
    // match success
    time.mon = _month_str_to_ushort(matches[1].str());
    time.dat = stoul(matches[2].str());
    time.hor = stoul(matches[3].str());
    time.min = stoul(matches[4].str());
    time.sec = stoul(matches[5].str());
    // TODO: leagality check
  } catch (const std::regex_error &e) {
    throw e;
  }
  return;
}


LogDate &
LogDate::operator+(const size_t sec) {
  auto ret = *this;
  for (size_t remain = sec; remain != 0; --remain) {
    ret.time.sec = (ret.time.sec + 1) % 60;
    // forward detect
    if (ret.time.sec != 0) { continue; }
    ret.time.min = (ret.time.min + 1) % 60;
    if (ret.time.min != 0) { continue; }
    ret.time.hor = (ret.time.hor + 1) % 24;
    if (ret.time.hor != 0) { continue; }
    // ret.time.hor == 0: forward a day
    ret.time.dat = ret.time.dat + 1;
    // forward a month
    switch (ret.time.mon) {
      case 1: case 3: case 5: case 7: case 8: case 10: case 12: {
        if (ret.time.dat == 0) {    // 32 wrap-around
          ret.time.mon = ret.time.mon + 1;
          if (ret.time.mon == 13) { ret.time.mon = 1;}
        }
      }
      case 4: case 6: case 9: case 11: {
        if (ret.time.dat == 31) {
          ret.time.mon = ret.time.mon + 1;
        }
      }
      case 2: { // HACK: Unfortunately, log does *NOT* record year.
                //       Treating Feb has 29 days
        if (ret.time.dat == 30) {   // TODO: Auto year recognition
          ret.time.mon = ret.time.mon + 1;
        }
      }
    }   // end of switch
  } // end of for
  return *this;
}


string
LogDate::str() const {
  try {
    string ret;
    ret.append(_month_ushort_to_str(time.mon) + " ");
    ret.append(_ushort_to_dualdigit(time.dat, " ") + " ");
    ret.append(_ushort_to_dualdigit(time.hor) + ":");
    ret.append(_ushort_to_dualdigit(time.min) + ":");
    ret.append(_ushort_to_dualdigit(time.sec));
    return ret;
  } catch (const std::runtime_error &e) {
    throw e;
  }
}


/****** Test ******/

// int
// main(int argc, const char *argv[]) {
//   cout << "==== LogDate Test ====" << endl;
//   string input_date;
//   while (getline(cin, input_date)) {
//     try {
//       auto date = LogDate(input_date);
//       cout << "matched LogDate: " << date.str() << endl;
//     } catch (const std::runtime_error &e) {
//       cout << e.what() << endl;
//     }
//   }
//   cout << "<C-D> pressed" << endl;
//   return 0;
// }
