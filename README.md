# GameEngine
CSE167 Final Project: Game Engine made using OpenGL, GLFW, GLM and GLEW via Xcode

## Project Blog and Video
* Blog: https://medium.com/@snyoonoh/created-by-alex-ouyang-seung-yoon-oh-dede5c7ac70a#.xm0hzkomt
* Video: https://www.youtube.com/watch?v=1vc1sbcdWMo


## Dependencies Included: 
Dependencies included under /lib
* glew-1.13.0
* glfw-3.1.2
* glm
* freetype-2.6.3

## Dependencies To Install:

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
        4. cd into SDL2.framework
        5. Sign the framework via command: codesign -f -s - SDL2
        6. Link SDL2.framework in GameEngine.xcodeproj under Link binary with libraries

* Install SDL2 Mixer:

        1. Go to the website: https://www.libsdl.org/projects/SDL_mixer/
        2. Download runtime binaries and extract them.
        3. Drag the .framework and drop it into directory: /Library/Frameworks 
        4. cd into SDL2_mixer.framework
        5. Sign the framework via command: codesign -f -s - SDL2_mixer
        6. Link SDL2_mixer.framework in GameEngine.xcodeproj under Link binary with libraries

* Link Freetype2 in Xcode:
        Since the header seach path is configured already, you'll just have to:
        1. Find libfreetype.dylib in either: /usr/X11/lib/libfreetype.6.dylib or /opt/X11/lib/libfreetype.dylib
        2. Drag libfreetype.dylib or libfreetype.6.dylib into Link binary with libraries section in GameEngine.xcodeproj 

## Compile and Run:

To configure and run on xcode: edit scheme -> Options -> check 'Use custome working directory' and point to the directory containing all the source code. Then the project should run without any problems.
