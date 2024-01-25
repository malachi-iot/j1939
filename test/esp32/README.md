# Detail

## Joystick

Listens for GPIO button presses and emits them as BJM1:button1 pressed

## Listener

Listener avoids anything fancy like `embr` services, etc. so as to be the most
direct and easy example + test to understand

# Results

|   Date  | Project      | Board                | Chip     | esp-idf  | Result   | Notes
| ------- | ------------ | -------------------- | -------- | -------  | -------- | -----
| 25JAN24 | Joystick     | ESP-WROVER-KIT v4.1  | ESP32    | v5.1.2   | Compiles | 