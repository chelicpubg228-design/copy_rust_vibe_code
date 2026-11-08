#include "../system_rust/copy_rust.h"

void kop_rast::load_json() {
  try {
    std::ifstream iin("config_cop_rast.json");
    if (iin.is_open()) {
      jjson = "json konfig is save!";
      nlohmann::json konf = nlohmann::json::parse(iin);
      iron_pick = konf["pick_iron"];
      tree = konf["tree"];
      stown = konf["stone"];
      stown_topor = konf["axe"];
      berry = konf["barries"];
      water = konf["water"];
      hp_person = konf["hp"];
      eda = konf["eda"];
      energy = konf["energ"];
      voda_butka = konf["but_water"];
      sera = konf["sera"];
      metal = konf["iron"];
      iron_topor = konf["iron_axe"];
      hp_stown_topor = konf["hp_stone_axe"];
      hp_iron_exa = konf["hp_iron_axe"];
      hp_pick = konf["hp_iron_pick"];
      bone = konf["bone"];
      kosha = konf["kosha"];
      meet = konf["meet"];
      tkan = konf["tkan"];
      nitki = konf["nitki"];
      leather_helmet = konf["leather_helmet"];
      leather_bib = konf["leather_bib"];
      leather_leggings = konf["leather_leggings"];
      leather_boots = konf["leather_boots"];
      is_bool_leathing_bib = konf["bool_leathing_bib"];
      is_bool_leathing_boots = konf["bool_leathing_boots"];
      is_bool_leathing_leggings = konf["bool_leathing_leggints"];
      is_bool_leathing_helmet = konf["bool_leathing_helmet"];
      x = konf["x"];
      y = konf["y"];
      hp_max = konf["hp_max"];
      wins_r = konf["wins_rand"];
      name_user = konf["name_user"];

      if (hp_person >= 250) {
        std::cout << "dishonest player spotted! deleted config......"
                  << std::endl;
        std::remove("config_cop_rast.json");
        reset();
      }
    } else {
      print("konfig is none!");
      name_user = "None";
    }

  }

  catch (...) {
    std::cout << "error json regestration...." << std::endl;
    name_user = "None";
  }
}
void kop_rast::ubdate_json() {
  try {
    nlohmann::json konf{{"tree", tree},
                        {"stone", stown},
                        {"axe", stown_topor},
                        {"hp", hp_person},
                        {"eda", eda},
                        {"water", water},
                        {"barries", berry},
                        {"energ", energy},
                        {"but_water", voda_butka},
                        {"sera", sera},
                        {"iron", metal},
                        {"iron_axe", iron_topor},
                        {"pick_iron", iron_pick},
                        {"hp_stone_axe", hp_stown_topor},
                        {"hp_iron_axe", hp_iron_exa},
                        {"hp_iron_pick", hp_pick},
                        {"bone", bone},
                        {"kosha", kosha},
                        {"meet", meet},
                        {"tkan", tkan},
                        {"nitki", nitki},
                        {"leather_helmet", leather_helmet},
                        {"leather_bib", leather_bib},
                        {"leather_leggings", leather_leggings},
                        {"leather_boots", leather_boots},
                        {"bool_leathing_bib", is_bool_leathing_bib},
                        {"bool_leathing_boots", is_bool_leathing_boots},
                        {"bool_leathing_leggints", is_bool_leathing_leggings},
                        {"bool_leathing_helmet", is_bool_leathing_helmet},
                        {"name_user", name_user},
                        {"hp_max", hp_max},
                        {"x", x},
                        {"y", y},
                        {"wins_rand", wins_r}

    };
    std::ofstream outp("config_cop_rast.json");
    outp << konf;
    outp.close();
  } catch (...) {
    std::cout << "error update json" << std::endl;
  }
}
