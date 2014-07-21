#include<Windows.h>

struct CHARACTER
{
	int CurrHp;    //Current health points
	int MaxHp;     //Max health points
	int CurrMp;    //Current magic points
	int MaxMp;	   //Max magic points
	int Level;     //Level of the character
	int Strength;  //Strength of the character
	int Agility;   //Agility of the character
	int Intellect; //Intellect of the character
};


enum Actions
{
	ACTION_NORMAL = 0,	 //normal attack
	ACTION_CRITICAL = 1, //critival strike
	ACTION_MAGIC = 2,    //magic attack
	ACTION_MISS = 3,
	ACTION_RECOVER = 4
};