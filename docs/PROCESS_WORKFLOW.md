# Machine Process Workflow

End-to-end description of the EcoBrick Machine production line.

## Process Flow

```
Shredding -> Bottle Filling -> Robotic Arm Transfer -> Compression
         -> Heat Sealing (Soldering Iron) -> Conveyor Transport -> Collection
```

## Station Details

### 1. Shredding and Bottle Filling

A shredder breaks down a waste plastic bottle into small flakes. These flakes fall by gravity into a second, clean bottle that serves as the container. This second bottle will become the final eco-brick.

**Shredder Motor Control:** The AC shredder motor is controlled by an SSR (solid-state relay) connected to ESP32 GPIO 18. The SSR module is active HIGH (GPIO HIGH = motor powered). The shredder turns ON automatically when the START button is pressed and remains ON for the entire production cycle.

**Bottle Fill Detection:** A 1kg load cell with HX711 ADC (connected to GPIO 34/35) monitors the weight of the bottle as flakes accumulate. The system waits in the WAIT_FOR_FILL state until the weight exceeds `FILL_WEIGHT_THRESHOLD_G` (default: 120g) before proceeding to the robotic arm transfer. This ensures each bottle has sufficient material for a proper eco-brick.

### 2. Robotic Arm Transfer

A 3-DOF pick-and-place robotic arm (controlled by PCA9685 servo driver on I2C GPIO 16/17) moves the filled container bottle from the shredding station to the pressing station. The arm consists of three servos:

| Servo | PCA9685 Channel | Function |
|-------|----------------|----------|
| Base | CH0 | Rotates arm between stations (30°-150°) |
| Shoulder | CH1 | Lifts/lowers arm (20°-100°) |
| Gripper | CH2 | Opens/closes to grab/release bottle (60°-120°) |

**Pick Sequence:**
1. Arm moves to pick position (base rotates toward shredding station)
2. Shoulder lowers arm, gripper opens
3. Gripper closes to grab bottle neck
4. Shoulder raises arm with bottle
5. Base rotates toward pressing station

**Place Sequence:**
1. Arm moves to place position (base rotates toward pressing station)
2. Shoulder lowers arm
3. Gripper opens to release bottle
4. Arm returns to home position

### 3. Compression

The press actuator drives a plunger down into the bottle neck, compressing the shredded plastic inside. The bottle is held under pressure for a short dwell time so the flakes settle and the volume reduces to a minimum. The press then retracts, leaving a dense pack of plastic inside the bottle.

**Button Control:** START and STOP tactile buttons control the machine. The START button (GPIO 21) turns ON the iron relay and shredder SSR, then begins the auto cycle. The STOP button (GPIO 22) pauses the cycle but keeps the iron and shredder ON.

### 4. Heat Sealing

The compressed bottle is positioned at the sealing station. The seal actuator lowers a 220 V soldering iron, fitted with a custom-shaped tip, onto the bottle neck. A 1CH 5V relay module (active LOW) controlled by the ESP32 switches the iron on. The hot tip contacts the PET neck material and melts it, fusing the opening shut. After the programmed hold time, the iron turns off and the actuator retracts. The seal cools and hardens within a couple of seconds.

**Tip note:** the custom tip is shaped to match the bottle neck contour and is coated or made from a material that resists PET adhesion. Standard iron tips degrade quickly when melted against PET.

### 5. Collection

The sealed bottle rides the conveyor to the end and drops into a collection box. From here the bricks can be stacked, stored, or sold as building material.

## Station Summary

| # | Station | Action |
|---|---------|--------|
| 1 | Shredding / Filling | Shred bottle, collect flakes in container bottle (monitored by load cell) |
| 2 | Robotic Arm | Pick-and-place: shredding station to pressing station (3-DOF PCA9685 controlled) |
| 3 | Pressing | Compress shredded plastic inside the bottle |
| 4 | Heat Sealing | Iron tip pressed to neck, relay powers iron, melt and fuse seal |
| 5 | Collection | Conveyor carries sealed brick to collection box |
