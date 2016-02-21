//
// Created by arne on 04.02.16.
//

#include <giomm/resource.h>
#include <iostream>
#include "DrawArea.h"

bool DrawArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    double middleX = _x + _monitor_config.get_monitor_cfg(0)->get_w();
    double rightX = _x + _monitor_config.get_monitor_cfg(0)->get_w() + _monitor_config.get_monitor_cfg(1)->get_w();
    double middleY =
            _y + (_monitor_config.get_monitor_cfg(2)->get_h() - _monitor_config.get_monitor_cfg(1)->get_h()) / 2;
    double leftY = _y + (_monitor_config.get_monitor_cfg(2)->get_h() - _monitor_config.get_monitor_cfg(0)->get_h()) / 2;

    Gdk::Cairo::set_source_pixbuf(cr, _image, 0, 0);
    cr->paint();

    DrawArea::draw_rectangle(cr, _x, leftY, _monitor_config.get_monitor_cfg(0)->get_w(),
                             _monitor_config.get_monitor_cfg(0)->get_h());
    DrawArea::draw_rectangle(cr, middleX, middleY, _monitor_config.get_monitor_cfg(1)->get_w(),
                             _monitor_config.get_monitor_cfg(1)->get_h());
    DrawArea::draw_rectangle(cr, rightX, _y, _monitor_config.get_monitor_cfg(2)->get_w(),
                             _monitor_config.get_monitor_cfg(2)->get_h());

    return true;
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
    _monitor_config.addMonitor(1024, 1280);
    _monitor_config.addMonitor(1920, 1080);
    _monitor_config.addMonitor(1080, 1920);
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
        float wFactor = ((float) (1920 - 500) / origWidth);
        float hFactor = ((float) 1000 / origHeight);
        factor = wFactor < hFactor ? wFactor : hFactor;
    }
    catch (const Gio::ResourceError &ex) {
        std::cerr << "ResourceError: " << ex.what() << std::endl;
    }
    catch (const Gdk::PixbufError &ex) {
        std::cerr << "PixbufError: " << ex.what() << std::endl;
    }

    if (_image) {
        set_image_scale(factor);
        set_monitor_scale(factor);
    }
}

DrawArea::~DrawArea() { }

void DrawArea::set_monitor_scale(double scale) {
    _last_monitor_scale = scale;
    _monitor_config.scale_all(scale);
    signal_monitor_scale.emit(_last_monitor_scale);
    update_scale_warning_label();
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
    signal_image_scale.emit(_last_image_scale);
    set_size_request(_image->get_width(), _image->get_height());
    update_scale_warning_label();
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

MonitorConfig DrawArea::get_monitor_config() {
    MonitorConfig cfg = MonitorConfig(_monitor_config);
    cfg.prepare_cfg_for_crop(_image_original->get_width(), _image_original->get_height(), _last_image_scale,
                                         _x, _y, _last_monitor_scale);
    return cfg;
}

void DrawArea::update_scale_warning_label() {
    bool needs_rescale = _monitor_config.requires_image_rescaling(_image_original->get_width(),
                                                                  _image_original->get_height(), _last_image_scale,
                                                                  _x, _y, _last_monitor_scale);
    if (needs_rescale) {
        std::string label = "Image scale factor: ";
        label.append(std::to_string(_last_image_scale / _last_monitor_scale));
        _scale_warning_label->set_text(label);
    }
    else {
        _scale_warning_label->set_text("No scaling needed");
    }
}

void DrawArea::set_scale_warning_label(Gtk::Label *label) {
    _scale_warning_label = label;
}
