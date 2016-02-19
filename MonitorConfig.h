//
// Created by arne on 19.02.16.
//

#ifndef DESKTOP_CROPPER_MONITORCONFIG_H
#define DESKTOP_CROPPER_MONITORCONFIG_H

#include <vector>
#include "Rect.h"

class MonitorConfig {
public:
    void get_image_magick_cmd(int index);

    double scale_monitors(double image_width, double image_height, double image_scale);

    void position_monitors(double x, double y);

    void set_size(unsigned long monitor_count);

    Rect *get_monitor_cfg(unsigned long index);

private:
    std::vector<Rect> _cfg_rectangles;

    void scale_all(double factor);

    double width_all();

    double height_max();
};


#endif //DESKTOP_CROPPER_MONITORCONFIG_H
