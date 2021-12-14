#pragma once
#include <memory>
#include <cstdint>

struct block
{
  std::uintptr_t* data_ptr = nullptr;
  std::size_t size = 0;
};

class map_file
{
public:
  map_file(void *region_ptr, std::size_t file_size, std::size_t block_size);
  std::unique_ptr<block> get_next_block();
private:

  std::uintptr_t *m_file_ptr;
  const std::size_t m_block_size;
  const std::size_t m_file_size;
  std::size_t m_red_bytes;
  std::size_t offset;


};
