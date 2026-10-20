# Machine Process Workflow

End-to-end description of the EcoBrick Machine production line.

## Process Flow

```
Shredding → Bottle Filling → Robotic Arm Transfer → Compression
         → Heat Sealing (Soldering Iron) → Conveyor Transport → Collection
```

## Station Details

### 1. Shredding and Bottle Filling

A shredder breaks down a waste plastic bottle into small flakes. These flakes fall by gravity into a second, clean bottle that serves as the container. This second bottle will become the final eco-brick.

### 2. Robotic Arm Transfer

A pick-and-place robotic arm moves the filled container bottle from the shredding station to the pressing station. The transfer is a simple single-step motion: pick, place, return.

### 3. Compression

The press actuator drives a plunger down into the bottle neck, compressing the shredded plastic inside. The bottle is held under pressure for a short dwell time so the flakes settle and the volume reduces to a minimum. The press then retracts, leaving a dense pack of plastic inside the bottle.

### 4. Heat Sealing

The compressed bottle is conveyed (or manually transferred) to the sealing station. The seal actuator lowers a 220 V soldering iron, fitted with a custom-shaped tip, onto the bottle neck. An SSR controlled by the ESP32 switches the iron on. The hot tip contacts the PET neck material and melts it, fusing the opening shut. After the programmed hold time, the iron turns off and the actuator retracts. The seal cools and hardens within a couple of seconds.

**Tip note:** the custom tip is shaped to match the bottle neck contour and is coated or made from a material that resists PET adhesion. Standard iron tips degrade quickly when melted against PET.

### 5. Collection

The sealed bottle rides the conveyor to the end and drops into a collection box. From here the bricks can be stacked, stored, or sold as building material.

## Station Summary

| # | Station | Action |
|---|---------|--------|
| 1 | Shredding / Filling | Shred bottle, collect flakes in container bottle |
| 2 | Robotic Arm | Pick-and-place: shredding station to pressing station |
| 3 | Pressing | Compress shredded plastic inside the bottle |
| 4 | Heat Sealing | Iron tip pressed to neck, SSR powers iron, melt and fuse seal |
| 5 | Collection | Conveyor carries sealed brick to collection box |
