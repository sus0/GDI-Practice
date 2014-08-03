#include<Windows.h>
#include<time.h>
#include<tchar.h>
#include "Design.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msimg32.lib")

#define WINDOW_TITLE L"GameDevPractice"
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define PARTICLE_NUMBER 50

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: Declare all global variables here
//------------------------------------------------------------------------------------------------------------
HDC g_hdc = NULL, g_mdc= NULL, g_bufdc = NULL; //global hdc
DWORD g_tPrev = 0, g_tCurr = 0;
RECT g_rect;
int g_iFrameNo, g_iTextLineNo;
int g_iRainNo = 0;
int g_iFlameNo = 0;
wchar_t text[7][100]; // store the output texts
RAIN Raindrops[PARTICLE_NUMBER];
BOOL g_bAttack, g_bGameOver;
BOOL g_bHover = false;
CHARACTER Hero, Dragon;
Actions Hero_Actions, Dragon_Actions;
//HBITMAPS
HBITMAP g_hRain;
HBITMAP g_hFlames;
HBITMAP g_hBackground, g_hGameOver, g_hVictory, g_hTryAgain, g_hTryAgain_Red;
		//hero side
HBITMAP g_hHero;
HBITMAP g_hSkillBt1, g_hSkillBt2, g_hSkillBt3, g_hSkillBt4;
HBITMAP g_hHeroEffect1, g_hHeroEffect2, g_hHeroEffect3, g_hHeroRecoverEffect;

		//dragon side
