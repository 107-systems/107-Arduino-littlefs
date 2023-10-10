/**
 * This software is distributed under the terms of the MIT License.
 * Copyright (c) 2023 LXRobotics.
 * Author: Alexander Entinger <alexander.entinger@lxrobotics.com>
 * Contributors: https://github.com/107-systems/107-Arduino-littlefs/graphs/contributors.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "107-Arduino-littlefs.h"

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

namespace littlefs
{

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

#ifndef LFS_READONLY
std::optional<Error> Filesystem::format()
{
  if (auto const err = lfs_format(&_lfs, &_cfg.raw_cfg()); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}
#endif

std::optional<Error> Filesystem::mount()
{
  if (auto const err = lfs_mount(&_lfs, &_cfg.raw_cfg()); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}

std::optional<Error> Filesystem::unmount()
{
  if (auto const err = lfs_unmount(&_lfs); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}

#ifndef LFS_READONLY
std::optional<Error> Filesystem::remove(std::string const & path)
{
  if (auto const err = lfs_remove(&_lfs, path.c_str()); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}

std::optional<Error> Filesystem::rename(std::string const & old_path, std::string const & new_path)
{
  if (auto const err = lfs_rename(&_lfs, old_path.c_str(), new_path.c_str()); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}
#endif

std::variant<Error, FileHandle> Filesystem::open(std::string const & path, OpenFlag const flags)
{
  auto file_hdl = std::make_shared<lfs_file_t>();

  int const rc = lfs_file_open(&_lfs, file_hdl.get(), path.c_str(), static_cast<int>(flags));

  if (rc < LFS_ERR_OK)
    return static_cast<Error>(rc);

  _file_desc_map[_file_dsc_cnt++] = file_hdl;
  return (_file_dsc_cnt - 1);
}

std::variant<Error, size_t> Filesystem::read(FileHandle const fd, void * read_buf, size_t const bytes_to_read)
{
  auto iter = _file_desc_map.find(fd);
  if (iter == _file_desc_map.end())
    return Error::NO_FD_ENTRY;

  int const rc = lfs_file_read(&_lfs, iter->second.get(), read_buf, bytes_to_read);

  if (rc < LFS_ERR_OK)
    return static_cast<Error>(rc);

  return static_cast<size_t>(rc);
}

#ifndef LFS_READONLY
std::variant<Error, size_t> Filesystem::write(FileHandle const fd, void const * write_buf, size_t const bytes_to_write)
{
  auto iter = _file_desc_map.find(fd);
  if (iter == _file_desc_map.end())
    return Error::NO_FD_ENTRY;

  int const rc = lfs_file_write(&_lfs, iter->second.get(), write_buf, bytes_to_write);

  if (rc < LFS_ERR_OK)
    return static_cast<Error>(rc);

  return static_cast<size_t>(rc);
}
#endif

#ifndef LFS_READONLY
std::optional<Error> Filesystem::truncate(FileHandle const fd, int const size)
{
  auto iter = _file_desc_map.find(fd);
  if (iter == _file_desc_map.end())
    return Error::NO_FD_ENTRY;

  if (auto const err = lfs_file_truncate(&_lfs, iter->second.get(), size); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}
#endif

std::variant<Error, size_t> Filesystem::tell(FileHandle const fd)
{
  auto iter = _file_desc_map.find(fd);
  if (iter == _file_desc_map.end())
    return Error::NO_FD_ENTRY;

  int const rc = lfs_file_tell(&_lfs, iter->second.get());

  if (rc < LFS_ERR_OK)
    return static_cast<Error>(rc);

  return static_cast<size_t>(rc);
}

std::variant<Error, size_t> Filesystem::size(FileHandle const fd)
{
  auto iter = _file_desc_map.find(fd);
  if (iter == _file_desc_map.end())
    return Error::NO_FD_ENTRY;

  int const rc = lfs_file_size(&_lfs, iter->second.get());

  if (rc < LFS_ERR_OK)
    return static_cast<Error>(rc);

  return static_cast<size_t>(rc);
}

std::variant<Error, size_t> Filesystem::seek(FileHandle const fd, int const offset, WhenceFlag const whence)
{
  auto iter = _file_desc_map.find(fd);
  if (iter == _file_desc_map.end())
    return Error::NO_FD_ENTRY;

  int const rc = lfs_file_seek(&_lfs, iter->second.get(), offset, static_cast<int>(whence));

  if (rc < LFS_ERR_OK)
    return static_cast<Error>(rc);

  return static_cast<size_t>(rc);
}

std::optional<Error> Filesystem::rewind(FileHandle const fd)
{
  auto iter = _file_desc_map.find(fd);
  if (iter == _file_desc_map.end())
    return Error::NO_FD_ENTRY;

  if (auto const err = lfs_file_rewind(&_lfs, iter->second.get()); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}

std::optional<Error> Filesystem::sync(FileHandle const fd)
{
  auto iter = _file_desc_map.find(fd);
  if (iter == _file_desc_map.end())
    return Error::NO_FD_ENTRY;

  if (auto const err = lfs_file_sync(&_lfs, iter->second.get()); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}

std::optional<Error> Filesystem::close(FileHandle const fd)
{
  auto iter = _file_desc_map.find(fd);
  if (iter == _file_desc_map.end())
    return Error::NO_FD_ENTRY;

  _file_desc_map.erase(fd);

  if (auto const err = lfs_file_close(&_lfs, iter->second.get()); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}

#ifndef LFS_READONLY
std::optional<Error> Filesystem::mkdir(std::string const & path)
{
  if (auto const err = lfs_mkdir(&_lfs, path.c_str()); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}
#endif

std::variant<Error, DirHandle> Filesystem::dir_open(std::string const & path)
{
  auto dir_hdl = std::make_shared<lfs_dir_t>();

  int const rc = lfs_dir_open(&_lfs, dir_hdl.get(), path.c_str());

  if (rc < LFS_ERR_OK)
    return static_cast<Error>(rc);

  _dir_desc_map[_dir_dsc_cnt++] = dir_hdl;
  return (_dir_dsc_cnt - 1);
}

std::optional<Error> Filesystem::dir_close(DirHandle const dd)
{
  auto iter = _dir_desc_map.find(dd);
  if (iter == _dir_desc_map.end())
    return Error::NO_DD_ENTRY;

  _dir_desc_map.erase(dd);

  if (auto const err = lfs_dir_close(&_lfs, iter->second.get()); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}

std::variant<Error, size_t> Filesystem::dir_read(DirHandle const dd, std::string & name, Type & type)
{
  auto iter = _dir_desc_map.find(dd);
  if (iter == _dir_desc_map.end())
    return Error::NO_DD_ENTRY;

  lfs_info info;
  int const rc = lfs_dir_read(&_lfs, iter->second.get(), &info);

  // Note: lfs_dir_read returns false (0) when no more entries, true (1) on success,
  // and possibly some lfs_error.
  if (rc == 0)
    return Error::NOENT;

  if (rc < LFS_ERR_OK)
    return static_cast<Error>(rc);

  name = std::string(info.name);
  type = static_cast<Type>(info.type);

  return type == Type::REG ? static_cast<size_t>(info.size) : 0;
}

std::optional<Error> Filesystem::dir_rewind(FileHandle const dd)
{
  auto iter = _dir_desc_map.find(dd);
  if (iter == _dir_desc_map.end())
    return Error::NO_DD_ENTRY;

  if (auto const err = lfs_dir_rewind(&_lfs, iter->second.get()); err != LFS_ERR_OK)
    return static_cast<Error>(err);

  return std::nullopt;
}

std::variant<Error, size_t> Filesystem::fs_size()
{
  int const rc = lfs_fs_size(&_lfs);

  if (rc < LFS_ERR_OK)
    return static_cast<Error>(rc);

  return static_cast<size_t>(rc);
}

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* littlefs */
