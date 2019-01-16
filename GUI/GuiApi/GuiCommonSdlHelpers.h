#ifndef GUICOMMONSDLHELPERS_H_
#define GUICOMMONSDLHELPERS_H_


#include "../../MemGateway/MemGateway.h"
#include <SDL.h>
#include <SDL_video.h>
#include <stdlib.h>


SDL_Rect guiGetAsRect(int x, int y, int w, int h);

SDL_Rect* guiCreateRectByCopy(SDL_Rect* src);
SDL_Rect* guiCreateRectByInts(int x, int y, int w, int h);

void guiUpdateRectBySpriteRowColInSpriteSheet(SDL_Rect* rect,
		int rowIndex, int colIndex,
		 int frameWidth, int frameHeight);

void guiUpdateRectXY(SDL_Rect* rect, int x, int y);

void guiUpdateRectFromRect(SDL_Rect* updateMe, SDL_Rect* updateFrom);


SDL_Texture* guiLoadTexture(const char* imagePath, SDL_Renderer* renderTarget);

#endif
