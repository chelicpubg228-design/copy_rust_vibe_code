#include "../system_rust/copy_rust.h"
void kop_rast::ignore() {
    std::cin.ignore();
}
 void kop_rast::reset() {
     hp_person = 100;
     water = 100;
     eda = 100;
     energy = 100;
     bone = 0;
     meet = 0;
     kosha = 0;
     tree = 0;
     metal = 0;
     sera = 0;
     iron_pick = 0;
     iron_topor = 0;
     stown = 0;
     berry = 0;
     stown_topor = 1;
     fakel = 1;
     tkan = 0;
     leather_bib = 0;
     leather_boots = 0;
     leather_helmet = 0;
     leather_leggings = 0;
     hp_max = 100;
     is_bool_leathing_bib = false;
     is_bool_leathing_boots = false;
     is_bool_leathing_helmet = false;
     is_bool_leathing_leggings = false;

     x = 0;
     y = 0;


    }
void kop_rast::continue_enter() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore();
    std::cout << "\nloagin......." << std::endl;
}
 void kop_rast::hp_minys() {
     time(1);
     hp_person -= 10;
     std::cout << "-10 hp" << std::endl;
 }
 void kop_rast::regeneration() {
     if (water >= 70 and eda >= 70 and hp_person > 0) {

         if (hp_person < hp_max) {
             for (int i = 0; i < 10; i++) {
                 hp_person++;
                 eda -= 1;
                 water -= 1;
             }
         }
     }
 }

 void kop_rast::print_statisc_user() {
     std::cout << "HP: " << hp_person << std::endl;
     std::cout << "food: " << eda << std::endl;
     std::cout << "x: " << x << std::endl;
     std::cout << "y: " << y << std::endl;
     std::cout << "water: " << water << std::endl;
     std::cout << "energy: " << energy << std::endl;

 }

 void kop_rast::time(int time_cin) {
     std::this_thread::sleep_for(std::chrono::seconds(time_cin));

 }
 
 void kop_rast::sleep() {
     std::cout << "I am sleeping 10 second......." << std::endl;
     std::this_thread::sleep_for(std::chrono::seconds(10));
     energy += 75;
     system("cls");

     std::cout << "good! energy + 75" << std::endl;



 }
void kop_rast::prov_coord() {
    locate_berry();
    locate_tkan();
    locate_iron();
    locate_sera();
    stone_process();

    rybka_tree_kamen_topor_big();
    rybka_tree_kamen_topor_normal();
    rybka_tree_kamen_topor_small();

    vstr_wolf();
    vstr_mouse();
    vstr_bear();
    vstr_olen();
}
 void kop_rast::hunting() {
     print("1 - olen");
     print("2 - bear");
     print("3 - wolf");
     std::cin >> cin_hunting;
     if (cin_hunting == 1) {
         vstr_olen();
     }
     else if (cin_hunting == 2) {
         vstr_bear();
     }
     else if (cin_hunting == 3) {
         vstr_wolf();
     }
 }
