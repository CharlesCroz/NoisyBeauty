#include <iostream>

#include "./NoiseTransformer/NoiseTransformer.hpp"
#include "./FastNoiseLite/Cpp/FastNoiseLite.h"
#include "./CPP_UTILS/Matrix.h"

#define MULTIPLIER 0.2f

int clickCount = 0;
int i1_;
int j1_;
int i2_;
int j2_;
cv::Mat imageSource;
cv::Mat imageSourceDisplay;
std::string imageSourceName;
std::string imageSourceZSliderName;
std::string imageTargetName;
std::vector<cv::Point3_<uchar>> noiseToCvPoint;
Matrix<float> noiseMatrix;
int noiseZValue_ = 0;
FastNoiseLite noiseGenerator;

void setNoiseMatrix(int zValue, float multiplier) {
    float zVal = (float) zValue * multiplier;
    int h = (int) noiseMatrix.get_height();
    int w = (int) noiseMatrix.get_width();
#pragma omp parallel for default(none) shared(h, w, noiseGenerator, zVal, noiseMatrix)
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float val = 0.5f * (1.0f + noiseGenerator.GetNoise((float) x, (float) y, zVal));
            noiseMatrix.at(x, y) = val;
        }
    }
}

cv::Mat drawPoint1(const cv::Mat &src, int i1, int j1) {
    cv::Mat result;
    cv::copyTo(src, result, cv::Mat());
    cv::Scalar_<uchar> colorRed(0, 0, 255);

    cv::drawMarker(result, cv::Point(i1, j1), colorRed,
                   cv::MARKER_CROSS, 5, 1, 8);
    return result;
}

cv::Mat drawPoint12(const cv::Mat &src, int i1, int j1, int i2, int j2) {
    cv::Mat result;
    cv::copyTo(src, result, cv::Mat());
    cv::Scalar_<uchar> colorRed(0, 0, 255);

    cv::line(result, cv::Point(i1, j1), cv::Point(i2, j2),
             colorRed);
    cv::drawMarker(result, cv::Point(i1, j1), colorRed,
                   cv::MARKER_CROSS, 5, 1, 8);
    cv::drawMarker(result, cv::Point(i2, j2), colorRed,
                   cv::MARKER_CROSS, 5, 1, 8);
    return result;
}

static void onMouse(int event, int x, int y, int, void *) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        if (clickCount % 2 == 0) {
            i1_ = x;
            j1_ = y;
            cv::Mat displaySrc = drawPoint1(imageSource, i1_, j1_);
            cv::imshow(imageSourceName, displaySrc);
        } else {
            i2_ = x;
            j2_ = y;
            cv::Mat displaySrc = drawPoint12(imageSource, i1_, j1_, i2_, j2_);
            cv::imshow(imageSourceName, displaySrc);

            noiseToCvPoint = NoiseTransformer::imageToVector(imageSource, 200,
                                                             i1_, j1_, i2_, j2_);
            cv::Mat displayTarget = NoiseTransformer::noiseToCvMat(noiseMatrix, noiseToCvPoint);
            cv::imshow(imageTargetName, displayTarget);
        }
        clickCount++;
    } else if (event == cv::EVENT_RBUTTONDOWN) {
        exit(0);
    }
}

static void onTrackBar(int, void *) {
    setNoiseMatrix(noiseZValue_, MULTIPLIER);
    cv::Mat displayTarget = NoiseTransformer::noiseToCvMat(noiseMatrix, noiseToCvPoint);
    cv::imshow(imageTargetName, displayTarget);
}

int main() {
    int height = 600;
    int width = 800;

    imageSource = cv::imread("../Images/Face1.jpg");
    if (imageSource.rows > 600) {
        cv::resize(imageSource, imageSource,
                   cv::Size((int) ((float) imageSource.cols * 600 / (float) imageSource.rows), 600));
    }

    imageSourceName = "Pick to points on me";
    imageSourceZSliderName = "zValue";
    imageTargetName = "Enjoy the beauty";
    cv::namedWindow(imageSourceName);
    cv::namedWindow(imageTargetName);

    i1_ = imageSource.rows / 2;
    j1_ = imageSource.cols / 3;
    i2_ = imageSource.rows / 2;
    j2_ = imageSource.cols / 2;

    imageSourceDisplay = drawPoint12(imageSource, i1_, j1_, i2_, j2_);
    cv::imshow(imageSourceName, imageSourceDisplay);

    noiseToCvPoint = NoiseTransformer::imageToVector(imageSource, 200,
                                                     i1_, j1_, i2_, j2_);

    noiseGenerator = FastNoiseLite(FastNoiseLite::NoiseType_Perlin);
    noiseMatrix = Matrix<float>(width, height);
    setNoiseMatrix(noiseZValue_, MULTIPLIER);


    cv::Mat display = NoiseTransformer::noiseToCvMat(noiseMatrix, noiseToCvPoint);


    cv::imshow(imageTargetName, display);

    cv::setMouseCallback(imageSourceName, onMouse, 0);
    cv::createTrackbar(imageSourceZSliderName, imageSourceName, &noiseZValue_, 1000, &onTrackBar);

    while (1) {
        cv::waitKey(0);
    }
}
