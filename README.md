# GameEngine
CSE167 Final Project

* Make sue you have Boost 1.60.0_2 installed. If not install via command: 

        brew update
        brew install boost


* To Make sure Boost 1.60.0_2 is installed, we want to check if the following directory exists via command:
    
        /usr/local/Cellar/boost/1.60.0_2/include/
        Expected result: /usr/local/Cellar/boost/1.60.0_2/include/: is a directory


* Configure and run on xcode: edit scheme -> Options -> check 'Use custome working directory' and point to the directory containing all the source code. Then the project should run without any problems.
