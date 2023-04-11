/**
 * This software is distributed under the terms of the MIT License.
 * Copyright (c) 2023 LXRobotics.
 * Author: Alexander Entinger <alexander.entinger@lxrobotics.com>
 * Contributors: https://github.com/107-systems/107-Arduino-littlefs/graphs/contributors.
 */

#ifndef _107_ARDUINO_LITTLEFS_H_
#define _107_ARDUINO_LITTLEFS_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "littlefs-v2.5.1/lfs.h"

#include <map>
#include <memory>
#include <string>
#include <variant>

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

namespace littlefs
{

/**************************************************************************************
 * TYPEDEF
 **************************************************************************************/

enum class OpenFlag : int
{
  RDONLY  = LFS_O_RDONLY,
#ifndef LFS_READONLY
  WRONLY  = LFS_O_WRONLY,
  RDWR    = LFS_O_RDWR,
  CREAT   = LFS_O_CREAT,
  EXCL    = LFS_O_EXCL,
  TRUNC   = LFS_O_TRUNC,
  APPEND  = LFS_O_APPEND,
#endif
};

enum class WhenceFlag : int
{
  SET = LFS_SEEK_SET,
  CUR = LFS_SEEK_CUR,
  END = LFS_SEEK_END,
};

enum class Error : int
{
  OK          = LFS_ERR_OK,
  IO          = LFS_ERR_IO,
  CORRUPT     = LFS_ERR_CORRUPT,
  NOENT       = LFS_ERR_NOENT,
  EXIST       = LFS_ERR_EXIST,
  NOTDIR      = LFS_ERR_NOTDIR,
  ISDIR       = LFS_ERR_ISDIR,
  NOTEMPTY    = LFS_ERR_NOTEMPTY,
  BADF        = LFS_ERR_BADF,
  FBIG        = LFS_ERR_FBIG,
  INVAL       = LFS_ERR_INVAL,
  NOSPC       = LFS_ERR_NOSPC,
  NOMEM       = LFS_ERR_NOMEM,
  NOATTR      = LFS_ERR_NOATTR,
  NAMETOOLONG = LFS_ERR_NAMETOOLONG,
  NO_FD_ENTRY = -50,                 // No entry found for given file descriptor
};

typedef size_t FileHandle;

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class Filesystem
{
private:
  lfs_t _lfs;
  lfs_config & _cfg;
  size_t _file_dsc_cnt;
  std::map<size_t, std::shared_ptr<lfs_file_t>> _file_desc_map;
public:
  Filesystem(lfs_config & cfg)
  : _cfg{cfg}
  , _file_dsc_cnt{0}
  {
    memset(&_lfs, 0, sizeof(_lfs));
  }

#ifndef LFS_READONLY
  [[nodiscard]] Error format() { return static_cast<Error>(lfs_format(&_lfs, &_cfg)); }
#endif
  [[nodiscard]] Error mount() { return static_cast<Error>(lfs_mount(&_lfs, &_cfg)); }
  [[nodiscard]] Error unmount() { return static_cast<Error>(lfs_unmount(&_lfs)); }

#ifndef LFS_READONLY
  [[nodiscard]] Error remove(std::string const & path) { return static_cast<Error>(lfs_remove(&_lfs, path.c_str())); }
  [[nodiscard]] Error rename(std::string const & old_path, std::string const & new_path) { return static_cast<Error>(lfs_rename(&_lfs, old_path.c_str(), new_path.c_str())); }
#endif

  [[nodiscard]] std::variant<Error, FileHandle> open (std::string const & path, int const flags);
  [[nodiscard]] Error                           sync (FileHandle const fd);
  [[nodiscard]] Error                           close(FileHandle const fd);

  [[nodiscard]] std::variant<Error, size_t> read    (FileHandle const fd, void * read_buf, size_t const bytes_to_read);
#ifndef LFS_READONLY
  [[nodiscard]] std::variant<Error, size_t> write   (FileHandle const fd, void const * write_buf, size_t const bytes_to_write);
  [[nodiscard]] Error                       truncate(FileHandle const fd, int const size);
#endif

  [[nodiscard]] std::variant<Error, size_t> tell  (FileHandle const fd);
  [[nodiscard]] std::variant<Error, size_t> size  (FileHandle const fd);
  [[nodiscard]] std::variant<Error, size_t> seek  (FileHandle const fd, int const offset, WhenceFlag const whence);
  [[nodiscard]] Error                       rewind(FileHandle const fd);
};

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* littlefs */

#endif /* _107_ARDUINO_LITTLEFS_H_ */
