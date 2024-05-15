#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <time.h>

using namespace cv;
cv::Mat generateHighPassKernel(int size) {
    // Check if size is valid
    if (size % 2 == 0 || size < 3) {
        std::cerr << "Invalid kernel size. It should be an odd number >= 3." << std::endl;
        return cv::Mat();
    }

    // Create the kernel matrix
    cv::Mat kernel(size, size, CV_32S, cv::Scalar(0));

    // Calculate the center index
    int center = size / 2;

    // Set the values for high pass filtering
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (i == center && j == center) {
                kernel.at<int>(i, j) = size * size - 1;
            }
            else {
                kernel.at<int>(i, j) = -1;
            }
        }
    }

    return kernel;
}

int main()
{
    int start_s, stop_s, TotalTime = 0;

    cv::Mat img = cv::imread("D:/Samples/lena.png");

    // Check if the image is loaded successfully
    if (img.empty()) {
        std::cerr << "Error: Unable to load image." << std::endl;
        return 1;
    }
    int size;
    std::cout << "Enter the size of the kernel (odd and greater than or equal to 3): ";
    std::cin >> size;
    start_s = clock();
    cv::Mat kernel = generateHighPassKernel(size);

    if (!kernel.empty()) {
        std::cout << "Generated High Pass Kernel:" << std::endl;
        std::cout << kernel << std::endl;
    }
    else {
        return 0;
    }

    // Define the output image
    cv::Mat output_img(img.rows - kernel.rows + 1, img.cols - kernel.cols + 1, CV_32SC3);

    // Iterate over each pixel in the output image
    for (int i = 0; i < output_img.rows; ++i) {
        for (int j = 0; j < output_img.cols; ++j) {
            // Compute the sum of element-wise products between the kernel and the corresponding section of the image
            cv::Vec3i sum(0, 0, 0);
            for (int m = 0; m < kernel.rows; ++m) {
                for (int n = 0; n < kernel.cols; ++n) {
                    cv::Vec3b pixel = img.at<cv::Vec3b>(i + m, j + n);
                    sum[0] += pixel[0] * kernel.at<int>(m, n);
                    sum[1] += pixel[1] * kernel.at<int>(m, n);
                    sum[2] += pixel[2] * kernel.at<int>(m, n);
                }
            }
            // Store the result in the output image
            output_img.at<cv::Vec3i>(i, j) = sum;
        }
    }
    output_img.convertTo(output_img, CV_8UC3);

    // Sum the input and output images element-wise
    cv::Mat sum_img;
    cv::add(img(cv::Rect(0, 0, output_img.cols, output_img.rows)), output_img, sum_img);

    stop_s = clock();
    TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
    std::cout << "time: " << TotalTime << "ms";

    // Display images
    //namedWindow("Sum Image", WINDOW_AUTOSIZE);
    //imshow("Sum Image", sum_img);
    //moveWindow("Sum Image", 0, 45);

    //namedWindow("Input Image", WINDOW_AUTOSIZE);
    //imshow("Input Image", img);
    //moveWindow("Input Image", sum_img.cols + 20, 45);

    namedWindow("Output Image", WINDOW_AUTOSIZE);
    imshow("Output Image", output_img);
    //moveWindow("Output Image", img.cols + 20, 45);
    waitKey(0);
    destroyAllWindows();

    return 0;
}
