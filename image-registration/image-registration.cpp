#include "image-registration.h"

void printVerbose(bool isVerbose, std::string text)
{
    if (isVerbose)
        std::cout << text << std::endl;
}

void printVersion()
{
    std::cout << std::endl << "image-registration version "
        << IMAGE_REGISTRATION_VERSION_MAJOR << "."
        << IMAGE_REGISTRATION_VERSION_MINOR << "."
        << std::endl;
}

void printHelp()
{
    printVersion();
    std::cout << "This application performs image registration. "
        << "Use the following command to launch the program:"
        << std::endl;
    std::cout << "image-registration <image to register> "
        << "<image options> <reference image> <image options> "
        << "<general options>" << std::endl << std::endl;
    std::cout << "Images options can be of the following:" << std::endl;
    std::cout << "-c or --crop: Crop the image with a rectangle provided "
        << "as <top-left x> <top-left y> <width> <height>" << std::endl;
    std::cout << "-g or --graphic: open a graphical interface to "
        << "crop the image manually." << std::endl << std::endl;
    std::cout << "General options can be of the following: " << std::endl;
    std::cout << "-v or --verbose: Verbose mode (display all operations)"
        << std::endl;
    std::cout << "-V or --version: Display the version of the program."
        << std::endl;
    std::cout << "-h or --help: Print this message and close the program."
        << std::endl;
}

void printCommandLineFail() {
    std::cout << "Incorrect input. Please review the documentation or run "
        << "the command 'image-registration -h' for more information."
        << std::endl;
}

int determineROI(int argc, char** argv, int initialArg,
    bool& mustCrop, cv::Rect& ROI, bool& useGraphic)
{
    int currentArg = initialArg;
    // If no arguments are present, the full image must be used.
    if (currentArg >= argc) {
        mustCrop = false;
        useGraphic = false;
        return currentArg++;
    }
    // Get the crop region, if specified with the 'crop' argument.
    if (strcmp(argv[currentArg], "-c") == 0
            || strcmp(argv[currentArg], "--crop") == 0) {
        // Not enough arguments are provided.
        if (currentArg + 4 >= argc) {
            return -1;
        }
        // Four integer values are expected, which correspond to x, y, width,
        // and height. If all values are found with no error, the ROI can be
        // defined. Otherwise, the arguments were not well specified.
        try {
            int x = std::stoi(argv[++currentArg]);
            int y = std::stoi(argv[++currentArg]);
            int w = std::stoi(argv[++currentArg]);
            int h = std::stoi(argv[++currentArg]);
            mustCrop = true;
            ROI = cv::Rect(x, y, w, h);
            useGraphic = false;
            return currentArg + 1;
        }
        catch (...) {
            return -1;
        }
    }
    // Assess whether the graphical interface must be used or not.
    else if (strcmp(argv[currentArg], "-g") == 0
            || strcmp(argv[currentArg], "--graphic") == 0) {
        mustCrop = true;
        useGraphic = true;
        return currentArg + 1;
    }
    // If neither the '-c' or '-g' options are provided, use the whole
    // image as a ROI.
    mustCrop = false;
    useGraphic = false;
    return currentArg;
}

int parseArguments(int argc, char** argv,
    std::string& imageToRegisterPath,
    std::string& referenceImagePath,
    bool& mustCropImageToRegister, bool& mustCropImageReference,
    cv::Rect& imageToRegisterROI, cv::Rect& referenceImageROI,
    bool& useGraphicToRegister, bool& useGraphicToReference,
    bool& isVerbose)
{
    // The program cannot be executed with no argument.
    if (argc == 1) {
        printCommandLineFail();
        return -1;
    }
    // If a single argument is provided, it can either be for the '--version'
    // or '--help' commands. Otherwise, the input is incorrect.
    if (argc == 2) {
        if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version") == 0) {
            printVersion();
            return 1;
        }
        else if (strcmp(argv[1], "-h") == 0
                || strcmp(argv[1], "--help") == 0) {
            printHelp();
            return 1;
        }
        else {
            printCommandLineFail();
            return -1;
        }
    }
    // If the number of arguments is greater than one, the function looks
    // for file names and crop regions.
    int currentArg = 1;
    imageToRegisterPath = argv[currentArg++];
    currentArg = determineROI(argc, argv, currentArg,
        mustCropImageToRegister, imageToRegisterROI, useGraphicToRegister);
    if (currentArg < 0) {
        printCommandLineFail();
        return -1;
    }
    // Determine whether there are enough arguments for the second image.
    if (currentArg >= argc) {
        printCommandLineFail();
        return -1;
    }
    referenceImagePath = argv[currentArg++];
    currentArg = determineROI(argc, argv, currentArg,
        mustCropImageReference, referenceImageROI, useGraphicToReference);
    if (currentArg < 0) {
        printCommandLineFail();
        return -1;
    }
    // Last option can be used to activate verbose mode.
    isVerbose = false;
    if (currentArg < argc) {
        if (strcmp(argv[currentArg], "-v") == 0
            || strcmp(argv[currentArg], "--verbose") == 0) {
            isVerbose = true;
        }
    }
    return 0;
}

