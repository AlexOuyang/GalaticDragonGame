# GameEngine
CSE167 Final Project: Game Engine made using OpenGL, GLFW, GLM and GLEW via Xcode

* Install the latest Boost (currently 1.60.0_2), via command: 

        brew update
        brew install boost


* To Make sure Boost 1.60.0_2 is installed, we want to check if the following directory exists via command:
    
        /usr/local/Cellar/boost/1.60.0_2/include/
        Expected result: /usr/local/Cellar/boost/1.60.0_2/include/: is a directory

* Install SDL2:

        1. Go to the website: https://www.libsdl.org/download-2.0.php 
        2. Download runtime binaries and extract them.
        3. Drag the .framework and drop it into directory: /Library/Frameworks 
        4. Sign the framework via command: codesign -f -s - SDL2

* Install SDL2 mixer:
        1 Go to the website: https://www.libsdl.org/projects/SDL_mixer/
        2. Download runtime binaries and extract them.
        3. Drag the .framework and drop it into directory: /Library/Frameworks 
        4. Sign the framework via command: codesign -f -s - SDL2_mixer
        

* Configure and run on xcode: edit scheme -> Options -> check 'Use custome working directory' and point to the directory containing all the source code. Then the project should run without any problems.