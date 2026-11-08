#include "copy_rust.h"

#include <iso646.h>
void kop_rast::print_info() {
    std::cout << "MVK: " << MVK << std::endl;
    std::cout << "stown: " << stown << std::endl;
    std::cout << "tree: " << tree << std::endl;
    std::cout << "byr: " << byr << std::endl;
    std::cout << "sera: " << sera << std::endl;
    std::cout << "fakel: " << fakel << std::endl;
    std::cout << "bone: " << bone << std::endl;
    std::cout << "berry: " << berry << std::endl;
    std::cout << "nitki: " << nitki << std::endl;
    std::cout << "skrap: " << skrap << std::endl;
    std::cout << "chainsaw: " << chainsaw << std::endl;
    std::cout << "meet: " << meet << std::endl;
    std::cout << "kosha: " << kosha << std::endl;
    std::cout << "iron_topor: " << iron_topor << std::endl;
    std::cout << "iron_pick: " << iron_pick << std::endl;
    std::cout << "Hp: " << hp_person << std::endl;
    std::cout << "eda: " << eda << std::endl;
    std::cout << "woter: " << water << std::endl;
    std::cout << "woter_butlka: " << voda_butka << std::endl;
    std::cout << "MVK_pick: " << MVk_pick << std::endl;
    std::cout << "MVK_topor: " << MVK_topor << std::endl;
    std::cout << "metal: " << metal << std::endl;
    std::cout << "skrap: " << skrap << std::endl;
    std::cout << "stown topor: " << stown_topor << std::endl;
    std::cout << "energy:" << energy << std::endl;
    std::cout << "leathing_bib:" << leather_bib << std::endl;
    std::cout << "leathing_boots:" << leather_boots << std::endl;
    std::cout << "leathing helmet:" << leather_helmet << std::endl;
    std::cout << "leathing legints:" << leather_leggings << std::endl;
    std::cout << "tkan:" << tkan << std::endl;
    

}

