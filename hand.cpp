#include "hand.hpp"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace Encom13
{
    namespace Gesture
    {

        void
        Hand::filter_and_threshold(const Mat & input, Mat & output)
        {
            Mat temp_image3;
            static Mat kernel = getStructuringElement(CV_SHAPE_RECT, Size(9, 9),
                    Point(4, 4));
            GaussianBlur(input, temp_image3, Size(11, 11), 0);
            medianBlur(temp_image3, temp_image3, 11);
            cvtColor(temp_image3, temp_image3, CV_BGR2HSV);
            inRange(temp_image3, Scalar(0, 0, 160, 255),
                    Scalar(255, 400, 300, 0), output);
            /* Apply morphological opening */
            morphologyEx(output, output, MORPH_OPEN, kernel);
            GaussianBlur(output, output, Size(3, 3), 0);
        }

        bool
        Hand::find_contour(const Mat & input)
        {
            Mat temp_image1 = input.clone();

            Mat canny_output;
            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;

            //! retrieves contours and the hierarchical information from black-n-white image.
            findContours(temp_image1, contours, hierarchy, CV_RETR_EXTERNAL,
                    CV_CHAIN_APPROX_SIMPLE);

            double max_area = 0.0;
            int found = -1;
            bool contourFound = false;
            for (size_t i = 0; i < contours.size(); ++i)
                {
                    double area = fabs(contourArea(contours[i]));
                    if (area > max_area)
                        {
                            found = i;
                        }
                }

            if (-1 != found)
                {
                    contour = contours[found];
                    approxPolyDP(contour, curve, 2, true);
                    contour = curve;
                    contourFound = true;
                }
            return contourFound;
        }

        void
        Hand::find_convex_hull()
        {
            vector<int> hull;
            vector<Vec4i> defects;
            convexHull(contour, hull);
            //We calculate convexity defects
//    convexityDefects(contour, hull, defects);
//    int x = 0, y = 0;
//    int dist = 0;
//
//    if (!defects.empty()) {
//        for (size_t cDefIt = 0; cDefIt < defects.size(); cDefIt++) {
//            int defectPtIdx = defects[cDefIt].val[2];
//            // do something with **.. This is e.g. the defect point.
//            Point p = contour.at(defectPtIdx);
//            x += p.x;
//            y += p.y;
//            defects.push_back(p);
//        }
//        int num_defects = defects.size();
//        x /= num_defects;
//        y /= num_defects;
//        center = Point(x, y);
//        for (size_t cDefIt = 0; cDefIt < defects.size(); cDefIt++) {
//            int defectPtIdx = defects[cDefIt].val[2];
//            Point dfct = contour.at(defectPtIdx);
//            Point diff = center;
//
//            diff.x -= dfct.x;
//            diff.y -= dfct.y;
//            diff.x *= diff.x;
//            diff.y *= diff.y;
//            dist += sqrt(diff.x + diff.y);
//        }
//        radius = num_defects;
//    }
        }

        void
        Hand::find_fingers(Mat & image)
        {
            int num_fingers = 0;
            if (defects.empty())
                {
                    return;
                }

            /*
             * Fingers are detected as points where the distance to the center
             * is a local maximum
             */
            Point max_point;
            int dist1 = 0, dist2 = 0;
            for (int i = 0; i < contour.size(); i++)
                {
                    const Point & c = center;
                    Point d = contour.at(i);
                    d.x -= c.x;
                    d.y -= c.y;
                    d.x *= d.x;
                    d.y *= d.y;
                    int dist = d.x + d.y;

                    if (dist < dist1 && dist1 > dist2 && max_point.x != 0
                            && max_point.y < image.rows - 10)
                        {
                            finderDists.push_back(dist);
                            fingers.push_back(max_point);
                            if (num_fingers >= 6)
                                break;
                        }

                    dist2 = dist1;
                    dist1 = dist;
                    max_point = contour.at(i);
                }
        }

        void
        Hand::annotate(Mat & image)
        {
            //    CV_EXPORTS_W void drawContours( InputOutputArray image, InputArrayOfArrays contours,
            //                                  int contourIdx, const Scalar& color,
            //                                  int thickness=1, int lineType=8,
            //                                  InputArray hierarchy=noArray(),
            //                                  int maxLevel=INT_MAX, Point offset=Point() );

            vector<vector<Point> > vvp;
            vvp.push_back(contour);
            drawContours(image, vvp, 0, CV_RGB(0,0,255), 1, CV_AA);

            //! draws the circle outline or a solid circle in the image
            circle(image, center, 5, Scalar(255, 0, 255), 1, CV_AA);
            circle(image, center, radius, Scalar(255, 0, 0), 1,
                    CV_AA);

            for (int i = 0; i < fingers.size(); i++)
                {
                    Point p = fingers.at(i);
                    circle(image, p, 10, Scalar(0, 255, 0), 3, CV_AA, 0);
                    line(image, center, p, Scalar(255, 255, 0), 1, CV_AA,
                            0);
                }

            for (int i = 0; i < defects.size(); i++)
                {
                    circle(image, defects[i], 2, Scalar(200, 200, 200), 2,
                            CV_AA, 0);
                }
            //    }
        }
    }
}
