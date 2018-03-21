#ifndef LOGCLASS_H_
#define LOGCLASS_H_


#include <iostream>
using std::cout; using std::cin; using std::endl;

#include <string>
using std::string; using std::to_string;

#include <cstring>
using std::memset;

#include <regex>
using std::regex; using std::regex_match;
using std::smatch; using std::ssub_match;

#include <vector>
using std::vector;

#include <stdexcept>


/****** Regular Expressions ******/
// bsd format: syslog -F bsd > target_file

const string months[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
    "Aug", "Sep", "Oct", "Nov", "Dec"
};

const string _re_month = "Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec";

const string RE_DATE = string("(") + _re_month + ") ([0-3| ][0-9]) "
    + "([0-9]{2}):([0-9]{2}):([0-9]{2})";

const string RE_WHOLE = (string("((?:") + _re_month
    + ") [1-3| ][0-9] [0-9]{2}:[0-9]{2}:[0-9]{2}) ")    // date
                            //   eg "Jan  1 19:43:19"
    + "([a-zA-Z\\-]+) "     // host name (lower/upper and short-dash)
                            //   eg "zhuxiaoguangs-MacBook-Air"
    + "([\\s\\S]+?)[\\n]?\\[[0-9]+\\][:]? " // sender (lower) and threadID
                                            //   eg "steam_osx[584]"
    + "([\\s\\S]+)";        // message (anything hence to the eol)


/****** Class Declarations ******/

class LogDate {
public:
  struct {
    unsigned short mon : 4;     // month:  1 to 12
    unsigned short dat : 5;     // date:   1 to 31
    unsigned short hor : 5;     // hour:   0 to 23
    unsigned short min : 6;     // minute: 0 to 59
    unsigned short sec : 6;     // second: 0 to 59
    unsigned long  _   : 6;     // reserved
  } time;   // self-designed time_t

public:

  /* 函数名称：_init_to_zero
   * 函数参数：无
   * 函数功能：将 LogDate 对象的数据域置零
   * 返回值：  无
   */
  void _init_to_zero();

  /* 函数名称：LogDate（构造函数）
   * 函数参数：无
   * 函数功能：构造数据域置零的初始化 LogDate 对象
   * 返回值：  无
   */
  LogDate();

  /* 函数名称：LogDate（类型转换构造函数 explicit）
   * 函数参数：以标准 BSD 格式时间字符串开头的字符串（string）
   * 函数功能：提取字符串中开头的时间数据，并以此为依据生成 LogDate 对象
   * 返回值：  无
   * 抛出异常：std::runtime_error
   *               当传入的字符串不符合标准 BSD 时间格式时抛出
   */
  explicit LogDate(const string &re_mathed_date_string); // whole_log_string is fine

  /* 函数名称：operator==、operator!=、operator> 、operator<=、
   *           operator< 、operator>=（数值关系比较运算符）
   * 函数参数：需要比较的 LogDate 对象（LogDate）
   * 函数功能：比较两个对象是否满足这些比较关系
   * 返回值：  比较结果（bool）
   */
  inline bool operator==(const LogDate &other) {
    if (this->time.mon != other.time.mon) { return false; }
    if (this->time.dat != other.time.dat) { return false; }
    if (this->time.hor != other.time.hor) { return false; }
    if (this->time.min != other.time.min) { return false; }
    if (this->time.sec != other.time.sec) { return false; }
    return true;
  }

  inline bool operator!=(const LogDate &other) {
    return !(*this == other);
  }

  inline bool operator> (const LogDate &other) {
    if (this->time.mon > other.time.mon) { return true; }
    else if (this->time.mon < other.time.mon) { return false; }
    if (this->time.dat > other.time.dat) { return true; }
    else if (this->time.dat < other.time.dat) { return false; }
    if (this->time.hor > other.time.hor) { return true; }
    else if (this->time.hor < other.time.hor) { return false; }
    if (this->time.min > other.time.min) { return true; }
    else if (this->time.min < other.time.min) { return false; }
    if (this->time.sec <= other.time.sec) { return false; }
    return true;
  }

  inline bool operator<=(const LogDate &other) {
    return !(*this > other);
  }