void printImageFailMessage(std::string& path) {
    std::cout << "Loading of image '" << path << "' failed. Please "
        << "ensure that the name of the file is valid." << std::endl;
}

void mouseCropCallback(int event, int x, int y, int flags, void* userdata)
{
    std::tuple<std::pair<cv::Point2i, cv::Point2i>, cv::Mat>* data =
        (std::tuple<std::pair<cv::Point2i, cv::Point2i>, cv::Mat>*) userdata;
    std::pair<cv::Point2i, cv::Point2i>* points = &(std::get<0>(*data));
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        points->first.x = x;
        points->first.y = y;
    }
    else if (event == cv::EVENT_LBUTTONUP)
    {
        points->second.x = x;
        points->second.y = y;
    }
    cv::Mat frame = std::get<1>(*data).clone();
    cv::putText(frame, "Select a ROI with the mouse and confirm by pressing a key.",
        cv::Point2f(0, 10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0), 2, 8, false);
    if (points->first.x != points->second.x && points->first.y != points->second.y) {
        // Case 1: the user is tracing the rectangle.
        if (points->first.x >= 0 && points->first.y >= 0
                && points->second.x == -1 && points->second.y == -1) {
            cv::rectangle(frame, cv::Point2i(x, y), points->first, cv::Scalar(0, 0, 255));
        }
        // Case 2: the ROI is traced.
        else if (points->first.x >= 0 && points->first.y >= 0
                && points->second.x >= 0 && points->second.y >= 0) {
            cv::rectangle(frame, points->first, points->second, cv::Scalar(0, 0, 255));
        }
    }
    cv::imshow("ROI Selection", frame);
}

int graphicSelectROI(cv::Mat& image, cv::Rect& rectangle)
{
    std::string windowName = "ROI Selection";
    cv::namedWindow(windowName, 1);
    std::tuple<std::pair<cv::Point2i, cv::Point2i>, cv::Mat> data;
    std::get<0>(data) = std::pair<cv::Point2i, cv::Point2i>(cv::Point2i(-1, -1), cv::Point2i(-1, -1));
    std::get<1>(data) = image;
    cv::setMouseCallback(windowName, mouseCropCallback, &data);
    cv::waitKey(0);
    std::pair<cv::Point2i, cv::Point2i> points = std::get<0>(data);
    int minX = std::min(points.first.x, points.second.x);
    int minY = std::min(points.first.y, points.second.y);
    int maxX = std::max(points.first.x, points.second.x);
    int maxY = std::max(points.first.y, points.second.y);
    // Terminate the program if the user did not select anything.
    if (minX == minY && maxX == maxY) {
        std::cout << "No registration can be performed because the ROI was "
            "not selected. Terminating." << std::endl;
        return -1;
    }
    // Some values may be out of bound because of the BUTTONUP event.
    if (minX < 0) {
        minX = 0;
    }
    if (minY < 0) {
        minY = 0;
    }
    if (maxX > image.size().width) {
        maxX = image.size().width;
    }
    if (maxY > image.size().height) {
        maxY = image.size().height;
    }
    rectangle = cv::Rect(minX, minY, maxX - minX, maxY - minY);
    return 0;
}

