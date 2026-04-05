import board
import pwmio # get access to PWM
import time

# GP16 is the PWM pin
pwmPin = pwmio.PWMOut(board.GP16, variable_frequency=True)
pwmPin.frequency = 50 # in hz
pwmPin.duty_cycle = 0 # initially off, at 16bit number so max on is 65535

def setServo(angle):
    dutyPercentage = 0.025 + 0.1*angle/180
    pwmPin.duty_cycle = int(dutyPercentage*65535)
    time.sleep(0.01)
    return None

while True:
    for i in range(10, 170):
        setServo(i)
    for i in range(170, 10, -1):
        setServo(i)
        