#include "gcadapter_plugin.h"
#include <cstring>

//This file provides the entry point that
//fills out the needed function pointers.

//A plugin should change the manager .name
//and device .name_stem fields, as well
//as the device get_description().

//Not many other changes should be needed here, other
//than changing variable names as needed.

device_plugin gcadapter_dev;

PLUGIN_INIT(gcadapter)(plugin_api api) {
  if (!API_EXACT(api))
    return -1;
  //set static vars
  gcadapter_manager::methods = *(api.head.manager);
  gcadapter_device::methods = *(api.head.device);
  gcadapter_manager* manager = new gcadapter_manager();

  //set manager call backs
  manager_plugin gcadapter_man;
  memset(&gcadapter_man, 0, sizeof(gcadapter_man));
  gcadapter_man.size = sizeof(gcadapter_man);
  gcadapter_man.name = gcadapter_manager::name;
  gcadapter_man.subscribe_to_gamepad_profile = true;
  gcadapter_man.init = [] (void* plug_data, device_manager* ref) -> int {
    return ((gcadapter_manager*)plug_data)->init(ref);
  };
  gcadapter_man.destroy = [] (void* data) -> int {
    delete (gcadapter_manager*) data;
    return 0;
  };
  gcadapter_man.start = [] (void* data) { 
    return ((gcadapter_manager*)data)->start();
  };
  gcadapter_man.process_manager_option = [] (void* ref, const char* opname, MGField opvalue) {
    return ((gcadapter_manager*)ref)->process_option(opname, opvalue);
  };
  gcadapter_man.process_udev_event = [] (void* data, struct udev* udev, struct udev_device* dev) {
    return ((gcadapter_manager*)data)->accept_device(udev, dev);
  };

  //set device call backs
  memset(&gcadapter_dev, 0, sizeof(gcadapter_dev));
  gcadapter_dev.size = sizeof(gcadapter_dev);
  gcadapter_dev.name_stem = gcadapter_device::name_stem;
  gcadapter_dev.uniq = "";
  gcadapter_dev.phys = "";
  gcadapter_dev.init = [] (void* data, input_source* ref) -> int {
    return ((gcadapter_device*)data)->init(ref);
  };
  gcadapter_dev.destroy = [] (void* data) -> int {
    delete (gcadapter_device*) data;
    return 0;
  };
  gcadapter_dev.get_description = [] (const void* data) {
    return "gcadapter Plugin Device";
  };
  gcadapter_dev.get_type = [] (const void* data) {
    return "gamepad";
  };
  gcadapter_dev.process_event = [] (void* data, void* tag) -> int {
    ((gcadapter_device*)data)->process(tag);
    return 0;
  };
  gcadapter_dev.process_option = [] (void* data, const char* opname, MGField opvalue) {
    return ((gcadapter_device*)data)->process_option(opname, opvalue);
  };
  gcadapter_dev.upload_ff = [] (void* data, ff_effect* effect) {
    return ((gcadapter_device*)data)->upload_ff(effect);
  };
  gcadapter_dev.erase_ff = [] (void* data, int id) {
    return ((gcadapter_device*)data)->erase_ff(id);
  };
  gcadapter_dev.play_ff = [] (void* data, int id, int repetitions) {
    return ((gcadapter_device*)data)->play_ff(id, repetitions);
  };

  api.mg.add_manager(gcadapter_man,  manager);
  return 0;
}
