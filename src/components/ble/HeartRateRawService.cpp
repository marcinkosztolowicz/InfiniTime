#include "components/ble/HeartRateRawService.h"
#include "components/heartrate/Ppg.h"
#include <nrf_log.h>

using namespace Pinetime::Controllers;

constexpr ble_uuid16_t HeartRateRawService::heartRateRawServiceUuid;
constexpr ble_uuid16_t HeartRateRawService::heartRateRawMeasurementUuid;

namespace {
  int HeartRateRawServiceCallback(uint16_t /*conn_handle*/, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
    NRF_LOG_INFO("attrhandle = %d", attr_handle);
    auto* heartRateRawService = static_cast<HeartRateRawService*>(arg);
    return heartRateRawService->OnHeartRateRawRequested( ctxt);
  }
}

// TODO Refactoring - remove dependency to SystemTask
HeartRateRawService::HeartRateRawService(Controllers::Ppg& ppg)
  : ppg {ppg},
    characteristicDefinition {{.uuid = &heartRateRawMeasurementUuid.u,
                               .access_cb = HeartRateRawServiceCallback,
                               .arg = this,
                               .flags = BLE_GATT_CHR_F_READ},
                               
                              {0}},
    serviceDefinition {
      {/* Heart rate raw data Service */
       .type = BLE_GATT_SVC_TYPE_PRIMARY,
       .uuid = &heartRateRawServiceUuid.u,
       .characteristics = characteristicDefinition},
      {0},
    }{}

void HeartRateRawService::Init() {
  int res = 0;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);
}

int HeartRateRawService::OnHeartRateRawRequested( ble_gatt_access_ctxt* context) { 
    uint8_t* buffer = ppg.getRawData(); 
    int res = os_mbuf_append(context->om, buffer, 128); //TODO check if & is ok
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  
}
