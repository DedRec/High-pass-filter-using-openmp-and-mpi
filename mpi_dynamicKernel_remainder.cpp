#include <iostream>
#include <opencv2/opencv.hpp>
#include <mpi.h>
#include <time.h>

using namespace cv;
using namespace std;

Mat generateHighPassKernel(int size) {
   // Check if size is valid
   if (size % 2 == 0 || size < 3) {
       cerr << "Invalid kernel size. It should be an odd number >= 3." << endl;
       return Mat();
   }
   // Create the kernel matrix
   Mat kernel(size, size, CV_32F, Scalar(0));
   // Calculate the center index
   int center = size / 2;
   // Set the values for high pass filtering
   for (int i = 0; i < size; ++i) {
       for (int j = 0; j < size; ++j) {
           if (i == center && j == center) {
               kernel.at<float>(i, j) = size * size - 1;
           }
           else {
               kernel.at<float>(i, j) = -1;
           }
       }
   }
   return kernel;
}

Mat highPassFilter(const Mat& originalImage, const Mat& kernel) {
   // Define padding size
   int paddingSize = (kernel.rows - 1) / 2; // Assuming KERNEL_HEIGHT is odd

   // Create a padded version of the original image
   Mat paddedImage;
   copyMakeBorder(originalImage, paddedImage, paddingSize, paddingSize, paddingSize, paddingSize,
       BORDER_CONSTANT, Scalar(0));
   // Convert the padded image to float
   Mat floatImage;
   paddedImage.convertTo(floatImage, CV_32F); // Convert to float

   Mat highPassImage(originalImage.size(), originalImage.type()); // Create output image

   // Iterate over each pixel in the image
   for (int i = 0; i < originalImage.rows; ++i) {
       for (int j = 0; j < originalImage.cols; ++j) {
           // Compute the sum of element-wise products between the kernel and the corresponding section of the image
           Vec3f sum(0, 0, 0);
           for (int m = 0; m < kernel.rows; ++m) {
               for (int n = 0; n < kernel.cols; ++n) {
                   Vec3f pixel = floatImage.at<Vec3f>(i + m, j + n);
                   sum[0] += pixel[0] * kernel.at<float>(m, n);
                   sum[1] += pixel[1] * kernel.at<float>(m, n);
                   sum[2] += pixel[2] * kernel.at<float>(m, n);
               }
           }
           // Store the result in the output image
           highPassImage.at<Vec3b>(i, j) = sum;
       }
   }
   return highPassImage;
}


