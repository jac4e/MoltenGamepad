# GameCube USB Adapter Plugin Driver

This plugin is for Mayflash and derivative GameCube USB Adapters while in PC mode. Currently on linux, dolphin emu only detects the adapter as a single device whether in Wii U mode or PC Mode.

This plugin uses the linux joystick library to read the inputs of individual controllers on the hub and then maps them to a virtual controller which is visible within dolphin emu.

Currently it strictly targets devices with [Vendor ID 0079](https://devicehunt.com/view/type/usb/vendor/0079), which is more or less the Mayflash USB GameCube Controller Adapters.

## Useful Commands

View udev information of a controller:

`udevadm info -a -n /dev/input/js<number>`

View controller output:

`jstest /dev/input/js<number>`
