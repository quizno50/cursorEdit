#include "controls.h"

Button::Button()
{
  name.clear();
  theFont = TTF_OpenFont(DEFAULT_FONT, 10);
}

Button::~Button()
{
  name.clear();
  TTF_CloseFont(theFont);
}

bool Button::isClicked()
{
  int x;
  int y;
  Uint8 buttons;
  
  buttons = SDL_GetMouseState(&x, &y);
  if (buttons & SDL_BUTTON(1))
  {
    if (x > location.x && x < location.x + location.w)
    {
      if (y > location.y && y < location.y + location.h)
      {
        return true;
      }
    }
  }
  return false;
}

bool Button::isOver()
{
  int x;
  int y;
  
  SDL_GetMouseState(&x, &y);
  if (x > location.x && x < location.x + location.w)
  {
    if (y > location.y && y < location.y + location.h)
    {
      return true;
    }
  }
  return false;
}

void Button::setName(std::string newName)
{
  name = newName;
}

void Button::setLocation(int x, int y, int w, int h)
{
  location.x = x;
  location.y = y;
  location.w = w;
  location.h = h;
}

void Button::draw()
{
  SDL_Surface *screen;
  SDL_Surface *renderedText;
  SDL_Rect textLocal;
  SDL_Color white = {255, 255, 255};
  
  screen = SDL_GetVideoSurface();
  
  if (screen == NULL)
  {
    return;
  }
  
  box(screen, location.x, location.y, location.w, location.h, SDL_MapRGB(screen->format, 255, 255, 255));
  
  renderedText = TTF_RenderText_Blended(theFont, name.c_str(), white);
  
  textLocal.x = location.x + location.w / 2 - renderedText->w / 2;
  textLocal.y = location.y + location.h / 2 - renderedText->h / 2;
  textLocal.w = renderedText->w;
  textLocal.h = renderedText->h;
  
  SDL_BlitSurface(renderedText, NULL, screen, &textLocal);
  SDL_FreeSurface(renderedText);
}
