# Heart rate raw data service (hrs3300)
Bluetooth service that expose raw PPG heart rate data from hrs3300 senosor.

* Imporatatnt UUID = 00002a39-0000-1000-8000-00805f9b34fb

Especially this part 2a39

## Service result data 

Basically it result of `dataHRS.data()` from `Ppg.cpp`. This is 64 16bit integers of raw hrs3300 sensor outputs.
That is why there is 128 bytes of data, 2 bytes per one entry in array.
When deltaMs == 100ms then we can assume (I am not sure if it is good assumption) that we have 10 readings per second. So there should be around 6.5 seconds of raw data from sensor.

There is one technical detail how dataHRS is appended in `Ppg.cpp`. When dataHRS array is full then it is shifted by size of 5 (on code this is code overlapWindow), so first 5 elements are removed and there is 5 slots at the end of array to be filled with new sensor readings.
There is data shift but it does not removed or erase last elements in array. This is an example, you can see at line 4 that after shift old in the end of array are still there.
For simplicity purpose, values are just incremented count that correlate with time order instead of real sensor readings. 

```
[ 1,  2,  3,  4,  5,  6,  7,  9, 10, ..., 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,  0,  0 ]
[ 1,  2,  3,  4,  5,  6,  7,  9, 10, ..., 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,  0 ]
[ 1,  2,  3,  4,  5,  6,  7,  9, 10, ..., 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64 ]
[ 6,  7,  9, 10, 11, 12 ,13 ,14, 15, ..., 58, 59, 60, 61, 62, 63, 64, 65, 61, 62, 63, 64 ]
[ 6,  7,  9, 10, 11, 12 ,13 ,14, 15, ..., 58, 59, 60, 61, 62, 63, 64, 65, 66, 62, 63, 64 ]
[ 6,  7,  9, 10, 11, 12 ,13 ,14, 15, ..., 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 63, 64 ]
[ 6,  7,  9, 10, 11, 12 ,13 ,14, 15, ..., 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 64 ]
[ 6,  7,  9, 10, 11, 12 ,13 ,14, 15, ..., 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69 ]
[12, 13, 14, 15, 16, 17 ,18, 19, 20, ..., 63, 64, 65, 66, 67, 68, 69, 70, 66, 67, 68, 69 ]
```

## How to read data from service 
1. find your device bluetooth mac adress 
1. (if applicable) disconnect with itd `systemctl --user stop itd`
1. (if applicable) disconnect from bluetooth `bluetoothctl disconnect XX:XX:XX:XX:XX:XX`
1. run this sample scripture (change your bluetooth device mac address)

``` python3
#!/usr/bin/env python3

import asyncio
from bleak import BleakClient
import time
import struct

address = "XX:XX:XX:XX:XX:XX"
MODEL_NBR_UUID = "2A39"

async def main(address):
    async with BleakClient(address) as client:
        raw_data = await client.read_gatt_char(MODEL_NBR_UUID)
        int_array = list(struct.unpack('<64H', raw_data))
        print(int_array)
asyncio.run(main(address))

```

## Analysis with python notebook
`ppg_analysis/ppg_raw_data_analysis.ipynb`

## Record raw ppg signal
`ppg_analysis/record_raw_data.ipynb`

# HRS 3300 settings from file
hrs3300 driver (src/drivers/hrs3300.cpp) was modified to get drive and enable from file /settingshrs.dat
So it is possible to modify this settings file (with use of ble fs ) to change hrs3300 configuration.
After coping settingshrs.dat file to device it is necessary to restart device.
Official sensor documentation: https://files.pine64.org/doc/datasheet/pinetime/HRS3300%20Heart%20Rate%20Sensor.pdf

Enable register - 00000000
                   ^^^^
                   xxxB

Drive register  - 00000000
                   ^
                   A

xxx - 3 bits that determine HRS wait time

000:wait time between each conversion cycle is 800 ms
001:wait time between each conversion cycle is 400 ms
010:wait time between each conversion cycle is 200 ms
011:wait time between each conversion cycle is 100 ms
100:wait time between each conversion cycle is 75 ms
101:wait time between each conversion cycle is 50 ms
110:wait time between each conversion cycle is 12.5 ms
111:wait time between each conversion cycle is 0 ms

BA - 2 bits that determine led current

00: set current 12.5mA
01: set current 20mA
10: set current 30mA
11: set current 40mA

## register default values 1.14.0

enable = 0x50 = 01010000

drive  = 0x2f = 00101111

xxx = 101 = 50ms
BA  =  00 = 12.5mA

## set values

1. get settingshrs.dat
    `itctl fs read /settingshrs.dat .` TODO check if this works
2. modify settings
    `echo -n -e "\x50\x2f" | dd of=settingshrs.dat bs=1 seek=4 count=2 conv=notrunc`
3. save to device 
    `itctl fs write ./settingshrs.dat /settingshrs.dat`
4. reset device

### set values strongest light
40mA - led current
50ms - time between conversion cycle (as in default)
`echo -n -e "\x58\x6f" | dd of=settingshrs.dat bs=1 seek=4 count=2 conv=notrunc`

### show local settings file
`xxd settingshrs.dat`


## troubleshooting

### hci dev id

Device id have to be the same check `hcitool dev` eg. `hci0`
configure it in bluetooth adapter in  `~/.config/itd/itd.toml`


