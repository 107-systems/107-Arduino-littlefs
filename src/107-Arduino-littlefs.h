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
#include <optional>

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
  NO_DD_ENTRY = -51,                 // No entry found for given directory descriptor
};

enum class Type : int
{
  REG = LFS_TYPE_REG,
  DIR = LFS_TYPE_DIR,
};

typedef size_t FileHandle;
typedef size_t DirHandle;

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class FilesystemConfig
{
private:
  lfs_config _cfg;

public:
  typedef int (*ReadFuncPtr)(const struct lfs_config *, lfs_block_t , lfs_off_t, void *, lfs_size_t);
  typedef int (*ProgFuncPtr)(const struct lfs_config *, lfs_block_t, lfs_off_t, const void *, lfs_size_t);
  typedef int (*EraseFuncPtr)(const struct lfs_config *, lfs_block_t);
  typedef int (*SyncFuncPtr)(const struct lfs_config *);

  FilesystemConfig(ReadFuncPtr  read_func,
                   ProgFuncPtr  prog_func,
                   EraseFuncPtr erase_func,
                   SyncFuncPtr  sync_func,
                   lfs_size_t const read_size,
                   lfs_size_t const prog_size,
                   lfs_size_t const block_size,
                   lfs_size_t const block_count,
                   int32_t    const block_cycles,
                   lfs_size_t const cache_size,
                   lfs_size_t const lookahead_size)
  {
    memset(&_cfg, 0, sizeof(_cfg));

    _cfg.read  = read_func;
    _cfg.prog  = prog_func;
    _cfg.erase = erase_func;
    _cfg.sync  = sync_func;

    _cfg.read_size      = read_size;
    _cfg.prog_size      = prog_size;
    _cfg.block_size     = block_size;
    _cfg.block_count    = block_count;
    _cfg.block_cycles   = block_cycles;
    _cfg.cache_size     = cache_size;
    _cfg.lookahead_size = lookahead_size;
  }

  [[nodiscard]] lfs_config & raw_cfg() { return _cfg; }
};

class Filesystem
{
private:
  FilesystemConfig & _cfg;
  lfs_t _lfs;
  size_t _file_dsc_cnt;
  std::map<size_t, std::shared_ptr<lfs_file_t>> _file_desc_map;
  size_t _dir_dsc_cnt;
  std::map<size_t, std::shared_ptr<lfs_dir_t>> _dir_desc_map;
public:
  Filesystem(FilesystemConfig & cfg)
  : _cfg{cfg}
  , _file_dsc_cnt{0}
  , _dir_dsc_cnt{0}
  {
    memset(&_lfs, 0, sizeof(_lfs));
  }

#ifndef LFS_READONLY
  [[nodiscard]] std::optional<Error> format();
#endif
  [[nodiscard]] std::optional<Error> mount();
  [[nodiscard]] std::optional<Error> unmount();

#ifndef LFS_READONLY
  [[nodiscard]] std::optional<Error> remove(std::string const & path);
  [[nodiscard]] std::optional<Error> rename(std::string const & old_path, std::string const & new_path);
#endif

  [[nodiscard]] std::variant<Error, FileHandle> open (std::string const & path, OpenFlag const flags);
  [[nodiscard]] std::optional<Error>            sync (FileHandle const fd);
  [[nodiscard]] std::optional<Error>            close(FileHandle const fd);

  [[nodiscard]] std::variant<Error, size_t> read    (FileHandle const fd, void * read_buf, size_t const bytes_to_read);
#ifndef LFS_READONLY
  [[nodiscard]] std::variant<Error, size_t> write   (FileHandle const fd, void const * write_buf, size_t const bytes_to_write);
  [[nodiscard]] std::optional<Error>        truncate(FileHandle const fd, int const size);
#endif

  [[nodiscard]] std::variant<Error, size_t> tell  (FileHandle const fd);
  [[nodiscard]] std::variant<Error, size_t> size  (FileHandle const fd);
  [[nodiscard]] std::variant<Error, size_t> seek  (FileHandle const fd, int const offset, WhenceFlag const whence);
  [[nodiscard]] std::optional<Error>        rewind(FileHandle const fd);

#ifndef LFS_READONLY
  [[nodiscard]] std::optional<Error> mkdir(std::string const & path);
#endif

  [[nodiscard]] std::variant<Error, DirHandle> dir_open (std::string const & path);
  [[nodiscard]] std::optional<Error>           dir_close(DirHandle const dd);
  [[nodiscard]] std::variant<Error, size_t>    dir_read(DirHandle const dd, std::string & name, Type & type);
  [[nodiscard]] std::optional<Error>           dir_rewind(DirHandle const dd);

  [[nodiscard]] std::variant<Error, size_t> fs_size();
};

/**************************************************************************************
 * FREE FUNCTIONS
 **************************************************************************************/

constexpr OpenFlag operator | (OpenFlag const & lhs, OpenFlag const & rhs)
{
  return static_cast<OpenFlag>(static_cast<int>(lhs) | static_cast<int>(rhs));
}
/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* littlefs */

#endif /* _107_ARDUINO_LITTLEFS_H_ */
