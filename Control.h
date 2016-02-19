//
// Created by arne on 05.02.16.
//

#ifndef DESKTOP_CROPPER_CONTROL_H
#define DESKTOP_CROPPER_CONTROL_H

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/builder.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/scale.h>
#include "DrawArea.h"


class Control {
public:
    void onCreateClicked();

    void onLoadClicked();

    void monitor_moved_x(double x);

    void monitor_moved_y(double y);

    void monitor_scaled(double new_factor);

    void image_scaled(double new_factor);

    Gtk::Window *init(Glib::RefPtr<Gtk::Builder> builder);

    Control();

private:
    DrawArea _drawArea;

    void init_scales(Glib::RefPtr<Gtk::Builder> builder);

    void init_spin_buttons(Glib::RefPtr<Gtk::Builder> builder);

    void init_find_all(Glib::RefPtr<Gtk::Builder> builder);

    void image_scale_moved();

    void monitor_scale_moved();

    void x_scale_moved();

    void y_scale_moved();

    void image_spin_button_clicked();

    void monitor_spin_button_clicked();

    void x_spin_button_clicked();

    void y_spin_button_clicked();

    std::string get_crop_args(Rect r);

    Cairo::RefPtr<Cairo::Context> drawContext;
    Gtk::Scale *image_scale;
    Gtk::Scale *monitor_scale;
    Gtk::Scale *_x_monitor_position_scale;
    Gtk::Scale *y_monitor_position_scale;
    Gtk::SpinButton *_image_scale_spin_btn;
    Gtk::SpinButton *_monitor_scale_spin_btn;
    Gtk::SpinButton *_x_monitor_position_spin_btn;
    Gtk::SpinButton *_y_monitor_position_spin_btn;
    std::string _image_path;
};

#endif //DESKTOP_CROPPER_CONTROL_H
