#include "../system_rust/copy_rust.h"
void kop_rast::craft() {
    print("======================craft menu========================");
    print(" 1 - stone axe (5 tree and 1 stone)                    ");
    print(" 2 - iron axe (5 tree and 5 iron)                      ");
    print(" 3 - leather helmet (3 tkan, 3 threads, 3 kosha)       ");
    print(" 4 - leather bib    (5 tkan, 6 threads, 10 kosha)      ");
    print(" 5 - leather leggings (4 tkan, 5 threads, 4 kosha)     ");
    print(" 6 - leather boots  (3 tkan, 3 threads, 5 kosha)       ");
    print(" 7 - threads (5 tkan)                                  ");
    print(" 8 - exit                                              ");
    print("======================craft menu========================");

    int cin_craft;
    if (!(std::cin >> cin_craft)) {                
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        print("error: bad input");
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (cin_craft == 1) {
        craft_axe_stone();
    }
    else if (cin_craft == 2) {
        craft_axe_iron();
    }
    else if (cin_craft == 3) {
        craft_helmet_leather();
    }
    else if (cin_craft == 4) {
        craft_bib_leather();
    }
    else if (cin_craft == 5) {
        craft_leggings_leather();
    }
    else if (cin_craft == 6) {
        craft_boots_leather();
    }
    else if (cin_craft == 7) {
        craft_threads();
    }
    else if (cin_craft == 8) {
        return;
    }
    else {
        print("error: unknown option");
    }
}
void kop_rast::craft_axe_iron() {
    if (tree >= 5 and metal >= 5) {
        tree -= 5;
        metal -= 5;
        iron_topor += 1;
        print("+ 1 iron axe");
    }
}
void kop_rast::craft_threads() {
    if (tkan >= 5) {
        tkan -= 5;
        nitki += 5;
        print("+ 5 threads");
        print("- 5 tkan");
        
    }
    else {
        std::cout << "error, no resours" << std::endl;
    }
}
void kop_rast::craft_helmet_leather() {
    if (tkan >= 3 and nitki >= 3 and kosha >= 3) {
        nitki -= 3;
        tkan -= 3;
        kosha -= 3;
        leather_helmet += 1;
        print(" nitki - 3");
        print("tkan - 3");
        print("kosha - 3");
        print("helmet leather + 1");

    }
    else {
        print("no resours");
    }
}
void kop_rast::craft_boots_leather() {
    if (tkan >= 3 and nitki >= 3 and kosha >= 5) {
        nitki -= 3;
        tkan -= 3;
        kosha -= 5;
        leather_boots += 1;
        print(" nitki - 3");
        print("tkan - 3");
        print("kosha - 5");
        print("helmet boots + 1");

    }
    else {
        print("no resours");
    }
}
void kop_rast::craft_bib_leather() {
    if (tkan >= 5 and nitki >= 6 and kosha >= 10) {
        nitki -= 6;
        tkan -= 5;
        kosha -= 10;
        leather_bib += 1;
        print(" nitki - 6");
        print("tkan - 5");
        print("kosha - 10");
        print("helmet bib + 1");

    }
    else {
        print("no resours");
    }
}
void kop_rast::craft_leggings_leather() {
    if (tkan >= 4 and nitki >= 5 and kosha >= 4) {
        nitki -= 5;
        tkan -= 4;
        kosha -= 4;
        leather_leggings += 1;
        print(" nitki - 5");
        print("tkan - 4");
        print("kosha - 4");
        print("helmet bib + 1");

    }
    else {
        print("no resours");
    }
}
void kop_rast::craft_axe_stone() {
    if (tree >= 5 and stown >= 1) {
        tree -= 5;
        stown -= 1;
        stown_topor += 1;
        print("good! +1 stone axe!");
    }
    else {
        print("resurs is none");

    }
}