  inline bool operator< (const LogDate &other) {
    if (this->time.mon == 12  // HACK: last day is before everyday
        && this->time.dat == 31) { return true; }
    if (this->time.mon < other.time.mon) { return true; }
    else if (this->time.mon > other.time.mon) { return false; }
    if (this->time.dat < other.time.dat) { return true; }
    else if (this->time.dat > other.time.dat) { return false; }
    if (this->time.hor < other.time.hor) { return true; }
    else if (this->time.hor > other.time.hor) { return false; }
    if (this->time.min < other.time.min) { return true; }
    else if (this->time.min > other.time.min) { return false; }
    if (this->time.sec >= other.time.sec) { return false; }
    return true;
  }

  inline bool operator>=(const LogDate &other) { return !(*this < other); }

  /* 函数名称：operator+（操作符）
   * 函数参数：当前时间到目标时间的偏移量，单位为秒（size_t）
   * 函数功能：以当前时间为基准，生成向后偏移指定秒数的 LogDate 对象
   * 返回值：  偏移后的 LogDate 对象
   */
  LogDate operator+ (const size_t sec);

  /* 函数名称：str
   * 函数参数：无
   * 函数功能：将当前 LogDate 对象转化标准 BSD 时间格式字符串
   * 返回值：  BSD 时间格式字符串（string）
   * 抛出异常：std::runtime_error
   *               转化失败时抛出
   */
  string str() const;

};


/**** LogMessage ****/

class LogMessage {
public:
  string    host;       // host of log, could be an IP or computer name
  string    sender;     // sender of log, i.e. the name of the process
  string    message;    // the actual message content

public:

  /* 函数名称：_init_to_zero
   * 函数参数：无
   * 函数功能：将 LogMessage 对象的数据域置零
   * 返回值：  无
   */
  void _init_to_zero();

  /* 函数名称：LogMessage（构造函数）
   * 函数参数：无
   * 函数功能：构造数据域置零的初始化 LogMessage 对象
   * 返回值：  无
   */
  LogMessage();

  /* 函数名称：LogMessage（类型转换构造函数 explicit）
   * 函数参数：标准 BSD 系统日志格式字符串（string）
   *           是否只含日志的消息部分（bool，默认为 false）
   * 函数功能：提取字符串中有关数据，并以此为依据生成 LogMessage 对象
   * 返回值：  无
   * 抛出异常：std::runtime_error
   *               当传入的字符串不符合标准 BSD 系统日志格式时抛出
   */
  explicit LogMessage(const string &log_string_whole,
                      const bool message_only=false);

  /* 函数名称：LogMessage（拷贝构造函数）
   * 函数参数：模板 LogMessage 对象
   * 函数功能：拷贝构造 LogMessage 对象
   * 返回值：  无
   */
  LogMessage(const LogMessage &copy);

  /* 函数名称：operator=（赋值运算操作符）
   * 函数参数：右值 LogMessage 对象
   * 函数功能：使用右值对象的数据覆盖当前对象
   * 返回值：  左值对象
   */
  inline LogMessage &operator= (const LogMessage &other) {
    if (*this == other) { return *this; }
    this->host    = other.host;
    this->sender  = other.sender;
    this->message = other.message;
    return *this;
  }

  /* 函数名称：operator==、operator!=（数值关系比较运算符）
   * 函数参数：需要比较的 LogMessage 对象（LogMessage）
   * 函数功能：比较两个对象是否满足这些比较关系
   * 返回值：  比较结果（bool）
   */
  inline bool operator!=(const LogMessage &other) {
    if (this->host    != other.host   ) { return true; }
    if (this->sender  != other.sender ) { return true; }
    if (this->message != other.message) { return true; }    // costs time!
    return false;
  }

  inline bool operator==(const LogMessage &other) { return !(*this != other); }

  /* 函数名称：notempty、empty
   * 函数参数：无
   * 函数功能：判断当前 LogMessage 对象是否为空 / 非空
   * 返回值：  判断结果（bool）
   */
  inline bool notempty() { return !(this->host.empty()); }
  inline bool empty() { return !(this->notempty()); }

  /* 函数名称：append_msg
   * 函数参数：要追加的日志信息（string）
   * 函数功能：在当前日志信息之后另起一行，追加信息
   * 返回值：  无
   */
  void append_msg(const string &more_message_str);

