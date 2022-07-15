//
// Created by charles on 26/06/22.
//

#ifndef NOISYBEAUTY_NOISETRANSFORMER_HPP
#define NOISYBEAUTY_NOISETRANSFORMER_HPP

#include "../CPP_UTILS/Matrix.h"
#include <vector>
#include "opencv2/opencv.hpp"

class NoiseTransformer {
public:
    static cv::Mat noiseToCvMat(const Matrix<float> &noiseMatrix,
                                const std::vector<cv::Point3_<uchar>> &noiseToCvPoint) {

        auto h = (int) noiseMatrix.get_height();
        auto w = (int) noiseMatrix.get_width();
        auto vecSize = (float) noiseToCvPoint.size();

        cv::Mat result = cv::Mat(h, w, CV_8UC3);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                auto noiseVal = noiseMatrix.at(x, y);
                assert(noiseVal >= 0.0f);
                assert(noiseVal <= 1.0f);
                auto pointIndex = (size_t) (noiseVal * vecSize);
                result.at<cv::Point3_<uchar>>(y, x) = noiseToCvPoint[pointIndex];
            }
        }

        return result;
    }

    static std::vector<cv::Point3_<uchar>> imageToVector(const cv::Mat &image, int pointCount,
                                                         int i1, int j1, int i2, int j2) {
        assert(pointCount >= 2);
        std::vector<cv::Point3_<uchar>> result;
        result.reserve(pointCount);
        for (int a = 0; a < pointCount; ++a) {
            float ratio = (float) a / (float) (pointCount - 1);
            int i = i1 + (int) (ratio * (float) (i2 - i1));
            int j = j1 + (int) (ratio * (float) (j2 - j1));
            result.emplace_back(image.at<cv::Point3_<uchar>>(cv::Point(i, j)));
        }

        return result;
    }
};


#endif //NOISYBEAUTY_NOISETRANSFORMER_HPP
