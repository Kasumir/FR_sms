#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QtGlobal>

#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

namespace imageutil{
    /*
     * convert Mat to  QImage or QPixmap
     */
    QImage cvMatToQImage(const cv::Mat &inMat);
    QPixmap cvMatToQPixmap(const cv::Mat &inMat);



}
#endif // IMAGEUTIL_H
