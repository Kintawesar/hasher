#include "hashlog.h"
#include <cstring>

void hashlog::write(std::string&& str)
{
  static constexpr auto end_of_line("\n");

  std::lock_guard lk(m_mtx);
  m_file.write(str.c_str(), str.size());
  m_file.write(end_of_line, std::strlen(end_of_line));
}

hashlog::hashlog(const std::string& file_path)
 : m_file(file_path, std::ios::app | std::ios::binary)
{
  if(!m_file)
  {
    throw std::runtime_error("cant open or create file " + file_path);
  }

}
