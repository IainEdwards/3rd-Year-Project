#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <stdio.h>
#include <string>
#include <SDL.h>
#include <map>

class TextureManager
{
public:
	bool load(std::string fileName, std::string id, SDL_Renderer* pRenderer);

	SDL_Rect draw(std::string id, int x, int y, int width, int height, SDL_Renderer* renderer, SDL_RendererFlip flip);

	SDL_Rect drawObject(std::string id, int x, int y, int srcX, int srcY, int width, int height, SDL_Renderer* renderer, SDL_RendererFlip flip);

	SDL_Rect drawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* renderer, SDL_RendererFlip flip);

	std::map<std::string, SDL_Texture*> textureMap;


private:

};


#endif