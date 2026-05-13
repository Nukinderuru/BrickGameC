# Tetris FSM

States:

- `Start`
- `Spawn`
- `Moving`
- `Move`
- `Attaching`
- `GameOver`

Transitions:

- `Start -> Spawn`: start button
- `Spawn -> Moving`: next piece fits on the field
- `Spawn -> GameOver`: next piece collides on spawn
- `Moving -> Move`: gravity or soft-drop timer expires and downward move is possible
- `Moving -> Attaching`: gravity or soft-drop timer expires and downward move is blocked
- `Move -> Moving`: piece moved down one row
- `Move -> Attaching`: piece can no longer move down
- `Attaching -> Spawn`: piece merged, rows cleared, continue game
- `Attaching -> GameOver`: top row reached after merge
- `GameOver -> Spawn`: start button begins a new game

Pause is implemented as a flag that freezes `Moving` transitions instead of a separate FSM state.
