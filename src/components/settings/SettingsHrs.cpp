#include "components/settings/SettingsHrs.h"
#include <cstdlib>
#include <cstring>

using namespace Pinetime::Controllers;

SettingsHrs::SettingsHrs(Pinetime::Controllers::FS& fs) : fs {fs} {
}

void SettingsHrs::Init() {

  // Load default settings from Flash
  if(SettingsFileExists() == 1){
    LoadSettingsFromFile();
  }else{
    SaveSettingsToFile();
  }
}


void SettingsHrs::LoadSettingsFromFile() {
  SettingsData bufferSettings;
  lfs_file_t settingsFile;

  if (fs.FileOpen(&settingsFile, "/settingshrs.dat", LFS_O_RDONLY) != LFS_ERR_OK) {
    return;
  }
  fs.FileRead(&settingsFile, reinterpret_cast<uint8_t*>(&bufferSettings), sizeof(settings));
  fs.FileClose(&settingsFile);
  if (bufferSettings.version == settingsVersion) {
    settings = bufferSettings;
  }
}

void SettingsHrs::SaveSettingsToFile() {
  lfs_file_t settingsFile;

  if (fs.FileOpen(&settingsFile, "/settingshrs.dat", LFS_O_WRONLY | LFS_O_CREAT) != LFS_ERR_OK) {
    return;
  }
  fs.FileWrite(&settingsFile, reinterpret_cast<uint8_t*>(&settings), sizeof(settings));
  fs.FileClose(&settingsFile);
}

uint8_t SettingsHrs::SettingsFileExists() {
    struct lfs_info info;
    int res = fs.Stat( "/settingshrs.dat", &info);
    if (res == LFS_ERR_OK) {
        // File exists
        return 1;
    } else if (res == LFS_ERR_NOENT) {
        // File does not exist
        return 0;
    } else {
        // Some other error occurred
        return -1;
    }
}