Test Suite
==========

The testing procedure is made with Pytest and is separated in two main
components: command line interface tests and image registration tests.

For information on how to run tests, refer to the user guide.

Command Line Interface Tests
----------------------------

These tests check if the program reacts appropriately to command line
arguments. It checks version display, help message display, and if invalid
inputs make the program fail as expected.

Image Registration Tests
------------------------

The package "imagehash", available through pip, is used to compare
registrations with expected results. Both complete and cropped images are
tested.
