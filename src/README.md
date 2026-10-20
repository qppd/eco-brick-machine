# src/

Source code for the EcoBrick Machine.

## Conventions

- Each `.ino`, `.cpp`, and `.h` file has a single responsibility.
- `EcoBrickMachine.ino` contains the state machine and serial command handler.
- `STEPPER_CONF.*` wraps the TB6600 driver with blocking and non-blocking move APIs.
- `RELAY_CONF.*` manages relay channels, actuator interlocks, and SSR output.
- No build artifacts or generated files are committed.
