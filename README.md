# 3D game engine made in C++ with OpenGL

![](https://github.com/Sonni/OpenGL-Game-Engine/blob/master/engine_pic.png?raw=true)


[Click me for video demo on YouTube](https://www.youtube.com/watch?v=Q2YozT22QDc)


Note that the makefile is only made for MAC OS thus, the engine itself is cross platform.

Feature:
- 3D model loading (.tna - the engines own file type. [Link to my dae -> tna parser](https://github.com/Sonni/DAE_to_TNA/tree/master))
- Entity/Component based
- Light, water and shadows
- Normal mapping and parallax displacement mapping
- First and 3rd person camera
- GUI and billboards
- Skinning animation
- Skybox
- Post processing


Upcoming:
- Particle system
- Physics
- Sound
- Mouse picking

3rd party libraries used by the engine is a follows:
- SDL2 window and input handling.
- stb_image.h JPG and PNG decoder.
- OpenGL with glsl version 330.
- GLEW extension loading.
- Math lib