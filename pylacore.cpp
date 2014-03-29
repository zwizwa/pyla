#include "pylacore.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
// ???
#else
#include <sys/mman.h>
#include <fcntl.h>
#endif



/* Buffers */

boost::shared_ptr<chunk> hole::read() {
  boost::shared_ptr<chunk> output = boost::shared_ptr<chunk>(new chunk());
  return output;
}
void hole::write(boost::shared_ptr<chunk> input) {
  std::cerr << "drop: " << input->size() << std::endl;
}
hole::~hole() {
  // LOG("~hole()\n");
}


memory::memory() : _log(NULL) {
}
void memory::set_log(const char *filename) {
  _mutex.lock();
  if(_log) {
    fclose(_log);
  }
  _log = fopen(filename, "a+");
  if (!_log) {
    LOG("WARNING: Can't open file %s\n", filename);
  }
  _mutex.unlock();
}
void memory::write(boost::shared_ptr<chunk> input) {
  _mutex.lock();
  if (!input->empty()) {
    _buf.push_back(input);
    if (_log) {
      fwrite(&((*input)[0]), 1, input->size(), _log);
    }
  }
  _mutex.unlock();
}
boost::shared_ptr<chunk> memory::read() {
   boost::shared_ptr<chunk> output;
   _mutex.lock();
   
   if (_buf.empty()) {
     output = boost::shared_ptr<chunk>(new chunk());
   }
   else {
     output = _buf.front();
     _buf.pop_front();
   }
   _mutex.unlock();
   return output;
}

memory::~memory() {
}


#if defined(_WIN32)
// ???
memmap::memmap(const char *filename, uint64_t size) {}
memmap::~memmap() {}
void memmap::write(boost::shared_ptr<chunk> input) {}
void memmap::clear()  {}
boost::shared_ptr<chunk> memmap::read(){
	boost::shared_ptr<chunk> p = boost::shared_ptr<chunk>(new chunk());
	return p;
}
#else
// http://mentablog.soliveirajr.com/2012/12/asynchronous-logging-versus-memory-mapped-files/

// http://stackoverflow.com/questions/1201261/what-is-the-fastest-method-for-high-performance-sequential-file-i-o-in-c

memmap::memmap(const char *filename, uint64_t size) {
  char c = 0;
  _write_index = _read_index = 0;
  _size = size;
  _store = fopen(filename, "w+");
  if (!_store) {
    LOG("Can't open file %s\n", filename);
    throw filename;
  }
  uint8_t sum;

  fseek(_store, _size-1, SEEK_SET);
  fwrite(&c, 1, 1, _store); // write a single byte to set size
  fseek(_store, 0, SEEK_CUR);

  int fd = fileno(_store);
  _buf = (uint8_t *)mmap(0, _size,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED, fd, 0);

  posix_fadvise(fd, 0, _size, 
                POSIX_FADV_SEQUENTIAL |
                POSIX_FADV_NOREUSE);
  // madvise( MADV_MERGEABLE )
                
}
memmap::~memmap() {
  munmap(_buf, _size);
  fclose(_store);
}
void memmap::write(boost::shared_ptr<chunk> input) {
  int chunk_size = input->size();
  if (chunk_size > _size - _write_index) {
    LOG("WARNING: buffer overflow\n");
    chunk_size = _size - _write_index;
  }

  memcpy(_buf + _write_index, &((*input)[0]), chunk_size);
  _write_index += chunk_size;
}


// FIXME: just read max chunk size
// or is it possible to create a vector with different underlying store?
// http://stackoverflow.com/questions/14807192/c-can-i-create-a-stdvector-to-manage-an-array-of-elements-specific-known-add
boost::shared_ptr<chunk> memmap::read() {
  uint64_t chunk_size = _write_index - _read_index;
  boost::shared_ptr<chunk>output = boost::shared_ptr<chunk>(new chunk());
  fflush(_store); // make sure data hits the disk before reading it

  output->assign(_buf + _read_index,
                 _buf + _read_index + chunk_size);
  _read_index += chunk_size;
  return output;
}
void memmap::clear() {
  bzero(_buf, _size);
}
#endif


