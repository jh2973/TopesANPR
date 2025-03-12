/**
 * Copyright 2020 (C) Hailo Technologies Ltd.
 * All rights reserved.
 *
 * Hailo Technologies Ltd. ("Hailo") disclaims any warranties, including, but not limited to,
 * the implied warranties of merchantability and fitness for a particular purpose.
 * This software is provided on an "AS IS" basis, and Hailo has no obligation to provide maintenance,
 * support, updates, enhancements, or modifications.
 *
 * You may use this software in the development of any project.
 * You shall not reproduce, modify or distribute this software without prior written permission.
 **/
/**
 * @file yolov5_post_processing.hpp
 * @brief Yolov5 Post-Processing
 **/

#ifndef _HAILO_YOLOV5_POST_PROCESSING_HPP_
#define _HAILO_YOLOV5_POST_PROCESSING_HPP_

//#include <vector>
#include <unordered_map>
//#include <stdint.h>

//constexpr uint32_t IMAGE_SIZE = 640;
//constexpr uint32_t YOLOV5M_IMAGE_WIDTH = 640;
//constexpr uint32_t YOLOV5M_IMAGE_HEIGHT = 640;
//constexpr uint32_t YOLOV5M_IMAGE_WIDTH = 1280;
//constexpr uint32_t YOLOV5M_IMAGE_HEIGHT = 1280;

//const uint32_t MAX_BOXES = 50;

struct DetectionObject {
    float ymin, xmin, ymax, xmax, confidence;
    uint32_t class_id, track_id;

    DetectionObject(float ymin, float xmin, float ymax, float xmax, float confidence, uint32_t class_id, uint32_t track_id):
        ymin(ymin), xmin(xmin), ymax(ymax), xmax(xmax), confidence(confidence), class_id(class_id), track_id(track_id)
        {}

    bool operator<(const DetectionObject &s2) const {
        return this->confidence > s2.confidence;
    }
};

/*
    API for yolo postprocessing
    Given all parameters this function returns boxes with class and confidence
    Inputs:
        feature map1: 20x20x255
        feature map2: 40x40x255
        feature map3: 80x80x255
    Outputs:
        final boxes for display (Nx6) - DetectionObject
*/
std::vector<DetectionObject> post_processing(
    uint8_t *fm1, float qp_zp_1, float qp_scale_1,
    uint8_t *fm2, float qp_zp_2, float qp_scale_2,
    uint8_t *fm3, float qp_zp_3, float qp_scale_3,
    size_t img_width, size_t img_height, int cls_num, float thr);


#endif /* _HAILO_YOLOV5_POST_PROCESSING_HPP_ */
