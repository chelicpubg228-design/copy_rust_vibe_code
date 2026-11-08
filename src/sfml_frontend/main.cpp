#include "SfmlApp.h"

#include <iostream>

int main() {
    ui::SfmlApp app;
    if (!app.init()) {
        std::cerr << "Failed to start Copy Rust 2D (window or font missing).\n";
        return 1;
    }
    return app.run();
}
