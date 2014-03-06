#include "pylacore.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <sys/mman.h> // windows?
#include <fcntl.h>

/* Wrapper functions for Python to work around pass-by-reference. */
chunk process(operation *op, chunk input) {
  chunk output;
  op->process(output, input);
  return output;
}
chunk read(source *src) {
  chunk output;
  src->read(output);
  return output;
}
void write(sink *snk, chunk input) {
  snk->write(input);
}



/* Buffers */

void hole::read(chunk& output) {
  chunk empty; 
  output = empty;
}
void hole::write(chunk& input) {
  std::cerr << "drop: " << input.size() << std::endl;
}
hole::~hole() {
  LOG("~hole()\n");
}


memory::memory() {
}
void memory::write(chunk& input) {
  _mutex.lock();
  if (!input.empty()) {
    _buf.push_back(input);
  }
  _mutex.unlock();
}
void memory::read(chunk& output) {
   _mutex.lock();
   
   if (_buf.empty()) {
     chunk empty;
     output = empty;
   }
   else {
     output = _buf.front();
     _buf.pop_front();
   }
   _mutex.unlock();
}

memory::~memory() {
}

// http://mentablog.soliveirajr.com/2012/12/asynchronous-logging-versus-memory-mapped-files/

// http://stackoverflow.com/questions/1201261/what-is-the-fastest-method-for-high-performance-sequential-file-i-o-in-c

file::file(const char *filename, uint64_t size) {
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
file::~file() {
  munmap(_buf, _size);
  fclose(_store);
}
void file::write(chunk& input) {
  int chunk_size = input.size();
  if (chunk_size > _size - _write_index) {
    LOG("WARNING: buffer overflow\n");
    chunk_size = _size - _write_index;
  }

  memcpy(_buf + _write_index, &input[0], chunk_size);
  _write_index += chunk_size;
}


// FIXME: just read max chunk size
// or is it possible to create a vector with different underlying store?
// http://stackoverflow.com/questions/14807192/c-can-i-create-a-stdvector-to-manage-an-array-of-elements-specific-known-add
void file::read(chunk& output) {
  uint64_t chunk_size = _write_index - _read_index;
  output.resize(chunk_size);
  fflush(_store); // make sure data hits the disk before reading it
  memcpy(&output[0], _buf + _read_index, chunk_size);
  _read_index += chunk_size;
}
void file::clear() {
  bzero(_buf, _size);
}
