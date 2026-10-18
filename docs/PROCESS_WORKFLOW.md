# Machine Process Workflow

End-to-end production flow of the EcoBrickMachine.

## Overall Process

```
Shredding → Bottle Filling → Robotic Arm Transfer → Compression
         → Induction Sealing → Conveyor Transport → Collection Box
```

## Steps

### 1. Plastic Bottle Shredding and Filling

The shredder processes the plastic bottle into smaller pieces, which are
collected inside another plastic bottle.

### 2. Robotic Arm Transfer

A robotic arm performs a simple pick-and-place operation, transferring the
filled bottle from the shredding station to the pressing station.

### 3. Plastic Compression

The press compresses the shredded plastic inside the bottle to reduce its
volume and make the contents more compact.

### 4. Conveyor and Induction Sealing

The conveyor transports the compressed plastic-filled bottle to the induction
sealing station, where heat is applied to seal the bottle's opening.

### 5. Final Conveying and Collection

The sealed bottle continues along the conveyor until it reaches the end and
drops into a collection box.

## Station Overview

| # | Station              | Action                                              |
|---|----------------------|-----------------------------------------------------|
| 1 | Shredding / Filling  | Shred plastic bottle, fill the collection bottle    |
| 2 | Robotic Arm          | Pick-and-place: shredding station → pressing station |
| 3 | Pressing             | Compress shredded plastic inside the bottle         |
| 4 | Induction Sealing    | Conveyor to sealer, heat-seal the bottle opening    |
| 5 | Collection           | Conveyor to the end, drop into the collection box   |
