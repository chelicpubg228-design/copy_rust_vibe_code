#include "../system_rust/copy_rust.h"



void kop_rast::print_menu() {
  std::cout << "=============menu===========\n"
               "1 info        2 exit\n"
               "3 sleep       4 help\n"
               "5 craft       6 don armor\n"
               "7 undon armor 8 statistic user\n"
               "9 statistic axe 10 free mode\n"
               "11 to setting 12 to console\n"
               "13 to stastic resours 14 to status table\n"
               "15 to mini plays 16 to out menu\n"
               "=============menu===========\n"
            << "x = " << x << "\ny = " << y << "\n" << std::endl;
}

void kop_rast::vstr_mouse() {
  if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
    fight(y_small_mouse, hp_mouse, x_small_mouse, damage_mouse, 12, "mouse");

  } else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
    fight(y_normal_mouse, hp_mouse, x_normal_mouse, damage_mouse, 12, "mouse");

  } else {
    fight(y_big_mouse, hp_mouse, x_big_mouse, damage_mouse, 12, "mouse");
  }
}

void kop_rast::vstr_bear() {
  if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
    fight(y_small_bear, hp_bear, x_small_bear, damage_bear, 120, "bear");

  } else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
    fight(y_normal_bear, hp_bear, x_normal_bear, damage_bear, 120, "bear");

  } else {
    fight(y_big_bear, hp_bear, x_big_bear, damage_bear, 120, "bear");
  }
}
void kop_rast::vstr_olen() {

  if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
    fight(y_small_olen, hp_olen, x_small_olen, damage_olen, 80, "olen");

  } else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
    fight(y_normal_olen, hp_olen, x_normal_olen, damage_olen, 80, "olen");

  } else {
    fight(y_big_olen, hp_olen, x_big_olen, damage_olen, 80, "olen");
  }
} 
// уроны предметов  по существам
/*//топоры
int damage_stone_axe_for_animal = 15;
int damage_iron_axe_for_animal = 25;
// кирка
int damage_iron_pick_for_animal = 35;
// факел
int damage_fakel_for_animal = 10;
//волк
int damage_wolf = 10;
//медведь
int damage_bear = 15;
//олень
int damage_olen = 1;
//мышь
int damage_mouse = 5;*/
void kop_rast::menu() {
  load_json();
  std::cout << "v 5.5" << std::endl;

  if (name_user == "None") { std::cout << "enter name user~# "; update_coordinats(); std::getline(std::cin, name_user); }

  print_menu();
  update_coordinats();

  while (true) {
    if (for_exit_rust == true) { break; }
    regeneration();
    prov_instryments();
    random_ubdate_();
    prov_bron();
    prov_alive();

    std::getline(std::cin, cin_menu);

    if (cin_menu.find_first_not_of(" \t") == std::string::npos) { continue; }
    else if (cin_menu == "1")  { continue_enter(); print_info(); print_menu(); continue_enter(); }
    else if (cin_menu == "2" or cin_menu == "exit")  { continue_enter(); ubdate_json(); break; }
    else if (cin_menu == "3")  { continue_enter(); sleep(); print_menu(); continue_enter(); }
    else if (cin_menu == "4")  { continue_enter(); print(help_text); print_menu(); continue_enter(); }
    else if (cin_menu == "5")  { continue_enter(); craft(); print_menu(); continue_enter(); }
    else if (cin_menu == "6") { continue_enter(); donning_armor(); print_menu(); continue_enter(); }
    else if (cin_menu == "7") { continue_enter(); undonning_armor(); print_menu(); continue_enter(); }
    else if (cin_menu == "8") { continue_enter(); print_statisc_user(); print_menu(); continue_enter(); }
    else if (cin_menu == "9") { continue_enter(); print_statistic_axe(); print_menu(); continue_enter(); }
    else if (cin_menu == "10") { free_mode(); print_menu(); continue_enter(); }
    else if (cin_menu == "11") {continue_enter(); setting_user(); continue_enter(); }
    else if (cin_menu == "12") {continue_enter(); console_rust(); continue_enter();}
    else if (cin_menu == "13") {continue_enter(); min_res(); continue_enter();}
    else if (cin_menu == "14") {continue_enter(); max_min_pers(); continue_enter();}  
    else if (cin_menu == "15") {continue_enter(); ygaday_numbers(); continue_enter();}  
    else if (cin_menu == "16") {continue_enter(); print_menu(); continue_enter();}
    else if (cin_menu == "help")  { continue_enter(); print(help_text); print_menu(); continue_enter(); }
    else if (cin_menu == "clear" or cin_menu == "cls") { system("cls"); }
    else { continue_enter(); system("cls"); hp_minys(); std::cout << "error " << std::endl; print_menu(); continue_enter(); }

    ubdate_json();
  }
}