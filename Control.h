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
    void on_save_clicked();

    void on_open_clicked();

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

    void init_menu(Glib::RefPtr<Gtk::Builder> builder);

    void on_save_as_clicked();

    void create(std::string path, bool verbose);

    void image_scale_moved();

    bool on_shortcut(GdkEventKey *event);

    void monitor_scale_moved();

    void x_scale_moved();

    void y_scale_moved();

    void image_spin_button_clicked();

    void monitor_spin_button_clicked();

    void x_spin_button_clicked();

    void y_spin_button_clicked();

    Cairo::RefPtr<Cairo::Context> drawContext;
    Gtk::Scale *_image_scale;
    Gtk::Scale *_monitor_scale;
    Gtk::Scale *_x_monitor_position_scale;
    Gtk::Scale *_y_monitor_position_scale;
    Gtk::MenuItem *_menu_open;
    Gtk::MenuItem *_menu_save;
    Gtk::MenuItem *_menu_save_as;
    Gtk::SpinButton *_image_scale_spin_btn;
    Gtk::SpinButton *_monitor_scale_spin_btn;
    Gtk::SpinButton *_x_monitor_position_spin_btn;
    Gtk::SpinButton *_y_monitor_position_spin_btn;
    std::string _image_path, _last_filename;
};

#endif //DESKTOP_CROPPER_CONTROL_H
