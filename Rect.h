//
// Created by arne on 19.02.16.
//

#ifndef DESKTOP_CROPPER_RECT_H
#define DESKTOP_CROPPER_RECT_H


class Rect {
public:
    int get_x();

    int get_y();

    int get_w();

    int get_h();

    void set_position(double x, double y);

    void set_original_size(double w, double h);

    void scale_size(double factor);

    void scale_position(double factor);

    void scale_all(double factor);

    Rect(int x, int y, int w, int h);

    Rect();

private:
    double x = 0, y = 0, w = 0, h = 0, w_original = 0, y_original = 0, x_original = 0, h_original = 0;
};


#endif //DESKTOP_CROPPER_RECT_H
