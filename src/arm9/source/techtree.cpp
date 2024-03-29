// THIS FILE IS GENERATED BY BUILDTT.PL

#include "techtree.h"

const TechnologyInfo gTechInfo[] =
{
	{ "Farming", "You can now build Farms", 1250, 120, 128, 55, 19, { Tech_Wheel, Tech_Animal__Husbandry, Tech_Invalid, Tech_Invalid, } },
	{ "Wheel", "You are now more productive", 300, 235, 88, 45, 19, { Tech_Invalid, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Animal Husbandry", "Bonus production from Crops", 300, 57, 88, 115, 19, { Tech_Agriculture, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Agriculture", "Crops supply more food per acre", 250, 57, 49, 70, 19, { Tech_Invalid, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Masonry", "Huts support more people", 350, 153, 88, 56, 19, { Tech_Mining, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Mining", "Faster building of Huts and Crops", 200, 212, 9, 48, 19, { Tech_Invalid, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Metal Work", "Defense bonus against attacks", 200, 255, 49, 73, 19, { Tech_Mining, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Irrigation", "Hardy Crops, Bonus food from Farms", 350, 52, 128, 61, 19, { Tech_Masonry, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Machinery", "Building now proceeds faster", 800, 278, 128, 66, 19, { Tech_Metal_Work, Tech_Wheel, Tech_Fossil_Fuels, Tech_Invalid, } },
	{ "Fossil Fuels", "Can build Factories; beware pollution", 1100, 326, 88, 74, 19, { Tech_Metal_Work, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Mathematics", "Research now proceeds faster", 2000, 380, 128, 78, 19, { Tech_Alphabet, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Alphabet", "You can now build Schools", 800, 409, 88, 59, 19, { Tech_Invalid, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Maps", "You can now see the whole map", 1000, 449, 128, 41, 19, { Tech_Alphabet, Tech_Compass, Tech_Invalid, Tech_Invalid, } },
	{ "Compass", "You can now attack the other island", 450, 481, 88, 60, 19, { Tech_Invalid, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "War Boats", "Double destruction when attacking", 2250, 452, 167, 68, 19, { Tech_Maps, Tech_Compass, Tech_Invalid, Tech_Invalid, } },
	{ "Construction", "You can now build Houses", 1250, 197, 128, 78, 19, { Tech_Masonry, Tech_Wheel, Tech_Metal_Work, Tech_Invalid, } },
	{ "Engineering", "You can now reclaim land", 6000, 274, 167, 74, 19, { Tech_Machinery, Tech_Mathematics, Tech_Invalid, Tech_Invalid, } },
	{ "Currency", "You are now more productive", 3000, 377, 167, 60, 19, { Tech_Mathematics, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Trade Routes", "You can now import food", 4500, 423, 206, 83, 19, { Tech_War_Boats, Tech_Currency, Tech_Invalid, Tech_Invalid, } },
	{ "Education", "You can now build a University", 9000, 451, 246, 63, 19, { Tech_Trade_Routes, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Religion", "Much higher birthrate", 35000, 382, 246, 54, 19, { Tech_Trade_Routes, Tech_Invalid, Tech_Invalid, Tech_Invalid, } },
	{ "Medicine", "You can now build a Hospital", 20000, 195, 206, 58, 19, { Tech_Irrigation, Tech_Engineering, Tech_Invalid, Tech_Invalid, } },
	{ "Fusion", "You now now launch Missiles to 'win'.", 35000, 272, 246, 47, 19, { Tech_Advanced_Construction, Tech_Medicine, Tech_Invalid, Tech_Invalid, } },
	{ "Adv. Construction", "You can now build Apartments", 15000, 300, 206, 132, 19, { Tech_Fossil_Fuels, Tech_Engineering, Tech_Construction, Tech_Currency, } },
	{ "Tornado Resistance", "Buildings are tornado resistant", 9000, 146, 167, 120, 19, { Tech_Construction, Tech_Irrigation, Tech_Machinery, Tech_Invalid, } },
};

