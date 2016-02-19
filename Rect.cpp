//
// Created by arne on 19.02.16.
//

#include "Rect.h"

void Rect::scale_position(double factor) {
    x = (x_original * factor);
    y = (y_original * factor);
}

void Rect::scale_size(double factor) {
    w = (w_original * factor);
    h = (h_original * factor);
}

void Rect::scale_all(double factor) {
    scale_position(factor);
    scale_size(factor);
}

Rect::Rect(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->x_original = x;
    this->y_original = y;
    this->w_original = w;
    this->h_original = h;
}

Rect::Rect() {

}

void Rect::set_position(double x, double y) {
    this->x = (int) x;
    this->y = (int) y;
}

int Rect::get_h() {
    return (int) h;
}

int Rect::get_w() {
    return (int) w;
}

int Rect::get_y() {
    return (int) y;
}

int Rect::get_x() {
    return (int) x;
}

void Rect::set_original_size(double w, double h) {
    w_original = w;
    h_original = h;
}
