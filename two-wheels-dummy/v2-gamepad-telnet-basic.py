from __future__ import print_function
from inputs import get_gamepad
from telnetlib import Telnet
import time

"""
This is a rough first approach to using a PS3 gamepad to 
command the telnet-serial communication.
"""

if __name__ == "__main__":
    buttons_now = {}
    buttons_before = {}
    analogs_now = {}
    analogs_before = {}

    speed = 5

    with Telnet('192.168.1.109', '1234') as tn:
        while 1:
            events = get_gamepad()
            for event in events:

                if event.ev_type == 'Key':
                    buttons_now[event.code] = event.state
                    # print(event.ev_type, event.code, event.state)
                if event.ev_type == 'Absolute':
                    analogs_now[event.code] = event.state
                    # print(event.ev_type, event.code, event.state)

            if analogs_now.get('ABS_RZ', 0) >= 200 and analogs_before.get('ABS_RZ',0) < 200:
                # print('Right trigger')
                tn.write(f'f{differential}s{speed}t1'.encode('ascii') + b'\n')

            if analogs_now.get('ABS_Z', 0) >= 200 and analogs_before.get('ABS_Z',0) < 200:
                # print('Left trigger')
                tn.write(f'r{differential}s{speed}t1'.encode('ascii') + b'\n')

            if analogs_now.get('ABS_X', 0) >= 5000:
                # print('Left joystick right')
                differential = 31
            elif analogs_now.get('ABS_X', 0) <= -5000:
                # print('Left joystick left')
                differential = 13
            else:
                differential = 11

            if buttons_now.get('BTN_SOUTH',0) and not(buttons_before.get('BTN_SOUTH',0)):
                # print('X button')
                speed += 1

            if buttons_now.get('BTN_WEST',0) and not(buttons_before.get('BTN_WEST',0)):
                # print('Square button')
                speed -= 1

            buttons_before['BTN_SOUTH'] = buttons_now.get('BTN_SOUTH')
            buttons_before['BTN_WEST'] = buttons_now.get('BTN_WEST')
            analogs_before['ABS_RZ'] = analogs_now.get('ABS_RZ')
            analogs_before['ABS_Z'] = analogs_now.get('ABS_Z')
