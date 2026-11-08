#include "../system_rust/copy_rust.h"



void kop_rast::console_rust() {
    print("Welcome to the Copy Rust console! Type 'help' to get started.");
    print("version 1.5 ");
    while (true) {
        std::cout << "~& ";
        std::getline(std::cin, cin_console_rust);
        if (cin_console_rust == "help") {print_help_console_cop_rust (); }
        else if (cin_console_rust.find_first_not_of(" \t") == std::string::npos) { continue; }
        else if (cin_console_rust == "exit") { break; }
        else if (cin_console_rust == "tp") {continue_enter(); tp_rust_console(); continue_enter();}
        else if (cin_console_rust == "user_change") {continue_enter();user_change_v(); continue_enter();}
        else if (cin_console_rust == "status") {continue_enter();max_min_pers(); continue_enter();}
        else if (cin_console_rust == "setting") {continue_enter(); setting_user(); continue_enter();}
        else if (cin_menu == "men_play") {continue_enter(); print_menu(); continue_enter();}
        else if (cin_menu == "men_con") {continue_enter(); print_menu(); continue_enter();}
        else if (cin_console_rust == "stat") { print_statisc_user(); continue_enter();}
        else if (cin_console_rust == "cls" or cin_console_rust == "cls") {system("cls"); }  
        else if (cin_console_rust == "setting") {continue_enter(); min_res(); continue_enter();}
        else { print("error syntaxis"); }
    }

}