#!/usr/bin/env python3

import asyncio
from bleak import BleakClient
import time
import struct

address = "CC:7B:FA:D0:A6:DB"
MODEL_NBR_UUID = "2A39"

async def main(address):
    async with BleakClient(address) as client:
        raw_data = await client.read_gatt_char(MODEL_NBR_UUID)
        int_array = list(struct.unpack('<64H', raw_data))
        print(int_array)
asyncio.run(main(address))
