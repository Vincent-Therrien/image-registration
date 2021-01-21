"""Image registration/alignment test module.

This module performs image registration with the tested application
and compares obtained results to expected outputs."""

import os
import subprocess
import pytest
from PIL import Image
import imagehash

full_image = "data/test_image_gray_rotated.png"
reference = "data/test_image_rgb.png"
image_aligned = "data/test_image_gray_rotated-aligned.png"

full_expected_result = "data/test_image_gray.png"
cropped_expected_result = "data/test_image_gray_half.png"

keypoints = "matching-keypoints.jpg"

def erase_output_images() -> None:
    """Delete images created by the program to ensure that tests are
    not performed on the wrong files."""
    if os.path.exists(image_aligned):
        os.remove(image_aligned)
    if os.path.exists(keypoints):
        os.remove(keypoints)

class TestRegistration:
    """Image registration test class.

    This class tests how well images are registered by the program.
    It utilizes the module 'imagehash' to evaluate the similarity
    between expected and yielded results, which is available through
    the standard Python package installer (pip)."""

    def test_full_registration(self, path):
        """This method uses test images in the /data directory whose
        properties are known in advance and compared to obtained results.
        Images are not cropped."""
        erase_output_images()
        subprocess.check_output([path,
            full_image, reference])
        # Ensure that the registration file was created.
        assert os.path.exists(image_aligned)
        # Ensure that matching keypoints are registered.
        assert os.path.exists(keypoints)
        # Compare the registration result to the expected output.
        hash1 = imagehash.average_hash(Image.open(image_aligned))
        hash2 = imagehash.average_hash(Image.open(full_expected_result))
        assert abs(hash1 - hash2) <= 3

    def test_cropped_registration(self, path):
        """Same as above, but the image to be registered is cropped so
        as to select its left half. Since this image is rotated by -45
        degrees, the top-left half of the original image is obtained."""
        erase_output_images()
        subprocess.check_output([path,
            full_image, "-c", "0", "0", "415", "800", reference])
        # Ensure that the registration file was created.
        assert os.path.exists(image_aligned)
        # Ensure that matching keypoints are registered.
        assert os.path.exists(keypoints)
        # Compare the registration result to the expected output.
        hash1 = imagehash.average_hash(Image.open(image_aligned))
        hash2 = imagehash.average_hash(Image.open(cropped_expected_result))
        assert abs(hash1 - hash2) <= 3
    
    @classmethod
    def teardown_method(cls):
        """Erase test images after tests are performed."""
        erase_output_images()
