# Challenge 2: Image Registration

This project has been created as part of an internship application process at
GHGSat. It comprises an image registration program, a test suite, a build
compiled for the ARM platform, and a Docker container. More precisely, the
repository is organized in the following subdirectories:
- **data** contains images to demonstrate and test the application.
- **docs** contains the source files for building the documentation using
  Sphinx and a build of the documentation. To read the documentation, open
  the file "docs/build/index.html".
- **image-registration** contains the C++ source files for the application.
- **tests** contains the Python test suite.

The procedure to build, use, and test the application is described in the
[user guide](#user-guide) below. Possible [improvements](#improvements) are
listed at the end of the file.

## User Guide

The application performs image registration on specified images.
The [installation](#installation) explains how to build the project.
Section [interface](#interface) shows how to use it.

### Installation

The source files "image-registration.cpp" and "image-registration.hpp", located
in the "image-registration" directory, can be compiled on any platform with a
valid C++ compiler and the OpenCV library. The procedure below shows how to
build the application for the ARM platform.

1. **Installation of a cross compiler and OpenCV**: This step can be skipped
  if a cross compiler for the ARM platform as well as the OpenCV library are
  installed on the system. Commands are given for an Ubuntu environment and
  were tested on WSL. First, obtain the cross compiler:

    > sudo apt-get install arm-linux-gnueabi-gcc
    > sudo apt-get install arm-linux-gnueabi-g++
   
  Then, download latest stable version of OpenCV::

    > sudo apt update && sudo apt install -y cmake g++ wget unzip
    > wget -O opencv.zip https://github.com/opencv/opencv/archive/4.5.1.zip
    > unzip opencv.zip
    > mkdir -p build && cd build

  To enable cross compilation for the ARM platform, open file
  "opencv-4.5.1/platforms/linux/arm-gnueabi.toolchain.cmake" and add the
  following lines::

    > set(CMAKE_SYSTEM_NAME Linux)
    > set(CMAKE_SYSTEM_PROCESSOR arm)
    > set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabi-gcc)
    > set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabi-g++)

    > set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    > set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    > set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    > set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
   
  Run:

    > sudo cmake  ../opencv-4.5.1 \
    >   -D CMAKE_TOOLCHAIN_FILE=platforms/linux/arm-gnueabi.toolchain.cmake
    > sudo make
    > sudo make install

2. **Compilation**: Compile the application with the following command::

    > arm-linux-gnueabi-gcc \
    >   -I/usr/local/include/opencv4 \
    >   -L/usr/local/ \
    >   -g -o image-registration  image-registration.cpp \
    >   -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml \
    >   -lopencv_video -lopencv_features2d -lopencv_calib3d \
    >   -lopencv_objdetect -lopencv_stitching -lm -lstdc++

3. **Transfer on ARM**: I used a QEMU emulator to test how the application
  works in an ARM environment.

  * Download QEMU (for Windows) from https://qemu.weilnetz.de/w64/2020/.
    Distributions on other platforms are supposed to work.
  * Download ARM images from https://people.debian.org/~aurel32/qemu/armel/.
    The following files are required:

      * debian_squeeze_armel_standard.qcow2
      * initrd.img-2.6.32-5-versatile
      * vmlinuz-2.6.32-5-versatile

  * Load the QEMU virtual machine and ensure that ports will enable file
    transfer from the host system (ports 2222 and 22 are used):

      > qemu-system-arm.exe -M versatilepb \
      >   -kernel vmlinuz-2.6.32-5-versatile \
      >   -initrd initrd.img-2.6.32-5-versatile \
      >   -hda debian_squeeze_armel_standard.qcow2 \
      >   -append "root=/dev/sda1" \
      >   -net user,id=net0,hostfwd=tcp::2222-:22 -net nic

  * Transfer the executable file on QEMU from the host system:

      > scp -P 2222 image-registration root@localhost:~

4. **Test on ARM**. Tests were performed on a QEMU virtual machine.

   * Transfer the test suite on QEMU from the host system:

        > scp -P 2222 -r tests root@localhost:~
  
   * Create a Python virtual environment with the required packages.
      Python 3.5 or more recent needs to be installed on the system:

          > sudo apt-get install python3

      Install venv to create virtual environments:

          > python3 -m pip install virtualenv

      Create the environment:

          > python3 -m venv env

      Activate the environment:

          > source env/bin/activate

      This will ensure that Python commands will be executed with the
      interpreter version in the environment. Install required packages:

          > (env) python3 -m pip install -r tests/requirements.txt
      
   3. Run tests:

          > (env) pytest tests --path <file path to the executable file>

      Providing the name of a directory to pytest will make it look
      automatically for test scripts.

5. **Build the documentation**: Documentation can be built by creating
  a Python virtual environment with the right packages. Use
  "project_requirements.txt" for packages for the documentation and
  tests or just "docs/docs_requirements.txt" for the documentation only:

      > python3 -m venv env
      > source env/bin/activate
      > (env) python3 -m pip install -r project_requirements.txt
      > (env) cd docs
      > (env) docs make html

  Documentation will be created in "docs/build/html". Open "index.html"
  to read the documentation.

### Interface

The program can be launched with a terminal using the following command:

    > image-registration \<image to register> \<image options> \<reference image> \<image options> \<general options>

Image options determine how the loaded images must be
cropped. They are:

* **-c** or **--crop**: crop the image according to a rectangular region
  specified as "\<top left x coordinate> \<top left y coordinate> \<width>
  \<height>".
* **-g** or **--graphic**: open a graphical interface to crop the image
  manually.
* **No option**: the whole image is used.

General options affect the behavior of the whole application. They are:

* **-v** (lower case) or **--verbose**: Make the application describe the
  operations it is performing (useful for debugging).
* **-V** (upper case) or **--version**: display the version of the program.
* **-h** or **--help**: print a help message.

The registered image will be saved with the name of the reference image with an
"-aligned.png" suffix.

For example, the command:

    > image-registration a.png -c 0 0 100 200 b.png -g

will crop a 100 (width) by 200 (height) pixel rectangle in the top left corner
of image "a.png" and use it as the image to register. A graphical interface
will open and let the user manually select the region of interest for "b.png".
Meanwhile, the command:

    > image-registration a.png b.png

will use "a.png" as a whole for the image to register and "b.png" for the
reference.

## Improvements

The application can be improved in the following ways:

1. **More Parameterization**: Currently, options such as:
  * local detection algorithm selection (ORB),
  * number of features to detect, and
  * minimum distance between features

  are pre-determined in the .h file and thus impossible for the user to
  change. Although it makes the application simpler, an actual program should
  make it possible to set parameters on each use.

2. **Less Repetitions**: Some parts of the code are repeated because
  it results in more readability. For example, in the file
  "image-registration.cpp" on lines 236 and 237, vectors are declared for each
  image with an explicit name instead of being grouped in a single structure.
  This practice would however not work for a program that uses more than
  two images at once because individual variables would have to be declared
  for each picture. Repetitions would make the code harder to maintain and
  understand. Therefore, these repetitions should be avoided.

3. **Performance-conscious Tests**: Current tests only check if the application
  meets minimum requirements, but the test framework should also evaluate
  performances (memory use and rapidity, among others) to ease comparison with
  similar programs.

4. **GUI Tests**: The graphical interface is not tested, but it could be
  automated in the suite along with the other test for cropping images.

5. **Stricter Programming Practices**: C++ programming conventions are not
  rigorously followed:
  * ".h" and ".cpp" should be placed in "include" and "source" directories,
    respectively.
  * The argument parsing function takes a lot of input/output arguments,
    a class could replace it for more abstraction.
