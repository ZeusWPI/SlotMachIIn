import RPi.GPIO as gpio
import time
import _thread
import sys

GET_OPEN = 21
GET_CLOSE = 22
OPEN = 23
CLOSE = 24
old_GET_CLOSE = True
old_GET_OPEN = True

gpio.setmode(gpio.BMC)
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
    _thread.start_new_thread( check_status, ())
    old_GET_OPEN = gpio.input(GET_OPEN)
    old_GET_CLOSE = gpio.input(GET_CLOSE)
    for line in sys.stdin:
        last_cmd_time = time.time()
        line = line.split(',')
        user = line[1]
        cmd = line[0]
        if cmd.upper() == 'OPEN':
            gpio.output(CLOSE, LOW)
            gpio.output(OPEN, HIGH)
            print(f"opened;p:{user}")

        elif cmd.upper() == 'CLOSE':
            gpio.output(OPEN, LOW)
            gpio.output(CLOSE, HIGH)
            print(f"closed;p:{user}")

        print(line)

def check_status():
    while True:
        if (gpio.input(GET_OPEN) == gpio.input(GET_CLOSE)):
           print("NXT: Error: Both pins are the same")
        if not ((old_GET_OPEN == gpio.input(GET_OPEN)) and (old_GET_CLOSE == gpio.input(GET_CLOSE))):
            t = time.time()
            if last_cmd_time + time_for_not_manual < t:
                # Manually opened
                if is_open:
                    print("opened;manual")
                else:
                    print("closed;manual")
        time.sleep(1)

if __name__ == '__main__':
    _thread.start_new_thread( check_status, ())

    for line in sys.stdin:
        lline = line
        try:
            last_cmd_time = time.time()
            line = line.strip().split(';')
            user = line[1]
            cmd = line[0]
            if cmd.upper() == 'OPEN':
                # OPEN DOOR
                # Set pin's idk what to HIGH
                print(f"opened;p:{user}")

            elif cmd.upper() == 'CLOSE':
                # CLOSE DOOR
                # Set pin's idk what to HIGH
                print(f"closed;p:{user}")

            else:
                print(f"nxt: invalid command: {lline}")
        except:
            print(f"nxt: shit happend with line \"{lline}\"")


# GPIO.setmode(GPIO.BCM)
# GPIO.setwarnings(False)
# GPIO.setup(18,GPIO.OUT)
# print "LED on"
# GPIO.output(18,GPIO.HIGH)
# time.sleep(1)
# print "LED off"
# GPIO.output(18,GPIO.LOW)
