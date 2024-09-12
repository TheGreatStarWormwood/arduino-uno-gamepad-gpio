
#include <ctype.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

void trim_whitespace(char *str) {
  int len = strlen(str);
  while (len > 0 && isspace(str[len - 1])) {
    str[len - 1] = '\0';
    len--;
  }
}

int open_serial_port(const char *port_name) {
  int serial_port = open(port_name, O_RDWR);
  if (serial_port < 0) {
    printf("Error opening serial port\n");
    return -1;
  }

  struct termios tty;
  memset(&tty, 0, sizeof(tty));

  if (tcgetattr(serial_port, &tty) != 0) {
    printf("Error getting serial attributes\n");
    return -1;
  }

  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);

  tty.c_cflag |= (CLOCAL | CREAD);
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;
  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  tcflush(serial_port, TCIFLUSH);
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error setting serial attributes\n");
    return -1;
  }

  return serial_port;
}

int setup_uinput_device() {
  int uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (uinput_fd < 0) {
    printf("Error opening /dev/uinput\n");
    return -1;
  }

  struct uinput_user_dev uidev;
  memset(&uidev, 0, sizeof(uidev));
  strcpy(uidev.name, "PS2 Controller");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor = 0x1234;
  uidev.id.product = 0x5678;
  uidev.id.version = 1;

  ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_SOUTH);
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_EAST);
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_WEST);
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_NORTH);
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_START);

  ioctl(uinput_fd, UI_SET_EVBIT, EV_ABS);
  ioctl(uinput_fd, UI_SET_ABSBIT, ABS_X);
  ioctl(uinput_fd, UI_SET_ABSBIT, ABS_Y);

  uidev.absmin[ABS_X] = -32768;
  uidev.absmax[ABS_X] = 32767;
  uidev.absmin[ABS_Y] = -32768;
  uidev.absmax[ABS_Y] = 32767;

  write(uinput_fd, &uidev, sizeof(uidev));

  if (ioctl(uinput_fd, UI_DEV_CREATE) < 0) {
    printf("Error creating uinput device\n");
    return -1;
  }

  return uinput_fd;
}

void emit_event(int uinput_fd, int type, int code, int value) {
  struct input_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.type = type;
  ev.code = code;
  ev.value = value;
  write(uinput_fd, &ev, sizeof(ev));
}

void map_to_ps2_controller(int uinput_fd, const char *data) {

  int button1, button2, button3, button4, joystick_x, joystick_y,
      joystick_button;

  printf("\033[2J\033[H");
  fflush(stdout);
  // printf("Received raw data: '%s'\n", data);

  char trimmed_data[256];
  strncpy(trimmed_data, data, sizeof(trimmed_data) - 1);
  trimmed_data[sizeof(trimmed_data) - 1] = '\0'; 
  trim_whitespace(trimmed_data);

  if (strlen(trimmed_data) == 0) {
    printf("Skipping empty line.\n");
    return;
  }

  // printf("Trimmed data: '%s'\n", trimmed_data);

  if (sscanf(trimmed_data, "%d %d %d %d %d %d %d", &button1, &button2, &button3,
             &button4, &joystick_x, &joystick_y, &joystick_button) != 7) {
    printf("Invalid data format: '%s'\n", trimmed_data);
    return;
  }

  
  printf("Parsed values -> Button1: %d, Button2: %d, Button3: %d, Button4: %d, "
         "JoystickX: %d, JoystickY: %d, JoystickButton: %d\n",
         button1, button2, button3, button4, joystick_x, joystick_y,
         joystick_button);

  emit_event(uinput_fd, EV_KEY, BTN_SOUTH, button1 == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_EAST, button2 == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_WEST, button3 == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_NORTH, button4 == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_START, joystick_button == 0 ? 1 : 0);

  joystick_x = ((joystick_x - 512) / 4) * 300;
  joystick_y = ((joystick_y - 512) / 4) * 300;

  emit_event(uinput_fd, EV_ABS, ABS_X, joystick_x);
  emit_event(uinput_fd, EV_ABS, ABS_Y, joystick_y);

  emit_event(uinput_fd, EV_SYN, SYN_REPORT, 0);
}

int main() {
  const char *serial_port_name = "/dev/ttyACM0";
  int serial_port = open_serial_port(serial_port_name);

  if (serial_port < 0) {
    return 1;
  }

  int uinput_fd = setup_uinput_device();
  if (uinput_fd < 0) {
    close(serial_port);
    return 1;
  }

  char buffer[256];
  while (1) {
    int num_bytes = read(serial_port, buffer, sizeof(buffer) - 1);
    if (num_bytes > 0) {
      buffer[num_bytes] = '\0';
      map_to_ps2_controller(uinput_fd, buffer);
    }
    usleep(100000); // Sleep for 100 ms
  }

  // Clean up uinput device
  ioctl(uinput_fd, UI_DEV_DESTROY);
  close(uinput_fd);
  close(serial_port);
  return 0;
}
