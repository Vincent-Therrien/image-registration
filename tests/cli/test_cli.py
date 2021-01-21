"""Command line interface test module."""

import subprocess
import pytest
import re

# Constant values used to validate outputs.
version_pattern = r'image-registration version [\d.\d]'

help_string_lines = [
    "This application performs image registration. Use the following "
    + "command to launch the program:",
    "image-registration <image to register> <image options> "
    + "<reference image> <image options> <general options>",
    "Images options can be of the following:",
    "-c or --crop: Crop the image with a rectangle provided as "
    + "<top-left x> <top-left y> <width> <height>",
    "-g or --graphic: open a graphical interface to crop the image "
    + "manually.",
    "General options can be of the following: ",
    "-v or --verbose: Verbose mode (display all operations)",
    "-V or --version: Display the version of the program.",
    "-h or --help: Print this message and close the program."
]

cmd_fail_string = ("Incorrect input. Please review the documentation or run the "
    + "command 'image-registration -h' for more information.")

def is_help_string_complete(output: str) -> bool:
    """This function returns True if the string contains the help
    string lines in the correct order."""
    output_lines = output.replace(r'\r', '').split(r'\n')
    full_lines = [line for line in output_lines if len(line) > 2]
    # Skip the first line to avoid verifying the version.
    for output_line, reference_line in zip(full_lines[1:], help_string_lines):
        if reference_line not in output_line:
            return False
    return True

class TestCli:
    """Command-line interface test class.

    This class comprises methods to validate the behavior of the application
    by comparing the output obtained after submitting various commands to
    desired results."""

    def test_version(self, path):
        """Ensure that the version pattern is correct."""
        output = str(subprocess.check_output([path, '-V']))
        assert re.search(version_pattern, output)
        output = str(subprocess.check_output([path, '--version']))
        assert re.search(version_pattern, output)

    def test_help(self, path):
        """Ensure that the help message is printed and well formatted."""
        output = str(subprocess.check_output([path, '-h']))
        assert re.search(version_pattern, output)
        assert is_help_string_complete(output)
        output = str(subprocess.check_output([path, '--help']))
        assert re.search(version_pattern, output)
        assert is_help_string_complete(output)
    
    def test_cmd_error(self, path):
        """Ensure that the program prints a fail message and returns a
        non-zero return code when invalid inputs are provided."""
        # The program stops if a single invalid argument is provided.
        try:
            subprocess.check_output([path, 'x'])
        except subprocess.CalledProcessError as error:
            assert error.returncode != 0
            assert cmd_fail_string in str(error.output)
        # The program stops if images cannot be loaded.
        try:
            subprocess.check_output([path, 'image1', 'image2'])
        except subprocess.CalledProcessError as error:
            assert error.returncode != 0
        # The program stops when the '-c' argument is used incorrectly.
        try:
            subprocess.check_output([path,
                '-c', '0', '0', '0', 'a', 'b'])
        except subprocess.CalledProcessError as error:
            assert error.returncode != 0
