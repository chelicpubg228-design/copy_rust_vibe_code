#include "../system_rust/copy_rust.h"


#include <iso646.h>
void kop_rast::ate_berry() {
    if (berry >= 5) {
        berry -= 5;
        energy += 10;
        eda += 10;
        if (hp_person < 91) {
            hp_person += 10;
            std::cout << "+ 10 hp!" << std::endl;
        }
        system("cls");
        std::cout << "+ 10 food!" << std::endl;
        water += 10;
        std::cout << "+ 10 water!" << std::endl;

        std::cout << "+ 5 energy!" << std::endl;



    }
    else {
        system("cls");
        std::cout << "berry is note!" << std::endl;


    }
}

void kop_rast::locate_berry() {
     if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
    kop_rast::resurs_mine(berry, hp_berry, x_small_berries, y_small_berries, 0, hp_berry, " berry ", 3, &kop_rast::update_coordinats_berry);

    }
    else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
    kop_rast::resurs_mine(berry, hp_berry, x_normal_berries, y_normal_berries, 0, hp_berry, " berry ", 3, &kop_rast::update_coordinats_berry);


    }
    else {
    kop_rast::resurs_mine(berry, hp_berry, x_big_berries, y_big_berries, 0, hp_berry, " berry ", 3, &kop_rast::update_coordinats_berry);
   

    }
    

   

}
void kop_rast::locate_iron() {
     if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
        resurs_mine(metal, hp_iron, x_small_iron, y_small_iron, time_to_iron, hp_iron, " iron ", 2, &kop_rast::update_coordinats_iron);


    }
    else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
        resurs_mine(metal, hp_iron, x_normal_iron, y_normal_iron, time_to_iron, hp_iron, " iron ", 2, &kop_rast::update_coordinats_iron);
    

    }
    else {
        resurs_mine(metal, hp_iron, x_big_iron, y_big_iron, time_to_iron, hp_iron, " iron ", 2, &kop_rast::update_coordinats_iron);
    

    }

}
void kop_rast::locate_tkan() {
    if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
    kop_rast::resurs_mine(tkan, hp_tkan, x_small_tkan, y_small_tkan, time_tkan, hp_tkan, " tkan ", 3, &kop_rast::update_coordinats_tkan);

    }
    else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
    kop_rast::resurs_mine(tkan, hp_tkan, x_normal_tkan, y_normal_tkan, time_tkan, hp_tkan, " tkan ", 3, &kop_rast::update_coordinats_tkan);

    }
    else {
    kop_rast::resurs_mine(tkan, hp_tkan, x_big_tkan, y_big_tkan, time_tkan, hp_tkan, " tkan ", 3, &kop_rast::update_coordinats_tkan);

    }
    
}

 void kop_rast::rybka_tree_kamen_topor_big() {
    if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
  resurs_mine(tree, hp_tree_big, x_small_big_tree, y_small_big_tree, tree_time, hp_tree_big, "big tree ", 1, &kop_rast::update_coordinats_big_tree);
       
   

    }
    else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
  resurs_mine(tree, hp_tree_big, x_normal_big_tree, y_normal_big_tree, tree_time, hp_tree_big, "big tree ", 1, &kop_rast::update_coordinats_big_tree);
       
    


    }
    else {
  resurs_mine(tree, hp_tree_big, x_big_big_tree, y_big_big_tree, tree_time, hp_tree_big, "big tree ", 1, &kop_rast::update_coordinats_big_tree);
    
    
   

    }


 }
 void kop_rast::rybka_tree_kamen_topor_normal() {
       if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
  resurs_mine(tree, hp_tree_normal, x_small_normal_tree, y_small_normal_tree, tree_time, hp_tree_normal, "normal tree ", 1, &kop_rast::update_coordinats_normal_tree);
       
   

    }
    else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
        resurs_mine(tree, hp_tree_normal, x_normal_normal_tree, y_normal_normal_tree, tree_time, hp_tree_normal, "normal tree ", 1, &kop_rast::update_coordinats_normal_tree);
 
       
    


    }
    else {
        resurs_mine(tree, hp_tree_normal, x_big_normal_tree, y_big_normal_tree, tree_time, hp_tree_normal, "normal tree ", 1, &kop_rast::update_coordinats_normal_tree);

    
    
   

    }

 }

 void kop_rast::rybka_tree_kamen_topor_small() {
         if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
            resurs_mine(tree, hp_tree_small, x_small_small_tree, y_small_small_tree, tree_time, hp_tree_small, "small tree ", 1, &kop_rast::update_coordinats_small_tree);
       
   

    }
    else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
            resurs_mine(tree, hp_tree_small, x_normal_small_tree, y_normal_small_tree, tree_time, hp_tree_small, "small tree ", 1, &kop_rast::update_coordinats_small_tree);
        
 
       
    


    }
    else {
            resurs_mine(tree, hp_tree_small, x_big_small_tree, y_big_small_tree, tree_time, hp_tree_small, "small tree ", 1, &kop_rast::update_coordinats_small_tree);

 }
 }

 void kop_rast::locate_sera() {
    if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
        resurs_mine(sera, hp_sera, x_small_sulfur, y_small_sulfur, time_sera, hp_sera, "sera", 2, &kop_rast::update_coordinats_sulfrum);
   

    }
    else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
        resurs_mine(sera, hp_sera, x_normal_sulfur, y_normal_sulfur, time_sera, hp_sera, "sera", 2, &kop_rast::update_coordinats_sulfrum);
    


    }
    else {
        resurs_mine(sera, hp_sera, x_big_sulfur, y_big_sulfur, time_sera, hp_sera, "sera", 2, &kop_rast::update_coordinats_sulfrum);
    
   

    }

 }

 void kop_rast::stone_process() {
     if (x < 10000 and x > -10000 and y < 10000 and y > -10000) {
            resurs_mine(stown, hp_stown, x_small_stone, y_small_stone, shans_n_stoe, hp_stown, " stone ", 2, &kop_rast::update_coordinats_stone);
       
   

    }
    else if (x < 1000000 and x > -1000000 and y < 1000000 and y > -1000000) {
            resurs_mine(stown, hp_stown, x_normal_stone, y_normal_stone, shans_n_stoe, hp_stown, " stone ", 2, &kop_rast::update_coordinats_stone);
        
    


    }
    else {
            resurs_mine(stown, hp_stown, x_big_stone, y_big_stone, shans_n_stoe, hp_stown, " stone ", 2, &kop_rast::update_coordinats_stone);
        
    
   

    }



 }


