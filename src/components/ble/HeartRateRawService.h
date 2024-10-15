#pragma once
#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#undef max
#undef min
#include <atomic>

namespace Pinetime {
  namespace Controllers {
    class Ppg;

    class HeartRateRawService {
    public:
      HeartRateRawService(Controllers::Ppg& ppg);
      void Init();
      int OnHeartRateRawRequested(ble_gatt_access_ctxt* context);
      static constexpr uint16_t heartRateRawServiceId {0x180E};
      static constexpr ble_uuid16_t heartRateRawServiceUuid {.u {.type = BLE_UUID_TYPE_16}, .value = heartRateRawServiceId};

    private:
      
      Controllers::Ppg& ppg;
      static constexpr uint16_t heartRateRawMeasurementId {0x2A39};
      static constexpr ble_uuid16_t heartRateRawMeasurementUuid {.u {.type = BLE_UUID_TYPE_16}, .value = heartRateRawMeasurementId};
      struct ble_gatt_chr_def characteristicDefinition[2];
      struct ble_gatt_svc_def serviceDefinition[2];
      
    };
  }
}
