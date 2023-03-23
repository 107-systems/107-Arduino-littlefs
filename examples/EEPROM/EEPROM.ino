/*
 * Basic example demonstrating how to use this library with an 24LCxx EEPROM.
 * Heavily inspired by the official littlefs minimal example.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Wire.h>

#include <107-Arduino-littlefs.h>
#include <107-Arduino-24LCxx.hpp>

/**************************************************************************************
 * GLOBAL CONSTANTS
 **************************************************************************************/

static uint8_t const EEPROM_I2C_DEV_ADDR = 0x50;

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

static lfs_t lfs;
static lfs_file_t file;
static struct lfs_config cfg;

EEPROM_24LCxx eeprom(EEPROM_24LCxx_Type::LC64,
                     EEPROM_I2C_DEV_ADDR,
                     [](size_t const dev_addr) { Wire.beginTransmission(dev_addr); },
                     [](uint8_t const data) { Wire.write(data); },
                     []() { return Wire.endTransmission(); },
                     [](uint8_t const dev_addr, size_t const len) -> size_t { return Wire.requestFrom(dev_addr, len); },
                     []() { return Wire.available(); },
                     []() { return Wire.read(); });

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }
  delay(1000);

  Wire.begin();

  /* Commenting in below comment enables "erasing" of the EEPROM.
   * littlefs as a flash filesystem expects the memory to read
   * 0xFF in erased state as this is the nature of the beast for
   * flash memory.
   */
// #define ERASE_EEPROM
#ifdef ERASE_EEPROM
  size_t const NUM_PAGES = eeprom.device_size() / eeprom.page_size();
  for(size_t page = 0; page < NUM_PAGES; page++)
    eeprom.fill_page(page * eeprom.page_size(), 0xFF);
#endif /* ERASE_EEPROM */

  // block device operations
  cfg.read  = +[](const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) -> int
    {
      eeprom.read_page((block * c->block_size) + off, (uint8_t *)buffer, size);
      return LFS_ERR_OK;
    };
  cfg.prog  = +[](const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) -> int
    {
      eeprom.write_page((block * c->block_size) + off, (uint8_t const *)buffer, size);
      return LFS_ERR_OK;
    };
  cfg.erase = +[](const struct lfs_config *c, lfs_block_t block) -> int
    {
      for(size_t off = 0; off < c->block_size; off += eeprom.page_size())
        eeprom.fill_page((block * c->block_size) + off, 0xFF);
      return LFS_ERR_OK;
    };
  cfg.sync  = +[](const struct lfs_config *c) -> int
    {
      return LFS_ERR_OK;
    };

  // block device configuration
  cfg.read_size      = eeprom.page_size();
  cfg.prog_size      = eeprom.page_size();
  cfg.block_size     = (eeprom.page_size() * 4); /* littlefs demands (erase) block size to exceed read/prog size. */
  cfg.block_count    = eeprom.device_size() / cfg.block_size;
  cfg.block_cycles   = 500;
  cfg.cache_size     = eeprom.page_size();
  cfg.lookahead_size = eeprom.page_size();

  // mount the filesystem
  int err_mount = lfs_mount(&lfs, &cfg);

  // reformat if we can't mount the filesystem
  // this should only happen on the first boot
  if (err_mount) {
    Serial.println(eeprom);
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
