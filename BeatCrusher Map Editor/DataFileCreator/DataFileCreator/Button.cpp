#include "Button.h"
#include "LoadTexture.h"
#include <iostream>
#define RGB(x, y) (y < 0 ? (x+y < 0 ? 0 : x+y) : (x+y > 255 ? 255 : x+y))
void Button::MakeButton(SDL_Renderer *renderer, string str, int x, int y, int w, int h, int fontsize,int r, int g, int b, int tr, int tg, int tb)
{
	this->text = LoadText(str.c_str(), renderer, fontsize, "MainFont");
	this->rect.x = x;
	this->rect.y = y;
	this->rect.w = w;
	this->rect.h = h;
	this->color.r = r;
	this->color.g = g;
	this->color.b = b;
	this->color_text.r = tr;
	this->color_text.g = tg;
	this->color_text.b = tb;
	this->color.a = 255;
	this->renderer = renderer;
	SDL_QueryTexture(this->text, NULL, NULL, &this->text_rect.w, &this->text_rect.h);
	this->text_rect.x = this->rect.x + this->rect.w / 2 - this->text_rect.w / 2;
	this->text_rect.y = this->rect.y + this->rect.h / 2 - this->text_rect.h / 2;
	this->click = false;
	this->select = true;
	//this->mouse = false;
}

void Button::Move(int x, int y)
{
	this->rect.x = x;
	this->rect.y = y;
	this->text_rect.x = this->rect.x + this->rect.w / 2 - this->text_rect.w / 2;
	this->text_rect.y = this->rect.y + this->rect.h / 2 - this->text_rect.h / 2;
}

void Button::Rendering()
{
	if (this->click)
	{
		SDL_SetRenderDrawColor(this->renderer, RGB(this->color.r, -30), RGB(this->color.g, -30), RGB(this->color.b, -30), this->color.a);
	}
	else if (this->select)
	{
		SDL_SetRenderDrawColor(this->renderer, this->color.r, this->color.g, this->color.b, this->color.a);
	}
	else
	{
		SDL_SetRenderDrawColor(this->renderer, RGB(this->color.r,-20), RGB(this->color.g, -20), RGB(this->color.b, -20), this->color.a);
	}

	SDL_RenderFillRect(this->renderer, &this->rect);
	SDL_SetRenderDrawColor(this->renderer, RGB(this->color.r, -30), RGB(this->color.g, -30), RGB(this->color.b, -30), 255);
	SDL_RenderDrawRect(this->renderer, &this->rect);
	
	SDL_SetTextureColorMod(this->text, this->color_text.r, this->color_text.g, this->color_text.b);
	SDL_RenderCopy(this->renderer, this->text, NULL, &this->text_rect);
}

bool Button::Click(SDL_Event event)
{
	if (this->select) {
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (event.button.x > this->rect.x && event.button.x < this->rect.x + this->rect.w)
			{
				if (event.button.y > this->rect.y && event.button.y < this->rect.y + this->rect.h)
				{
					this->click = true;
					return false;
				}
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP && this->click)
		{
			this->click = false;
			if (event.button.x > this->rect.x && event.button.x < this->rect.x + this->rect.w)
			{
				if (event.button.y > this->rect.y && event.button.y < this->rect.y + this->rect.h)
				{
					return true;
				}
			}
		}
		/*else if (event.type == SDL_MOUSEMOTION)
		{
			if (event.button.x > this->rect.x && event.button.x < this->rect.x + this->rect.w)
			{
				if (event.button.y > this->rect.y && event.button.y < this->rect.y + this->rect.h)
				{
					this->mouse = true;
				}
			}
		}*/
	}
	return false;
}

void Button::Select(bool select)
{
	this->select = select;
}

void Button::Destroy()
{
	SDL_DestroyTexture(this->text);
}
