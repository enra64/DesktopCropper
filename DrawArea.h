//
// Created by arne on 04.02.16.
//

#ifndef DESKTOP_CROPPER_DRAWAREA_H
#define DESKTOP_CROPPER_DRAWAREA_H


#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>
#include <gdkmm/general.h>
#include <gtkmm/label.h>
#include "Rect.h"
#include "MonitorConfig.h"


class DrawArea : public Gtk::DrawingArea {
public:
    DrawArea();

    virtual ~DrawArea();

    void set_position(double x, double y);

    double get_last_image_scale();

    MonitorConfig get_monitor_config();

    double get_last_monitor_scale();

    void set_image_scale(double factor);

    void change_image_scale(double delta_factor);

    void set_monitor_scale(double scale);

    void change_monitor_scale(double delta_scale);

    void loadImage(std::string path);

    void set_scale_warning_label(Gtk::Label *label);

    sigc::signal<void, double> signal_x_pos;
    sigc::signal<void, double> signal_y_pos;
    sigc::signal<void, double> signal_monitor_scale;
    sigc::signal<void, double> signal_image_scale;
protected:
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;

    // Override mouse events
    bool on_motion_notify_event(GdkEventMotion *event);

    bool on_button_press_event(GdkEventButton *event);

    bool on_button_release_event(GdkEventButton *event);

    bool on_scroll_event(GdkEventScroll *event);

private:
    void update_scale_warning_label();

    Gtk::Label *_scale_warning_label;
    void draw_rectangle(const Cairo::RefPtr<Cairo::Context> &cr, double x, double y, double width, double height);
    double _x = 0, _y = 0, _mouse_x_at_click = 0, _mouse_y_at_click = 0, _monitor_x_at_click = 0, _monitor_y_at_click = 0;
    double _last_image_scale = 0, _last_monitor_scale = 0;
    Glib::RefPtr<Gdk::Pixbuf> _image, _image_original;
    MonitorConfig _monitor_config;
    bool _pressed = false;
};


#endif //DESKTOP_CROPPER_DRAWAREA_H
