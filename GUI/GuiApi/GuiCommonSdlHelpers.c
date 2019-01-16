#include "GuiCommonSdlHelpers.h"

// Helper function




SDL_Rect* guiCreateRectByCopy(SDL_Rect* src){
	if(src == NULL){
		return NULL;
	}
	SDL_Rect* res = myMalloc(sizeof(SDL_Rect), __func__, __LINE__);
	if(res==NULL){
		return NULL;
	}
	res->h = src->h;
	res->w = src->w;
	res->x = src->x;
	res->y = src->y;
	return res;
}


SDL_Rect* guiCreateRectByInts(int x, int y, int w, int h) {
	SDL_Rect* res = myMalloc(sizeof(SDL_Rect), __func__, __LINE__);
		if(res==NULL){return NULL;}
	res->x = x;
	res->y = y;
	res->w = w;
	res->h = h;
	return res;
}




SDL_Rect guiGetAsRect(int x, int y, int w, int h) {
	SDL_Rect res;
	res.x = x;
	res.y = y;
	res.w = w;
	res.h = h;
	return res;
}



void guiUpdateRectBySpriteRowColInSpriteSheet(SDL_Rect* rect, int rowIndex, int colIndex,
		int frameWidth, int frameHeight) {

	if (rect == NULL) { //TODO ERROR
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, __func__, "", NULL);
		return;
	}

		rect->x = colIndex * frameWidth;
		rect->y = rowIndex * frameHeight;
		rect->w = frameWidth;
		rect->h = frameHeight;

}


void guiUpdateRectXY(SDL_Rect* rect, int x, int y){
	if (rect == NULL) {return;}

	rect->x = x;
	rect->y = y;
}



void guiUpdateRectFromRect(SDL_Rect* updateMe, SDL_Rect* updateFrom) {

	if (updateMe == NULL || updateFrom == NULL) { //TODO ERROR
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, __func__, "", NULL);
		return;
	}

	updateMe->x = updateFrom->x;
	updateMe->y = updateFrom->y;
	updateMe->w = updateFrom->w;
	updateMe->h = updateFrom->h;

}





SDL_Texture* guiLoadTexture(const char* imagePath, SDL_Renderer* renderTarget){

	SDL_Texture* texture = NULL;
	SDL_Surface* surface = SDL_LoadBMP(imagePath);

	if (surface == NULL){
		// TODO printERR
	}
	else {
		// Set SDL COLOR FOR TRANSPARENT GOLBAL
		if (SDL_SetColorKey(surface, SDL_TRUE,
				SDL_MapRGB(surface->format, 0, 255, 0)) < 0)
		{}// TODO printERR SDL ERROR

		texture = SDL_CreateTextureFromSurface(renderTarget, surface);

		if (texture == NULL){
			// TODO printERR
		}
	}

	SDL_FreeSurface(surface);

	return texture;


}






/*

 // Helper function
SDL_Rect* guiGetRectFrame(int rowIndex, int colIndex,
		 int frameWidth, int frameHeight) {

	SDL_Rect* res = malloc(sizeof(SDL_Rect));
	if (res == NULL) {
		return NULL;
	}

	res->x = colIndex * frameWidth;
	res->y = rowIndex * frameHeight;
	res->w = frameWidth;
	res->h = frameHeight;
	return res;
}



 */


