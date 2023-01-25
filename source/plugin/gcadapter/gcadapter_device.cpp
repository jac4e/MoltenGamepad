#include "gcadapter_plugin.h"
#include <cstdio>
#include <cstring>
#include <linux/joystick.h>
#include <linux/input.h>

//a little brevity...
#define BTN DEV_KEY
#define ABS DEV_AXIS
#define OPT DEV_OPTION

device_methods gcadapter_device::methods;

//static array of event declarations, loaded by gcadapter_manager init_profile();
//{"name","description",type,"default mapping"}
const event_decl gcadapter_events[] = {
  {"a", "a", BTN, "a"},
  {"b", "b", BTN, "b"},
  {"x", "x", BTN, "x"},
  {"y", "y", BTN, "y"},
  {"z", "z", BTN, "z"},
  {"start", "start", BTN, "start"},
  {"lt_abs", "lt_abs", ABS, "lt_abs"},
  {"rt_abs", "rt_abs", ABS, "rt_abs"},
  {"lt_btn", "lt_btn", BTN, "lt_btn"},
  {"rt_btn", "rt_btn", BTN, "rt_btn"},
  {"control_x", "control_x", ABS, "control_x"},
  {"control_y", "control_y", ABS, "control_y"},
  {"c_x", "c_x", ABS, "c_x"},
  {"c_y", "c_y", ABS, "c_y"},
  {"d_up_btn", "d_up_btn", BTN, "d_up_btn"},
  {"d_down_btn", "d_down_btn", BTN, "d_down_btn"},
  {"d_left_btn", "d_left_btn", BTN, "d_left_btn"},
  {"d_right_btn", "d_right_btn", BTN, "d_right_btn"},
  {"d_up_hat", "d_up_hat", BTN, "d_up_hat"},
  {"d_down_hat", "d_down_hat", BTN, "d_down_hat"},
  {"d_left_hat", "d_left_hat", BTN, "d_left_hat"},
  {"d_right_hat", "d_right_hat", BTN, "d_right_hat"},
  {nullptr, nullptr, NO_ENTRY, nullptr}
};

gcadapter_device::gcadapter_device(int fdin, char * path): devnode(path) {
  fd = fdin;
  
  gcadapter_dev.name_stem = name_stem;
  //basic initialization
  //it might be wise to provide a file path to a device node
  //via the constructor.
}
gcadapter_device::~gcadapter_device() {
  //close any file descriptors...
  close(fd);
}

int gcadapter_device::init(input_source* ref) {
  //main purpose is to provide the ref used to call methods.
  this->ref = ref;

  //now would be a good time tell MoltenGamepad to start watching a file...
  methods.watch_file(ref, fd, (void*)0);

  //The tag is some arbitrary value to distinguish file descriptors.
  //It CANNOT be equal to ref.
  return 0;
}

// Process js_event and pass on to moltengamepad
void gcadapter_device::process(void* tag) {

  struct js_event event;
  int64_t value;
  int res = read(fd, &event, sizeof(event));
  char back = devnode.back();
  printf("GC%c:\t%s\t%d\n", back, gcadapter_events[event.number].name, event.value);
  
  switch (event.type)
  {
  case JS_EVENT_BUTTON:
    // Button is either 1 or 0, no range issues here.
    value = event.value;
    break;
  case JS_EVENT_AXIS:
    // process axis so it fits within ABS_RANGE
    // for now, do +/- ABS_RANGE until proper range conversion is sorted
    value = event.value > 0 ? ABS_RANGE : -ABS_RANGE;
    break;
  }

  methods.send_value(ref,event.number,value);

  //send a SYN_REPORT as appropriate. Without it, events may be ignored while
  //software waits for the SYN_REPORT.
  methods.send_syn_report(ref);
}

int gcadapter_device::process_option(const char* opname, const MGField value) {
  //an option was changed! Use the name and value to do what needs to be done.
  return 0; //should be possible in the future to signal an error...
}

int gcadapter_device::upload_ff(ff_effect* effect) {
  if (fd < 0)
    return -1;
  int ret = ioctl(fd, EVIOCSFF, effect);
  if (ret < 0)
    perror("gcadapter upload FF");
  return effect->id;
}

int gcadapter_device::erase_ff(int id) {
  if (fd < 0)
    return -1;
  int ret = ioctl(fd, EVIOCRMFF, id);
  if (ret < 0)
    perror("gcadapter erase FF");
  return 0;
}

int gcadapter_device::play_ff(int id, int repeats){
  if (fd < 0)
    return -1;
  input_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.type = EV_FF;
  ev.code = id;
  ev.value = repeats;
  ssize_t res = write(fd, &ev, sizeof(ev));
  if (res < 0)
    perror("gcadapter write FF event");
  return 0;
}
