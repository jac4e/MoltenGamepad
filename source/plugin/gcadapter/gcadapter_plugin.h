#pragma once
#include "../plugin.h"
#include <linux/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <vector>
#include <mutex>
#include <time.h>

extern device_plugin gcadapter_dev;

extern const event_decl gcadapter_events[];

class gcadapter_device {
public:
  gcadapter_device(int fd);
  ~gcadapter_device();
  int init(input_source* ref);

  input_source* ref;
  int fd;
  //let our init func call private methods by marking it as a friend.
  friend PLUGIN_INIT_FUNC(gcadapter)(plugin_api api);
  static device_methods methods;

  constexpr static const char* name_stem = "GC";
protected:
  void process(void*);
  int process_option(const char* opname, const MGField value);
};

class gcadapter_manager {
public:

  void init_profile();

  int init(device_manager* ref);

  int start();

  int accept_device(struct udev* udev, struct udev_device* dev);

  int process_option(const char* name, const MGField value);

  gcadapter_manager();

  ~gcadapter_manager();

  friend PLUGIN_INIT_FUNC(gcadapter)(plugin_api api);
  static manager_methods methods;
  constexpr static const char* name = "gcadapter";
private:
  device_manager* ref;
};

