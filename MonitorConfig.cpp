//
// Created by arne on 19.02.16.
//

#include <iostream>
#include <sstream>
#include "MonitorConfig.h"

std::string MonitorConfig::get_image_magick_cmd(unsigned long index, std::string dir, std::string name, std::string ext,
                                                std::string resized_image_path) {
    std::stringstream right_stream;
    char ending;
    switch (index) {
        case 0:
            ending = 'l';
            break;
        case 1:
            ending = 'm';
            break;
        case 2:
            ending = 'r';
            break;
        default:
            ending = (char) (48 + index);//very good
    }
    right_stream << "convert \"" << resized_image_path << "\" -crop " << get_crop_args(index) << " \"" <<
    dir << "/" << name << "_" << ending << ext << "\"";
    std::cout << right_stream.str() << '\n';
    return right_stream.str();
}

std::string MonitorConfig::get_crop_args(unsigned long index) {
    Rect *r = &_cfg_rectangles.at(index);
    std::stringstream ss;
    ss << r->get_w() << "x" << r->get_h() << "+" << r->get_x() << "+" << r->get_y();
    return ss.str();
}

void MonitorConfig::scale_all(double factor) {
    for (unsigned long i = 0; i < _cfg_rectangles.size(); i++) {
        Rect *r = &_cfg_rectangles.at(i);
        r->scale_all(factor);
    }
}

void MonitorConfig::scale_sizes(double factor) {
    for (unsigned long i = 0; i < _cfg_rectangles.size(); i++) {
        Rect *r = &_cfg_rectangles.at(i);
        r->scale_size_from_original(factor);
    }
}

void MonitorConfig::scale_positions(double factor) {
    for (unsigned long i = 0; i < _cfg_rectangles.size(); i++) {
        Rect *r = &_cfg_rectangles.at(i);
        r->scale_position(factor);
    }
}

Rect *MonitorConfig::get_monitor_cfg(unsigned long index) {
    return &_cfg_rectangles.at(index);
}

void MonitorConfig::scale_monitors(double image_width, double image_height, double image_scale) {
    if (width_all() < image_width && height_max() < image_height) {
        scale_all(1);
        _required_image_scale = 1;
    }
    else {
        double scale_to_fit_x = width_all() / image_width;
        double scale_to_fit_y = height_max() / image_height;
        double scale = scale_to_fit_x < scale_to_fit_y ? scale_to_fit_y : scale_to_fit_x;
        scale_all(1 / image_scale);
        std::cout << "not implemented!\n";
        _required_image_scale = scale;
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
        r->set_position(x_sum, y + (h_max - r->get_h()) / 2);
        x_sum += r->get_w();
    }
}

void MonitorConfig::set_original_size(unsigned long index, double w, double h) {
    _cfg_rectangles.at(index).set_original_size(w, h);
}

void MonitorConfig::addMonitor(double w, double h) {
    _cfg_rectangles.push_back(Rect(0, 0, w, h));
}

void MonitorConfig::prepare_cfg_for_crop(double orig_image_width, double orig_image_height, double image_scale,
                                         double x, double y, double monitor_scale) {
    if (!requires_image_rescaling(orig_image_width, orig_image_height, image_scale, x, y, monitor_scale)) {
        _required_image_scale = 1;
        //the monitors fit 1:1 into the image, we just need to scale everything to original size and calculate the monitor offset
        scale_sizes(1);
        x /= monitor_scale;
        y /= monitor_scale;
        position_monitors(x, y);
    }
    else {
        //monitors out of image bounds, increase image size
        _required_image_scale = image_scale / monitor_scale;
        std::cout << "scaling image" << _required_image_scale << "\n";
        scale_sizes(1);
        x *= 1 / monitor_scale;
        y *= 1 / monitor_scale;
        position_monitors(x, y);
    }
}

bool MonitorConfig::requires_image_rescaling(double orig_image_width, double orig_image_height, double image_scale,
                                             double x, double y, double monitor_scale) {
    double scaled_background_x = (x + width_all()) / monitor_scale;
    double scaled_background_y = (y + height_max()) / monitor_scale;
    return scaled_background_x > orig_image_width || scaled_background_y > orig_image_height;
}
