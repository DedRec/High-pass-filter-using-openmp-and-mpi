#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>  // Standard input/output stream library
#include <omp.h>     // OpenMP header file


using namespace cv;
using namespace std;

void OMP_High_Pass_Filter(const Mat& imageData, const Mat& kernel, int kernel_size) {

    // Check if the image is loaded successfully
    if (imageData.empty()) {
        std::cerr << "Error: Unable to load image." << std::endl;
        return;
    }

    // Create an output image matrix to store the filtered result
    cv::Mat output_img(imageData.rows - kernel_size + 1, imageData.cols - kernel_size + 1, CV_32SC3);

    //omp_set_num_threads(5);

    int i, j, m, n;
    double start_time = omp_get_wtime(); // Start timing

    // Iterate over each pixel in the output image
#pragma omp parallel for shared(output_img, kernel) private(i,j,m,n) num_threads(5)
    for (i = 0; i < output_img.rows; ++i) {
        for (j = 0; j < output_img.cols; ++j) {
            // Compute the sum of element-wise products between the kernel and the corresponding section of the image
            cv::Vec3i sum(0, 0, 0);
            for (m = 0; m < kernel_size; ++m) {
                for (n = 0; n < kernel_size; ++n) {
                    cv::Vec3b pixel = imageData.at<cv::Vec3b>(i + m, j + n);
                    // Multiply each pixel in the kernel with the corresponding pixel in the image section and accumulate the sum
                    sum[0] += pixel[0] * kernel.at<int>(m, n);
                    sum[1] += pixel[1] * kernel.at<int>(m, n);
                    sum[2] += pixel[2] * kernel.at<int>(m, n);
                }
            }
            // Store the result in the output image
            output_img.at<cv::Vec3i>(i, j) = sum;
        }
    }
    double end_time = omp_get_wtime(); // End timing
    double elapsed_time = end_time - start_time; // Calculate elapsed time
    cout << "Elapsed time: " << elapsed_time *1000<< " msec" << endl; // Output elapsed time


    // Convert the output image to the appropriate data type for display
    output_img.convertTo(output_img, CV_8UC3);

    // Create a Window and Display the output image
    namedWindow("Output Image", WINDOW_AUTOSIZE);
    imshow("Output Image", output_img);
    moveWindow("Output Image", 0, 45);

    waitKey(0);
    destroyAllWindows();
}

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

    // Read the input image
    cv::Mat img = cv::imread("D:/Samples/cat.jpeg");

    // Check if the image is loaded successfully
    if (img.empty()) {
        std::cerr << "Error: Unable to load image." << std::endl;
        return 1;
    }
    int size;
    std::cout << "Enter the size of the kernel (odd and greater than or equal to 3): ";
    std::cin >> size;
    cv::Mat kernel = generateHighPassKernel(size);

    if (!kernel.empty()) {
        std::cout << "Generated High Pass Kernel:" << std::endl;
        std::cout << kernel << std::endl;
        // Apply high pass filtering using OpenMP
        OMP_High_Pass_Filter(img, kernel, size);
    }

    
    return 0;
}
