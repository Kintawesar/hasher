
#include "map_file.h"
#include <iostream>

map_file::map_file(void *region_ptr, std::size_t file_size, std::size_t block_size)
        : m_file_ptr(static_cast<std::uintptr_t*>(region_ptr))
        , m_block_size(block_size)
        , m_file_size(file_size)
        , m_red_bytes(0)
        , offset(0)
{}


std::unique_ptr<block>  map_file::get_next_block()
{
  if(m_red_bytes == m_file_size)
  {
    return nullptr;
  }

  std::uintptr_t *ptr = m_file_ptr + offset;
  std::size_t s_block = 0;

  if(m_red_bytes + m_block_size > m_file_size)
  {
    s_block = m_file_size - m_red_bytes;
  }
  else
  {
    s_block = m_block_size;
  }

  m_red_bytes += s_block;
  offset += m_block_size / sizeof(uintptr_t);

  return std::make_unique<block>(block{ptr, s_block});
}