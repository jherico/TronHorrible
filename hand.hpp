#pragma once

#include <vector>
#include <opencv2/core/core.hpp>

namespace Encom13 { namespace Gesture {
    class Hand {
    public:
        std::vector<cv::Point> contour;
        std::vector<cv::Point> curve;
        int radius;
        cv::Point center;
        std::vector<cv::Point> defects;
        std::vector<cv::Point> fingers;
        std::vector<int> finderDists;

        void filter_and_threshold(const cv::Mat & input, cv::Mat & output);
        bool find_contour(const cv::Mat & input);
        void find_convex_hull();
        void find_fingers(cv::Mat & image);
        void annotate(cv::Mat & image);
    };
} }
