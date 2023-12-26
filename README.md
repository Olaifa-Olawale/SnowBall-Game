# Snowball-Game

Welcome to the Snowball Fight Game! This game is a simple implementation of a snowball fight between a player and an opponent in a 3D environment. Players can move, jump, throw snowballs, and engage in a snowy battle.


# Table of Contents
- Prerequisites
- Features
- Controls
- Gameplay
- GameVideos


# Prerequisites

- [OpenGL](https://www.opengl.org/) (make sure it's properly installed on your system)
- [GLUT](https://www.opengl.org/resources/libraries/glut/) (OpenGL Utility Toolkit)



# Features

- Player Movement: Control your character using the keyboard to move forward, backward, strafe left, and strafe right.
- Player Interaction: Make snowballs and throw them at your opponent to score points.
- Opponent AI: An AI-controlled opponent navigates the environment, makes snowballs, and throws them back at the player.
- Dynamic Terrain: The game features a dynamic terrain with different surfaces, affecting player and opponent movement.

# Controls

- ESC: Exit the program
- 1: Toggle debugging cube
- 2: Switch between debugging cube sizes
- 3: Toggle display of collision boxes
- 4: Toggle display of frame rate information
- 5: Switch between different path display options
- '-' and =: Decrease and increase snowball radius
- F: Throw a snowball
- R: Reset player position
- P: Pause/unpause the game
- T: Calls the opponenet to the direction player is looking at 
- W: Move player forward
- S: Move player backward
- A: Strafe player left
- D: Strafe player right
- Arrow keys: Adjust player's view (up/down for pitch, left/right for yaw)
- Space: Jump
- Y: Slow down the game for debugging purposes


# Gameplay

- Objective: Score points by hitting the opponent with snowballs while avoiding getting hit.
- Player Snowballs: Create snowballs when standing on snowy terrain.
- Opponent AI: The opponent follows a path, makes snowballs on snowy terrian, and throws them on high points.
- Terrain Types: Different terrain types (snow, block, ground) affect movement speed and snowball creation.

# GameVideos
- " Links and Node.mp4 " shows how i connected the maps using Nodes and Links 
- " Player Movment example.mp4 " shows the player movement and Opp. throwing snowballs 
- " Opp. Path Finding.mp4 " shows how opponent navigates the map using path finding algorithm

