#include "../system_rust/copy_rust.h"

void kop_rast::tp_rust_console() {
    print("enter a position for teleportation...");
    print("enter position x");
    std::cin >> x; 
    print("enter position y");
    std::cin >> y;
    print("Excellent! The coordinates have been updated.");
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
    
}