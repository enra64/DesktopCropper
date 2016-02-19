//
// Created by arne on 04.02.16.
//

#include <giomm/resource.h>
#include <iostream>
#include "DrawArea.h"

bool DrawArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    double middleX = _x + _monitors[LEFT].get_w();
    double rightX = _x + _monitors[LEFT].get_w() + _monitors[MIDDLE].get_w();
    double middleY = _y + (_monitors[RIGHT].get_h() - _monitors[MIDDLE].get_h()) / 2;
    double leftY = _y + (_monitors[RIGHT].get_h() - _monitors[LEFT].get_h()) / 2;

    Gdk::Cairo::set_source_pixbuf(cr, _image, 0, 0);
    cr->paint();

    DrawArea::draw_rectangle(cr, _x, leftY, _monitors[LEFT].get_w(), _monitors[LEFT].get_h());
    DrawArea::draw_rectangle(cr, middleX, middleY, _monitors[MIDDLE].get_w(), _monitors[MIDDLE].get_h());
    DrawArea::draw_rectangle(cr, rightX, _y, _monitors[RIGHT].get_w(), _monitors[RIGHT].get_h());

    return true;
}

void DrawArea::set_monitor_sizes(int wLeft, int hLeft, int wMiddle, int hMiddle, int wRight, int hRight) {
    _monitors[LEFT].set_original_size(wLeft, hLeft);
    _monitors[MIDDLE].set_original_size(wMiddle, hMiddle);
    _monitors[RIGHT].set_original_size(wRight, hRight);
}

void DrawArea::set_position(double x, double y) {
    if (x >= 0)
        _x = x;
    if (y >= 0)
        _y = y;
    queue_draw();
}

void DrawArea::draw_rectangle(const Cairo::RefPtr<Cairo::Context> &cr, double x, double y, double width,
                              double height) {
    cr->set_source_rgb(1, 1, 1);
    cr->move_to(x, y);
    cr->line_to(x + width, y);
    cr->line_to(x + width, y + height);
    cr->line_to(x, y + height);
    cr->line_to(x, y);
    cr->stroke();
}

DrawArea::DrawArea() {
    set_monitor_sizes(1024, 1280, 1920, 1080, 1080, 1920);
    //set_monitor_sizes(5, 4, 16, 9, 9, 16);
    set_position(0, 0);
    add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK);
}

void DrawArea::loadImage(std::string path) {
    float origWidth, origHeight;
    float factor;
    try {
        _image_original = Gdk::Pixbuf::create_from_file(path);
        _image = Gdk::Pixbuf::create_from_file(path);
        origWidth = _image->get_width();
        origHeight = _image->get_height();
        /*float wFactor = (origWidth / (float) (1920-500));
        float hFactor = (origHeight / (float) 1000);
        factor = wFactor < hFactor ? wFactor : hFactor;*/
    }
    catch (const Gio::ResourceError &ex) {
        std::cerr << "ResourceError: " << ex.what() << std::endl;
    }
    catch (const Gdk::PixbufError &ex) {
        std::cerr << "PixbufError: " << ex.what() << std::endl;
    }

    if (_image) {
        set_image_scale(1);
        set_monitor_scale((float) _image->get_width() / get_all_monitor_width());
        std::cout << _image->get_width() << 'x' << get_all_monitor_width() << '\n';
    }
}

DrawArea::~DrawArea() { }

void DrawArea::set_monitor_scale(double scale) {
    _last_monitor_scale = scale;
    _monitors[LEFT].scale_size(scale);
    _monitors[MIDDLE].scale_size(scale);
    _monitors[RIGHT].scale_size(scale);
    queue_draw();
}

void DrawArea::set_image_scale(double factor) {
    if (!_image_original)
        return;
    _last_image_scale = factor;
    double orig_width = _image_original->get_width();
    double orig_height = _image_original->get_height();
    _image = _image_original->scale_simple((int) (factor * orig_width), (int) (factor * orig_height),
                                           Gdk::INTERP_BILINEAR);
    set_size_request(_image->get_width(), _image->get_height());
    queue_draw();
}

void DrawArea::change_monitor_scale(double delta_scale) {
    set_monitor_scale(_last_monitor_scale + delta_scale);
    signal_monitor_scale.emit(_last_monitor_scale + delta_scale);
}

void DrawArea::change_image_scale(double delta_factor) {
    set_image_scale(_last_image_scale + delta_factor);
    signal_image_scale.emit(_last_image_scale + delta_factor);
}

float DrawArea::get_all_monitor_width() {
    return _monitors[0].get_w() + _monitors[1].get_w() + _monitors[2].get_w();
}

double DrawArea::get_last_image_scale() {
    return _last_image_scale;
}

double DrawArea::get_last_monitor_scale() {
    return _last_monitor_scale;
}

bool DrawArea::on_motion_notify_event(GdkEventMotion *event) {
    if (!_pressed)
        return false;
    double delta_x = event->x - _mouse_x_at_click;
    double delta_y = event->y - _mouse_y_at_click;
    set_position(_monitor_x_at_click + delta_x, _monitor_y_at_click + delta_y);
    signal_x_pos.emit(_monitor_x_at_click + delta_x);
    signal_y_pos.emit(_monitor_y_at_click + delta_y);
    return true;
}

bool DrawArea::on_button_press_event(GdkEventButton *event) {
    if (event->button != 1)
        return false;
    _mouse_x_at_click = event->x;
    _mouse_y_at_click = event->y;
    _monitor_x_at_click = _x;
    _monitor_y_at_click = _y;
    _pressed = true;
    return true;
}

bool DrawArea::on_button_release_event(GdkEventButton *event) {
    if (event->button != 1)
        return false;
    _pressed = false;
    return true;
}

bool DrawArea::on_scroll_event(GdkEventScroll *event) {
    if (event->direction == GDK_SCROLL_DOWN) {
        if (event->state & GDK_CONTROL_MASK)
            change_image_scale(0.03);
        else
            change_monitor_scale(0.005);
    } else if (event->direction == GDK_SCROLL_UP) {
        if (event->state & GDK_CONTROL_MASK)
            change_image_scale(-0.03);
        else
            change_monitor_scale(-0.005);
    } else
        return false;
    return true;
}

Rect *DrawArea::get_crop_config() {
    //get instances of everything

    Rect *cfg = new Rect[3];
    Rect *left = &cfg[0];
    Rect *middle = &cfg[1];
    Rect *right = &cfg[2];
    //sizes
    left->w = 1024;
    left->h = 1280;
    middle->w = 1920;
    middle->h = 1080;
    right->w = 1080;
    right->h = 1920;
    //positions
    //scale image and monitors by the same factor so that the image is 1:1
    //double image_one_to_one_factor = 1 / _last_image_scale;
    //int new_x = (int) (image_one_to_one_factor * _x), new_y = (int) (image_one_to_one_factor * _y);
    //x
    left->x = (int) _x;
    middle->x = left->x + left->w;
    right->x = left->x + left->w + middle->w;
    //y
    right->y = (int) _y;
    middle->y = right->y + (right->h - middle->h) / 2;
    left->y = right->y + (right->h - left->h) / 2;

    return _monitors;
}