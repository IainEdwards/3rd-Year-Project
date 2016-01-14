#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

class Texture2D
{
public:

	//Initializes variables
	Texture2D();

	//Deallocates memory
	~Texture2D();

	//Loads image at specified path
	bool LoadTexture(std::string filename, SDL_Renderer* renderer);

	//Deallocates texture
	void Free();

	//Set color modulation
	void SetColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void SetBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void SetAlpha(Uint8 alpha);

	//Renders texture at given point
	void DrawTexture(int destX, int destY, int srcX, int srcY, float angle, SDL_Renderer* renderer, SDL_RendererFlip flip);

	//Gets image dimensions
	int GetWidth();
	int GetHeight();

private:
	//The actual hardware texture
	SDL_Texture* Texture;
	
	//Image dimensions
	int Width;
	int Height;

	SDL_Rect srcRect;
	SDL_Rect destRect;
};
#endif