  /* 函数名称：get_message、get_sender、get_host
   * 函数参数：无
   * 函数功能：获取对应日志属性
   * 返回值：  日志对应属性（string）
   */
  inline string get_message() const { return string(this->message); }
  inline string get_sender()  const { return string(this->sender); }
  inline string get_host()    const { return string(this->host); }

};


// used by LogRecord::iterator
enum axis_type : unsigned char { TIME, SENDER, UNSPECIFIED };

class LogRecord {
public:
  LogMessage   *message     = nullptr;  // pointer to LogMessage
                                        // HACK: Keeping only one LogMessage
                                        //       instance, simplify further
                                        //       comparison
  LogDate       date;                   // when the log occured
  LogRecord    *time_suc    = nullptr;  // the log immediately after this one
  LogRecord    *msg_suc     = nullptr;  // next occurance with the same message
  LogRecord    *sender_suc  = nullptr;  // next log sent by the same program

  class iterator {
  public:
    LogRecord    *pRec    = nullptr;    // pointer to current iteration entity
    axis_type     axis    = TIME;       // itertion axis

  public:
    /* 函数名称：iterator（迭代器构造函数）
     * 函数参数：迭代范围的第一个实例（LogRecord）
     *           迭代方向（axis_type，默认为时间轴 TIME）
     * 函数功能：生成一个迭代器，并把初次迭代位置放在传入的实例处
     * 返回值：  生成的迭代器（LogRecord::iterator)
     */
    inline iterator(LogRecord &start, const axis_type passed_axis=TIME) {
      this->pRec = &start;
      this->axis = passed_axis;
      return;
    }
    /* 函数名称：iterator（迭代器构造函数）
     * 函数参数：指向迭代范围第一个实例的指针（LogRecord *）
     *           迭代方向（axis_type，默认为时间轴 TIME）
     * 函数功能：生成一个迭代器，并把初次迭代位置放在传入的实例处
     * 返回值：  生成的迭代器（LogRecord::iterator)
     */
    inline iterator(LogRecord *p, const axis_type passed_axis=TIME) {
      this->pRec = p;
      this->axis = passed_axis;
      return;
    }
    /* 函数名称：operator++（迭代器操作符）
     * 函数参数：无
     * 函数功能：向由 this->axis 指定的迭代方向，进行一次迭代
     * 返回值：  完成迭代步骤之后的迭代器（LogRecord::iterator）
     */
    iterator &operator++();  // see ./LogRecord_iterator.cpp
    /* 函数名称：operator==、operator!=（迭代器边界判断操作符）
     * 函数参数：指向某一实例或 one-off-end 的另一迭代器
     * 函数功能：判断当前迭代器与传入的迭代器的位置关系
     * 返回值：  判断结果（bool）
     */
    inline bool operator==(const iterator &other) {
      if (this->axis != other.axis) {
        throw std::runtime_error("LogRecord::iterator::operator==() Cannot "
            "compare iterators on different axes!");
      }
      return this->pRec == other.pRec;
    }
    inline bool operator!=(const iterator &other) { return !(*this == other); }
    /* 函数名称：operator*、operator->（解引用操作符）
     * 函数参数：无
     * 函数功能：返回迭代器指向的实例
     * 返回值：  迭代器指向的实例（LogRecord &）
     */
    inline LogRecord &operator* () {
      if (this->pRec == nullptr) {
        throw std::runtime_error("LogRecord::iterator::operator*() Cannot get "
            "reference of `nullptr`!");
      }
      return *(this->pRec);
    }
    inline LogRecord *operator->() {
      if (this->pRec == nullptr) {
        throw std::runtime_error("LogRecord::iterator::operator->() Iterator "
            "pointing to `nullptr`!");
      }
      return this->pRec;
    }
  };

public:

  inline LogRecord() { return; }

  explicit LogRecord(const string &re_mathed_date_string,
                     const bool whole=false);

  explicit LogRecord(const LogRecord &other) {
    this->message    = other.message;
    this->date       = other.date;
    this->time_suc   = nullptr;     // don't copy relations for a discreet node
    this->msg_suc    = nullptr;
    this->sender_suc = nullptr;
    return;
  }

  ~LogRecord();

