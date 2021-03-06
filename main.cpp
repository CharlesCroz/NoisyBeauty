#include <iostream>

#include "./NoiseTransformer/NoiseTransformer.hpp"
#include "../FastNoiseLite/Cpp/FastNoiseLite.h"
#include "./CPP_UTILS/Matrix.h"

#define MULTIPLIER 0.2f
#define Z_NOISE_MAX 1000

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
std::vector<Matrix<float>> noiseVolume;
int noiseZValue_ = 0;
FastNoiseLite noiseGenerator;

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
            cv::Mat displayTarget = NoiseTransformer::noiseToCvMat(noiseVolume[noiseZValue_], noiseToCvPoint);
            cv::imshow(imageTargetName, displayTarget);
        }
        clickCount++;
    } else if (event == cv::EVENT_RBUTTONDOWN) {
        exit(0);
    }
}

static void onTrackBar(int, void *) {
    cv::Mat displayTarget = NoiseTransformer::noiseToCvMat(noiseVolume[noiseZValue_], noiseToCvPoint);
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

    noiseToCvPoint = NoiseTransformer::imageToVector(imageSource, 200,
                                                     i1_, j1_, i2_, j2_);

    noiseGenerator = FastNoiseLite(FastNoiseLite::NoiseType_Perlin);
    noiseVolume.resize(Z_NOISE_MAX + 1);
    for(int i = 0; i <= Z_NOISE_MAX ; i++){
        noiseVolume[i] = Matrix<float>(width, height);
        float zVal = (float) i * MULTIPLIER;
#pragma omp parallel for default(none) shared(height, width, noiseGenerator, zVal, noiseVolume, i)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float val = 0.5f * (1.0f + noiseGenerator.GetNoise((float) x, (float) y, zVal));
                noiseVolume[i].at(x, y) = val;
            }
        }
    }

    cv::Mat display = NoiseTransformer::noiseToCvMat(noiseVolume[noiseZValue_], noiseToCvPoint);

    cv::imshow(imageSourceName, imageSourceDisplay);
    cv::imshow(imageTargetName, display);
    cv::setMouseCallback(imageSourceName, onMouse, nullptr);
    cv::createTrackbar(imageSourceZSliderName, imageSourceName, &noiseZValue_, Z_NOISE_MAX, &onTrackBar);

    while (1) {
        cv::waitKey(0);
    }
}
