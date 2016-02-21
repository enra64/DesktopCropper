//
// Created by arne on 05.02.16.
//


#include <gtkmm/paned.h>
#include <gtkmm/scale.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/stock.h>
#include <iostream>
#include "Control.h"

void Control::onCreateClicked() {
    Gtk::FileChooserDialog dialog("Save", Gtk::FILE_CHOOSER_ACTION_SAVE);

    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

    int result = dialog.run();

    std::string filename = dialog.get_filename();
    std::string dir = filename.substr(0, filename.find_last_of("/"));

    std::string name, ext;

    if (filename.find_last_of(".") == std::string::npos) {
        name = filename.substr(filename.find_last_of("/") + 1);
        ext = _image_path.substr(_image_path.find_last_of("."));
    }
    else {
        name = filename.substr(filename.find_last_of("/") + 1,
                               filename.find_last_of(".") - (filename.find_last_of("/") + 1));
        ext = filename.substr(filename.find_last_of("."));
    }

    MonitorConfig *cfg = _drawArea.get_monitor_config();

    std::stringstream resized_image_location;
    std::stringstream resize_image_cmd;
    std::stringstream remove_resized_image_cmd;

    resized_image_location << dir << "/" << name << "_res" << ext;

    std::string resized_image_path = resized_image_location.str();

    //resize image to fit monitors
    resize_image_cmd << "convert \"" << _image_path << "\" -resize " << cfg->_required_image_scale * 100 << "% \"" <<
    resized_image_path << "\"";
    system(resize_image_cmd.str().c_str());
    std::cout << resize_image_cmd.str() << '\n';

    system(cfg->get_image_magick_cmd(0, dir, name, ext, resized_image_path).c_str());
    system(cfg->get_image_magick_cmd(1, dir, name, ext, resized_image_path).c_str());
    system(cfg->get_image_magick_cmd(2, dir, name, ext, resized_image_path).c_str());

    //remove the resized image
    remove_resized_image_cmd << "rm " << "\"" << resized_image_path << "\"";
    //system(remove_resized_image_cmd.str().c_str());
    std::cout << remove_resized_image_cmd.str() << '\n';
}

void Control::onLoadClicked() {
    Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
    //Add response buttons the the dialog:
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Open", Gtk::RESPONSE_OK);

    auto filter_any = Gtk::FileFilter::create();
    filter_any->set_name("Any files");
    filter_any->add_pattern("*");
    dialog.add_filter(filter_any);

    dialog.run();

    _image_path = dialog.get_filename();

    _drawArea.signal_x_pos.connect(sigc::mem_fun(this, &Control::monitor_moved_x));
    _drawArea.signal_y_pos.connect(sigc::mem_fun(this, &Control::monitor_moved_y));
    _drawArea.signal_monitor_scale.connect(sigc::mem_fun(this, &Control::monitor_scaled));
    _drawArea.signal_image_scale.connect(sigc::mem_fun(this, &Control::image_scaled));

    _drawArea.loadImage(_image_path);
    image_scale->set_value(_drawArea.get_last_image_scale());
    monitor_scale->set_value(_drawArea.get_last_monitor_scale());
    _drawArea.show();
}

Gtk::Window *Control::init(Glib::RefPtr<Gtk::Builder> builder) {
    Gtk::Window *window;
    Gtk::Button *loadFileButton;
    Gtk::Button *createOutputButton;
    Gtk::Paned *pane;

    builder->get_widget("main_window", window);
    builder->get_widget("load_file_button", loadFileButton);
    builder->get_widget("create_button", createOutputButton);
    builder->get_widget("main_pane", pane);

    init_find_all(builder);
    init_spin_buttons(builder);
    init_scales(builder);

    pane->add2(_drawArea);

    int a = 6;
    int b = 3;

    float test = (float) (a - b) / (float) 50;

    std::cout << test << '\n';

    loadFileButton->signal_clicked().connect(sigc::mem_fun(this, &Control::onLoadClicked));
    createOutputButton->signal_clicked().connect(sigc::mem_fun(this, &Control::onCreateClicked));

    window->set_size_request(1920, 1080);

    return window;
}

Control::Control() {

}

void Control::init_scales(Glib::RefPtr<Gtk::Builder> builder) {
    image_scale->set_range(0.5, 10);
    monitor_scale->set_range(0, 500);
    _x_monitor_position_scale->set_range(0, 5000);
    y_monitor_position_scale->set_range(0, 5000);

    image_scale->signal_value_changed().connect(sigc::mem_fun(this, &Control::image_scale_moved));
    monitor_scale->signal_value_changed().connect(sigc::mem_fun(this, &Control::monitor_scale_moved));
    _x_monitor_position_scale->signal_value_changed().connect(sigc::mem_fun(this, &Control::x_scale_moved));
    y_monitor_position_scale->signal_value_changed().connect(sigc::mem_fun(this, &Control::y_scale_moved));
}

