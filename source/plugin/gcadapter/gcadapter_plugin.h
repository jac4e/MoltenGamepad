#pragma once
#include "../plugin.h"
#include <string>
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


enum gcadapter_event_id {
  a,
  b,
  x,
  y,
  z,
  start,
  lt_abs,
  rt_abs,
  lt_btn,
  rt_btn,
  control_x,
  control_y,
  c_x,
  c_y,
  d_up_btn,
  d_down_btn,
  d_left_btn,
  d_right_btn,
  d_up_hat,
  d_down_hat,
  d_left_hat,
  d_right_hat,
};

class gcadapter_device {
public:
  gcadapter_device(int fd, std::string jsnum);
  ~gcadapter_device();
  int init(input_source* ref);

  input_source* ref;
  int fd;
  std::string jsnum;
  //let our init func call private methods by marking it as a friend.
  friend PLUGIN_INIT_FUNC(gcadapter)(plugin_api api);
  static device_methods methods;

  constexpr static const char* name_stem = "GC";

  int upload_ff(ff_effect* effect);
  int erase_ff(int id);
  int play_ff(int id, int repeats);

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

