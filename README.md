# Space Shooter

*In the following, the game scenario, requirements, and the provided hardware and software will be presented. This is followed by the activities that you must do to solve the exercise.*

### Scenario

You are tasked with building a simple **space shooter game** on a Raspberry Pi, displayed on a small **OLED screen**. The spaceship is controlled by **four direction buttons**, and the player can fire bullets using a **shooting button**. Some of the code will be provided to kick-start your work, but you will have to implement access to the button gpios, control the space ship and bullets among others.

### Game Requirements

* The spaceship starts at the left center of the screen.
* Direction buttons move it left/right/up/down.
* Tapping the shooting button fires one bullet (per tap)
* Holding the shooting button must fire only one bullet.
* The bullets move to the right at fixed speed.
* Enemies moves in from the right to left at fixed speed (random vertical positions).
* As a bullet and an enemy collide (meet) → bullet and enemy disappears, score increases.
* If an enemy reaches the left side → game over
* There should be no tearing/flickering on the display

Beside these requirements, you are allowed to create additional rules :-)

---

### Prerequisites

You must [fork](https://www.geeksforgeeks.org/git/how-to-fork-a-project-in-gitlab/) this repository to you own Gitlab group and clone it to you RPI from there. 

To communicate with the OLED display, the required libraries must be installed on your RPI. You can find files and instructions here: [gitlab.au.dk/platforms/rpi5/ecesyshat/](https://gitlab.au.dk/platforms/rpi5/ecesyshat/-/tree/main/Libraries?ref_type=heads)

---

### Hardware Design

The ECE-SYS-HAT provides six buttons (switches), three LEDs, accelerometer, and an OLED dislay.

#### Buttons

You'll see the names of the buttons printed on the printed circuit board (PCB) as seen below. We need this information to assign the buttons to the correct game functions. All buttons are active-low ('0' is actice).

![ECE-SYS-HAT PCB](figures/ece_hat.jpg)

#### Display

The Display is an 128 x 32 pixel OLED display. The pixels are oriented as shown below.

![OLED Display](figures/bitmaps-display_legend.png)

To move the ship *left*, you must *increment* the x-position. To move the ship *up*, you must *decrement* the y-position.

The individual bitmaps (ship, bullet, enemy) use a similar orientation. The provided ship and enemy are 8x7 pixels and the bullet is 8x1 pixels.

![Ship Bitmap](figures/bitmaps-ship.png)

This means that placing the ship at (0,0) will place the ship's upper-left corner in the display's upper-left corner. Placing the ship at (28,0) will place the ship's bottom three pixel rows below the screen. You will get run-time warnings if you place bitmaps outside the screen area.

---

### Software Design

Since we are training working with threads, many of the software classes also contains threads to perform work for the individual class. The classes as shown in the Class Diagram below. 

![Class Diagram](figures/cd_space_shooter.png)

The game is assembled and started in *space_shooter.cpp*

#### `Accelerometer`

This class is a stub, but can be used in later extensions of the game.

The idea is: 
  * Reads the accelerometer at fixed intervals.
  * Updates a global "gravity vector" that affects object motion.

#### `Bullet`

Controls adding bullets and moving them on the screen.

**YOU MUST IMPLEMENT THE CONTENT OF THIS CLASS**

Contains two threads:

  1. Update the bullet vector when with one new bullet if the shoot button has been tapped (pressed-released)
  2. Update the bullet position at a fixed speed

#### `Buttons`

Takes button (gpio) inputs and publishes them for the game.

**YOU MUST IMPLEMENT THE CONTENT OF THIS CLASS**

1) The *Constructor* must be updated so, the gpio inputs are requested and prepared for reading

2) Create at thread that runs at fixed interval to:
  * Read the GPIO-connected direction + shooting buttons.
  * Publish when shooting button has been tapped (pressed-released)
  * Update the shared *game state*.

#### `Display`

Runs a thread at fixed interval to:
* Draw the game state (spaceship, bullets, enemies)
* Draw score, number of enemies and bullets

#### `Enemy`

Runs a thread at fixed interval to:
* Spawn new enemies
* Move enemy position
* Delete enemies if they move out of screen (left)

#### `GameController`

Runs the main game control loop thread to:
* Provide collision detection of bullets and enemies. 
* Delete bullet and enemy from `GameState` if they collide.

A collision is detected just as the face of a bullet and the face of an enemy intersect.

![](figures/bitmaps-collision.png)

Note! The game controller thread deletes bullets and enemies if they collide. Both are stored in vectors. Erasing an element in a vector will invalidate current iteraters using those vectors.

#### `GameState`

Contains the current state of the game. It is accessed by almost all classes and contains vectors of bullets, enemies, x-y positions, score and more. 

#### `I2CDriver`

Implements I2C communication to the OLED screen to be used by the OLED library.

#### `Ship`

Runs a thread at fixed interval to:
* Update ship position according to Move Button state

#### `Main` (space_shooter)

The main loop does the following:
* Initialize I2C and OLED library
* Instantiate objects
* Start threads
* Join threads

---

### Activities

To implement the missing parts of the game, the following activities are proposed:

#### Activity 1: Buttons Class

**Task 1:**
Implement the constructor for the `Buttons` to allocate gpios and set their direction.

**Task 2:** Implement `buttonsThread`. The thread must run at a fixed interval and read all gpios. 
The direction (move) button state must be accessable using `Buttons::readMoveButtons()`. See details in the function. 
Shoot button taps (press-relase) inputs must be signalled imidiately using `GameState::shootButton_cv`. 

#### Activity 2: Implement Bullets Class

**Task 1:** Implement `Bullet::bulletInputThread()`. This thread must wait for the bullet button to be tapped. When this has occurred, a new bullet must be added to `GameState.bullets` with x-y position at the nose of the space ship.

**Task 2:** Implement `Bullet::bulletUpdateThread()`. This thread must run at a fixed 25 ms interval and must update all `GameState.bullets`, so that they moves one pixel to the left. Changing the update interval effects the bullet movement speed. 

#### Activity 3: Analyse and fix synchronization issues

**Task 1:** Analyze how threads are currently synchronized. Draw an activity diagram showing the flow of the individual threads and the interaction between them. Argue whether synchronization is sufficient or not.

**Task 2:** Fix synchronization issues and test if requirements have been met.

Consider the following:
* Where is synchronization needed?
* Does your current implementation supports all requirements? Ex: Holding shooting button only emits ONE bullet? Emeny and bullet always dissapear when colliding?

If your current implementation does not fulfill the requirements 100%, then design and implement the required synchronization. It's considered unsafe to simply adjust times to try and fix synchronization issues. 

#### Optional Activities

If you finish up early, you are welcome to implement some of the folowing ideas:

* Use **double-buffering** for OLED updates to prevent flickering.
* If Ship and Enemy collide, game is over.
button).
* Introduce different bullet types and/or directions depending on which shooting button is pressed.
* Add a **pause** function (e.g., pressing right shooting
