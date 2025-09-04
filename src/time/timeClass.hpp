#ifndef TIMECLASS_HPP
#define TIMECLASS_HPP

class timeClass {
public:
  unsigned int HOUR;
  unsigned int MINUTE;
  unsigned int SECOND;

  timeClass(const unsigned int HOUR_, const unsigned int MINUTE_, const unsigned int SECOND_) {
    HOUR = HOUR_;
    MINUTE = MINUTE_;
    SECOND = SECOND_;
  }
};

#endif // TIMECLASS_HPP