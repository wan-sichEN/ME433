import pygame
import pgzrun
import serial
import threading

WIDTH = 800
HEIGHT = 400

GROUND_Y = HEIGHT - 80
WATER_X = WIDTH // 2

SKY   = (135, 206, 235)
GRASS = (34, 139, 34)
WATER = (30, 100, 200)
WHITE = (255, 255, 255)

paddle_pos = 0.0
force_val = 0

try:
    ser = serial.Serial('COM8', 115200, timeout=1)  # check your COM port
except serial.SerialException:
    print("Pico not found, using mouse instead")
    ser = None

def serial_thread():
    global paddle_pos, force_val
    while True:
        try:
            line = ser.readline().decode().strip()
            if line:
                parts = line.split()
                if len(parts) >= 2:
                    paddle_pos = -float(parts[0]) # directions were flipped from encoder to python
                    force_val = int(parts[1])
        except:
            pass

if ser:
    t = threading.Thread(target=serial_thread, daemon=True)
    t.start()

def update():
    global paddle_pos
    if not ser:
        mouse_x, _ = pygame.mouse.get_pos()
        paddle_pos = (mouse_x / WIDTH) * 2 - 1  # mouse drives position

def draw():
    screen.fill(SKY)
    screen.draw.filled_rect(Rect(0, GROUND_Y, WATER_X, HEIGHT - GROUND_Y), GRASS)
    screen.draw.filled_rect(Rect(WATER_X, GROUND_Y, WIDTH - WATER_X, HEIGHT - GROUND_Y), WATER)

    x = int((paddle_pos + 1) / 2 * WIDTH)
    y = GROUND_Y - 5

    screen.draw.filled_circle((x, y - 60), 12, WHITE)  # head
    screen.draw.line((x, y - 48), (x, y - 20), WHITE)  # body
    screen.draw.line((x, y - 40), (x - 15, y - 25), WHITE)  # left arm
    screen.draw.line((x, y - 40), (x + 15, y - 25), WHITE)  # right arm
    screen.draw.line((x, y - 20), (x - 12, y), WHITE)  # left leg
    screen.draw.line((x, y - 20), (x + 12, y), WHITE)  # right leg

    screen.draw.text(
    f"Force: {force_val}",
    (10, 10),
    fontsize=30,
    color="black"
    )

import atexit

def cleanup():
    if ser:
        ser.close()

atexit.register(cleanup)

pgzrun.go()