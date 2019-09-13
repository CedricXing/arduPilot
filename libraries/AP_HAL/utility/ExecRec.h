#include <map>
#include <string>
#include <iostream>
#include <fstream>

#define LOG_EXECUTION
#ifdef LOG_EXECUTION
  #define EXECUTE_MARK() ExecRec::Instance()->record(__FILE__, __func__, __LINE__)
#else
  #define EXECUTE_MARK()
#endif

typedef std::map<std::string, int>::iterator it_type;

class ExecRec {
public:
  static ExecRec* Instance() {
      if(singleTon == NULL)
        singleTon = new ExecRec();
      return singleTon;
  }

  void record(const char* fileName, const char* funcName, int lineNo);
  void printResult();

private:
  std::map<std::string, int> executionMap;
  static ExecRec *singleTon;
  // std::ofstream outfile;
  ExecRec();                            // ctor hidden
  ExecRec(ExecRec const&);            // copy ctor hidden
  ExecRec* operator=(ExecRec const*); // assign op. hidden
  ~ExecRec();                           // dtor hidden
};

