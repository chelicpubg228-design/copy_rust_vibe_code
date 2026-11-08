#include "../system_rust/copy_rust.h"

void kop_rast::setting_user() {
    std::cout << "--setting--" << std::endl;
    print("q - exit");
    print("h - help");
    print("r - remove config");
    print("n - setting name");
    std::string help_setting, for_cin_set_rem, cin_for_setting;
    while (true) {
    std::cout << "setting -@ ";
    std::getline(std::cin, cin_for_setting);
    if (cin_for_setting == "r") {
        continue_enter();

        std::cout << "y ot n -# ";
        std::cin >> for_cin_set_rem;
        if (for_cin_set_rem == "y") {

            remove("config_cop_rast.json");
            reset();
        }
        else {
            print("ok!");
        }
        continue_enter();
    }
    else if (cin_for_setting.find_first_not_of(" \t") == std::string::npos) { continue; }
   else if (cin_for_setting == "n") {
    continue_enter();
    print("enter new name: ");
    std::getline(std::cin, name_user);         
    if (name_user.empty()) name_user = "None"; 
    ubdate_json();                             
    print("name saved");
}
    else if (cin_for_setting == "h") {continue_enter(); print(help_text_for_set); } 
    else if (cin_for_setting == "q") {continue_enter(); break; }
    else print("error");
    
}

}