#ifndef IMAGEREGISTRATION_H
#define IMAGEREGISTRATION_H

#include <iostream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#define IMAGE_REGISTRATION_VERSION_MAJOR 1
#define IMAGE_REGISTRATION_VERSION_MINOR 0

int const N_FEATURES = 4000;
int const MIN_N_MATCHES = 50;
float const MIN_DISTANCE = 25;

/**
 * Printing function used for verbose outputs.
 */
void printVerbose(bool isVerbose, std::string text);

/**
 * Version printing function.
 */
void printVersion();

/**
 * Help message printing function.
 */
void printHelp();

/**
 * Command line input fail message print function.
 * 
 * This function is used to tell the user that the arguments they
 * provided to the program are incorrect. It indicates that they
 * can find more information with the "-h" command and the documentation.
 */
void printCommandLineFail();

/**
 * Utility function used by 'parseArguments' to determine ROI for
 * a single image.
 *
 * @param argc Number of arguments.
 * @param argv Values of the arguments.
 * @param mustCrop If set to true, the image must be cropped.
 * @param initialArg Index of the first argument to evaluate.
 * @param ROI Rectangle of the ROI.
 * @param useGraphic The graphical interface must be opened.
 *
 * @return The index of the next argument to evalued is returned
 *   on success. -1 is returned if an error occured.
*/
int determineROI(int argc, char** argv, int initialArg,
    bool& mustCrop, cv::Rect& ROI, bool& useGraphic);

/**
 * Argument parsing and validation function.
 * 
 * This function reads the arguments provided from the terminal
 * to determine:
 *   - The file paths of the images,
 *   - The regions of interest (ROI),
 *   - Whether the graphical interface must be used,
 *   - Whether a verbose output must be provided.
 * If the input is incorrect, an error message is printed.
 * 
 * @param argc Number of arguments.
 * @param argv Values of the arguments.
 * @param imageToRegisterPath File path of the image to register.
 * @param referenceImagePath File path of the reference image.
 * @param cropImageToRegister If set to true, the image to register
 *   must be cropped before further operations.
 * @param cropImageReference Same as above but for the reference image.
 * @param imageToRegisterROI Rectangular region of interest to use
 *   in the case of the image to register.
 * @param referenceImageROI Same as above but for the reference image.
 * @param useGraphicToRegister A 'true' value indicates that the user must
 *   select the ROI manually. 'false' means that the graphical interface
 *   must not be used.
 * @param useGraphicToReference Same as above but for the reference image.
 * @param isVerbose Indicates if the application must run in verbose mode.
 * 
 * @return 0 is returned when no error is detected. -1 indicates that
 *   an error is encountered and the program cannot continue further.
 *   1 means that the input was valid but that further operations must
 *   not be performed (e.g. display the help message and close).
 */
int parseArguments(int argc, char** argv,
    std::string& imageToRegisterPath,
    std::string& referenceImagePath,
    bool& mustCropImageToRegister, bool& mustCropImageReference,
    cv::Rect& imageToRegisterROI, cv::Rect& referenceImageROI,
    bool& useGraphicToRegister, bool& useGraphicToReference,
    bool& isVerbose);

/**
 * Image loading error printing function.
 */
void printImageFailMessage(std::string& path);

/**
 * Callback function used for selecting the ROI.
 * 
 * @param event Identifier for the event.
 * @param x Coordinate along the horizontal axis.
 * @param y Coordinate along he vertical axis.
 * @param flags Control flags.
 * @param userdata Data to transmit (in this case, a rectangle).
 */
void mouseCropCallback(int event, int x, int y, int flags, void* userdata);

/**
 * Graphical cropping function
 * 
 * This function is used to allow the user to manually select a rectangular
 * region of interest with a graphical interface.
 * 
 * @param image The image to crop.
 * @param rectangle The region of interest selected by the user.
 * 
 * @return 0 is return when no error is encountered, 1 indicates a failure.
 */
int graphicSelectROI(cv::Mat& image, cv::Rect& rectangle);

/**
 * Image registration function.
 * 
 * This function registers two images passed as references to compute
 * an aligned version of the images using ORB.
 * 
 * @param imageToRegister Image intended to be aligned.
 * @param referenceImage Image used as a reference.
 * @param registeredImage Result of the registration operation.
 * @param homography Homography matrix used to perform registration.
 * 
 * @return 0 is returned on success. -1 indicates a failure.
 */
int registerImage(cv::Mat& imageToRegister, cv::Mat& referenceImage,
    cv::Mat& registeredImage);

#endif // IMAGEREGISTRATION_H
