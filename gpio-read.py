
from evdev import UInput, ecodes as e
import serial
import time

SERIAL_PORT = '/dev/ttyACM0'  
BAUD_RATE = 9600

device = UInput({
    e.EV_KEY: [e.BTN_SOUTH, e.BTN_EAST, e.BTN_WEST, e.BTN_NORTH, e.BTN_START],  
    e.EV_ABS: [e.ABS_X, e.ABS_Y],
})

ser = serial.Serial(SERIAL_PORT, BAUD_RATE)

def map_to_ps2_controller(data):
    print(f"Received data: s{data}")  
    parts = data.strip().split()
    print(f"Parsed partsdsas: {parts}")
    if len(parts) != 7:  
        print("Invalid data format")
        return

    button1, button2, button3, button4, joystick_x, joystick_y, joystick_button = parts
    print(f"Button states: {button1}, {button2}, {button3}, {button4}")
    print(f"Joystick values: {joystick_x}, {joystick_y}")

    device.write(e.EV_KEY, e.BTN_SOUTH, 1 if button1 == "0" else 0)
    device.write(e.EV_KEY, e.BTN_EAST, 1 if button2 == "0" else 0)
    device.write(e.EV_KEY, e.BTN_WEST, 1 if button3 == "0" else 0)
    device.write(e.EV_KEY, e.BTN_NORTH, 1 if button4 == "0" else 0)

    device.write(e.EV_KEY, e.BTN_START, 1 if joystick_button == "0" else 0)  # Assuming BTN_START for joystick button

    joystick_x_value = int(joystick_x)
    joystick_y_value = int(joystick_y)

    joystick_x_value = ((joystick_x_value - 512) // 4)*245
    joystick_y_value = ((joystick_y_value - 512) // 4)*245

    device.write(e.EV_ABS, e.ABS_X, joystick_x_value)
    device.write(e.EV_ABS, e.ABS_Y, joystick_y_value)

    device.syn()  

while True:
    if ser.in_waiting > 0:
        data = ser.readline().decode('utf-8')
        map_to_ps2_controller(data)
        time.sleep(0.1)