  inline bool operator<=(const LogRecord &other) {
    // NOTE: relation defined as time relation
    return this->date <= other.date;
  }

  inline bool operator>=(const LogRecord &other) {
    return this->date >= other.date;
  }

  inline LogRecord &operator= (const LogRecord &other) {
    this->message   = other.message;
    this->date      = other.date;
    this->time_suc  = other.time_suc;
    this->sender_suc= other.sender_suc;
    return *this;
  }

  // NOTE: Called by `Storage` after `LogMessage` is inserted.
  inline LogRecord &set_message(LogMessage &msg) {
    message = &msg;
    return *this;
  }

  inline LogRecord &set_time_successor(LogRecord &nxt) {
    time_suc = &nxt;
    return *this;
  }

  inline LogRecord &set_message_successor(LogRecord &nxt) {
    this->msg_suc = &nxt;
    return *this;
  }

  inline LogRecord &set_sender_successor(LogRecord &nxt) {
    sender_suc = &nxt;
    return *this;
  }

  inline string get_message() const { return this->message->get_message(); }
  inline string get_date()    const { return this->date.str(); }
  inline string get_sender()  const { return this->message->get_sender(); }
  inline string get_host()    const { return this->message->get_host(); }
  inline string _repr() const {
    string datestr  = "Date :"      + this->get_date();
    string snderstr = "Sender: "    + this->get_sender();
    string msgstr   = "Message: \n" + this->get_message();
    return datestr + '\n' + snderstr + '\n' + msgstr;
  }

  /* iterator */
  inline iterator begin(axis_type axis=TIME) { return iterator(*this, axis); }
  inline iterator end(axis_type axis=TIME) { return iterator(nullptr, axis); }

  vector<LogRecord *> peek(size_t sec=5, const bool allow_repeat=false) {
    vector<LogRecord *> ret;
    ret.push_back(this);    // add self to peak-view
    auto border = this->date + sec;
    for (LogRecord *each = this->time_suc; each; each = each->time_suc) {
      if (!allow_repeat && each->message == this->message) {
        ret.push_back(each); return ret;
      }
      if (each->date > border) { break; }
      ret.push_back(each);
    }
    return ret;
  }

};


/****** Public Utilities ******/

constexpr const size_t STRHASH_RANGE  = 30;
constexpr const size_t HASH_SPACE     = 5000;

#include <cmath>

class HashFunc {
public:
  size_t hash_range = STRHASH_RANGE;    // range of summing string ASCII vals
  size_t pool_size  = HASH_SPACE;   // target space
  unsigned char selection = 0;  // (reserved) select with hash func to use

public:

  inline HashFunc(size_t hrange = STRHASH_RANGE,
                  size_t psize  = HASH_SPACE,
                  unsigned char slct = 0) {
    this->hash_range= hrange;
    this->pool_size = psize;
    this->selection = slct;
    return;
  }

  inline HashFunc &operator= (const HashFunc &other) {
    this->hash_range= other.hash_range;
    this->pool_size = other.pool_size;
    this->selection = other.selection;
    return *this;
  }

  inline uint64_t operator()(const string &str) {
    int64_t ret = 0;
    constexpr const uint8_t multiplier = 10U;
    // sum from head
    auto end = str.end();
    auto beg = str.begin();
    for (size_t max_range = 0;
         max_range != this->hash_range && beg != end;
         ++max_range, ++beg) {
      ret += static_cast<uint8_t>(*beg) * multiplier;
    }
    // sum from tail
    end = str.end(); --end;
    beg = str.begin();
    for (size_t max_range = this->hash_range;
         max_range != 0 && beg != end;
         --end, --max_range) {
      ret += static_cast<uint8_t>(*end) * multiplier;
    }
    // `ret` is sum of ASCII chars
    ret -= this->hash_range * multiplier * 2
           * (static_cast<int64_t>('a') - 6U);
    // (expecting) `ret` is deviation from 0, pos. and neg.
    // HACK: Use tuned sigmoid to map deviation range to index range
    double shived = (
      static_cast<double>(this->pool_size - 1)
      / ( 1.0 + exp(-2.8 * 1E-8 * static_cast<double>(this->pool_size) * ret) )
    );
    return static_cast<uint64_t>(shived);
  }

};



#endif
