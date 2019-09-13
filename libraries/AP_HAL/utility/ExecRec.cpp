#include "ExecRec.h"

ExecRec* ExecRec::singleTon = NULL;
void ExecRec::record(const char* fileName, const char* funcName, int lineNo)
{
  std::string file(fileName), func(funcName);
  std::string key = file + "-" + func + "-" + std::to_string(lineNo);
  it_type it = executionMap.find(key);
  if (it != executionMap.end()) {
    executionMap[key]++;
  } else {
    std::cout << key << std::endl;
    executionMap[key] = 1;
  }
}

void ExecRec::printResult()
{
  for (it_type it = executionMap.begin(); it != executionMap.end(); it++) {
    std::cout << it->first << ": " << it->second << std::endl;
    // outfile << it->first << ":" << it->second << std::endl;
  }
}

ExecRec::ExecRec(){
  // outfile.open("/home/cedric/ArduPilot/experiment/output/PA/0/trace.txt");
}

ExecRec::ExecRec(ExecRec const&)
{

}

ExecRec* ExecRec::operator=(ExecRec const*)
{
  return NULL;
}

ExecRec::~ExecRec()
{
  // outfile.close();
  printResult();
}

