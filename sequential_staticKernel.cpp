//#include <opencv2/core.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui.hpp>
//#include <iostream>
//#include <time.h>
//
//#define KERNEL_HEIGHT 3
//#define KERNEL_WIDTH 3
//
//using namespace cv;
//
//int main()
//{
//    int start_s, stop_s, TotalTime = 0;
//    start_s = clock();
//
//    cv::Mat img = cv::imread("D:/Samples/lena.png");
//
//    // Check if the image is loaded successfully
//    if (img.empty()) {
//        std::cerr << "Error: Unable to load image." << std::endl;
//        return 1;
//    }
//
//    // Define the kernel
//    cv::Mat kernel = (cv::Mat_<int>(3, 3) <<
//        0, -1, 0,
//        -1, 4, -1,
//        0, -1, 0);
//
//    // Define the output image
//    cv::Mat output_img(img.rows - kernel.rows + 1, img.cols - kernel.cols + 1, CV_32SC3);
//
//    // Iterate over each pixel in the output image
//    for (int i = 0; i < output_img.rows; ++i) {
//        for (int j = 0; j < output_img.cols; ++j) {
//            // Compute the sum of element-wise products between the kernel and the corresponding section of the image
//            cv::Vec3i sum(0, 0, 0);
//            for (int m = 0; m < kernel.rows; ++m) {
//                for (int n = 0; n < kernel.cols; ++n) {
//                    cv::Vec3b pixel = img.at<cv::Vec3b>(i + m, j + n);
//                    sum[0] += pixel[0] * kernel.at<int>(m, n);
//                    sum[1] += pixel[1] * kernel.at<int>(m, n);
//                    sum[2] += pixel[2] * kernel.at<int>(m, n);
//                }
//            }
//            // Store the result in the output image
//            output_img.at<cv::Vec3i>(i, j) = sum;
//        }
//    }
//    output_img.convertTo(output_img, CV_8UC3);
//
//    // Sum the input and output images element-wise
//    cv::Mat sum_img;
//    cv::add(img(cv::Rect(0, 0, output_img.cols, output_img.rows)), output_img, sum_img);
//
//    stop_s = clock();
//    TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
//    std::cout << "time: " << TotalTime << "ms";
//
//    // Display images
//    //namedWindow("Sum Image", WINDOW_AUTOSIZE);
//    //imshow("Sum Image", sum_img);
//    //moveWindow("Sum Image", 0, 45);
//
//    //namedWindow("Input Image", WINDOW_AUTOSIZE);
//    //imshow("Input Image", img);
//    //moveWindow("Input Image", sum_img.cols + 20, 45);
//
//    namedWindow("Output Image", WINDOW_AUTOSIZE);
//    imshow("Output Image", output_img);
//    //moveWindow("Output Image", img.cols + 20, 45);
//    waitKey(0);
//    destroyAllWindows();
//
//    return 0;
//}
