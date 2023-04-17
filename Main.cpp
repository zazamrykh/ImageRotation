#include <iostream>
#include<opencv2/opencv.hpp>
#include <math.h>

using namespace cv;

double pi = 3.14159265358979323846;

void addLinesForRotation(Mat& inputImage) {
    int height = inputImage.rows;
    int width = inputImage.cols;
    // Radius of circumscribed circum
    double R = sqrt((height / 2.0) * (height / 2.0) + (width / 2.0) * (width / 2.0));
    int whiteRectanglesHeight = (int)R - height / 2.0 + 1;
    int whiteRectanglesWidth = (int)R - width / 2.0 + 1;

    Mat topOrBottom(whiteRectanglesHeight , width, CV_8UC3, Scalar(0, 0, 0));
    vconcat(topOrBottom, inputImage, inputImage);
    vconcat(inputImage, topOrBottom, inputImage);

    Mat leftOrRight(2 * whiteRectanglesHeight + height, whiteRectanglesWidth, CV_8UC3, Scalar(0, 0, 0));
    hconcat(leftOrRight, inputImage, inputImage);
    hconcat(inputImage, leftOrRight, inputImage);

    ~(topOrBottom);
    ~(leftOrRight);
}   

void rotateImageUsingOpenCV(Mat& image, double angle, Point2f center) {
    Mat rotationMatix = getRotationMatrix2D(center, angle, 1.0);
    warpAffine(image, image, rotationMatix, image.size());
}

void rotateImageStraightOrder(Mat& image, double angle, Point2f center) {
    int rows = image.rows;
    int cols = image.cols;
   
    Mat rotatedImage(rows, cols, CV_8UC3, Scalar(0, 0, 0));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double shiftedX = j - center.x;
            double shiftedY = i - center.y;
            double newX = shiftedX * cos(angle * pi / 180.0) + shiftedY * sin(angle * pi / 180.0);
            double newY = -shiftedX * sin(angle * pi / 180.0) + shiftedY * cos(angle * pi / 180.0);
            newX += center.x;
            newY += center.y;
            if (newX >= rows || newY >= cols || newY <= 0 || newX <= 0) {
                continue;
            }

            Vec3b& color = image.at<Vec3b>(i, j);
            int newI = (int)newX;
            int newJ = (int)newY;

            if (newI >= 0 && newJ >= 0) {
                rotatedImage.at<Vec3b>(Point(newI, newJ)) += color * (1 - newX + newI) * (1 - newY + newJ);
            }
            if (newI + 1 < cols && newJ + 1 < rows) {
                rotatedImage.at<Vec3b>(Point(newI + 1, newJ + 1)) += color * (newX - newI) * (newY - newJ);
            }
            if (newI >= 0 && newJ + 1 < rows) {
                rotatedImage.at<Vec3b>(Point(newI, newJ + 1)) += color * (1 - newX + newI) * (newY - newJ);
            }
            if (newI + 1 < cols && newJ >= 0) {
                rotatedImage.at<Vec3b>(Point(newI + 1, newJ)) += color * (newX - newI) * (1 - newY + newJ);
            }
        }
    }
    ~(image);
    image = rotatedImage;
}

void rotateImage(Mat& image, double angle, Point2f center) {
    int rows = image.rows;
    int cols = image.cols;

    Mat rotatedImage(rows, cols, CV_8UC3, Scalar(0, 0, 0));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double shiftedX = j - center.x;
            double shiftedY = i - center.y;
            double newX = shiftedX * cos(angle * pi / 180.0) + shiftedY * sin(angle * pi / 180.0);
            double newY = -shiftedX * sin(angle * pi / 180.0) + shiftedY * cos(angle * pi / 180.0);
            newX += center.x;
            newY += center.y;
            if (newX >= rows - 1 || newY >= cols - 1 || newY <= 0 || newX <= 0) {
                continue;
            }
            int newIntPartX = (int)newX;
            int newIntPartY = (int)newY;
            Vec3b& color = image.at<Vec3b>(newIntPartX, newIntPartY);
            rotatedImage.at<Vec3b>(Point(i, j)) = image.at<Vec3b>(newIntPartX, newIntPartY) * (1 - newX + newIntPartX) * (1 - newY + newIntPartY)
                + image.at<Vec3b>(newIntPartX + 1, newIntPartY + 1) * (newX - newIntPartX) * (newY - newIntPartY)
                + image.at<Vec3b>(newIntPartX, newIntPartY + 1) * (1 - newX + newIntPartX) * (newY - newIntPartY)
                + image.at<Vec3b>(newIntPartX + 1, newIntPartY) * (newX - newIntPartX) * (1 - newY + newIntPartY);
        }
    }

    ~(image);
    image = rotatedImage;
}

int main(int, char**) {
    Mat image = imread("what_a_rotation.png");
    imshow("Input image", image);
    waitKey(0);

    // Add white lines from each side of image for pixels don't dissapear while rotation
    addLinesForRotation(image);
    imshow("Add white lines", image);
    waitKey(0);

    std::cout << "Input rotation angle" << '\n';
    double rotationAngle;
    std::cin >> rotationAngle;

    // You can set rotation center there
    Point2f rotationCenter((image.cols - 1) / 2.0, (image.rows - 1) / 2.0);

    while (true) {
        rotateImage(image, rotationAngle, rotationCenter);
        imshow("Rotatated image", image);
        waitKey(0);
    }
}