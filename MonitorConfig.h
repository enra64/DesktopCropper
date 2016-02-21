//
// Created by arne on 19.02.16.
//

#ifndef DESKTOP_CROPPER_MONITORCONFIG_H
#define DESKTOP_CROPPER_MONITORCONFIG_H

#include <vector>
#include "Rect.h"

class MonitorConfig {
public:
    void prepare_cfg_for_crop(double orig_image_width, double orig_image_height, double image_scale, double x, double y,
                              double monitor_scale);

    Rect *get_monitor_cfg(unsigned long index);

    void scale_all(double factor);

    void scale_positions(double factor);

    void scale_sizes(double factor);

    double width_all();

    double height_max();

    void set_original_size(unsigned long index, double w, double h);

    void addMonitor(double w, double h);

    std::string get_image_magick_cmd(unsigned long index, std::string dir, std::string name, std::string ext,
                                     std::string resized_image_path);

    double _required_image_scale = 0;

    bool requires_image_rescaling(double orig_image_width, double orig_image_height, double image_scale,
                                  double x, double y, double monitor_scale);
private:
    void scale_monitors(double image_width, double image_height, double image_scale);

    void position_monitors(double x, double y);

    std::string get_crop_args(unsigned long index);

    std::vector<Rect> _cfg_rectangles;
};


#endif //DESKTOP_CROPPER_MONITORCONFIG_H
