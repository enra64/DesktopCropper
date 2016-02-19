#include <gtkmm.h>
#include "Control.h"

int main(int argCount, char *argv[]) {
    auto app = Gtk::Application::create(argCount, argv, "de.oerntec.desktopcropper");

    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("main_window.glade");

    Control control;
    Gtk::Window *win = control.init(builder);

    return app->run(*win);
}