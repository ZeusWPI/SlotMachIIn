import RPi.GPIO as gpio
import time
import thread as _thread
import sys
import logging
import traceback


def check_status(handle):
    while True:
        global old_GET_CLOSE
        global old_GET_OPEN

        if not ((old_GET_OPEN == gpio.input(GET_OPEN)) and (old_GET_CLOSE == gpio.input(GET_CLOSE))):
            old_GET_OPEN = gpio.input(GET_OPEN)
            old_GET_CLOSE = gpio.input(GET_CLOSE)

            if last_cmd_time + time_for_not_manual < time.time():
                if gpio.input(GET_OPEN) == gpio.input(GET_CLOSE):
                    print("NXT: nxt: Error: Both pins are the same")
                    sys.stdout.flush()
                else:
                    # Manually opened
                    if old_GET_CLOSE == gpio.HIGH:
                        print("opened;manual")
                        sys.stdout.flush()
                        handle.write("detected open\n")
                        handle.flush()
                    else:
                        print("closed;manual")
                        sys.stdout.flush()
                        handle.write("detected close\n")
                        handle.flush()
        time.sleep(1)

GET_OPEN = 27
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


with open("/home/slotmachien/SlotMachIIn/PI/log.me", "a") as f:
    f.write("starting"+str(time.time())+"\n")
    f.flush()
    try:

        gpio.output(OPEN, gpio.HIGH)
        gpio.output(CLOSE, gpio.HIGH)
        old_GET_OPEN = gpio.input(GET_OPEN)
        old_GET_CLOSE = gpio.input(GET_CLOSE)
        _thread.start_new_thread(check_status, (f,))
        print("opened;manual")
        sys.stdout.flush()
        while True:
            line = sys.stdin.readline()
            last_cmd_time = time.time()
            line = line.split(';')
            if len(line) < 2:
                continue
            f.write("got line "+line+"\n")
            f.flush()
            cmd = line[0]
            user = line[1]
            if cmd.upper() == 'OPEN':
                gpio.output(OPEN, gpio.LOW)
                time.sleep(0.5)
                gpio.output(OPEN, gpio.HIGH)
                print("opened;p:"+user)
                sys.stdout.flush()
            elif cmd.upper() == 'CLOSE':
                gpio.output(CLOSE, gpio.LOW)
                time.sleep(0.5)
                gpio.output(CLOSE, gpio.HIGH)
                print("closed;p:"+user)
                sys.stdout.flush()
            else:
                print("opened;manual")
                sys.stdout.flush()
    except Exception as e:
        f.write(traceback.format_exc())
    finally:
        f.write("cleanup\n")
        f.flush()
        gpio.cleanup()
