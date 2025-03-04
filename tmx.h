#pragma once

#define THRUSTMASTER_VID 0x044F
// TMX wheel identifies as a Thrustmaster TMX GIP Racing Wheel on connection
// After first init packet sent it changes to a Thrustmaster FFB Racing Wheel
// After the second init packet is sent in becomes the Thrustmaster TMX Racing
// Wheel
#define TMX_GIP 0xB67E
#define TMX_HID 0xB65D

struct tmx_data {
  unsigned long quirks;
  void *device_props;
};

extern struct hid_driver hid_tmx_driver;
