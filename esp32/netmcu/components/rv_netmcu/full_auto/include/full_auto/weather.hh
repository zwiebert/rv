#pragma once

#include <webapp/content.h>
/**
 * \brief  call this once at each full hour (+-5min).
 *
 */
bool fa_poll_weather_full_hour();

class FaContentReader final: public ContentReader {
public:
  virtual int open(const char *name, const char *query);
  virtual int read(int fd, char *buf, unsigned buf_size);
  virtual int close(int fd);
private:
  static constexpr unsigned OUR_NMB_FILES = 1;
  struct FileData {
  int bytes_read = 0;
  bool is_open = false; // FIXME: only one open file allowed for now
  } m_file_data[OUR_NMB_FILES];
};