void parallelHighPassFilter(const Mat& imageData, const Mat& kernel, int rank, int size, int start_s) {
   if (size == 1) {
       // Only one process, apply high-pass filter directly
       Mat processedImage = highPassFilter(imageData, kernel);
       imshow("Processed Image", processedImage);
       waitKey(0); // Wait for a key press to close the window
       cout << "Result Image Displayed (Single Process)" << endl;
   }
   else if (rank == 0) {
       // Create processed image
       Mat processedImage(imageData.rows, imageData.cols, imageData.type());
       int imageHeight = imageData.rows; // Height of the whole image
       int remainder = imageHeight % (size - 1);
       int maxRank = size;
       if (imageHeight < size - 1) {
           maxRank = imageHeight;
       }

       /*cout << "MaxRank: " << maxRank;
       cout << "remainder: " << remainder<<endl;
       cout << "Height: " << imageHeight << endl;*/

       // Get processed parts from processes to combine them
       for (int i = 1; i < maxRank; i++) {
           // Get processed subimage and its position from rank i
           Mat receivedSubImage;
           int y, width, height;

           MPI_Recv(&y, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
           MPI_Recv(&width, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
           MPI_Recv(&height, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

           // Allocate memory for the received subimage
           receivedSubImage.create(height, width, imageData.type());

           // Get the subimage data
           MPI_Recv(receivedSubImage.data, width * height * imageData.elemSize(), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

           // Combine the received subimage into the processed image
           receivedSubImage.copyTo(processedImage(Rect(0, y, width, height)));
       }
       if (remainder && imageHeight > size - 1) {
           for (int i = 1; i <= remainder; i++) {
               // Get processed subimage and its position from rank i
               Mat receivedSubImage;
               int y, width, height;
               MPI_Recv(&y, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
               //printf("Process %d received y value %d from process %d\n", 0, y, i);
               MPI_Recv(&width, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
               //printf("Process %d received width value %d from process %d\n", 0, width, i);
               MPI_Recv(&height, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
               //printf("Process %d received height value %d from process %d\n", 0, height, i);

               // Allocate memory for the received subimage
               receivedSubImage.create(height, width, imageData.type());

               // Get the subimage data
               MPI_Recv(receivedSubImage.data, width * height * imageData.elemSize(), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

               // Combine the received subimage into the processed image
               receivedSubImage.copyTo(processedImage(Rect(0, y, width, height)));
           }
       }

       int stop_s, TotalTime = 0;
       stop_s = clock();
       TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
       std::cout << "time: " << TotalTime << "ms" << endl;

       // Display final processed image
       imshow("Processed Image", processedImage);
       waitKey(0); // Wait for a key press to close the window
       cout << "Result Image Displayed" << endl;

   }
   else if (rank < size) {

       int imageHeight = imageData.rows; // Height of the whole image
       int stripHeight; // Height of each strip
       if (imageHeight < size - 1) {
           stripHeight = 1;
       }
       else {
           stripHeight = imageHeight / (size - 1);
       }
       int remainder = imageHeight % (size - 1);


       if (imageHeight + 1 > rank) {
           int startY = (rank - 1) * stripHeight; // Starting Y coordinate for the current rank

           // Process the assigned strip
           Mat subImage = imageData.rowRange(startY, startY + stripHeight);
           Mat processedSubImage = highPassFilter(subImage, kernel);

           // Sending the position and dimensions of the subimage to rank 
           int y = startY; // Initial Y coordinate
           int width = processedSubImage.cols; // Width of each subimage
           int height = processedSubImage.rows; // Height of each subimage
           //cout << y;

           MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
           MPI_Send(&width, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
           MPI_Send(&height, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

           // Send the subimage data to rank 0
           MPI_Send(processedSubImage.data, width * height * imageData.elemSize(), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
       }

       if ((rank < (remainder + 1)) && (imageHeight > (size - 1))) {
           // Calculate the start index for the remainder part of A and B
           int startY = stripHeight * (size - 1) + (rank - 1);
           int stripHeight = 1;

           Mat subImage = imageData.rowRange(startY, startY + stripHeight);
           Mat processedSubImage = highPassFilter(subImage, kernel);

           // Sending the position and dimensions of the subimage to rank 
           int y = startY; // Initial Y coordinate
           int width = processedSubImage.cols; // Width of each subimage
           int height = processedSubImage.rows; // Height of each subimage

           MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
           MPI_Send(&width, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
           MPI_Send(&height, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

           // Send the subimage data to rank 0
           MPI_Send(processedSubImage.data, width * height * imageData.elemSize(), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
       }

   }
}

int main(int argc, char** argv) {
   int N;
   MPI_Init(&argc, &argv);
   int rank, size;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   string imagePath = "D:/Samples/eins.jpeg";
   Mat imageData = imread(imagePath, IMREAD_COLOR);
   if (imageData.empty()) {
       cerr << "Error: Could not open or read the image" << endl;
       MPI_Finalize();
       return -1;
   }
   int start_s, stop_s, TotalTime = 0;

   if (rank == 0) {
       cout << "Enter the size of the kernel (odd and greater than or equal to 3): ";
       cin >> N;
   }
   start_s = clock();

   MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
   Mat kernel;
   if (rank == 0) {
       kernel = generateHighPassKernel(N);
       cout << kernel << endl;
   }
   // Broadcast the dimensions of the matrix
   int rows, cols;
   if (rank == 0) {
       rows = kernel.rows;
       cols = kernel.cols;
   }
   MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
   // Allocate memory for the kernel matrix in all processes
   if (rank != 0) {
       kernel = Mat(rows, cols, CV_32F);
   }
   MPI_Bcast(kernel.data, rows * cols, MPI_FLOAT, 0, MPI_COMM_WORLD);
   parallelHighPassFilter(imageData, kernel, rank, size, start_s);
   MPI_Finalize();
   return 0;
}