Currently, the "engine" is fairly completed with many optimizations both to how the world updates and how the world renders. More things that could be added is multi-threading of chunks and better randomization for smoother looking particles.

The main things to work on is the particles as the current data-oriented design is extremely limiting. Though the simulation can push 4000 fps at max load, the things the simulation can do is very limited. Switching to a feature-packed object-oriented design in the future would be a lot more fun.
