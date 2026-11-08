#include "../system_rust/copy_rust.h"

void kop_rast::auto_minus_leathing(int damage_helmet, int damage_bib, int damage_leggings, int damage_boots) {
	if (is_bool_leathing_bib == true) {
		hp_minus_for_fight(damage_bib, hp_leathing_bib);
	}
	if (is_bool_leathing_boots == true) {
		hp_minus_for_fight(damage_boots, hp_leating_boots);
	}
	if (is_bool_leathing_leggings == true) {
		hp_minus_for_fight(damage_leggings, hp_leathing_leggings);
	}
	if (is_bool_leathing_helmet == true) {
		hp_minus_for_fight(damage_helmet, hp_leathing_helmet);
	}
}
void kop_rast::proc_for_fight_all_animals(
    std::string name_animal_fight,
     int damage_animal,
     int fight_bone,
     int fight_meet,
     int fight_tkan,
     int fight_kosha,
     int& hp_animal_fight,
     int reset_hp_animal_for_fight,
     int leathing_bib_minus_sample,
     int leathing_helmet_minus_sample,
     int leathing_boots_minus_sample,
     int leathing_leggints_minus_sample


) {
	system("cls");
    std::cout << "you met " << name_animal_fight << std::endl;
			print("select actions: 1 - run to base 2 - fight ");
            int cin_fight, cin_instr_fight;
			std::cin >> cin_fight;
			if (cin_fight == 1) {
				print("you ran to the base");
				energy -= 10;
				eda -= 10;
				water -= 5;


			}
			/*int damage_stone_axe_for_animal = 10;
			int damage_iron_axe_for_animal = 25;
			кирка
			int damage_iron_pick_for_animal = 30;
			факел
			int damage_fakel_for_animal = 5;*/
			else if (cin_fight == 2) {
				while (true) {
					print("choose the weapon you have");
					print("1 - stone axe");
					print("2 - iron axe ");
					print("3 - iron pick");
					print("4 - fakel");
					std::cin >> cin_instr_fight;

					if (cin_instr_fight == 1) {
						if (stown_topor <= 0) {
							std::cout << "no ax";
							break;
						}
						if (hp_animal_fight <= 0) {
							std::cout << "you win!" << std::endl;
							hp_animal_fight = reset_hp_animal_for_fight;
							break;
						}
						else if (hp_person <= 0) {
							std::cout << "you lose!" << std::endl;
							reset();
							hp_animal_fight = reset_hp_animal_for_fight;
							break;
						}
						std::cout << "you have dealt: " << damage_stone_axe_for_animal << " damage he has " << hp_animal_fight << " hp left" << std::endl;
						hp_animal_fight -= damage_stone_axe_for_animal;
						bone += fight_bone;
						meet += fight_meet;
						kosha += fight_kosha;
						tkan += fight_tkan;
						auto_minus_leathing(leathing_helmet_minus_sample, leathing_bib_minus_sample, leathing_leggints_minus_sample,leathing_boots_minus_sample);
						hp_stown_topor -= 2;
						std::cout << "you lost " << damage_animal << " hp, your hp: " << hp_person << std::endl;
						hp_person -= damage_animal;
					}
					else if (cin_instr_fight == 2) {
						if (iron_topor <= 0) {
							std::cout << "no ax" << std::endl;
							break;
						}
						if (hp_animal_fight <= 0) {
							std::cout << "you win!" << std::endl;
							hp_animal_fight = reset_hp_animal_for_fight;
							break;
						}
						else if (hp_person <= 0) {
							std::cout << "you lose!" << std::endl;
							hp_animal_fight = reset_hp_animal_for_fight;
							reset();
							break;
						}
						std::cout << "you have dealt: " << damage_iron_axe_for_animal << " damage he has " << hp_animal_fight << " hp left" << std::endl;
						hp_animal_fight -= damage_iron_axe_for_animal;
						bone += fight_bone + 1;
						meet += fight_meet + 1;
						kosha += fight_kosha + 1;
						tkan += fight_tkan + 1;
						auto_minus_leathing(leathing_helmet_minus_sample, leathing_bib_minus_sample, leathing_leggints_minus_sample,leathing_boots_minus_sample);
						

						hp_iron_exa -= 1;
						std::cout << "you lost " << damage_animal << " hp, your hp: " << hp_person << std::endl;
						hp_person -= damage_animal;
					}
					else if (cin_instr_fight == 3) {
						if (iron_pick <= 0) {
							print("no pick");
							break;
						}
						if (hp_animal_fight <= 0) {
							std::cout << "you win!" << std::endl;
							hp_animal_fight = reset_hp_animal_for_fight;
							break;
						}
						else if (hp_person <= 0) {
							std::cout << "you lose!" << std::endl;
							reset();
							hp_animal_fight = reset_hp_animal_for_fight;
							break;
						}
						std::cout << "you have dealt: " << damage_iron_pick_for_animal << " damage he has " << hp_animal_fight << " hp left" << std::endl;
						hp_animal_fight -= damage_iron_pick_for_animal;
					    bone += fight_bone + 2;
						meet += fight_meet + 2;
						kosha += fight_kosha + 2;
						tkan += fight_tkan + 2;
						auto_minus_leathing(leathing_helmet_minus_sample, leathing_bib_minus_sample, leathing_leggints_minus_sample,leathing_boots_minus_sample);
						

						hp_pick -= 1;
						std::cout << "you lost " << damage_animal << " hp, your hp: " << hp_person << std::endl;
						hp_person -= damage_animal;
					}
					else if (cin_instr_fight == 4) {
						if (fakel <= 0) {
							print("no fakel");
							break;
						}
						if (hp_animal_fight <= 0) {
							std::cout << "you win!" << std::endl;
							hp_animal_fight = reset_hp_animal_for_fight;
							break;
						}
						else if (hp_person <= 0) {
							std::cout << "you lose!" << std::endl;
							hp_animal_fight = reset_hp_animal_for_fight;
							reset();
							break;
						}
						std::cout << "you have dealt: " << damage_fakel_for_animal << " damage he has " << hp_animal_fight << " hp left" << std::endl;
						hp_animal_fight -= damage_fakel_for_animal;
						bone += fight_bone - 1;
						meet += fight_meet - 1;
						kosha += fight_kosha - 1;
						tkan += fight_tkan - 1;
						auto_minus_leathing(leathing_helmet_minus_sample, leathing_bib_minus_sample, leathing_leggints_minus_sample,leathing_boots_minus_sample);
						

						std::cout << "you lost " << damage_animal << " hp, your hp: " << hp_person << std::endl;
						hp_person -= damage_animal;
					}

				}
			}
			
			
		
}
