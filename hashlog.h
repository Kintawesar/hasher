#pragma  once

#include <string>
#include <fstream>
#include <mutex>

class hashlog
{
public:
  explicit hashlog(const std::string& file);
  void write(std::string&& str);

private:
  std::mutex m_mtx;
  std::ofstream m_file;
};

