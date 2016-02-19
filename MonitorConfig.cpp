//
// Created by arne on 19.02.16.
//

#include <iostream>
#include "MonitorConfig.h"

void MonitorConfig::get_image_magick_cmd(int index) {

}

void MonitorConfig::set_size(unsigned long monitor_count) {
    _cfg_rectangles.reserve(monitor_count);
}

void MonitorConfig::scale_all(double factor) {
    for (unsigned long i = 0; i < _cfg_rectangles.size(); i++) {
        Rect *r = &_cfg_rectangles.at(i);
        r->scale_all(factor);
    }
}

Rect *MonitorConfig::get_monitor_cfg(unsigned long index) {
    return &_cfg_rectangles.at(index);
}

double MonitorConfig::scale_monitors(double image_width, double image_height, double image_scale) {
    if (width_all() < image_width && height_max() < image_height) {
        scale_all(1 / image_scale);
        return 1;//dont need to scale original image
    }
    else {
        double scale_to_fit_x = width_all() / image_width;
        double scale_to_fit_y = height_max() / image_height;
        double scale = scale_to_fit_x < scale_to_fit_y ? scale_to_fit_y : scale_to_fit_x;
        std::cout << "not implemented!\n";
        return scale;
    }
}

double MonitorConfig::width_all() {
    double sum = 0;
    for (unsigned long i = 0; i < _cfg_rectangles.size(); i++)
        sum += _cfg_rectangles.at(i).get_w();
    return sum;
}

double MonitorConfig::height_max() {
    double maximum = 0;
    for (unsigned long i = 0; i < _cfg_rectangles.size(); i++)
        if (_cfg_rectangles.at(i).get_w() > maximum)
            maximum = _cfg_rectangles.at(i).get_w();
    return maximum;
}

void MonitorConfig::position_monitors(double x, double y) {
    double x_sum = x;
    double h_max = height_max();
    for (unsigned long i = 0; i < _cfg_rectangles.size(); i++) {
        Rect *r = &_cfg_rectangles.at(i);
        r->set_position(x_sum, (h_max - r->get_h()) / 2);
        x_sum += r->get_w();
    }
}
