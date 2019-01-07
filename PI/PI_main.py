import RPi.GPIO as gpio
import time
import thread as _thread
import sys

def check_status():
    while True:
        global old_GET_CLOSE
        global old_GET_OPEN

        if not ((old_GET_OPEN == gpio.input(GET_OPEN)) and (old_GET_CLOSE == gpio.input(GET_CLOSE))):
            old_GET_OPEN = gpio.input(GET_OPEN)
            old_GET_CLOSE = gpio.input(GET_CLOSE)

            if last_cmd_time + time_for_not_manual < time.time():
                if (gpio.input(GET_OPEN) == gpio.input(GET_CLOSE)):
                    print("NXT: Error: Both pins are the same")
                else:
                    # Manually opened
                    if old_GET_CLOSE == gpio.HIGH:
                        print("opened;manual")
                    else:
                        print("closed;manual")
        time.sleep(1)

GET_OPEN = 21
GET_CLOSE = 22
OPEN = 23
CLOSE = 24
old_GET_CLOSE = True
old_GET_OPEN = True

gpio.setmode(gpio.BCM)
gpio.setup(GET_OPEN, gpio.IN)
gpio.setup(GET_CLOSE, gpio.IN)
gpio.setup(OPEN, gpio.OUT)
gpio.setup(CLOSE, gpio.OUT)


time_for_not_manual = 10

lline = ''
user = ''
last_cmd_time = time.time()

if __name__ == '__main__':
    gpio.output(OPEN, gpio.HIGH)
    gpio.output(CLOSE, gpio.HIGH)    
    old_GET_OPEN = gpio.input(GET_OPEN)
    old_GET_CLOSE = gpio.input(GET_CLOSE)
    _thread.start_new_thread( check_status, ())

    for line in sys.stdin:
        last_cmd_time = time.time()
        line = line.split(',')
        user = line[1]
        cmd = line[0]
        if cmd.upper() == 'OPEN':
            gpio.output(CLOSE, gpio.HIGH)
            gpio.output(OPEN, gpio.LOW)
            print("opened;p:"+user) 

        elif cmd.upper() == 'CLOSE':
            gpio.output(OPEN, gpio.HIGH)
            gpio.output(CLOSE, gpio.LOW)
            print("closed;p:"+user)