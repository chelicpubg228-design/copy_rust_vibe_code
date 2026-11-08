#include "../system_rust/copy_rust.h"


void kop_rast::user_change_v() {
    print("h - change hp");
    print("d - damage change");
    print("e - energy change");
    print("w - water change");
    print("f - food change");
    std::string cin_for_vbor_change;
    std::cin >> cin_for_vbor_change;

    if (cin_for_vbor_change == "h") {
        print("hahahahah if you hp < 250 - DEAD");
        print("okay, if your input < 250 - okay");
        std::cin >> hp_person;
        if (hp_person >= 1000) {
            print("BAN");
            remove("config_cop_rast.json");
        }
    }
    else if (cin_for_vbor_change == "d") {
        print("Look, bro, there are no restrictions here-though I could set some if I wanted to, or maybe not...");
        print("your input - yout damage STONE AXE for animals:");
        std::cin >> damage_stone_axe_for_animal;
        if (damage_stone_axe_for_animal >= 100) {
            print("BAN");
            remove("config_cop_rast.json");
            
        }
        
    }
    else if (cin_for_vbor_change == "e") {
        print("Why did you press E? Do you see any point to it? Neither do I.");
        print("Okay, input the energy.");
        std::cin >> energy;

    }
    else if (cin_for_vbor_change == "w") {
        print("I don't feel like writing much, so I'll just say: enter the amount of water:");
        std::cin >> water;
        
    }
    else if (cin_for_vbor_change == "f") {
        print("You know, for me to type out 'enter food,' I’d have to open my browser and go to a translator—so just figure out for yourself what you’re supposed to enter.");
        std::cin >> eda;
    }
}