HBITMAP g_hDragon;
HBITMAP g_hDragonEffect1, g_hDragonEffect2, g_hDragonEffect3;

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: Forward declaration
//------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL Game_Initializer(HWND hwnd);
VOID Game_Main(HWND hwnd);
BOOL Game_ShutDown(HWND hwnd);
VOID CheckDeath(int CurrHp, bool isHero);
VOID Msg_Insert(wchar_t *str);
VOID HeroAction_Logic();
VOID HeroAction_Paint();
VOID DragonAction_Logic();
VOID DragonAction_Paint();
VOID Rain_Paint();
VOID Game_Restart(HWND hwnd);

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: WinMain function
//------------------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wndClass = {
		sizeof(WNDCLASSEX),
		CS_VREDRAW | CS_HREDRAW,
		WindowProc,
		0,
		0,
		hInstance,
		(HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0,0, LR_DEFAULTSIZE|LR_LOADFROMFILE),
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(WHITE_BRUSH),
		NULL,
		L"GDIPractice"
	};

	//Register this window
	if(!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	//Create the window
	HWND hwnd = CreateWindow(
		L"GDIPractice", 
		WINDOW_TITLE, 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		WINDOW_WIDTH, 
		WINDOW_HEIGHT, 
		NULL, 
		NULL, 
		hInstance, 
		NULL
	);

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	if(!Game_Initializer(hwnd))
	{
		MessageBox(hwnd, L"Failed to load game resources", L"Error", 0);
	}

	PlaySound(L"Media\\bgm.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	//Message loop
	MSG msg = {0};
	while(msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) // get the msg from msg queue
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_tCurr = GetTickCount();
			if (g_tCurr - g_tPrev >= 60)
			{
				Game_Main(hwnd);
			}
		
		}
	}

	//Unregister the window
	UnregisterClass(L"GDIPractice", wndClass.hInstance);

	return 0;
}

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: WndProc function
//------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_KEYDOWN: //Any key is pressed
			if(wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			break;

		case WM_DESTROY: 
			Game_ShutDown(hwnd);
			PostQuitMessage(0);
			break;

		case WM_MOUSEHOVER:
			if (g_bGameOver == true)
			{
				int x = LOWORD(lParam); 
				int y = HIWORD(lParam);
				if (x >=360 && x <= 660 && y >=550 && y <=620)
				{
					g_bHover = true;
				}
			}
			break;

		case WM_LBUTTONDOWN: //left button of mouse clicked
			if(!g_bAttack)
			{
				int x = LOWORD(lParam);
				int y = HIWORD(lParam);
				
				//TCHAR buf[100];
				//swprintf_s(buf, _T("%d"), y);
				//MessageBox(hwnd, buf, L"Testing", MB_OK);
				//
				if(x >= 700 && x <= 750 && y >=610 && y <= 660)
				{
					g_bAttack = true;
					Hero_Actions = ACTION_NORMAL;
				}
				
				if(x >= 760 && x <= 810 && y >=610 && y <= 660)
				{
			
					g_bAttack = true;
					Hero_Actions = ACTION_MAGIC;
				}
				
				if(x >= 820 && x <= 870 && y >=610 && y <= 660)
				{
					g_bAttack = true;
					Hero_Actions = ACTION_RECOVER;
				}
			}

			if(g_bGameOver == true)
			{		
				int x = LOWORD(lParam);
				int y = HIWORD(lParam);
				if(x >=360 && x<= 660 && y >=550 && y<=620)
				{
					Game_Restart(hwnd);
				}
				
			}
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: Load all game resources
//------------------------------------------------------------------------------------------------------------
BOOL Game_Initializer(HWND hwnd)
{
	srand((unsigned)time(NULL)); //generate seed using current time

	HBITMAP bmp; //blank bitmap

	//buffers
	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);
	g_bufdc = CreateCompatibleDC(g_hdc);
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(g_mdc, bmp); 

	//load game resources
	//HBITMAP image=(HBITMAP)LoadImage(0,L"Media\\bt2.bmp",IMAGE_BITMAP,360, 360,LR_LOADFROMFILE);
	//if(image == NULL)
   //MessageBox(0, L"Couldn't load the image", L"Error", MB_OK);

	g_hBackground = (HBITMAP)LoadImage(NULL,L"Media\\nbk2.bmp", IMAGE_BITMAP, 1022, 767, LR_LOADFROMFILE);
	g_hGameOver = (HBITMAP)LoadImage(NULL,L"Media\\gameover.bmp", IMAGE_BITMAP, 1000, 400, LR_LOADFROMFILE);
	g_hVictory = (HBITMAP)LoadImage(NULL,L"Media\\victory.bmp", IMAGE_BITMAP, 1000, 400, LR_LOADFROMFILE);
	g_hHero = (HBITMAP)LoadImage(NULL,L"Media\\hero.bmp", IMAGE_BITMAP, 400, 200, LR_LOADFROMFILE);	
	g_hDragon = (HBITMAP)LoadImage(NULL, L"Media\\dragon.bmp", IMAGE_BITMAP, 800, 350, LR_LOADFROMFILE);
	g_hSkillBt1 = (HBITMAP)LoadImage(NULL, L"Media\\bt1.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE); 
	g_hSkillBt2 = (HBITMAP)LoadImage(NULL, L"Media\\bt2.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE); 
	g_hSkillBt3 = (HBITMAP)LoadImage(NULL, L"Media\\bt3.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE); 
	g_hSkillBt4 = (HBITMAP)LoadImage(NULL, L"Media\\bt4.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE); 
	g_hDragonEffect1 = (HBITMAP)LoadImage(NULL, L"Media\\dragon_critical.bmp", IMAGE_BITMAP, 1000,300, LR_LOADFROMFILE);
	g_hRain = (HBITMAP)LoadImage(NULL, L"Media\\raindrop.bmp", IMAGE_BITMAP, 10, 30, LR_LOADFROMFILE);
	g_hFlames = (HBITMAP)LoadImage(NULL, L"Media\\flames.bmp", IMAGE_BITMAP, 300, 100, LR_LOADFROMFILE);
	g_hTryAgain = (HBITMAP)LoadImage(NULL, L"Media\\tryagain_black.bmp", IMAGE_BITMAP, 600, 70, LR_LOADFROMFILE);
	g_hTryAgain_Red = (HBITMAP)LoadImage(NULL, L"Media\\tryagain_red.bmp", IMAGE_BITMAP, 300, 70, LR_LOADFROMFILE);

	GetClientRect(hwnd, &g_rect);
	

	//Config hero properties
	Hero.CurrHp = Hero.MaxHp = 100;
	Hero.Level = 6;
	Hero.CurrMp = Hero.MaxMp = 70;
	Hero.Strength = 10;
	Hero.Agility = 20;
	Hero.Intellect = 10;

	//Config dragon properties
	Dragon.CurrHp = Dragon.MaxHp = 2000;
	Dragon.Level = 10;
	Dragon.Strength = 10;
	Dragon.Agility = 10;
	Dragon.Intellect = 10;

	g_iTextLineNo = 0;
	
	//Config fonts
	HFONT hFont;
	hFont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0 , DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("Comic Sans MS"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_mdc, TRANSPARENT);

	Game_Main(hwnd);
	return true;
}

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: Game_Main entry
//------------------------------------------------------------------------------------------------------------
VOID Game_Main(HWND hwnd)
{
	wchar_t str[100];

	SelectObject(g_bufdc, g_hBackground);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);


	SetTextColor(g_mdc, RGB(0, 0, 0));
	
	for (int i = 0; i < g_iTextLineNo; i++)
	{
		TextOut(g_mdc, 40, 600+i*18, text[i], wcslen(text[i]));
	}

	//the dragon is alive
	if(Dragon.CurrHp > 0)
	{
		SelectObject(g_bufdc, g_hDragon);
		BitBlt(g_mdc, 50, 250, 400, 350, g_bufdc, 400, 0, SRCAND);
		BitBlt(g_mdc, 50, 250, 400, 350, g_bufdc, 0, 0, SRCPAINT);
	
		swprintf_s(str, L"%d / %d", Dragon.CurrHp, Dragon.MaxHp);
		SetTextColor(g_mdc, RGB(0, 0, 0));
		TextOut(g_mdc, 160, 560 , str, wcslen(str));
	}


	
	//the hero is alive
	if (Hero.CurrHp > 0)
	{
		SelectObject(g_bufdc, g_hHero);
		BitBlt(g_mdc, 700, 350, 200, 200, g_bufdc, 200, 0, SRCAND);
		BitBlt(g_mdc, 700, 350, 200, 200, g_bufdc, 0, 0, SRCPAINT);
		
		//show hp
		swprintf_s(str, L"%d / %d", Hero.CurrHp, Hero.MaxHp);
		SetTextColor(g_mdc, RGB(0, 0, 0));
		TextOut(g_mdc, 740, 560 , str, wcslen(str));
	
		//show mp
		swprintf_s(str, L"%d / %d", Hero.CurrMp, Hero.MaxMp);
		SetTextColor(g_mdc, RGB(0, 0, 0));
		TextOut(g_mdc, 750, 580 , str, wcslen(str));
	}

	//if game is over
	if(g_bGameOver)
	{
		if (Hero.CurrHp <= 0)
		{
			SelectObject(g_bufdc, g_hGameOver);	
			BitBlt(g_mdc, 260, 200, 500, 400, g_bufdc, 500, 0, SRCAND);
			BitBlt(g_mdc, 260, 200, 500, 400, g_bufdc, 0, 0, SRCPAINT);
			SelectObject(g_bufdc, g_hTryAgain);
			//TransparentBlt(g_mdc, 360, 600, 300, 70 , g_bufdc, 0, 0, 300, 70, RGB(0, 0, 0));
			BitBlt(g_mdc, 360, 550, 300, 70, g_bufdc, 300, 0, SRCAND);
			BitBlt(g_mdc, 360, 550, 300, 70, g_bufdc, 0, 0, SRCPAINT);
		}
		else
		{
			SelectObject(g_bufdc, g_hVictory);	
			BitBlt(g_mdc, 260, 200, 500, 400, g_bufdc, 500, 0, SRCAND);
			BitBlt(g_mdc, 260, 200, 500, 400, g_bufdc, 0, 0, SRCPAINT);

			if(g_bHover == true)
			{
				SelectObject(g_bufdc, g_hTryAgain_Red);
				TransparentBlt(g_mdc, 350, 550, 300, 70, g_bufdc, 0, 0, 300, 70, RGB(255, 255, 255));
				g_bHover = false;
			}
		}
	}
	
	else if(!g_bAttack)
	{ //skill buttons
		SelectObject(g_bufdc, g_hSkillBt1);
		BitBlt(g_mdc, 700, 610, 50, 50, g_bufdc, 0, 0, SRCCOPY);
		SelectObject(g_bufdc, g_hSkillBt2);
		BitBlt(g_mdc, 760, 610, 50, 50, g_bufdc, 0, 0, SRCCOPY);
		SelectObject(g_bufdc, g_hSkillBt3);
		BitBlt(g_mdc, 820, 610, 50, 50, g_bufdc, 0, 0, SRCCOPY);
		SelectObject(g_bufdc, g_hSkillBt4);
		BitBlt(g_mdc, 880, 610, 50, 50, g_bufdc, 0, 0, SRCCOPY);
	}

	else
	{
		g_iFrameNo ++;

		if (g_iFrameNo >= 5 && g_iFrameNo <= 10)
		{
			if ((g_iFrameNo == 5))
			{
				HeroAction_Logic();
				CheckDeath(Dragon.CurrHp, false);
			}
			HeroAction_Paint();
		}

		if(g_iFrameNo == 15)
		{
			DragonAction_Logic();
		}

		if(g_iFrameNo >=26 && g_iFrameNo <=40)
		{
			DragonAction_Paint();
		}

		if(g_iFrameNo == 40)
		{
			g_bAttack = false;
			g_iFrameNo = 0;
		

			if (!g_bGameOver)
			{
				int MpRecovered = 2*(rand()%Hero.Intellect) + 6;
				Hero.CurrMp += MpRecovered;

				if(Hero.CurrMp >= Hero.MaxMp)
				{
					Hero.CurrMp = Hero.MaxMp;
				}

			swprintf_s(str, L"Your MP recovers %d MP after this round", MpRecovered);
			Msg_Insert(str);
		
			}
		}
	}

	
	////Let it pour
	//if ( !g_bGameOver )
	//{
	//	Rain_Paint();
	//}
	g_iFlameNo++;
	if(g_iFlameNo == 3)
	{
		g_iFlameNo = 0;
	}

	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
	g_tPrev = GetTickCount();
}

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: check if the character dies
//------------------------------------------------------------------------------------------------------------
VOID CheckDeath(int CurrHp, bool isHero)
{
	wchar_t str[100];
	if (CurrHp <= 0)
	{
		g_bGameOver = true;
		if (isHero)
		{ 
			PlaySound(L"Media\\gameOverSound.wav", NULL, SND_FILENAME | SND_ASYNC);
			swprintf_s(str, L": You die!");		
			Msg_Insert(str);
		}
	
		else
		{
			swprintf_s(str, L": You Win!");
			Msg_Insert(str);
		}

	}
}

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: insert message into textbox
//------------------------------------------------------------------------------------------------------------
VOID Msg_Insert(wchar_t *str)
{
	if(g_iTextLineNo < 7)
	{
		swprintf_s(text[g_iTextLineNo], str);
		g_iTextLineNo ++;
	}
	else
	{
		for (int i = 0; i < g_iTextLineNo; i++)
		{
			swprintf_s(text[i], text[i+1]);
		}
			swprintf_s(text[6], str);
		
	}
}

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: logic of hero attacks
//------------------------------------------------------------------------------------------------------------
VOID HeroAction_Logic()
{
	int dragonHpLoss = 0;
	wchar_t str[100];

	switch(Hero_Actions)
	{
	case ACTION_NORMAL:
		if(1== rand()%5) // 20% chance to trigger critical attack
		{
			Hero_Actions = ACTION_CRITICAL;
			dragonHpLoss = (int)(5.5f*(float)(3*rand()%Hero.Agility) + Hero.Level*Hero.Strength + 20);
			Dragon.CurrHp -= (int)dragonHpLoss;
			swprintf_s(str, L"Lucky you! Critical Strike!  5+ times greater than a regular attack! You just caused %d hp loss of the boss", dragonHpLoss);
		}
		else
		{
			dragonHpLoss = 3*(rand()%Hero.Agility) + Hero.Level*Hero.Strength + 20;
			Dragon.CurrHp -= (int)dragonHpLoss;
			swprintf_s(str, L"Regular attack! You just caused the boss to lose %d hp", dragonHpLoss);
		}
		Msg_Insert(str);
		break;
		
	case ACTION_MAGIC:
		if(Hero.CurrMp >= 30)
		{
			dragonHpLoss = 5*(2*(rand()%Hero.Agility) + Hero.Level*Hero.Intellect);
			Dragon.CurrHp -= (int)dragonHpLoss;
			Hero.CurrMp -= 30;
			swprintf_s(str, L"Magic Attack! Caused %d points of health loss", dragonHpLoss);
		}
		else
		{
			Hero_Actions = ACTION_MISS;
			swprintf_s(str, L"You do not have enough magic points now! Please release the skill when you have 30 or more MP");
		}
		Msg_Insert(str);
		break;

	case ACTION_RECOVER:
		if(Hero.CurrMp >= 40)
		{
			Hero.CurrMp -= 40;
			int HpRecovered = 5*(5*(rand()%Hero.Intellect) + 40);
			Hero.CurrHp += HpRecovered;

			if(Hero.CurrHp >= Hero.MaxHp)
			{
				Hero.CurrHp = Hero.MaxHp;
			}
			swprintf_s(str, L"You just used recovery magic, and gained %d Hps. You feel much better now!", HpRecovered);
		}
		else
		{
			Hero_Actions = ACTION_MISS;
			swprintf_s(str, L"You do not have enough magic points now! Please release the skill when you have 40 or more MP");
		}
		Msg_Insert(str);
		break;
	}
}

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: Draw hero's attack effects here
//------------------------------------------------------------------------------------------------------------
VOID HeroAction_Paint()
{

	switch (Hero_Actions)
	{
	case ACTION_NORMAL:
		break;
	case ACTION_CRITICAL:
		break;
	case ACTION_MAGIC:
		SelectObject(g_bufdc, g_hFlames);
		TransparentBlt(g_mdc, 200, 300, 100, 100, g_bufdc, g_iFlameNo*100, 0, 100, 100, RGB(0, 0, 0));
		TransparentBlt(g_mdc, 100, 400, 100, 100, g_bufdc, g_iFlameNo*100, 0, 100, 100, RGB(0, 0, 0));
		TransparentBlt(g_mdc, 300, 400, 100, 100, g_bufdc, g_iFlameNo*100, 0, 100, 100, RGB(0, 0, 0));

		break;
	case ACTION_MISS:
		break;
	case ACTION_RECOVER:
		break;
	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: Dragon attack logics
//------------------------------------------------------------------------------------------------------------
VOID DragonAction_Logic()
{
	srand((unsigned)time(NULL));
	if(Dragon.CurrHp > (Dragon.MaxMp/2))
	{
		switch(rand()%3)
		{
		case 0:
			Dragon_Actions = ACTION_NORMAL;
			break;
		
		case 1:
			Dragon_Actions = ACTION_CRITICAL;
			break;

		case 2:
			Dragon_Actions = ACTION_MAGIC;
			break;
		}
	}
	else
	{
		switch(rand()%3)
		{
		case 0:
			Dragon_Actions = ACTION_MAGIC;
			break;

		case 1: 
			Dragon_Actions = ACTION_CRITICAL;
			break;

		case 2:
			Dragon_Actions = ACTION_RECOVER;
			break;
		
		}	
	}

}

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: Paint effect&insert messages about dragon attacks
//------------------------------------------------------------------------------------------------------------
VOID DragonAction_Paint()
{
	int loss = 0, recover = 0;
	wchar_t str[100];

	switch(Dragon_Actions)
	{
	case ACTION_NORMAL:							
		SelectObject(g_bufdc,g_hDragonEffect1);
		BitBlt(g_mdc, 470, 250, 500, 300, g_bufdc, 500, 0, SRCAND);
		BitBlt(g_mdc, 470, 250, 500, 300, g_bufdc, 0, 0, SRCPAINT);
		
		if(g_iFrameNo == 30)
		{
			loss = rand()%Dragon.Agility+ Dragon.Level*Dragon.Strength;
			Hero.CurrHp -= (int)loss;

			swprintf_s(str,L"Watch out! The boss cause %d hp loss on you...", loss);
			Msg_Insert(str);

			CheckDeath(Hero.CurrHp, true);
		}
		break;

	case ACTION_MAGIC:							
		Rain_Paint();
		if(g_iFrameNo == 40)
		{
			loss = 2*(2*(rand()%Dragon.Agility) + Dragon.Strength*Dragon.Intellect); 
			Hero.CurrHp -= loss;	   
			Dragon.CurrHp += recover;  \
			swprintf_s(str,L"The boss caused another %d hp loss due to its magic attack", loss, recover);  
			Msg_Insert(str);  

			CheckDeath(Hero.CurrHp,true);
		}
		break;

	case ACTION_CRITICAL:							

		if(g_iFrameNo == 40)
		{
			loss = 2*(rand()%Dragon.Agility+ Dragon.Level*Dragon.Strength);
			Hero.CurrHp -= (int)loss;

			swprintf_s(str,L"Poor you! The dragon just made a critical strike with %d Hp loss on you!", loss);
			Msg_Insert(str);

			CheckDeath(Hero.CurrHp,true);
		}
		break;

	case ACTION_RECOVER:						
		
		if(g_iFrameNo == 40)
		{
			recover= 2*Dragon.Intellect*Dragon.Intellect;
			Dragon.CurrHp +=recover;
			swprintf_s(str,L"It just recovered itself by %d hp. Got to take an actiom!",recover);
			Msg_Insert(str);
		}
		break;
	}
}

//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: release all game resources
//------------------------------------------------------------------------------------------------------------
BOOL Game_ShutDown(HWND hwnd)
{
	DeleteObject(g_hBackground);
	DeleteObject(g_hVictory);
	DeleteObject(g_hGameOver);
	DeleteObject(g_hFlames);
	DeleteObject(g_hTryAgain);
	DeleteObject(g_hTryAgain_Red);
	DeleteObject(g_hDragon);
	DeleteObject(g_hHero);
	DeleteObject(g_hSkillBt1);
	DeleteObject(g_hSkillBt2);
	DeleteObject(g_hSkillBt3);
	DeleteObject(g_hSkillBt4);
	DeleteObject(g_hHeroEffect1);
	DeleteObject(g_hHeroEffect2);
	DeleteObject(g_hHeroEffect3);
	DeleteObject(g_hDragonEffect1);
	DeleteObject(g_hDragonEffect2);
	DeleteObject(g_hDragonEffect3);
	
	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd, g_hdc);
	return true;
}
//------------------------------------------------------------------------------------------------------------
//DESCRIPTION: paint the rain drops
//------------------------------------------------------------------------------------------------------------
VOID Rain_Paint()
{
	if(g_iRainNo < PARTICLE_NUMBER)
	{
		Raindrops[g_iRainNo].x = g_rect.right/2 + rand()%(g_rect.right/2);
		Raindrops[g_iRainNo].y = 250 + rand()%400;
		Raindrops[g_iRainNo].ifExists = true;
		g_iRainNo ++;
	}

	for (int i = 0; i < PARTICLE_NUMBER; i++)
	{
		if (Raindrops[i].ifExists)
		{
			SelectObject(g_bufdc, g_hRain);
			TransparentBlt(g_mdc, Raindrops[i].x, Raindrops[i].y, 10, 30, g_bufdc, 0, 0, 10, 30, RGB(0, 0, 0));

			Raindrops[i].y += 10;

			if (Raindrops[i].y > 650)
			{
				//Raindrops[i].x = rand()%g_rect.right;
				Raindrops[i].x = g_rect.right/2 + rand()%(g_rect.right/2);
				Raindrops[i].y = 250 + rand()%400;
			}
			

		}

	}

}

VOID Game_Restart(HWND hwnd)
{
	PlaySound(L"Media\\bgm.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	g_bGameOver = false;
	g_iTextLineNo = 0;
	for (int i = 0; i < 7; i++)
	{
		swprintf_s(text[i], L"");
	}
	//wchar_t temp_text[7][100];
	//text[7][100] = temp_text[7][100];
	g_bAttack = false;
	g_iFrameNo = 0;
	g_iRainNo = 0;

	Game_Initializer(hwnd);
						

}

