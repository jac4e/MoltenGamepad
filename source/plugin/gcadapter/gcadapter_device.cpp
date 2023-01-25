#include "gcadapter_plugin.h"
#include <cstdio>
#include <cstring>
#include <linux/joystick.h>

//a little brevity...
#define BTN DEV_KEY
#define ABS DEV_AXIS
#define OPT DEV_OPTION

device_methods gcadapter_device::methods;

//static array of event declarations, loaded by gcadapter_manager init_profile();
//{"name","description",type,"default mapping"}
const event_decl gcadapter_events[] = {
  {"B","B Button",BTN,"B"},
  {"Y","Y Button",BTN,"Y"},
  {"X","X Button",BTN,"X"},
  {"A","A Button",BTN,"A"},
  {nullptr, nullptr, NO_ENTRY, nullptr}
};

gcadapter_device::gcadapter_device(int fdin) {
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

void gcadapter_device::process(void* tag) {
  //read the file identified by the tag
  //and compute values to be sent for translation.
  struct js_event event;
  int res = read(fd, &event, sizeof(event));
  printf("%d\t%d\t%d\n", event.type, event.value, event.number);

  // methods.send_value(ref,event.number,0);

  //ev_id is a 0-index of the events, in the order they were registered.
  //For this gcadapter, event "up" has an id of 4.
  //So if reading this file indicated the "up" button was pressed:
  //  methods.send_value(ref,4,1);
  

  //send a SYN_REPORT as appropriate. Without it, events may be ignored while
  //software waits for the SYN_REPORT.
  methods.send_syn_report(ref);
}

int gcadapter_device::process_option(const char* opname, const MGField value) {
  //an option was changed! Use the name and value to do what needs to be done.
  return 0; //should be possible in the future to signal an error...
}
