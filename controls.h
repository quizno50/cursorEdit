#ifndef CONTROLS_H
#define CONTROLS_H

#include <SDL.h>
#include <string>
#include <SDL_ttf.h>
#include "graphics.h"

#define DEFAULT_FONT "visitor1.ttf"

class Button
{
  public:
    Button();
    ~Button();
    bool isClicked();
    bool isOver();
    void setName(std::string newName);
    void setLocation(int, int, int, int);
    void draw();
    
  private:
    std::string name;
    SDL_Rect location;
    TTF_Font *theFont;
};

#endif
