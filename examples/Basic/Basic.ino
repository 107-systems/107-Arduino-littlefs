/*
 * Basic example demonstrating how to use this library. Heavily inspired by the official littlefs minimal example.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <107-Arduino-24LCxx-littlefs.h>

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

static lfs_t lfs;
static lfs_file_t file;

const struct lfs_config cfg =
{
  // block device operations
  .read  = nullptr,
  .prog  = nullptr,
  .erase = nullptr,
  .sync  = nullptr,

  // block device configuration
  .read_size = 16,
  .prog_size = 16,
  .block_size = 4096,
  .block_count = 128,
  .block_cycles = 500,
  .cache_size = 16,
  .lookahead_size = 16,
};

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }

  // mount the filesystem
  int err_mount = lfs_mount(&lfs, &cfg);

  // reformat if we can't mount the filesystem
  // this should only happen on the first boot
  if (err_mount) {
    Serial.print("Mounting failed with error code "); Serial.println(err_mount);
    lfs_format(&lfs, &cfg);
  }

  err_mount = lfs_mount(&lfs, &cfg);
  if (err_mount) {
    Serial.print("Mounting failed again with error code "); Serial.println(err_mount);
    return;
  }

  // read current count
  uint32_t boot_count = 0;
  lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
  lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

  // update boot count
  boot_count += 1;
  lfs_file_rewind(&lfs, &file);
  lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

  // remember the storage is not updated until the file is closed successfully
  lfs_file_close(&lfs, &file);

  // release any resources we were using
  lfs_unmount(&lfs);

  // print the boot count
  Serial.print("boot_count: ");
  Serial.println(boot_count);
}

void loop()
{

}
