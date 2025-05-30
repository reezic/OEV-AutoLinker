#include <fstream>
#include <iostream>
#include <filesystem>
#include "soh/OTRGlobals.h"
#include "randomizer/context.h"
#include "randomizer/entrance.h"
#include "randomizer/randomizer_entrance_tracker.h"
#include "game-interactor/GameInteractor_Hooks.h"

// Hook constants
#define CVAR_OEV_AUTOLINKER_NAME CVAR_ENHANCEMENT("OEVAutoLinker")
#define CVAR_OEV_AUTOLINKER_CONDITION true

EntranceOverride linkedEntrances[ENTRANCE_OVERRIDES_MAX_COUNT] = { 0 };

int32_t fileNumber = NULL;

// Struct for Obsidian template mappings
struct OEVEntranceData {
    std::string name;
    std::string tag;
    std::string color;
    std::string folder;
};

// For mapping the entrance names between entranceData and oevEntranceData
std::unordered_map<std::string, OEVEntranceData> entranceNameMap;

// Entrance metadata mapping for OEV
const OEVEntranceData oevEntranceData[] = {
    // Other
    { "Child Spawn", "#Spawn", "color_Spawn", "Spawn" },
    { "Adult Spawn", "#Spawn", "color_Spawn", "Spawn" },
    { "Minuet of Forest", "#OcarinaSong", "color_OcarinaSong", "Ocarina Songs" },
    { "Bolero of Fire", "#OcarinaSong", "color_OcarinaSong", "Ocarina Songs" },
    { "Serenade of Water", "#OcarinaSong", "color_OcarinaSong", "Ocarina Songs" },
    { "Requiem of Spirit", "#OcarinaSong", "color_OcarinaSong", "Ocarina Songs" },
    { "Nocturne of Shadow", "#OcarinaSong", "color_OcarinaSong", "Ocarina Songs" },
    { "Prelude of Light", "#OcarinaSong", "color_OcarinaSong", "Ocarina Songs" },

    // Kokiri Forest
    { "Kokiri Forest Lower Exit", "#KokiriForest", "color_KokiriForest", "Kokiri Forest" },
    { "Kokiri Forest Upper Exit", "#KokiriForest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Link's House Entry", "#KokiriForest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Mido's House Entry", "#KokiriForest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Saria's House Entry", "#KokiriForest", "color_KokiriForest", "Kokiri Forest" },
    { "KF House of Twins Entry", "#KokiriForest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Know-It-All House Entry", "#KokiriForest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Shop Entry", "#KokiriForest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Storms Grotto Entry", "#KokiriForest", "color_Grotto", "Kokiri Forest" },
    { "KF Outside Deku Tree", "#KokiriForest", "color_KokiriForest", "Kokiri Forest" },
    { "Link's House", "#LinksHouse", "color_KokiriForest", "Kokiri Forest" },
    { "Mido's House", "#MidosHouse", "color_KokiriForest", "Kokiri Forest" },
    { "Saria's House", "#SariasHouse", "color_KokiriForest", "Kokiri Forest" },
    { "House of Twins", "#HouseofTwins", "color_KokiriForest", "Kokiri Forest" },
    { "Know-It-All House", "#KnowItAllHouse", "color_KokiriForest", "Kokiri Forest" },
    { "Kokiri Shop", "#KokiriShop", "color_KokiriForest", "Kokiri Forest" },
    { "KF Storms Grotto", "#KFStormsGrotto", "color_Grotto", "Kokiri Forest" },
    { "Deku Tree Entrance", "#DekuTree", "color_DekuTree", "Kokiri Forest" },
    { "Deku Tree Boss Door", "#DekuTree", "color_DekuTree", "Kokiri Forest" },
    { "Gohma", "#Gohma", "color_DekuTree", "Kokiri Forest" },
    { "Gohma Blue Warp", "#Gohma", "color_DekuTree", "Kokiri Forest" },

    // Lost Woods
    { "Lost Woods Bridge East Exit", "#LostWoods", "color_KokiriForest", "Lost Woods" },
    { "Lost Woods Bridge West Exit", "#LostWoods", "color_KokiriForest", "Lost Woods" },
    { "Lost Woods South Exit", "#LostWoods", "color_KokiriForest", "Lost Woods" },
    { "Lost Woods Tunnel Shortcut", "#LostWoods", "color_KokiriForest", "Lost Woods" },
    { "Lost Woods Underwater Shortcut", "#LostWoods", "color_KokiriForest", "Lost Woods" },
    { "Lost Woods North Exit", "#LostWoods", "color_KokiriForest", "Lost Woods" },
    { "LW Tunnel Grotto Entry", "#LostWoods", "color_KokiriForest", "Lost Woods" },
    { "LW North Grotto Entry", "#LostWoods", "color_Grotto", "Lost Woods" },
    { "LW Meadow Grotto Entry", "#LostWoods", "color_Grotto", "Lost Woods" },
    { "LW Tunnel Grotto", "#LWTunnelGrotto", "color_Grotto", "Lost Woods" },
    { "LW Scrubs Grotto", "#LWScrubsGrotto", "color_Grotto", "Lost Woods" },
    { "Deku Theater", "#DekuTheater", "color_Grotto", "Lost Woods" },

    // Sacred Forest Meadow
    { "Sacred Forest Meadow South Exit", "#SacredForestMeadow", "color_KokiriForest", "Sacred Forest Meadow" },
    { "SFM Wolfos Grotto Entry", "#SacredForestMeadow", "color_Grotto", "Sacred Forest Meadow" },
    { "SFM Fairy Grotto Entry", "#SacredForestMeadow", "color_Grotto", "Sacred Forest Meadow" },
    { "SFM Storms Grotto Entry", "#SacredForestMeadow", "color_Grotto", "Sacred Forest Meadow" },
    { "Sacred Forest Meadow Outside Forest Temple", "#SacredForestMeadow", "color_KokiriForest",
      "Sacred Forest Meadow" },
    { "SFM Wolfos Grotto", "#SFMWolfosGrotto", "color_Grotto", "Sacred Forest Meadow" },
    { "SFM Fairy Grotto", "#SFMFairyGrotto", "color_Grotto", "Sacred Forest Meadow" },
    { "SFM Deku Scrub Grotto", "#SFMDekuScrubGrotto", "color_KokiriForest", "Sacred Forest Meadow" },
    { "Forest Temple Entrance", "#ForestTemple", "color_ForestTemple", "Sacred Forest Meadow" },
    { "Forest Temple Boss Door", "#ForestTemple", "color_ForestTemple", "Sacred Forest Meadow" },
    { "Phantom Ganon", "#PhantomGanon", "color_ForestTemple", "Sacred Forest Meadow" },
    { "Phantom Ganon Blue Warp", "#PhantomGanon", "color_ForestTemple", "Sacred Forest Meadow" },

    // Kakariko Village
    { "Kakariko Front Gate", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kakariko Southeast Exit", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kakariko Guard Gate Exit", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Boss House Entry", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Skulltula House Entry", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Impa's House Front Entry", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Impa's House Back Entry", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Windmill Entry", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Shooting Gallery Entry", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Granny's Potion Shop Entry", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Bazaar Entry", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Potion Shop Front Entry", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Potion Shop Back Entry", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Open Grotto Entry", "#KakarikoVillage", "color_Grotto", "Kakariko Village" },
    { "Kak Center Grotto Entry", "#KakarikoVillage", "color_Grotto", "Kakariko Village" },
    { "Kakariko Outside the Well", "#KakarikoVillage", "color_KakarikoVillage", "Kakariko Village" },
    { "Carpenter Boss House", "#CarpenterBossHouse", "color_KakarikoVillage", "Kakariko Village" },
    { "House of Skulltula", "#HouseofSkulltula", "color_KakarikoVillage", "Kakariko Village" },
    { "Impa's House Front", "#ImpasHouseFront", "color_KakarikoVillage", "Kakariko Village" },
    { "Impa's House Back", "#ImpasHouseBack", "color_KakarikoVillage", "Kakariko Village" },
    { "Windmill", "#Windmill", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Shooting Gallery", "#KakShootingGallery", "color_KakarikoVillage", "Kakariko Village" },
    { "Granny's Potion Shop", "#GrannysPotionShop", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Bazaar", "#KakBazaar", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Potion Shop Front", "#KakPotionShopFront", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Potion Shop Back", "#KakPotionShopBack", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Open Grotto", "#KakOpenGrotto", "color_Grotto", "Kakariko Village" },
    { "Kak Redead Grotto", "#KakRedeadGrotto", "color_Grotto", "Kakariko Village" },
    { "Bottom of the Well Entrance", "#BottomoftheWell", "color_BottomoftheWell", "Kakariko Village" },

    // The Graveyard
    { "Graveyard Entrance", "#Graveyard", "color_KakarikoVillage", "Graveyard" },
    { "GY Dampe's Hut Entry", "#Graveyard", "color_KakarikoVillage", "Graveyard" },
    { "GY Near-Hut Grave Entry", "#Graveyard", "color_Grotto", "Graveyard" },
    { "GY Near-Tomb Grave Entry", "#Graveyard", "color_Grotto", "Graveyard" },
    { "GY Royal Family's Tomb Entry", "#Graveyard", "color_Grotto", "Graveyard" },
    { "GY Near-Ledge Grave Entry", "#Graveyard", "color_Grotto", "Graveyard" },
    { "Graveyard Outside Temple", "#Graveyard", "color_KakarikoVillage", "Graveyard" },
    { "Dampe's Hut", "#DampesHut", "color_KakarikoVillage", "Graveyard" },
    { "Shield Grave", "#ShieldGrave", "color_Grotto", "Graveyard" },
    { "Heart Piece Grave", "#HeartPieceGrave", "color_Grotto", "Graveyard" },
    { "Royal Family's Tomb", "#RoyalFamilysTomb", "color_Grotto", "Graveyard" },
    { "Dampe's Grave", "#DampesGrave", "color_Grotto", "Graveyard" },
    { "Shadow Temple Entrance", "#ShadowTemple", "color_ShadowTemple", "Graveyard" },
    { "Shadow Temple Boss Door", "#ShadowTemple", "color_ShadowTemple", "Graveyard" },
    { "Bongo-Bongo", "#BongoBongo", "color_ShadowTemple", "Graveyard" },
    { "Bongo-Bongo Blue Warp", "#BongoBongo", "color_ShadowTemple", "Graveyard" },

    // Death Mountain Trail
    { "DMT Owl Flight", "#DeathMountainTrail", "color_DeathMountain", "Death Mountain Trail" },
    { "Death Mountain Trail Middle Exit", "#DeathMountainTrail", "color_DeathMountain", "Death Mountain Trail" },
    { "Death Mountain Trail Bottom Exit", "#DeathMountainTrail", "color_DeathMountain", "Death Mountain Trail" },
    { "Death Mountain Trail Top Exit", "#DeathMountainTrail", "color_DeathMountain", "Death Mountain Trail" },
    { "DMT Great Fairy Entry", "#DeathMountainTrail", "color_Grotto", "Death Mountain Trail" },
    { "DMT Rock Circle Grotto Entry", "#DeathMountainTrail", "color_Grotto", "Death Mountain Trail" },
    { "DMT Boulder Grotto Entry", "#DeathMountainTrail", "color_Grotto", "Death Mountain Trail" },
    { "Death Mountain Trail Outside Dodongo's Cavern", "#DeathMountainTrail", "color_DeathMountain",
      "Death Mountain Trail" },
    { "DMT Great Fairy Fountain", "#DMTGreatFairyFountain", "color_Grotto", "Death Mountain Trail" },
    { "DMT Storms Grotto", "#DMTStormsGrotto", "color_Grotto", "Death Mountain Trail" },
    { "DMT Cow Grotto", "#DMTCowGrotto", "color_Grotto", "Death Mountain Trail" },
    { "Dodongo's Cavern Entrance", "#DodongosCavern", "color_DodongosCavern", "Death Mountain Trail" },
    { "Dodongo's Cavern Boss Door", "#DodongosCavern", "color_DodongosCavern", "Death Mountain Trail" },
    { "King Dodongo", "#KingDodongo", "color_DodongosCavern", "Death Mountain Trail" },
    { "King Dodongo Blue Warp", "#KingDodongo", "color_DodongosCavern", "Death Mountain Trail" },

    // Death Mountain Crater
    { "Death Mountain Crater Bridge Exit", "#DeathMountainCrater", "color_DeathMountain", "Death Mountain Crater" },
    { "Death Mountain Crater Upper Exit", "#DeathMountainCrater", "color_DeathMountain", "Death Mountain Crater" },
    { "DMC Great Fairy Entry", "#DeathMountainCrater", "color_Grotto", "Death Mountain Crater" },
    { "DMC Upper Grotto Entry", "#DeathMountainCrater", "color_Grotto", "Death Mountain Crater" },
    { "DMC Hammer Grotto Entry", "#DeathMountainCrater", "color_Grotto", "Death Mountain Crater" },
    { "Death Mountain Crater Outside Temple", "#DeathMountainCrater", "color_DeathMountain", "Death Mountain Crater" },
    { "DMC Great Fairy Fountain", "#DMCGreatFairyFountain", "color_Grotto", "Death Mountain Crater" },
    { "DMC Upper Grotto", "#DMCUpperGrotto", "color_Grotto", "Death Mountain Crater" },
    { "DMC Scrubs Grotto", "#DMCScrubsGrotto", "color_Grotto", "Death Mountain Crater" },
    { "Fire Temple Entrance", "#FireTemple", "color_FireTemple", "Death Mountain Crater" },
    { "Fire Temple Boss Door", "#FireTemple", "color_FireTemple", "Death Mountain Crater" },
    { "Volvagia", "#Volvagia", "color_FireTemple", "Death Mountain Crater" },
    { "Volvagia Blue Warp", "#Volvagia", "color_FireTemple", "Death Mountain Crater" },

    // Goron City
    { "Goron City Upper Exit", "#GoronCity", "color_GoronCity", "Goron City" },
    { "Goron City Darunia's Room Backdoor", "#GoronCity", "color_GoronCity", "Goron City" },
    { "Goron City Tunnel Shortcut", "#GoronCity", "color_GoronCity", "Goron City" },
    { "GC Shop Entry", "#GoronCity", "color_GoronCity", "Goron City" },
    { "GC Lava Grotto Entry", "#GoronCity", "color_Grotto", "Goron City" },
    { "Goron Shop", "#GoronCity", "color_GoronCity", "Goron City" },
    { "GC Scrubs Grotto", "#GCScrubsGrotto", "color_Grotto", "Goron City" },

    // Zora's River
    { "Zora's River Lower Exit", "#ZorasRiver", "color_ZorasRiver", "Zora's River" },
    { "Zora's River Underwater Shortcut", "#ZorasRiver", "color_ZorasRiver", "Zora's River" },
    { "Zora's River Waterfall Exit", "#ZorasRiver", "color_ZorasRiver", "Zora's River" },
    { "ZR Rock Circle Grotto Entry", "#ZorasRiver", "color_Grotto", "Zora's River" },
    { "ZR Raised Boulder Grotto Entry", "#ZorasRiver", "color_Grotto", "Zora's River" },
    { "ZR Raised Open Grotto Entry", "#ZorasRiver", "color_Grotto", "Zora's River" },
    { "ZR Deku Scrub Grotto", "#ZRDekuScrubGrotto", "color_Grotto", "Zora's River" },
    { "ZR Fairy Grotto", "#ZRFairyGrotto", "color_Grotto", "Zora's River" },
    { "ZR Open Grotto", "#ZROpenGrotto", "color_Grotto", "Zora's River" },

    // Zora's Domain
    { "Zora's Domain Entrance", "#ZorasDomain", "color_ZorasDomain", "Zora's Domain" },
    { "Zora's Domain Underwater Shortcut", "#ZorasDomain", "color_ZorasDomain", "Zora's Domain" },
    { "Zora's Domain Behind King Zora", "#ZorasDomain", "color_ZorasDomain", "Zora's Domain" },
    { "ZD Shop Entry", "#ZorasDomain", "color_ZorasDomain", "Zora's Domain" },
    { "ZD Island Grotto Entry", "#ZorasDomain", "color_Grotto", "Zora's Domain" },
    { "Zora Shop", "#ZorasDomain", "color_ZorasDomain", "Zora's Domain" },
    { "ZD Fairy Grotto", "#ZDFairyGrotto", "color_Grotto", "Zora's Domain" },

    // Zora's Fountain
    { "Zora's Fountain Tunnel Exit", "#ZorasFountain", "color_ZorasFountain", "Zora's Fountain" },
    { "ZF Great Fairy Entry", "#ZorasFountain", "color_Grotto", "Zora's Fountain" },
    { "Zora's Fountain Outside Jabu Jabu", "#ZorasFountain", "color_ZorasFountain", "Zora's Fountain" },
    { "Zora's Fountain Outside Ice Cavern", "#ZorasFountain", "color_ZorasFountain", "Zora's Fountain" },
    { "ZF Great Fairy Fountain", "#ZFGreatFairyFountain", "color_Grotto", "Zora's Fountain" },
    { "Jabu Jabu's Belly Entrance", "#JabuJabu", "color_JabuJabu", "Zora's Fountain" },
    { "Jabu Jabu's Belly Boss Door", "#JabuJabu", "color_JabuJabu", "Zora's Fountain" },
    { "Barinade", "#Barinade", "color_JabuJabu", "Zora's Fountain" },
    { "Barinade Blue Warp", "#Barinade", "color_JabuJabu", "Zora's Fountain" },
    { "Ice Cavern Entrance", "#IceCavern", "color_IceCavern", "Zora's Fountain" },

    // Hyrule Field
    { "Hyrule Field Wooded Exit", "#HyruleField", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Drawbridge Exit", "#HyruleField", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Center Exit", "#HyruleField", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Stairs Exit", "#HyruleField", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field River Exit", "#HyruleField", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Fence Exit", "#HyruleField", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Rocky Path", "#HyruleField", "color_HyruleField", "Hyrule Field" },
    { "HF Near Market Boulder Grotto Entry", "#HyruleField", "color_Grotto", "Hyrule Field" },
    { "HF Stone Bridge Tree Grotto Entry", "#HyruleField", "color_Grotto", "Hyrule Field" },
    { "HF Northwest Tree Grotto Entry", "#HyruleField", "color_Grotto", "Hyrule Field" },
    { "HF Northwest Boulder Grotto Entry", "#HyruleField", "color_Grotto", "Hyrule Field" },
    { "HF West Rock Circle Grotto Entry", "#HyruleField", "color_Grotto", "Hyrule Field" },
    { "HF South Open Grotto Entry", "#HyruleField", "color_Grotto", "Hyrule Field" },
    { "HF Fenced Grotto Entry", "#HyruleField", "color_Grotto", "Hyrule Field" },
    { "HF Southeast Boulder Grotto Entry", "#HyruleField", "color_Grotto", "Hyrule Field" },
    { "HF Near Market Boulder Grotto", "#HFNearMarketBoulderGrotto", "color_Grotto", "Hyrule Field" },
    { "HF Stone Bridge Tree Grotto", "#HFStoneBridgeTreeGrotto", "color_Grotto", "Hyrule Field" },
    { "HF Tektite Grotto", "#HFTektiteGrotto", "color_Grotto", "Hyrule Field" },
    { "HF Fairy Grotto", "#HFFairyGrotto", "color_Grotto", "Hyrule Field" },
    { "HF Cow Grotto", "#HFCowGrotto", "color_Grotto", "Hyrule Field" },
    { "HF Open Grotto", "#HFOpenGrotto", "color_Grotto", "Hyrule Field" },
    { "HF Fenced Scrub Grotto", "#HFFencedScrubGrotto", "color_Grotto", "Hyrule Field" },
    { "HF Southeast Grotto", "#HFSoutheastGrotto", "color_Grotto", "Hyrule Field" },

    // Lon Lon Ranch
    { "Lon Lon Ranch Entrance", "#LonLonRanch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Talon's House Entry", "#LonLonRanch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Stables Entry", "#LonLonRanch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Tower Entry", "#LonLonRanch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Grotto Entry", "#LonLonRanch", "color_Grotto", "Lon Lon Ranch" },
    { "Talon's House", "#LonLonRanch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Stables", "#LonLonRanch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Tower", "#LonLonRanch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Deku Scrub Grotto", "#LLRDekuScrubGrotto", "color_Grotto", "Lon Lon Ranch" },

    // Lake Hylia
    { "LH Owl Flight", "#LakeHylia", "color_LakeHylia", "Lake Hylia" },
    { "Lake Hylia North Exit", "#LakeHylia", "color_LakeHylia", "Lake Hylia" },
    { "Lake Hylia Underwater Shortcut", "#LakeHylia", "color_LakeHylia", "Lake Hylia" },
    { "LH Lab Entry", "#LakeHylia", "color_LakeHylia", "Lake Hylia" },
    { "LH Fishing Pond Entry", "#LakeHylia", "color_LakeHylia", "Lake Hylia" },
    { "LH Grave Grotto Entry", "#LakeHylia", "color_Grotto", "Lake Hylia" },
    { "Lake Hylia Outside Temple", "#LakeHylia", "color_LakeHylia", "Lake Hylia" },
    { "LH Lab", "#LakeHylia", "color_LakeHylia", "Lake Hylia" },
    { "Fishing Pond", "#LakeHylia", "color_LakeHylia", "Lake Hylia" },
    { "LH Deku Scrub Grotto", "#LHDekuScrubGrotto", "color_Grotto", "Lake Hylia" },
    { "Water Temple Entrance", "#WaterTemple", "color_WaterTemple", "Lake Hylia" },
    { "Water Temple Boss Door", "#WaterTemple", "color_WaterTemple", "Lake Hylia" },
    { "Morpha", "#Morpha", "color_WaterTemple", "Lake Hylia" },
    { "Morpha Blue Warp", "#Morpha", "color_WaterTemple", "Lake Hylia" },

    // Gerudo Area
    { "Gerudo Valley East Exit", "#GerudoValley", "color_GerudoValley", "Gerudo Valley" },
    { "Gerudo Valley West Exit", "#GerudoValley", "color_GerudoValley", "Gerudo Valley" },
    { "Gerudo Valley River Exit", "#GerudoValley", "color_GerudoValley", "Gerudo Valley" },
    { "GV Carpenters' Tent Entry", "#GerudoValley", "color_GerudoValley", "Gerudo Valley" },
    { "GV Silver Rock Grotto Entry", "#GerudoValley", "color_Grotto", "Gerudo Valley" },
    { "GV Behind Tent Grotto Entry", "#GerudoValley", "color_Grotto", "Gerudo Valley" },
    { "Carpenters' Tent", "#GerudoValley", "color_GerudoValley", "Gerudo Valley" },
    { "GV Octorok Grotto", "#GVOctorokGrotto", "color_Grotto", "Gerudo Valley" },
    { "GV Deku Scrub Grotto", "#GVDekuScrubGrotto", "color_Grotto", "Gerudo Valley" },
    { "Gerudo Fortress East Exit", "#GerudoValley", "color_GerudoValley", "Gerudo Valley" },
    { "Gerudo Fortress Gate Exit", "#GerudoValley", "color_GerudoValley", "Gerudo Valley" },
    { "GF Storms Grotto Entry", "#GerudoValley", "color_Grotto", "Gerudo Valley" },
    { "GF Outside Training Ground", "#GerudoValley", "color_GerudoValley", "Gerudo Valley" },
    { "GF Fairy Grotto", "#GFFairyGrotto", "color_Grotto", "Gerudo Valley" },
    { "Gerudo Training Ground Entrance", "#GerudoTrainingGround", "color_GerudoTrainingGround", "Gerudo Valley" },

    // The Wasteland
    { "Haunted Wasteland East Exit", "#HauntedWasteland", "color_HauntedWasteland", "Haunted Wasteland" },
    { "Haunted Wasteland West Exit", "#HauntedWasteland", "color_HauntedWasteland", "Haunted Wasteland" },

    // Desert Colossus
    { "Desert Colossus East Exit", "#DesertColossus", "color_DesertColossus", "Desert Colossus" },
    { "Colossus Great Fairy Entry", "#DesertColossus", "color_Grotto", "Desert Colossus" },
    { "Colossus Grotto Entry", "#DesertColossus", "color_Grotto", "Desert Colossus" },
    { "Colossus Outside Temple", "#DesertColossus", "color_DesertColossus", "Desert Colossus" },
    { "Colossus Great Fairy Fountain", "#ColossusGreatFairyFountain", "color_Grotto", "Desert Colossus" },
    { "Colossus Deku Scrub Grotto", "#ColossusDekuScrubGrotto", "color_Grotto", "Desert Colossus" },
    { "Spirit Temple Entrance", "#SpiritTemple", "color_SpiritTemple", "Desert Colossus" },
    { "Spirit Temple Boss Door", "#SpiritTemple", "color_SpiritTemple", "Desert Colossus" },
    { "Twinrova", "#Twinrova", "color_SpiritTemple", "Desert Colossus" },
    { "Twinrova Blue Warp", "#Twinrova", "color_SpiritTemple", "Desert Colossus" },

    // Market
    { "Market Entrance South Exit", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "Market Entrance North Exit", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "MK Entrance Guard House Entry", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "Market South Exit", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "Market Castle Exit", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "Market Temple Exit", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "MK Shooting Gallery Entry", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "MK Bombchu Bowling Entry", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "MK Treasure Chest Game Entry", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "MK Man-in-Green House Entry", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "MK Mask Shop Entry", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "MK Bazaar Entry", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "MK Potion Shop Entry", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "MK Bombchu Shop Entry", "#HyruleMarket", "color_HyruleMarket", "Hyrule Market" },
    { "Guard House", "#GuardHouse", "color_HyruleMarket", "Hyrule Market" },
    { "MK Shooting Gallery", "#MKShootingGallery", "color_HyruleMarket", "Hyrule Market" },
    { "Bombchu Bowling", "#BombchuBowling", "color_HyruleMarket", "Hyrule Market" },
    { "Treasure Chest Game", "#TreasureChestGame", "color_HyruleMarket", "Hyrule Market" },
    { "Man-in-Green's House", "#ManinGreensHouse", "color_HyruleMarket", "Hyrule Market" },
    { "Mask Shop", "#MaskShop", "color_HyruleMarket", "Hyrule Market" },
    { "MK Bazaar", "#MKBazaar", "color_HyruleMarket", "Hyrule Market" },
    { "MK Potion Shop", "#MKPotionShop", "color_HyruleMarket", "Hyrule Market" },
    { "Bombchu Shop", "#BombchuShop", "color_HyruleMarket", "Hyrule Market" },
    { "ToT Courtyard Gossip Stones Exit", "#TempleOfTimeCourtyard", "color_TempleofTime", "Hyrule Market" },
    { "ToT Courtyard Temple Entry", "#TempleOfTimeCourtyard", "color_TempleofTime", "Hyrule Market" },
    { "Temple of Time Entrance", "#TempleOfTime", "color_TempleofTime", "Hyrule Market" },

    // Hyrule Castle
    { "Castle Grounds South Exit", "#HyruleCastle", "color_HyruleCastle", "Hyrule Castle" },
    { "HC Boulder Crawlspace", "#HyruleCastle", "color_Grotto", "Hyrule Castle" },
    { "HC Storms Grotto Entry", "#HyruleCastle", "color_Grotto", "Hyrule Castle" },
    { "HC Great Fairy Fountain", "#HCGreatFairyFountain", "color_Grotto", "Hyrule Castle" },
    { "HC Storms Grotto", "#HCStormsGrotto", "color_Grotto", "Hyrule Castle" },
    { "OGC Behind Pillar", "#HyruleCastle", "color_HyruleCastle", "Hyrule Castle" },
    { "OGC Rainbow Bridge Exit", "#HyruleCastle", "color_HyruleCastle", "Hyrule Castle" },
    { "OGC Great Fairy Fountain", "#OGCGreatFairyFountain", "color_Grotto", "Hyrule Castle" },
    { "Inside Ganon's Castle Entrance", "#GanonsCastle", "color_GanonsCastle", "Hyrule Castle" }
};

// Lambda function for finding a name in oevEntranceData
auto oevEntranceNames = [](const std::string& name) -> OEVEntranceData {
    for (const OEVEntranceData data : oevEntranceData) {
        if (data.name == name)
            return data;
    }
    return {};
};

// Create the obsidian markdown file for a given entrance
void CreateObsidianFile(const std::filesystem::path path, const OEVEntranceData& data, const std::string& linksTo) {
    // Create folder path if it doesn't exist
    std::filesystem::create_directories(path.parent_path());

    // Write to the new markdown file
    std::ofstream outFile(path);
    outFile << "Links to: [[" << linksTo << "]]\n";
    outFile << "Tag: " << data.tag << "\n";
    outFile << "Color: " << data.color << "\n";
    outFile.close();
}

// Link two entrances together by creating markdown files in the OEV folder
void LinkEntrances(const int32_t& fileNumber, const std::string& fromName, const std::string& toName) {
    // Check if the entrance names are valid
    if (entranceNameMap.find(toName) == entranceNameMap.end() ||
        entranceNameMap.find(fromName) == entranceNameMap.end()) {
        std::cerr << "One of the names is missing from the entrance name mapping.\n";
        return;
    }

    // Create the folder structure
    const auto& toData = entranceNameMap[toName];
    const auto& fromData = entranceNameMap[fromName];

    const std::filesystem::path toFilePath =
        std::filesystem::path(Ship::Context::GetPathRelativeToAppDirectory("Obsidian Entrance Visualizer")) /
        ("Save " + std::to_string(fileNumber)) / toData.folder / (toData.name + ".md");

    const std::filesystem::path fromFilePath =
        std::filesystem::path(Ship::Context::GetPathRelativeToAppDirectory("Obsidian Entrance Visualizer")) /
        ("Save " + std::to_string(fileNumber)) / fromData.folder / (fromData.name + ".md");

    // Check and create files if they don't exist
    if (!std::filesystem::exists(fromFilePath)) {
        CreateObsidianFile(fromFilePath, fromData, toData.name);
    }

    // Check and create files if they don't exist
    if (!std::filesystem::exists(toFilePath)) {
        CreateObsidianFile(toFilePath, toData, fromData.name);
    }
}

void CheckForUnlinkedEntrances() {
    // Only run if file has been loaded and sceneNum is valid
    if (!fileNumber) {
        return;
    }

    // Get the entrance shuffler context
    auto entranceCtx = Rando::Context::GetInstance()->GetEntranceShuffler();

    // Loop the randomizer's entire entrance pool.
    // If the entrance has been dscovered but not linked, create a markdown file
    // in the save's OEV folder and add it to the linkedEntrances array
    for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
        // Get entrance from the randomizer's entrance pool
        EntranceOverride entrance = entranceCtx->entranceOverrides[i];

        // If not discovered, skip this loop iteration
        if (!IsEntranceDiscovered(entrance.index)) {
            continue;
        }

        // Check if one or the other side of the entrance has already been linked
        bool alreadyLinked = false;
        u16 randFromIndex = entrance.index;
        u16 randToIndex = entrance.override;

        for (EntranceOverride linkedEntrance : linkedEntrances) {
            u16 linkedFromIndex = linkedEntrance.index;
            u16 linkedToIndex = linkedEntrance.override;

            if ((randFromIndex == linkedToIndex && randToIndex == linkedFromIndex) ||
                (randFromIndex == linkedFromIndex && randToIndex == linkedToIndex)) {
                alreadyLinked = true;
                break;
            }
        }

        // If entrance has already been linked, skip this loop iteration
        if (alreadyLinked) {
            continue;
        }

        // Get the source and destination names from the randomizer's entrance data
        std::string fromName = GetEntranceData(entrance.index)->source;
        std::string toName = GetEntranceData(entrance.override)->destination;

        // Find the matching markdown metadata mapping in oevEntranceData
        OEVEntranceData fromOEVData = oevEntranceNames(fromName);
        OEVEntranceData toOEVData = oevEntranceNames(toName);
        entranceNameMap[fromName] = fromOEVData;
        entranceNameMap[toName] = toOEVData;

        // Link the entrances
        LinkEntrances(fileNumber, fromName, toName);

        // Add the entrance to linkedEntrances
        linkedEntrances[i] = entrance;

        std::cout << "[OEV AutoLinker] Linked new entrance: " << fromName << " - " << toName << std::endl;
    }
}

// On scene initialization, check if any entrances that
// have not already been linked have been discovered and if so, link them
void OnSceneInit(u16 sceneNum) {
    std::cout << std::endl << "[OEV AutoLinker] Scene " << sceneNum << " initialized." << std::endl;

    // Check for discovered entrances that are not linked, and link them
    CheckForUnlinkedEntrances();

    std::cout << "" << std::endl;
    return;
}

// Initialize entrances and
void InitializeEntranceData(int32_t fileNum) {

    // Set file number to use
    fileNumber = fileNum + 1;

    // Check for discovered entrances that are not linked, and link them
    CheckForUnlinkedEntrances();

    std::cout << std::endl << "[OEV AutoLinker] Game " << fileNum << " loaded" << std::endl;
    std::cout << std::endl;
}

// On save unload
void ClearEntranceData(int32_t fileNum) {
    // Clear linkedEntrances
    std::fill(std::begin(linkedEntrances), std::end(linkedEntrances), EntranceOverride{});

    // Clear file number
    fileNumber = NULL;
}

// Register hooks
void RegisterOEVAutoLinker() {
    // Check if the OEV directory exists and create it if not
    if (!std::filesystem::exists(Ship::Context::GetPathRelativeToAppDirectory("Obsidian Entrance Visualizer"))) {
        std::filesystem::create_directory(Ship::Context::GetPathRelativeToAppDirectory("Obsidian Entrance Visualizer"));
    }

    // Hooks
    COND_HOOK(OnSceneInit, CVAR_OEV_AUTOLINKER_CONDITION, OnSceneInit);
    COND_HOOK(OnLoadGame, CVAR_OEV_AUTOLINKER_CONDITION, InitializeEntranceData);
    COND_HOOK(OnExitGame, CVAR_OEV_AUTOLINKER_CONDITION, ClearEntranceData);
}

static RegisterShipInitFunc initFunc(RegisterOEVAutoLinker, { CVAR_OEV_AUTOLINKER_NAME });
