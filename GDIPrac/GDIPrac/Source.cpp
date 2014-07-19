#include<Windows.h>
#include "Design.h"

#pragma comment(lib, "winmm.lib")

#define WINDOW_TITLE L"GameDevPractice"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


//-----------------------------------------------Global Variables---------------------------------------------
//DESCRIPTION: Declare all global variables here
//------------------------------------------------------------------------------------------------------------
HDC g_hdc = NULL; //global hdc

CHARACTER Hero, Dragon;
Actions Hero_Actions, Dragon_Actions;

//------------------------------------------------------Functions---------------------------------------------
//DESCRIPTION: Declare all functions here
//------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL Game_Initializer(HWND hwnd);

//------------------------------------------------WinMain Function---------------------------------------------
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
			//paint
		}
	}

	//Unregister the window
	UnregisterClass(L"GDIPractice", wndClass.hInstance);

	return 0;
}

//----------------------------------------------WindowProc()--------------------------------------------------
//DESCRIPTION: Window Procedure function
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
			//shut down the game
			PostQuitMessage(0);
			break;

		case WM_LBUTTONDOWN: //left button of mouse clicked
			//do something

			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

//---------------------------------------------Game Initializer-----------------------------------------------
//DESCRIPTION: Load all game resources
//------------------------------------------------------------------------------------------------------------
BOOL Game_Initializer(HWND hwnd)
{
	//for test
	return true;
}