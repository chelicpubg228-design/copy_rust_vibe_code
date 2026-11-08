#include "../system_rust/copy_rust.h"


 void kop_rast::prov_instryment_stone_axe_hp() {
     if (hp_stown_topor <= 0) {
         stown_topor -= 1;
         std::cout << "topor_stown - 1" << std::endl;
         if (stown_topor >= 1) {
             hp_stown_topor = 100;

         }

     }
 }

 void kop_rast::prov_pick_iron_hp() {
     if (hp_pick <= 0) {
         iron_pick -= 1;
         std::cout << "- 1 pick iron" << std::endl;
         if (iron_pick >= 1) {
             hp_pick = 150;
         }

     }
 }
 void kop_rast::prov_iron_axe_hp() {
     if (hp_iron_exa <= 0) {
         iron_topor -= 1;
         std::cout << "iron_axe - 1 " << std::endl;
         if (iron_topor >= 1) {
             hp_iron_exa = 140;
         }

     }
 }

  void kop_rast::prov_instryments() {
     prov_pick_iron_hp();
     prov_instryment_stone_axe_hp();
     prov_iron_axe_hp();
 }