# import RPi.GPIO as GPIO
import time
import _thread
import sys

time_for_not_manual = 10

user = ''
last_cmd_time = time.time()

is_open = False

if __name__ == '__main__':
    _thread.start_new_thread( check_status, ())

    for line in sys.stdin:
        last_cmd_time = time.time()
        line = line.split(',')
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

        print(line)

def check_status():
    while True:
        # check if pins have changed
        if new_status:
            t = time.time()
            if last_cmd_time + time_for_not_manual < t:
                # Manually opend
                if is_open:
                    print("opened;manual")
                else:
                    print("closed;manual")
        time.sleep(1)


# GPIO.setmode(GPIO.BCM)
# GPIO.setwarnings(False)
# GPIO.setup(18,GPIO.OUT)
# print "LED on"
# GPIO.output(18,GPIO.HIGH)
# time.sleep(1)
# print "LED off"
# GPIO.output(18,GPIO.LOW)
