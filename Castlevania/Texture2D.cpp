#include "Texture2D.h"
#include <SDL_image.h>

Texture2D::Texture2D()
{
	Texture = NULL;
	Width = 0;
	Height = 0;
}

Texture2D::~Texture2D()
{
	//Deallocate
	Free();
}

bool Texture2D::LoadTexture(std::string path, SDL_Renderer* renderer)
{
	//Get rid of preexisting texture
	Free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			Width = loadedSurface->w;
			Height = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	Texture = newTexture;
	return Texture != NULL;	
}

void Texture2D::Free()
{
	//Free texture if it exists
	if (Texture != NULL)
	{
		SDL_DestroyTexture(Texture);
		Texture = NULL;
		Width = 0;
		Height = 0;
	}
}

void Texture2D::SetColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(Texture, red, green, blue);
}

void Texture2D::SetBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(Texture, blending);
}

void Texture2D::SetAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(Texture, alpha);
}

//void Texture2D::DrawTexture(int x, int y, SDL_Renderer* renderer, SDL_Rect* clip)
//{
//
//	//Set rendering space and render to screen
//	SDL_Rect renderQuad = { x, y, Width, Height };
//
//	//Set clip rendering dimensions
//	if (clip != NULL)
//	{
//		renderQuad.w = clip->w;
//		renderQuad.h = clip->h;
//	}
//
//	//Render to screen
//	SDL_RenderCopy(renderer, Texture, clip, &renderQuad);

void Texture2D::DrawTexture(int destX, int destY, int srcX, int srcY, float angle, SDL_Renderer* renderer, SDL_RendererFlip flip)
{
	srcRect.x = srcX;
	srcRect.y = srcY;
	srcRect.w = destRect.w = Width;
	srcRect.h = destRect.h = Height;
	destRect.x = destX;
	destRect.y = destY;

	SDL_RenderCopyEx(renderer, Texture, &srcRect, &destRect, angle, 0, flip);
	
}

int Texture2D::GetWidth()
{
	return Width;
}

int Texture2D::GetHeight()
{
	return Height;
}