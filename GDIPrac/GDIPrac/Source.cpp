#include<Windows.h>
#include "Design.h"
#define WINDOW_TITLE L"GameDevPractice"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

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


	//whether the resources can be loaded

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
//DECRIPTION: Window Procedure function
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

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
			
	return 0;
	}




}