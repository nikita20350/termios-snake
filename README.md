# termios-snake
## Snake Game in C

### Overview
A **classic snake game** implemented in **C** using minimal libraries, primarily `termios.h` for terminal-based input.

### Customization
Modify the following macros to adjust game settings:

| Macro                  | Description               | Effect                       |
|-----------------------|--------------------------|-----------------------------|
| `SIZE_X` / `SIZE_Y`   | Board dimensions        | Adjust the game area size   |
| `TICKS_PER_MOVEMENT`   | Game speed             | Lower values = Faster speed |
| `WINNING_SCORE`        | Score to win           | Change the required score to win |

### Example Usage
```c
#define SIZE_X 20
#define SIZE_Y 10
#define TICKS_PER_MOVEMENT 100
#define WINNING_SCORE 10
```
