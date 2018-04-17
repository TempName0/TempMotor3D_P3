#ifndef _SCREENSHOT_AND_GIF_
#define _SCREENSHOT_AND_GIF_

#include "Math\float2.h"
#include "SDL\include\SDL_scancode.h"

#include "Globals.h"

static uint ScreenshotsNum = 0; //Used to enable multiple screenshots in the same second

enum Culverin_PartScreenshot_STATE
{
	Culverin_PartScreenshot_STATE_WAITING_FOR_POINT1,
	Culverin_PartScreenshot_STATE_WAITING_FOR_POINT2
};

struct Culverin_Screenshot
{
	SDL_Scancode FullScreenKey = SDL_Scancode::SDL_SCANCODE_0;
	SDL_Scancode PartScreenKey = SDL_Scancode::SDL_SCANCODE_9;
	Culverin_PartScreenshot_STATE state = Culverin_PartScreenshot_STATE::Culverin_PartScreenshot_STATE_WAITING_FOR_POINT1;

	float2 Point1 = float2::zero;
	float2 Point2 = float2::zero;

	void TakeFullScreen();
	void TakePartScreen();
};

enum Culverin_PartGif_STATE
{
	Culverin_PartGif_STATE_CAN_START,
	Culverin_PartGif_STATE_RUNNING
};

struct Culverin_Gif
{
	int FullScreenKey = SDL_Scancode::SDL_SCANCODE_8;
	int PartScreenKey = SDL_Scancode::SDL_SCANCODE_7;

	unsigned char* pixels = nullptr;
	Culverin_PartGif_STATE Gif_State = Culverin_PartGif_STATE::Culverin_PartGif_STATE_CAN_START;

	void TakeFullScreen(float dt);
	void TakePartScreen(float dt);
};

#endif