int registerImage(cv::Mat& imageToRegister, cv::Mat& referenceImage,
    cv::Mat& registeredImage)
{
    // Prepare objects for saving data.
    std::vector<cv::KeyPoint> keyPointsToRegister;
    std::vector<cv::KeyPoint> keyPointsReference;
    cv::Mat descriptorsToRegister;
    cv::Mat descriptorsReference;
    // Convert images to gray scale.
    cv::Mat imgToRegisterGray;
    cv::Mat imgReferenceGray;
    cv::cvtColor(imageToRegister, imgToRegisterGray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(referenceImage, imgReferenceGray, cv::COLOR_BGR2GRAY);
    // Evaluate features and determine descriptors. ORB is used because it
    // is not patented.
    cv::Ptr<cv::Feature2D> orb = cv::ORB::create(N_FEATURES);
    orb->detectAndCompute(imgToRegisterGray, cv::Mat(), keyPointsToRegister,
        descriptorsToRegister);
    orb->detectAndCompute(imgReferenceGray, cv::Mat(), keyPointsReference,
        descriptorsReference);
    // Match features and select the best ones.
    std::vector<cv::DMatch> matches;
    cv::Ptr<cv::DescriptorMatcher> matcher =
        cv::DescriptorMatcher::create("BruteForce-Hamming");
    matcher->match(descriptorsToRegister, descriptorsReference,
        matches, cv::Mat());
    if (matches.size() < MIN_N_MATCHES) {
        std::cout << matches.size() << " matches were found, which is "
            << "insufficient. The minimum number is "
			<< MIN_N_MATCHES << "." << std::endl;
        return -1;
    }
    std::sort(matches.begin(), matches.end());
    std::vector< cv::DMatch > bestMatches;
    for (int i = 0; i < matches.size(); i++) {
        if (matches[i].distance < MIN_DISTANCE) {
            bestMatches.push_back(matches[i]);
        }
    }
    if (bestMatches.size() <= 4) {
        std::cout << "Matches were found, but too few were of sufficient "
            << "quality to perform image registration." << std::endl;
        return -1;
    }
    // Save mathing keypoints
    cv::Mat imageMatches;
    cv::drawMatches(imageToRegister, keyPointsToRegister,
        referenceImage, keyPointsReference, bestMatches, imageMatches);
    cv::imwrite("matching-keypoints.jpg", imageMatches);

    // Determine the location of matching keypoints.
    std::vector<cv::Point2f> pointsToAlign, pointsReference;
    for (unsigned int i = 0; i < bestMatches.size(); i++) {
        pointsToAlign.push_back(keyPointsToRegister[bestMatches[i].queryIdx].pt);
        pointsReference.push_back(keyPointsReference[bestMatches[i].trainIdx].pt);
    }
    // Register the image with homography
    cv::Mat homography = cv::findHomography(pointsToAlign, pointsReference,
        cv::RANSAC);
    cv::warpPerspective(imageToRegister, registeredImage, homography,
        referenceImage.size());
    return 0;
}

int main(int argc, char** argv)
{
    // Read and validate the arguments provided by the user.
    std::string imageToRegisterPath;
    std::string referenceImagePath;
    bool mustCropImageToRegister;
    bool mustCropReferenceImage;
    cv::Rect imageToRegisterROI;
    cv::Rect referenceImageROI;
    bool useGraphicToRegister;
    bool useGraphicToReference;
    bool isVerbose;
    int parseResult = parseArguments(argc, argv,
        imageToRegisterPath, referenceImagePath,
        mustCropImageToRegister, mustCropReferenceImage,
        imageToRegisterROI, referenceImageROI,
        useGraphicToRegister, useGraphicToReference,
        isVerbose);
    if (parseResult < 0) {
        return parseResult;
    }
    if (parseResult == 1) {
        return 0;
    }
    // Load images and ensure that the operation was successful.
    printVerbose(isVerbose, "reading image '" + imageToRegisterPath + "'.");
    cv::Mat imageToRegister = cv::imread(imageToRegisterPath, cv::IMREAD_COLOR);
    if (imageToRegister.empty()) {
        printImageFailMessage(imageToRegisterPath);
        return -1;
    }
    printVerbose(isVerbose, "reading image '" + referenceImagePath + "'.");
    cv::Mat referenceImage = cv::imread(referenceImagePath, cv::IMREAD_COLOR);
    if (referenceImage.empty()) {
        printImageFailMessage(referenceImagePath);
        return -1;
    }
    // Crop images with the command line arguments or with the graphical
    // interface. If not specified, use the whole image.
    if (mustCropImageToRegister) {
        if (useGraphicToRegister) {
            if (graphicSelectROI(imageToRegister, imageToRegisterROI)) {
                return -1;
            }
        }
        imageToRegister = imageToRegister(imageToRegisterROI);
    }
    if (mustCropReferenceImage) {
        if (useGraphicToReference) {
            if (graphicSelectROI(referenceImage, referenceImageROI)) {
                return -1;
            }
        }
        referenceImage = referenceImage(referenceImageROI);
    }

    // Align images.
    printVerbose(isVerbose, "Initiating image registration.");
    cv::Mat registeredImage;
    if (registerImage(imageToRegister, referenceImage, registeredImage)) {
        return -1;
    }
    std::cout << "Saved maching keypoints in 'matching-keypoints.jpg'."
        << std::endl;
    // Save the registered image with the appropriate suffix.
    std::string registeredName;
    size_t dotIndex = imageToRegisterPath.find_last_of(".");
    if (dotIndex == std::string::npos) {
        registeredName = imageToRegisterPath + "-aligned.png";
    }
    else {
        registeredName = imageToRegisterPath.substr(0, dotIndex)
            + "-aligned.png";
    }
    cv::imwrite(registeredName, registeredImage);
    std::cout << "The registered image was saved at: '" << registeredName
        << "'." << std::endl;

    return 0;
}
