# 3D Printed Cup Design for EcoBrick Machine

## Purpose
Cup holds the PET bottle upright during transport from rotating platform to press/seal station. Ensures proper alignment for compression and sealing operations.

## Specifications

### Bottle Dimensions (Standard 500ml PET)
- Base diameter: ~55mm
- Neck diameter: ~28mm
- Height: ~200mm
- Wall thickness: ~0.3mm

### Cup Design Parameters
- Inner diameter: 60mm (5mm clearance on each side)
- Height: 120mm (holds bottle at stable position)
- Wall thickness: 3mm (structural strength)
- Base thickness: 4mm (durability)

## File Structure

```
cup/
├── cup_bottom.stl      # Base piece with drainage holes
├── cup_wall.stl        # Cylindrical wall
└── cup_assembly.md     # Assembly instructions
```

## Print Settings

| Parameter | Value |
|-----------|-------|
| Material | PETG (heat resistant, durable) |
| Infill | 40% (structural integrity) |
| Layers | 0.2mm (balanced quality/speed) |
| Walls | 3 perimeters (strength) |
| Speed | 50mm/s |
| Temperature | 240°C (nozzle), 85°C (bed) |
| Cooling | 100% fan |

## Design Features

### 1. Base Section
- **Outer diameter:** 70mm
- **Inner diameter:** 60mm
- **Thickness:** 4mm
- **Drainage holes:** 4x Ø3mm (allows water drainage if bottle leaks)
- **Feet:** 4x 10mm standoff (prevents direct contact with conveyor)

### 2. Wall Section
- **Height:** 120mm
- **Inner diameter:** 60mm (tapered slightly: 60mm top → 58mm bottom for easy removal)
- **Thickness:** 3mm
- **Ribbing:** Internal ribs every 30mm for strength
- **Opening:** Flared top (65mm) for easy bottle insertion

### 3. Release Mechanism (Optional)
Two options:

**Option A: Hinged Cup**
- Add hinge at bottom
- Servo or manual latch opens cup
- Bottle slides out after seal

**Option B: Sliding Base**
- Base slides out horizontally
- Released by cam or servo
- More complex but reliable

## Assembly Instructions

1. Print cup_wall.stl
2. Print cup_bottom.stl
3. Glue base to wall using PETG-compatible adhesive (or solvent weld with acetone for ABS)
4. Sand inner surface for smooth bottle movement
5. Test fit with actual bottle

## Tolerance Considerations

| Dimension | Design | Tolerance |
|-----------|--------|-----------|
| Inner diameter (bottom) | 60mm | +0.5mm (bottles vary) |
| Inner diameter (top) | 65mm | -0mm (flared for insertion) |
| Wall height | 120mm | ±0.5mm |
| Base diameter | 70mm | ±0.5mm |

## Material Recommendation

**PETG preferred over PLA because:**
- Higher heat resistance (up to 80°C vs 60°C)
- More durable and less brittle
- Better layer adhesion
- Food-safe (important for bottle contact)

If PETG unavailable, use **ABS** or **ASA** as alternatives.

## CAD Files

Generate using OpenSCAD or CAD software:

```openscad
// Cup base
difference() {
  cylinder(h=4, d=70, center=true);
  cylinder(h=5, d=60, center=true);
  
  // Drainage holes
  translate([20, 0, 0]) cylinder(h=5, d=3, center=true);
  translate([-20, 0, 0]) cylinder(h=5, d=3, center=true);
  translate([0, 20, 0]) cylinder(h=5, d=3, center=true);
  translate([0, -20, 0]) cylinder(h=5, d=3, center=true);
}

// Cup wall
difference() {
  cylinder(h=120, d=65, center=true);
  cylinder(h=121, d=60, center=true);
}
```

## Testing Checklist

- [ ] Bottle fits snugly (not too loose, not too tight)
- [ ] Cup stands upright on conveyor
- [ ] Base drainage holes work
- [ ] Can be removed easily after process
- [ ] Heat resistant to iron proximity
