
#include <cryptopp/crc.h>
#include <cryptopp/hex.h>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <filesystem>
#include "mt_queue.hpp"
#include "hashlog.h"
#include "map_file.h"

namespace fs = std::filesystem;
namespace bi = boost::interprocess;

int main(int argc, char *argv[])
{
  try
  {
    if(argc < 3)
    {
      std::cerr << "Need file name for hashing and output file" << std::endl;
      return EXIT_FAILURE;
    }

    const std::string hashfile_path = fs::canonical(argv[1]).string();
    const std::string logfile_path = argv[2];

    std::size_t READ_BLOCK_SIZE = 1024 * 1024; //1Mb
    if(argc == 4)
    {
      READ_BLOCK_SIZE = std::stoi(argv[3]);
    }

    std::cout << "start calculation hashes of " << hashfile_path << std::endl;
    std::cout << "output to " << logfile_path << std::endl;
    std::cout << "block size is " << READ_BLOCK_SIZE << std::endl;

    hashlog lg(logfile_path);
    mt_queue<block> queue;

    const bi::mode_t mode = boost::interprocess::read_only;
    bi::file_mapping fm(hashfile_path.c_str(), mode);
    bi::mapped_region region(fm, mode, 0, 0);

    map_file mapping(region.get_address(), fs::file_size(hashfile_path), READ_BLOCK_SIZE);

    auto calc_hash = [&queue, &lg]()
    {
      while(true)
      {
        try
        {
          const auto mem_block = queue.pop_and_wait();
          if(!mem_block)
            break;

          CryptoPP::CRC32 crc;
          std::string digest;

          CryptoPP::StringSource ss((const CryptoPP::byte *) mem_block->data_ptr, mem_block->size, true,
                                    new CryptoPP::HashFilter(crc,
                                                             new CryptoPP::HexEncoder(
                                                                     new CryptoPP::StringSink(digest)
                                                             )));
          lg.write(std::move(digest));
        }
        catch(const std::exception &e)
        {
          std::cerr << e.what() << std::endl;
        }

      }

    };

    unsigned int n_workers = std::thread::hardware_concurrency();
    std::cout << "run on " << n_workers << " threads" << std::endl;
    std::vector<std::thread> workers(n_workers);
    for(unsigned int i = 0; i < n_workers; ++i)
    {
      workers[i] = std::thread(calc_hash);
    }

    while(true)
    {
      auto data = mapping.get_next_block();
      if(data)
        queue.post(std::move(data));
      else
      {
        queue.post(nullptr);
        break;
      }
    }

    for(auto &worker: workers)
    {
      worker.join();
    }

    std::cout << "Done" << std::endl;
  }

  catch(const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }


  return 0;
}