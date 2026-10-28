# Machine Process Workflow

End-to-end description of the EcoBrick Machine production line.

## Process Flow

```
Shredding -> Bottle Filling (in cup on rotating platform) -> Drop to Conveyor
         -> Conveyor to Boom Gate -> Stop at Position -> Compression
         -> Heat Sealing (Soldering Iron) -> Collection
```

## Station Details

### 1. Shredding and Bottle Filling

A shredder breaks down a waste plastic bottle into small flakes. These flakes fall by gravity into a second, clean bottle that sits inside a 3D printed cup on a rotating platform. The cup keeps the bottle upright and properly aligned.

**Shredder Motor Control:** The AC shredder motor is controlled by an SSR (solid-state relay) connected to ESP32 GPIO 18. The SSR module is active HIGH (GPIO HIGH = motor powered). The shredder turns ON automatically when the START button is pressed and remains ON for the entire production cycle.

**Bottle Fill Detection:** A 1kg load cell with HX711 ADC (connected to GPIO 34/35) monitors the weight of the bottle+cup as flakes accumulate. The system waits in the WAIT_FOR_FILL state until the weight exceeds `FILL_WEIGHT_THRESHOLD_G` (default: 120g) before proceeding. This ensures each bottle has sufficient material for a proper eco-brick.

### 2. Rotating Platform Drop Mechanism

After the bottle is filled, a single servo (connected to GPIO 25) rotates the entire platform like a clock hand:

**Drop Sequence:**
1. **HOME position (0°):** Platform is horizontal, bottle+cups sits under shredder
2. **Rotate to DROP position (120°):** Platform tilts, bottle+cup rolls off onto conveyor
3. **Return to HOME:** Platform rotates back for next bottle

**Mechanical Design Notes:**
- Servo horn attaches to platform pivot point
- Platform must be balanced to prevent servo overload
- Load cell should be mounted securely and centered
- Cup should be lightweight but sturdy (PETG recommended)
- Conveyor should be positioned to catch bottle at drop angle

### 3. Conveyor Transport to Boom Gate

The bottle (still inside the cup) moves along the conveyor belt toward the boom gate. The cup ensures the bottle stays upright during transport.

### 4. Boom Gate Positioning

A servo-operated boom gate with limit switch precisely positions the bottle under the press:

**Positioning Sequence:**
1. **Boom UP (0°):** Gate raised, bottle rolls freely
2. **Bottle arrives:** Rolls until it hits the boom arm
3. **Limit switch triggered:** Signal sent to ESP32
4. **Boom DOWN (90°):** Servo lowers boom, clamping bottle in place
5. **Bottle STOPPED:** Perfectly positioned under press and seal station

**Advantages:**
- Mechanical stop = repeatable positioning (±0.5mm)
- No extra sensors needed — limit switch does double duty
- Simple, reliable design
- Can handle variable conveyor speeds

### 5. Compression

The press actuator drives a plunger down into the bottle neck, compressing the shredded plastic inside. The bottle is held under pressure for a short dwell time so the flakes settle and the volume reduces to a minimum. The press then retracts, leaving a dense pack of plastic inside the bottle.

**Button Control:** START and STOP tactile buttons control the machine. The START button (GPIO 21) turns ON the iron relay and shredder SSR, then begins the auto cycle. The STOP button (GPIO 22) pauses the cycle but keeps the iron and shredder ON.

### 6. Heat Sealing

The compressed bottle (held in cup, positioned by boom gate) is at the sealing station. The seal actuator lowers a 220 V soldering iron, fitted with a custom-shaped tip, onto the bottle neck. A 1CH 5V relay module (active LOW) controlled by the ESP32 switches the iron on. The hot tip contacts the PET neck material and melts it, fusing the opening shut. After the programmed hold time, the iron turns off and the actuator retracts. The seal cools and hardens within a couple of seconds.

**Tip note:** the custom tip is shaped to match the bottle neck contour and is coated or made from a material that resists PET adhesion. Standard iron tips degrade quickly when melted against PET.

### 7. Release and Collection

After sealing:
1. **Boom opens:** Gate raises, releasing the bottle
2. **Conveyor moves:** Bottle+cups continues to collection area
3. **Cup removal:** Operator removes cup, collects sealed eco-brick

## Station Summary

| # | Station | Action |
|---|---------|--------|
| 1 | Shredding / Filling | Shred bottle, collect flakes in cup (monitored by load cell) |
| 2 | Rotating Platform | Drop filled cup onto conveyor (1 servo, 120° rotation) |
| 3 | Conveyor Transport | Move cup to boom gate |
| 4 | Boom Gate Positioning | Stop bottle at exact position (limit switch) |
| 5 | Pressing | Compress shredded plastic inside the bottle |
| 6 | Heat Sealing | Iron tip pressed to neck, relay powers iron, melt and fuse seal |
| 7 | Collection | Conveyor carries sealed brick to collection box |

## Cycle Timing

| Step | Description | Duration |
|------|-------------|----------|
| FEED | Stepper positions bottle in cup | ~1.6s (1600 steps @ 60 RPM) |
| WAIT_FOR_FILL | Wait for fill threshold | Variable (depends on flake flow) |
| ROTATE_DROP | Servo rotates to drop | ~1s |
| RETURN_HOME | Servo returns to home | ~1s |
| CONVEYOR_MOVE | Cup moves to boom gate | ~2s |
| BOOM_CLOSE | Boom lowers, limit switch triggers | ~0.5s |
| PRESS_EXTEND | Press extends | 1.5s |
| PRESS_HOLD | Compression held | 0.5s |
| PRESS_RETRACT | Press retracts | 1.5s |
| SEAL_EXTEND | Iron lowers | 1.5s |
| SEAL_HOLD | Iron heats seal | 3s |
| SEAL_RETRACT | Iron lifts | 1.5s |
| BOOM_OPEN | Boom raises | ~0.5s |
| CONVEYOR_FORWARD | Bottle moves to collection | ~1.5s |
| RETURN | Stepper returns | ~1.6s |
| PAUSE | Brief idle | 1s |
| **Total** | **Per cycle** | **~20-25s** |

## Mechanical Design Notes

### 3D Printed Cup
- **Material:** PETG (heat resistant, durable)
- **Inner diameter:** 60mm (fits 500ml PET bottle)
- **Height:** 120mm
- **Weight:** ~30g (lightweight for servo)
- **Features:** Drainage holes, flared opening, standoff feet

### Boom Gate
- **Arm material:** Bamboo tube or carbon fiber (lightweight, rigid)
- **Limit switch:** Rocker type, mounted at arm tip
- **Servo:** MG90S (metal gears for durability)
- **Pivot point:** Above conveyor level for clean break

### Conveyor
- **Type:** Belt conveyor or roller conveyor
- **Speed:** Adjustable (2-5 cm/s recommended)
- **Guides:** Side rails to keep cup centered
- **Length:** ~500mm (from drop point to collection)
