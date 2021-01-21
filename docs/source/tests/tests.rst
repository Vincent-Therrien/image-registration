Test Suite
==========

The testing procedure is made with Pytest and is separated in two main
components: command line interface tests and image registration tests.

For information on how to run tests, refer to the user guide.

Command Line Interface Tests
----------------------------

These tests check if the program reacts appropriately to command line
arguments. It checks:

- version display
- help message display
- if invalid inputs make the program fail as expected.

Image Registration Tests
------------------------

The Python package "imagehash", available through pip, is used to compare how
similar aligned images are to expected results. Two tests are performed:

First, an image rotated by 45 degrees counter clock wise
(data/test_image_gray_rotated.png) is aligned with the
original image (data/test_image_rgb.png). It is expected to yield a full
gray scale non-rotated image.

Second, the rotated image is cropped so that only its left half is aligned.
Since it is rotated, this area corresponds to the top left half of the original
image. The test thus compares the result to the image
"test_image_gray_half.png".
