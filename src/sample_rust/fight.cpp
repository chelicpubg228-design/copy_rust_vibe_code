#include "../system_rust/copy_rust.h"


#include <iso646.h>
void kop_rast::hp_minus_for_fight(int ddmage_for_def_fight, int& hp_bron_for_def_fight) {
	prov_bron();
	random_ubdate_();
	prov_instryments();
	hp_bron_for_def_fight -= ddmage_for_def_fight;


}
/*void leather_minus_for_def_fuction_fight_ok_avtomatisation_for_bear();
void leather_minus_for_def_fuction_fight_ok_avtomatisation_for_wolf();
void leather_minus_for_def_fuction_fight_ok_avtomatisation_for_mouse();
void leather_minus_for_def_fuction_fight_ok_avtomatisation_for_olen();*/
void kop_rast::leather_minus_for_def_fuction_fight_ok_avtomatisation_for_bear() {
	if (is_bool_leathing_bib == true) {
		hp_minus_for_fight(5, hp_leathing_bib);
	}
	if (is_bool_leathing_boots == true) {
		hp_minus_for_fight(1, hp_leating_boots);
	}
	if (is_bool_leathing_leggings == true) {
		hp_minus_for_fight(3, hp_leathing_leggings);
	}
	if (is_bool_leathing_helmet == true) {
		hp_minus_for_fight(2, hp_leathing_helmet);
	}
	

}

void kop_rast::leather_minus_for_def_fuction_fight_ok_avtomatisation_for_mouse() {
	if (is_bool_leathing_bib == true) {
		hp_minus_for_fight(1, hp_leathing_bib);
	}
	if (is_bool_leathing_boots == true) {
		hp_minus_for_fight(2, hp_leating_boots);
	}
	if (is_bool_leathing_leggings == true) {
		hp_minus_for_fight(1, hp_leathing_leggings);
	}
	if (is_bool_leathing_helmet == true) {
		hp_minus_for_fight(0, hp_leathing_helmet);
	}
}
void kop_rast::leather_minus_for_def_fuction_fight_ok_avtomatisation_for_olen() {
	if (is_bool_leathing_bib == true) {
		hp_minus_for_fight(4, hp_leathing_bib);
	}
	if (is_bool_leathing_boots == true) {
		hp_minus_for_fight(1, hp_leating_boots);
	}
	if (is_bool_leathing_leggings == true) {
		hp_minus_for_fight(2, hp_leathing_leggings);
	}
	if (is_bool_leathing_helmet == true) {
		hp_minus_for_fight(3, hp_leathing_helmet);
	}
}
void kop_rast::leather_minus_for_def_fuction_fight_ok_avtomatisation_for_wolf() {
	if (is_bool_leathing_bib == true) {
		hp_minus_for_fight(3, hp_leathing_bib);
	}
	if (is_bool_leathing_boots == true) {
		hp_minus_for_fight(3, hp_leating_boots);
	}
	if (is_bool_leathing_leggings == true) {
		hp_minus_for_fight(5, hp_leathing_leggings);
	}
	if (is_bool_leathing_helmet == true) {
		hp_minus_for_fight(1, hp_leathing_helmet);
	}
}
void kop_rast::fight(long long y_animal, int& hp_animal, long long  x_animal, int damage_animal, int reset_hp_animal, std::string name_animal) {
	prov_bron();
	random_ubdate_();
	prov_instryments();
	int cin_fight{ 0 }, cin_instr_fight{0};
	if (x_animal == x and y_animal == y) {
		std::cout << "you met " << name_animal << std::endl;
		if (name_animal == "mouse") {
			hp_person -= damage_animal;
			std::cout << "- " << damage_animal  << "hp" << std::endl;
			print("you decide to run away because the mouse cannot be killed because it is small");

			leather_minus_for_def_fuction_fight_ok_avtomatisation_for_mouse();
		}
		else if (name_animal == "bear") {
			proc_for_fight_all_animals("bear", damage_bear, 2, 3, 3, 2, hp_bear, hp_bear, 2, 5, 4, 1);
			
		}
		else if (name_animal == "wolf") {
			proc_for_fight_all_animals("wolf", damage_wolf, 2, 1, 2, 1, hp_wolf, hp_wolf, 1, 3, 4, 2);
			}
		else if (name_animal == "olen") {
			proc_for_fight_all_animals("olen", damage_olen, 2, 2, 3, 2, hp_olen, hp_olen, 2, 3, 1, 1);
			
			}
	}
	else {
		std::cout << "you didn't see an animal" << std::endl;
		water -= 4;
		eda -= 5;
		energy -= 10;
	}
}