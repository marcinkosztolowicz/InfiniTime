#pragma once
#include <cstdint>
#include <bitset>
#include "components/fs/FS.h"
#include "displayapp/apps/Apps.h"

namespace Pinetime {
  namespace Controllers {
    class SettingsHrs {
    public:
      
      SettingsHrs(Pinetime::Controllers::FS& fs);
      SettingsHrs(const SettingsHrs&) = delete;
      SettingsHrs& operator=(const SettingsHrs&) = delete;
      SettingsHrs(SettingsHrs&&) = delete;
      SettingsHrs& operator=(SettingsHrs&&) = delete;

      void Init();
     

      int8_t GetHrsEnable() const {
        return settings.Hrs3300enable;
      }

      int8_t GetHrsDrive() const {
        return settings.Hrs3300drive;
      }


    private:
      Pinetime::Controllers::FS& fs;

      static constexpr uint32_t settingsVersion = 0x0001;

      struct SettingsData {
        uint32_t version = settingsVersion;
        uint8_t Hrs3300enable = 0x50;
        uint8_t Hrs3300drive = 0x2f;
      };

      SettingsData settings;


  

      void LoadSettingsFromFile();
      void SaveSettingsToFile();
      uint8_t SettingsFileExists();
    };
  }
}
