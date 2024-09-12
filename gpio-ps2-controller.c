



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
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_SOUTH);   // Circle
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_EAST);    // X
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_WEST);    // Square
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_NORTH);   // Triangle
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_TL);      // L1
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_TR);      // R1
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_TL2);     // L2
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_TR2);     // R2
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_DPAD_UP); // D-pad Up
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_DPAD_DOWN); // D-pad Down
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_DPAD_LEFT); // D-pad Left
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_DPAD_RIGHT); // D-pad Right
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_THUMBL); // Joystick 1 button
  ioctl(uinput_fd, UI_SET_KEYBIT, BTN_THUMBR); // Joystick 2 button

  ioctl(uinput_fd, UI_SET_EVBIT, EV_ABS);
  ioctl(uinput_fd, UI_SET_ABSBIT, ABS_X); // Joystick 1 X-axis
  ioctl(uinput_fd, UI_SET_ABSBIT, ABS_Y); // Joystick 1 Y-axis
  ioctl(uinput_fd, UI_SET_ABSBIT, ABS_RX); // Joystick 2 X-axis
  ioctl(uinput_fd, UI_SET_ABSBIT, ABS_RY); // Joystick 2 Y-axis

  uidev.absmin[ABS_X] = -32768;
  uidev.absmax[ABS_X] = 32767;
  uidev.absmin[ABS_Y] = -32768;
  uidev.absmax[ABS_Y] = 32767;
  uidev.absmin[ABS_RX] = -32768;
  uidev.absmax[ABS_RX] = 32767;
  uidev.absmin[ABS_RY] = -32768;
  uidev.absmax[ABS_RY] = 32767;

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
  int circle, triangle, x, square, r1, r2, l1, l2, dpad_up, dpad_down, dpad_left, dpad_right;
  int joystick1_x, joystick1_y, joystick1_button, joystick2_x, joystick2_y, joystick2_button;

  printf("\033[2J\033[H");
  fflush(stdout);

  char trimmed_data[256];
  strncpy(trimmed_data, data, sizeof(trimmed_data) - 1);
  trimmed_data[sizeof(trimmed_data) - 1] = '\0';
  trim_whitespace(trimmed_data);

  if (strlen(trimmed_data) == 0) {
    printf("skipping empty line\n"); // TODO Figure out whats causing this and how to fix it
    return;
  }

  if (sscanf(trimmed_data, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
             &circle, &triangle, &x, &square,
             &r1, &r2, &l1, &l2,
             &dpad_up, &dpad_down, &dpad_left, &dpad_right,
             &joystick1_x, &joystick1_y, &joystick1_button,
             &joystick2_x, &joystick2_y, &joystick2_button) != 17) {
    printf("Invalid data format: '%s'\n", trimmed_data);
    return;
  }

  printf("Parsed values -> Circle: %d, Triangle: %d, X: %d, Square: %d, "
         "R1: %d, R2: %d, L1: %d, L2: %d, Dpad-Up: %d, Dpad-Down: %d, Dpad-Left: %d, Dpad-Right: %d, "
         "Joystick1-X: %d, Joystick1-Y: %d, Joystick1-Button: %d, "
         "Joystick2-X: %d, Joystick2-Y: %d, Joystick2-Button: %d\n",
         circle, triangle, x, square,
         r1, r2, l1, l2,
         dpad_up, dpad_down, dpad_left, dpad_right,
         joystick1_x, joystick1_y, joystick1_button,
         joystick2_x, joystick2_y, joystick2_button);

  emit_event(uinput_fd, EV_KEY, BTN_SOUTH, circle == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_EAST, x == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_WEST, square == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_NORTH, triangle == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_TL, l1 == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_TR, r1 == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_TL2, l2 == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_TR2, r2 == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_DPAD_UP, dpad_up == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_DPAD_DOWN, dpad_down == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_DPAD_LEFT, dpad_left == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_DPAD_RIGHT, dpad_right == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_THUMBL, joystick1_button == 0 ? 1 : 0);
  emit_event(uinput_fd, EV_KEY, BTN_THUMBR, joystick2_button == 0 ? 1 : 0);

  joystick1_x = ((joystick1_x - 512) / 4) * 300;
  joystick1_y = ((joystick1_y - 512) / 4) * 300;
  joystick2_x = ((joystick2_x - 512) / 4) * 300;
  joystick2_y = ((joystick2_y - 512) / 4) * 300;

  emit_event(uinput_fd, EV_ABS, ABS_X, joystick1_x);
  emit_event(uinput_fd, EV_ABS, ABS_Y, joystick1_y);
  emit_event(uinput_fd, EV_ABS, ABS_RX, joystick2_x);
  emit_event(uinput_fd, EV_ABS, ABS_RY, joystick2_y);

  emit_event(uinput_fd, EV_SYN, SYN_REPORT, 0);
}

int main() {
  const char *serial_port_name = "/dev/ttyACM1";
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
    usleep(100000); 
  }

  ioctl(uinput_fd, UI_DEV_DESTROY);
  close(uinput_fd);
  close(serial_port);
  return 0;
}