void Control::init_spin_buttons(Glib::RefPtr<Gtk::Builder> builder) {
    _image_scale_spin_btn->set_range(0.1, 10);
    _monitor_scale_spin_btn->set_range(0, 500);
    _x_monitor_position_spin_btn->set_range(0, 5000);
    _y_monitor_position_spin_btn->set_range(0, 5000);

    _image_scale_spin_btn->set_increments(0.05, 1);
    _monitor_scale_spin_btn->set_increments(0.5, 25);
    _x_monitor_position_spin_btn->set_increments(1, 500);
    _y_monitor_position_spin_btn->set_increments(1, 500);

    _image_scale_spin_btn->signal_value_changed().connect(sigc::mem_fun(this, &Control::image_spin_button_clicked));
    _monitor_scale_spin_btn->signal_value_changed().connect(sigc::mem_fun(this, &Control::monitor_spin_button_clicked));
    _x_monitor_position_spin_btn->signal_value_changed().connect(sigc::mem_fun(this, &Control::x_spin_button_clicked));
    _y_monitor_position_spin_btn->signal_value_changed().connect(sigc::mem_fun(this, &Control::y_spin_button_clicked));
}

void Control::image_scale_moved() {
    _drawArea.set_image_scale(image_scale->get_value());
    _image_scale_spin_btn->set_value(image_scale->get_value());
}

void Control::monitor_scale_moved() {
    _drawArea.set_monitor_scale(monitor_scale->get_value());
    _monitor_scale_spin_btn->set_value(monitor_scale->get_value());
}

void Control::x_scale_moved() {
    _drawArea.set_position((int) _x_monitor_position_scale->get_value(), -1);
    _x_monitor_position_spin_btn->set_value(_x_monitor_position_scale->get_value());
}

void Control::y_scale_moved() {
    _drawArea.set_position(-1, (int) y_monitor_position_scale->get_value());
    _y_monitor_position_spin_btn->set_value(y_monitor_position_scale->get_value());
}

void Control::image_spin_button_clicked() {
    _drawArea.set_image_scale(_image_scale_spin_btn->get_value());
    image_scale->set_value(_image_scale_spin_btn->get_value());
}

void Control::monitor_spin_button_clicked() {
    _drawArea.set_monitor_scale(_monitor_scale_spin_btn->get_value());
    monitor_scale->set_value(_monitor_scale_spin_btn->get_value());
}

void Control::x_spin_button_clicked() {
    _drawArea.set_position((int) _x_monitor_position_spin_btn->get_value(), -1);
    _x_monitor_position_scale->set_value(_x_monitor_position_spin_btn->get_value());
}

void Control::y_spin_button_clicked() {
    _drawArea.set_position(-1, (int) _y_monitor_position_spin_btn->get_value());
    y_monitor_position_scale->set_value(_y_monitor_position_spin_btn->get_value());
}

void Control::init_find_all(Glib::RefPtr<Gtk::Builder> builder) {
    builder->get_widget("scale_image", image_scale);
    builder->get_widget("scale_monitors", monitor_scale);
    builder->get_widget("x_monitors", _x_monitor_position_scale);
    builder->get_widget("y_monitors", y_monitor_position_scale);

    builder->get_widget("image_scale_spin_button", _image_scale_spin_btn);
    builder->get_widget("monitor_scale_spin_button", _monitor_scale_spin_btn);
    builder->get_widget("x_monitors_spin_button", _x_monitor_position_spin_btn);
    builder->get_widget("y_monitors_spin_button", _y_monitor_position_spin_btn);
}

void Control::monitor_moved_x(double x) {
    _x_monitor_position_spin_btn->set_value(x);
    _x_monitor_position_scale->set_value(x);
}

void Control::monitor_moved_y(double y) {
    _y_monitor_position_spin_btn->set_value(y);
    y_monitor_position_scale->set_value(y);
}

void Control::monitor_scaled(double new_factor) {
    _monitor_scale_spin_btn->set_value(new_factor);
    monitor_scale->set_value(new_factor);
}

void Control::image_scaled(double new_factor) {
    _image_scale_spin_btn->set_value(new_factor);
    image_scale->set_value(new_factor);
}
