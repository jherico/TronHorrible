
/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    namedWindow("color", CV_WINDOW_AUTOSIZE);
    namedWindow("depth", CV_WINDOW_AUTOSIZE);
    namedWindow("fizz", CV_WINDOW_AUTOSIZE);
    namedWindow("bizz", CV_WINDOW_AUTOSIZE);
    int erosion_size = 2;
    /// Create Erosion Trackbar
    createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Erosion Demo", &erosion_size, 100, &callback);
    moveWindow("color", 50, 50);
    moveWindow("depth", 400, 50);
    moveWindow("fizz", 50, 400);
    moveWindow("bizz", 400, 400);

    /// Show some stuff
    callback(1, 0);
    waitKey(1);

    Camera camera;
    usleep(1000 * 1000);

    bool quit = false;
    while (!quit) {
        if (camera.isColorDirty()) {
            imshow("color", camera.getColorImage());
        }
        if (camera.isDepthDirty()) {
            static Mat kernel = getStructuringElement(CV_SHAPE_RECT, Size(9, 9), Point(4, 4));


            Mat mask1;
            Mat depthMasked;
            Mat depthOriginal = camera.getDepthImage();
            // filter down to between 0.1 and 0.6 meters
            {
//                GaussianBlur(depth, depth, Size(3, 3), 0);
                inRange(depthOriginal, Scalar(0.05), Scalar(0.6), mask1);
                morphologyEx(mask1, mask1, MORPH_OPEN, kernel);
                GaussianBlur(mask1, mask1, Size(3, 3), 0);
                depthOriginal.copyTo(depthMasked, mask1);
            }

            // Create a standard color image to annotate with hand findings
            Mat color;
            {
                double min, max;
                cv::minMaxIdx(depthOriginal, &min, &max);
                cv::convertScaleAbs(depthOriginal, color, 255 / (max - min));
                applyColorMap(color, color, cv::COLORMAP_BONE);
            }

            static vector<Point> contour;
            Hand hand;
            if (find_contour(mask1, hand)) {
                find_convex_hull(hand);
                find_fingers(mask1, hand);
                annotate(color, hand);
            }
            imshow("depth", depthMasked);
            imshow("color", color);
            imshow("fizz", mask1);
            imshow("bizz", depthOriginal);

            //Mat thr_image;
            //inRange(g_depthImage, Scalar(th_min), Scalar(th_max), thr_image);
            //imshow("color", threshold);
        }

        char key = cvWaitKey(1);
        switch (key) {
        case 27:
        case 'q':
            quit = true;
            break;

        default:
            usleep(1000);
            break;
        }

    }
// Get the list of currently connected devices

    return 0;
}
