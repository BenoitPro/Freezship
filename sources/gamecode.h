//  *********************************************************************************

// Gamecode.h		Version 6a		8/12/03
// Header file to declare the three game loop functions
#pragma once

int GameInit();

int GameMain();

void GameShutdown();

// For reading keyboard
#define KEYPRESSED(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

#define SCREENWIDTH 1024
#define SCREENHEIGHT 768
#define COLOURDEPTH 32

