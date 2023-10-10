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

static EEPROM_24LCxx eeprom(EEPROM_24LCxx_Type::LC64,
                            EEPROM_I2C_DEV_ADDR,
                            [](uint8_t const dev_addr) { Wire.beginTransmission(dev_addr); },
                            [](uint8_t const data) { Wire.write(data); },
                            []() { return Wire.endTransmission(); },
                            [](uint8_t const dev_addr, size_t const len) -> size_t { return Wire.requestFrom(dev_addr, len); },
                            []() { return Wire.available(); },
                            []() { return Wire.read(); });

static littlefs::FilesystemConfig filesystem_config
  (
    +[](const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) -> int
    {
      eeprom.read_page((block * c->block_size) + off, (uint8_t *)buffer, size);
      return LFS_ERR_OK;
    },
    +[](const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) -> int
    {
      eeprom.write_page((block * c->block_size) + off, (uint8_t const *)buffer, size);
      return LFS_ERR_OK;
    },
    +[](const struct lfs_config *c, lfs_block_t block) -> int
    {
      for(size_t off = 0; off < c->block_size; off += eeprom.page_size())
        eeprom.fill_page((block * c->block_size) + off, 0xFF);
      return LFS_ERR_OK;
    },
    +[](const struct lfs_config *c) -> int
    {
      return LFS_ERR_OK;
    },
    eeprom.page_size(),
    eeprom.page_size(),
    (eeprom.page_size() * 4), /* littlefs demands (erase) block size to exceed read/prog size. */
    eeprom.device_size() / (eeprom.page_size() * 4),
    500,
    eeprom.page_size(),
    eeprom.page_size()
  );
static littlefs::Filesystem filesystem(filesystem_config);

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);
  while (!Serial) { }
  delay(1000);

  Wire.begin();
  Serial.println(eeprom);

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

  // mount filesystem
  if (auto const err_mount = filesystem.mount(); err_mount.has_value())
  {
    Serial.print("Mounting failed with error code ");
    Serial.println(static_cast<int>(err_mount.value()));
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    (void)filesystem.format();
  }

  if (auto const err_mount = filesystem.mount(); err_mount.has_value())
  {
    Serial.print("Mounting failed again with error code ");
    Serial.println(static_cast<int>(err_mount.value()));
    return;
  }

  // read current count
  uint32_t boot_count = 0;
  auto const rc_open = filesystem.open("boot_count", littlefs::OpenFlag::RDWR | littlefs::OpenFlag::CREAT);
  if (std::holds_alternative<littlefs::Error>(rc_open))
  {
    Serial.print("open failed with error code ");
    Serial.println(static_cast<int>(std::get<littlefs::Error>(rc_open)));
    return;
  }
  littlefs::FileHandle const file_hdl = std::get<littlefs::FileHandle>(rc_open);
  (void)filesystem.read(file_hdl, &boot_count, sizeof(boot_count));

  // update boot count
  boot_count += 1;
  (void)filesystem.rewind(file_hdl);
  (void)filesystem.write(file_hdl, &boot_count, sizeof(boot_count));

  // remember the storage is not updated until the file is closed successfully
  (void)filesystem.close(file_hdl);

  auto const fs_size = filesystem.fs_size();
  Serial.print("fs_size(best effort): ");
  Serial.println(std::get<size_t>(fs_size));

  #ifndef LFS_READONLY
    // create some folders
    (void)filesystem.mkdir("folder1");
    (void)filesystem.mkdir("folder2");
  #endif

  // read contents of root folder
  auto const rc_dir_open = filesystem.dir_open("/");
  if (std::holds_alternative<littlefs::Error>(rc_dir_open))
  {
    Serial.print("dir_open failed with error code ");
    Serial.println(static_cast<int>(std::get<littlefs::Error>(rc_dir_open)));
    return;
  }

  Serial.println("root folder contents:");
  littlefs::DirHandle const dir_hdl = std::get<littlefs::DirHandle>(rc_dir_open);
  for(;;)
  {
    std::string name;
    littlefs::Type type;
    auto const rc_dir_read = filesystem.dir_read(dir_hdl, name, type);
    if (std::holds_alternative<littlefs::Error>(rc_dir_read))
    {
      // Error::NOENT signals end of dir contents reached
      if (std::get<littlefs::Error>(rc_dir_read) == littlefs::Error::NOENT)
      {
        (void)filesystem.dir_close(dir_hdl);
        break;
      }
      Serial.print("dir_read failed with error code ");
      Serial.println(static_cast<int>(std::get<littlefs::Error>(rc_dir_read)));
      return;
    }
    Serial.print(type == littlefs::Type::DIR ? "DIR" : "FILE");
    Serial.print('\t');
    Serial.print(name.c_str());
    Serial.print('\t');
    Serial.println(std::get<size_t>(rc_dir_read));
  }

  // release any resources we were using
  (void)filesystem.unmount();

  // print the boot count
  Serial.print("boot_count: ");
  Serial.println(boot_count);
}

void loop()
{

}
