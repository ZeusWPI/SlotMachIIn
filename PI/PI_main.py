import RPi.GPIO as gpio
import time
import _thread
import sys

def check_status():
    while True:
        global old_GET_CLOSE
        global GET_CLOSE
        global GET_OPEN
        global old_GET_OPEN

        if (gpio.input(GET_OPEN) == gpio.input(GET_CLOSE)):
           print("NXT: Error: Both pins are the same")
        if not ((old_GET_OPEN == gpio.input(GET_OPEN)) and (old_GET_CLOSE == gpio.input(GET_CLOSE))):
            old_GET_OPEN = gpio.input(GET_OPEN)
            old_GET_CLOSE = gpio.input(GET_CLOSE)
            t = time.time()
            if last_cmd_time + time_for_not_manual < t:
                # Manually opened
                if is_open:
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

is_open = False


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

        print(line)

# GPIO.setmode(GPIO.BCM)
# GPIO.setwarnings(False)
# GPIO.setup(18,GPIO.OUT)
# print "LED on"
# GPIO.output(18,GPIO.HIGH)
# time.sleep(1)
# print "LED off"
# GPIO.output(18,GPIO.LOW)
