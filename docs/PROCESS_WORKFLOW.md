# Machine Process Workflow

End-to-end description of the EcoBrick Machine production line.

## Process Flow

```
Shredding -> Bottle Filling (on rotating platform) -> Drop to Press
         -> Compression -> Heat Sealing (Soldering Iron) -> Collection
```

## Station Details

### 1. Shredding and Bottle Filling

A shredder breaks down a waste plastic bottle into small flakes. These flakes fall by gravity into a second, clean bottle that sits on a rotating platform with an integrated load cell.

**Shredder Motor Control:** The AC shredder motor is controlled by an SSR (solid-state relay) connected to ESP32 GPIO 18. The SSR module is active HIGH (GPIO HIGH = motor powered). The shredder turns ON automatically when the START button is pressed and remains ON for the entire production cycle.

**Bottle Fill Detection:** A 1kg load cell with HX711 ADC (connected to GPIO 34/35) monitors the weight of the bottle as flakes accumulate on the rotating platform. The system waits in the WAIT_FOR_FILL state until the weight exceeds `FILL_WEIGHT_THRESHOLD_G` (default: 120g) before proceeding to the drop sequence. This ensures each bottle has sufficient material for a proper eco-brick.

### 2. Rotating Platform Drop Mechanism

After the bottle is filled, a single servo (connected to GPIO 25) rotates the entire platform like a clock hand:

**Drop Sequence:**
1. **HOME position (0°):** Platform is水平 (level), bottle fills from shredder
2. **Rotate to DROP position (120°):** Platform tilts, bottle rolls off onto conveyor
3. **Return to HOME:** Platform rotates back for next bottle

**Mechanical Design Notes:**
- Servo horn attaches to platform pivot point
- Platform must be balanced to prevent servo overload
- Load cell should be mounted securely and centered
- Conveyor should be positioned to catch bottle at drop angle

### 3. Compression

The press actuator drives a plunger down into the bottle neck, compressing the shredded plastic inside. The bottle is held under pressure for a short dwell time so the flakes settle and the volume reduces to a minimum. The press then retracts, leaving a dense pack of plastic inside the bottle.

**Button Control:** START and STOP tactile buttons control the machine. The START button (GPIO 21) turns ON the iron relay and shredder SSR, then begins the auto cycle. The STOP button (GPIO 22) pauses the cycle but keeps the iron and shredder ON.

### 4. Heat Sealing

The compressed bottle is positioned at the sealing station. The seal actuator lowers a 220 V soldering iron, fitted with a custom-shaped tip, onto the bottle neck. A 1CH 5V relay module (active LOW) controlled by the ESP32 switches the iron on. The hot tip contacts the PET neck material and melts it, fusing the opening shut. After the programmed hold time, the iron turns off and the actuator retracts. The seal cools and hardens within a couple of seconds.

**Tip note:** the custom tip is shaped to match the bottle neck contour and is coated or made from a material that resists PET adhesion. Standard iron tips degrade quickly when melted against PET.

### 5. Collection

The sealed bottle continues along the conveyor to the collection area. From here the bricks can be stacked, stored, or sold as building material.

## Station Summary

| # | Station | Action |
|---|---------|--------|
| 1 | Shredding / Filling | Shred bottle, collect flakes in container bottle (monitored by load cell on rotating platform) |
| 2 | Rotating Platform | Drop filled bottle onto conveyor (1 servo, 120° rotation) |
| 3 | Pressing | Compress shredded plastic inside the bottle |
| 4 | Heat Sealing | Iron tip pressed to neck, relay powers iron, melt and fuse seal |
| 5 | Collection | Conveyor carries sealed brick to collection box |

## Cycle Timing

| Step | Description | Duration |
|------|-------------|----------|
| FEED | Stepper positions bottle | ~1.6s (1600 steps @ 60 RPM) |
| WAIT_FOR_FILL | Wait for fill threshold | Variable (depends on flake flow) |
| ROTATE_DROP | Servo rotates to drop | ~1s |
| RETURN_HOME | Servo returns to home | ~1s |
| PRESS_EXTEND | Press extends | 1.5s |
| PRESS_HOLD | Compression held | 0.5s |
| PRESS_RETRACT | Press retracts | 1.5s |
| SEAL_EXTEND | Iron lowers | 1.5s |
| SEAL_HOLD | Iron heats seal | 3s |
| SEAL_RETRACT | Iron lifts | 1.5s |
| RETURN | Stepper returns | ~1.6s |
| PAUSE | Brief idle | 1s |
| **Total** | **Per cycle** | **~15-20s** |