//системные  функции
void kop_rast::random_ubdate_() {
    //! рандомные числа для добычи дерева
    tree_rand_kamen_topor = rand() % 8 + 4;
    tree_rand_iron_topor = rand() % 15 + 7;
    tree_rand_MVK_topor = rand() % 20 + 11;
    tree_chainsaw = rand() % 40 + 22;
    //рандомное время находки дерева
    tree_time = rand() % 20 + 5;
    //шанс встретить большое дерево
    tree__big_shans = rand() % 6 + 1;
    //шанс втретить волка
    shans_wolf = rand() % 30 + 1;
    //шанс втретить мышь
    shans_mouse = rand() % 15 + 1;
    //шанс втретить медведя
    shans_bear = rand() % 40 + 1;
    //шанс втретить оленя
    shanc_olen = rand() % 10 + 1;
    shans_tkan = rand() % 10 + 1;
    time_tkan = rand() % 20 + 1;
  



    //шанс втретить среднее дерево
    tree_normal_shans = rand() % 7 + 1;
    shans_berry = rand() % 7 + 1;
    // время серы
    time_sera = rand() % 40 + 5;
    //шанс втретить маленькое дерево
    tree_small_shans = rand() % 8 + 1;
    time_normal_tree = rand() % 15 + 1;
    //шанс размера дерево
    tree_razmer_shans = rand() % 3 + 1;
    time_small_tree = rand() % 10 + 1;
    //шанс встретить руды
    stown_shans = rand() % 10 + 1;
    sera_shans = rand() % 20 + 1;
    metal_shans = rand() % 15 + 1;
    //время железа
    time_to_iron = rand() % 20 + 5;
    //шанс встретить бочку
    bochka_shans = rand() % 30 + 1;

    shans_n_stoe = rand() % 20 + 4;

    //! шанс встретить камень
    stown_shans = rand() % 10 + 1;

    //! рандомные числа для камня
    stown_stown_topor = rand() % 5 + 2;
    stown_iron_pick = rand() % 10 + 5;
    stown_MVK_pick = rand() % 20 + 15;
    stown_byr = rand() % 40 + 24;
}
void kop_rast::prov_alive() {
    if (hp_person <= 0 or eda <= 0 or water <= 0 or energy <= -100) {
        std::cout << "you dead!" << std::endl;
        reset();
        ubdate_json();
        print("resources reset game resumed");
    }
    else {
        std::cout << name_user << "~$ ";
    }


    
}
void kop_rast::print_statistic_axe() {
    std::cout << "stone axe: " << stown_topor << std::endl;
    std::cout << "iron axe: " << iron_topor << std::endl;
    std::cout << "stone axe hp: " << hp_stown_topor << std::endl;
    std::cout << "iron axe hp: " << hp_iron_exa << std::endl;


}
void kop_rast::undonning_armor() {
    print("1 - leathing bib");
    print("2 - leathing boots");
    print("3 - leathing helmet");
    print("4 - leathing leggings");

    int cin_armor;
    std::cin >> cin_armor;

    if (cin_armor == 1) {
        if (is_bool_leathing_bib) {
            if (leather_bib >= 1) {

            hp_person -= 20;
            hp_max -= 20;
            is_bool_leathing_bib = false;
        }
        else {
            std::cout << "error" << std::endl;
        }
    }
    }
    else if (cin_armor == 2) {
        if (is_bool_leathing_boots) {
            if (leather_boots >= 1) {
            hp_max -= 5;
            hp_person -= 5;
            is_bool_leathing_boots = false;  }
             else {
            std::cout << "error" << std::endl;
        }
        }
    }
    else if (cin_armor == 3) {
        if (is_bool_leathing_helmet) {
            if (leather_helmet >= 1) {

            
            is_bool_leathing_helmet = false;
            hp_max -= 10;
            hp_person -= 10; }
             else {
            std::cout << "error" << std::endl;
        }
        }
    }
    else if (cin_armor == 4) {
        if (is_bool_leathing_leggings) {
            if (leather_leggings >= 1) {
            is_bool_leathing_leggings = false;
            hp_max -= 15;
            hp_person -= 15; }
             else {
            std::cout << "error" << std::endl;
        }
        }
    }
    else {
        print("error");
    }
}
void kop_rast::donning_armor() {
    prov_bron();
    print("1 - leathing bib");
    print("2 - leathing boots");
    print("3 - leathing helmet");
    print("4 - leathing leggings");
    
    int cin_armor;
    std::cin >> cin_armor;

    if (cin_armor == 1) {
        if (leather_bib >= 1 && !is_bool_leathing_bib) {        
            hp_person += 20;
            hp_max += 20;
            is_bool_leathing_bib = true;
        }
    }
    else if (cin_armor == 2) {
        if (leather_boots >= 1 && !is_bool_leathing_boots) {
            hp_person += 5;
            hp_max += 5;
            is_bool_leathing_boots = true;
        }
    }
    else if (cin_armor == 3) {
        if (leather_helmet >= 1 && !is_bool_leathing_helmet) {
            hp_max += 10;
            hp_person += 10;
            is_bool_leathing_helmet = true;
        }
    }
    else if (cin_armor == 4) {
        if (leather_leggings >= 1 && !is_bool_leathing_leggings) {
            hp_max += 15;
            hp_person += 15;
            is_bool_leathing_leggings = true;
        }
    }
    else {
        print("error");
    }
}
//int hp_leathing_bib = 50;
//int hp_leathing_leggings = 20;
//int hp_leating_boots = 10;
//int hp_leathing_helmet = 25;

void kop_rast::vstr_wolf() {
  if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
    fight(y_small_wolf, hp_wolf, x_small_wolf, damage_wolf, 100, "wolf");

  } else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
    fight(y_normal_wolf, hp_wolf, x_normal_wolf, damage_wolf, 100, "wolf");

  } else {
    fight(y_big_wolf, hp_wolf, x_big_wolf, damage_wolf, 100, "wolf");
  }
}
void kop_rast::prov_bron() {
    if (hp_leathing_bib <= 0) {
        if (is_bool_leathing_bib) {
            hp_max -= 20;
            hp_person -= 20;
        }
        leather_bib -= 1;
        hp_leathing_bib = 50;
    }

    if (hp_leathing_leggings <= 0) {
        if (is_bool_leathing_leggings) {
            hp_person -= 15;
            hp_max -= 15;
        }
        leather_leggings -= 1;
        hp_leathing_leggings = 20;
    }

    if (hp_leating_boots <= 0) {
        if (is_bool_leathing_boots) {
            hp_person -= 5;
            hp_max -= 5;
        }
        leather_boots -= 1;
        hp_leating_boots = 10;
    }

    if (hp_leathing_helmet <= 0) {
        if (is_bool_leathing_helmet) {
            hp_person -= 10;
            hp_max -= 10;
        }
        leather_helmet -= 1;
        hp_leathing_helmet = 25;
    }
}

