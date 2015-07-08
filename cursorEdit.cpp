#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdio>

#include "SDL.h"
#include "SDL_ttf.h"
#include "controls.h"

#define PIXEL_WIDTH 15
#define PIXEL_HEIGHT 15

#define SCREEN_X 0
#define SCREEN_Y 1
#define CURSOR_X 2
#define CURSOR_Y 3

using namespace std;

SDL_Rect *tempRect(int x, int y, unsigned int w, unsigned int h)
{
   static SDL_Rect tempRect;
   tempRect.x = x;
   tempRect.y = y;
   tempRect.w = w;
   tempRect.h = h;
   return &tempRect;
}

void swap(int *a, int *b)
{
   *a = *a ^ *b;
   *b = *a ^ *b;
   *a = *a ^ *b;
}

void setPixel(SDL_Surface *screen, int x, int y, Uint32 c)
{
  Uint8 *ubuff8;
  Uint16 *ubuff16;
  Uint32 *ubuff32;
  Uint32 color;
  char c1, c2, c3;
  
  
  //Ron Moore - addition: prevent out of bounds pointers
  if (x > screen->w || x < 0)
  {
	return;
  }
  
  if (y > screen->h || y < 0)
  {
	return;
  }
  //Ron Moore - end addition
  
  /* Lock the screen, if needed */
  if(SDL_MUSTLOCK(screen)) {
    if(SDL_LockSurface(screen) < 0) 
      return;
  }
  
  /* Get the color */
  color = c;
  
  /* How we draw the pixel depends on the bitdepth */
  switch(screen->format->BytesPerPixel) 
    {
    case 1: 
      ubuff8 = (Uint8*) screen->pixels;
      ubuff8 += (y * screen->pitch) + x; 
      *ubuff8 = (Uint8) color;
      break;

    case 2:
      ubuff8 = (Uint8*) screen->pixels;
      ubuff8 += (y * screen->pitch) + (x*2);
      ubuff16 = (Uint16*) ubuff8;
      *ubuff16 = (Uint16) color; 
      break;  

    case 3:
      ubuff8 = (Uint8*) screen->pixels;
      ubuff8 += (y * screen->pitch) + (x*3);
      

      if(SDL_BYTEORDER == SDL_LIL_ENDIAN) {
	c1 = (color & 0xFF0000) >> 16;
	c2 = (color & 0x00FF00) >> 8;
	c3 = (color & 0x0000FF);
      } else {
	c3 = (color & 0xFF0000) >> 16;
	c2 = (color & 0x00FF00) >> 8;
	c1 = (color & 0x0000FF);	
      }

      ubuff8[0] = c3;
      ubuff8[1] = c2;
      ubuff8[2] = c1;
      break;
      
    case 4:
      ubuff8 = (Uint8*) screen->pixels;
      ubuff8 += (y*screen->pitch) + (x*4);
      ubuff32 = (Uint32*)ubuff8;
      *ubuff32 = color;
      break;
      
    default:
      fprintf(stderr, "Error: Unknown bitdepth!\n");
    }
  
  /* Unlock the screen if needed */
  if(SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }
}

void line(SDL_Surface *surface, int x0, int y0, int x1, int y1, Uint32 c)
{
   char steep = (abs(y1 - y0) > abs(x1 - x0));
   if (steep)
   {
      swap(&x0, &y0);
      swap(&x1, &y1);
   }
   if (x0 > x1)
   {
      swap(&x0, &x1);
      swap(&y0, &y1);
   }
   int deltax = x1 - x0;;
   int deltay = abs(y1 - y0);
   int error = 0;
   int ystep;
   int y = y0;
   int x;
   if (y0 < y1)
      ystep = 1;
   else
      ystep = -1;
   for (x = x0; x <= x1; ++x)
   {
      if (steep)
      {
         setPixel(surface, y, x, c);
      }
      else
      {
         setPixel(surface, x, y, c);
      }
      error += deltay;
      if (2 * error > deltax)
      {
         y += ystep;
         error -= deltax;
      }
   }
}

void box(SDL_Surface* screen, int x, int y, int w, int h, Uint32 c)
{
	SDL_Rect baseRect;
	SDL_Surface *tempSurface;
	int cx;
	int cy;
	
	baseRect.x = x;
	baseRect.y = y;
	baseRect.w = w;
	baseRect.h = h;
	
	for (cx = 0; cx <= baseRect.w; ++cx)
	{
		setPixel(screen, x + cx, y, c);
		setPixel(screen, x + cx, y + h, c);
	}
	
	for (cy = 0; cy <= baseRect.h; ++cy)
	{
		setPixel(screen, x, y + cy, c);
		setPixel(screen, x + w, y + cy, c);
	}
}

class Cursor
{
   public:
      Cursor();
      void save(string filename);
      void load(string filename);
      void clear();
      void changeValueUp(int x, int y);
      void changeValueDown(int x, int y);
      void draw(Uint8 *mouseOver);
      SDL_Cursor *convert();
      void setHotSpot(int, int);
      
   private:
      SDL_Cursor *theCursor;
      Uint8 pixelData[32][32];
      SDL_Surface *images[4];
      Uint8 hotSpotX;
      Uint8 hotSpotY;
};

void Cursor::setHotSpot(int x, int y)
{
  hotSpotX = x;
  hotSpotY = y;
}

Cursor::Cursor()
{
   SDL_Surface *temp;
   for (int y = 0; y < 32; ++y)
   {
      for (int x = 0; x < 32; ++x)
      {
         pixelData[x][y] = 0;
      }
   }
   
   temp = SDL_LoadBMP("0.bmp");
   if (temp != NULL)
   {
      images[0] = SDL_DisplayFormat(temp);
      SDL_FreeSurface(temp);
   }
   temp = SDL_LoadBMP("1.bmp");
   if (temp != NULL)
   {
      images[1] = SDL_DisplayFormat(temp);
      SDL_FreeSurface(temp);
   }
   temp = SDL_LoadBMP("2.bmp");
   if (temp != NULL)
   {
      images[2] = SDL_DisplayFormat(temp);
      SDL_FreeSurface(temp);
   }
   temp = SDL_LoadBMP("3.bmp");
   if (temp != NULL)
   {
      images[3] = SDL_DisplayFormat(temp);
      SDL_FreeSurface(temp);
   }
}

void Cursor::clear()
{
   for (int y = 0; y < 32; ++y)
   {
      for (int x = 0; x < 32; ++x)
      {
         pixelData[x][y] = 0;
      }
   }
}

void Cursor::changeValueUp(int x, int y)
{
   ++pixelData[x][y];
   if (pixelData[x][y] > 3) pixelData[x][y] = 0;
   draw(NULL);
}

void Cursor::changeValueDown(int x, int y)
{
   --pixelData[x][y];
   if (pixelData[x][y] > 3) pixelData[x][y] = 3;
   draw(NULL);
}

void Cursor::draw(Uint8 *mouseOver)
{
   SDL_Surface *screen;
   screen = SDL_GetVideoSurface();
   
   for (int y = 0; y < 32; ++y)
   {
      for (int x = 0; x < 32; ++x)
      {
         if (mouseOver != NULL)
            SDL_FillRect(screen, 
               tempRect(x * (PIXEL_WIDTH), y * (PIXEL_HEIGHT),
               PIXEL_WIDTH, PIXEL_HEIGHT),
               mouseOver[CURSOR_X] == x && mouseOver[CURSOR_Y] == y ? 
               SDL_MapRGB(screen->format, 255, 0, 0) : 
               SDL_MapRGB(screen->format, 255, 255, 255) );
         else
            SDL_FillRect(screen, 
               tempRect(x * (PIXEL_WIDTH), y * (PIXEL_HEIGHT),
               PIXEL_WIDTH, PIXEL_HEIGHT),
               SDL_MapRGB(screen->format, 255, 255, 255));
         SDL_BlitSurface(images[pixelData[x][y]], NULL, screen,
            tempRect(x * PIXEL_WIDTH + 1, y * PIXEL_HEIGHT + 1,
            PIXEL_WIDTH - 2, PIXEL_HEIGHT - 2));
      }
   }
   line(screen, hotSpotX * PIXEL_WIDTH, hotSpotY * PIXEL_HEIGHT,
      hotSpotX * PIXEL_WIDTH + PIXEL_WIDTH, 
      hotSpotY * PIXEL_HEIGHT + PIXEL_HEIGHT,
      SDL_MapRGB(screen->format, 255, 255, 255));
   line(screen, hotSpotX * PIXEL_WIDTH + PIXEL_WIDTH, hotSpotY * PIXEL_HEIGHT,
      hotSpotX * PIXEL_WIDTH, hotSpotY * PIXEL_HEIGHT + PIXEL_HEIGHT,
      SDL_MapRGB(screen->format, 255, 255, 255));
}

void Cursor::save(string filename)
{
  ofstream destFile;
  destFile.open(filename.c_str());
  
  if (destFile.good())
  {
    for (int y = 0; y < 32; ++y)
    {
      for (int x = 0; x < 32; ++x)
      {
        destFile.put(pixelData[x][y]);
      }
    }
    destFile.flush();
    destFile.close();
  }
}

void Cursor::load(string filename)
{
  ifstream srcFile;
  srcFile.open(filename.c_str());
  
  if (srcFile.good())
  {
    for (int y = 0; y < 32; ++y)
    {
      for (int x = 0; x < 32; ++x)
      {
        srcFile.get((char&)pixelData[x][y]);
      }
    }
    srcFile.close();
  }
  draw(NULL);
}

SDL_Cursor *Cursor::convert()
{
  SDL_Cursor *returnValue;         //what we'll be returning
  Uint8 cursor[32][32];            //the binary cursor data
  Uint8 mask[32][32];              //the binary mask data
  
  int newByte[2] = {0, 0};         //the byte to add to the final array for cursor and mask
  int bitCount = 7;                //what bit we're on, start high because we have to go backwards?
  int byteCount = 0;               //which byte in the final array we're on.
  
  Uint8 *maskFinal;                //the final data to pass to SDL_CreateCursor()
  Uint8 *cursorFinal;              //the final data to pass to SDL_CreateCursor()
  
  for (int y = 0; y <= 32; ++y)     //start looping through all the y values
  {
    for (int x = 0; x < 32; ++x)   //start looping through all the x values
    {
      switch (pixelData[x][y])     //check which kind of point we have
      {
        case 0:                    //transparent
          cursor[x][y] = 0;
          mask[x][y] = 0;
          break;
        case 1:                    //inverted
          cursor[x][y] = 1;
          mask[x][y] = 0;
          break;
        case 2:                    //solid black
          cursor[x][y] = 1;
          mask[x][y] = 1;
          break;
        case 3:                    //solid white
          cursor[x][y] = 0;
          mask[x][y] = 1;
          break;
        default:                   //someone loaded an invalid cursor
          cerr << "Something broke at: " << x << ", " << y << "\n";
          break;
      }
    }
  }
  
  maskFinal = new Uint8[32 * 32 / 8 + 1];     //alloc space for the final array
  cursorFinal = new Uint8[32 * 32 / 8 + 1];   //see above
  
  // IN newByte ARRAY:
  // 0 is cursor data
  // 1 is mask
  
  for (int y = 0; y < 32; ++y)     //looping through Y values again
  {
    for (int x = 0; x < 32; ++x)   //looping through X values again
    {
      if (bitCount < 0)            //if we're done with this byte
      {
        maskFinal[byteCount] = newByte[1];  //set the byte in the final array
        cursorFinal[byteCount] = newByte[0];//set the byte in the final array
        byteCount++;                        //goto next byte
        bitCount = 7;                       //reset bitCount
        newByte[0] = 0;                     //reset our newByte
        newByte[1] = 0;                     //reset our newByte
      }
      //next two lines set the bits in the newByte
      newByte[0] += (Uint8)pow((double)2, (double)bitCount) * cursor[x][y];
      newByte[1] += (Uint8)pow((double)2, (double)bitCount) * mask[x][y];
      --bitCount;                           //goto next (previous?) bit
    }
  }
  //make the cursor
  returnValue = SDL_CreateCursor(cursorFinal, maskFinal, 32, 31, hotSpotX, hotSpotY);
  
  //clean up
  delete[] maskFinal;
  delete[] cursorFinal;
  
  //done
  return returnValue;
}

int main(int argc, char **argv)
{
   SDL_Surface *screen;
   SDL_Surface *toolboxFrame;
   SDL_Event newEvent;
   Cursor *mainCursor;
   Uint8 running = 1;
   Uint8 mouseOver[4];
   SDL_Cursor *theCursor = NULL;
   Uint8 settingHotSpot = false;
   
   TTF_Init();
   
   Button buttons[5];
   
   buttons[0].setName("Save (F2)");
   buttons[0].setLocation(500, 32, 290, 70);
   buttons[1].setName("Load (F3)");
   buttons[1].setLocation(500, 134, 290, 70);
   buttons[2].setName("Test (F4)");
   buttons[2].setLocation(500, 236, 290, 70);
   buttons[3].setName("Reset (F5)");
   buttons[3].setLocation(500, 338, 290, 70);
   buttons[4].setName("Set HotSpot (F6)");
   buttons[4].setLocation(500, 440, 290, 70);
   
   SDL_Init(SDL_INIT_VIDEO);
   
   screen = SDL_SetVideoMode(800, 600, 32, SDL_DOUBLEBUF);
   
   if (screen == NULL)
   {
      cout << "Display didn't happen...\n";
      exit(1);
   }
   
   mainCursor = new Cursor;
   
   while (running)
   {
      SDL_WaitEvent(&newEvent);
      if (newEvent.type == SDL_QUIT)
      {
         running = 0;
      }
      else if (newEvent.type == SDL_MOUSEMOTION)
      {
         mouseOver[CURSOR_X] = newEvent.motion.x / PIXEL_WIDTH;
         mouseOver[CURSOR_Y] = newEvent.motion.y / PIXEL_HEIGHT;
      }
      else if (newEvent.type == SDL_MOUSEBUTTONDOWN)
      {
         mouseOver[CURSOR_Y] = newEvent.button.y / PIXEL_HEIGHT;
         mouseOver[CURSOR_X] = newEvent.button.x / PIXEL_WIDTH;
         if (!settingHotSpot)
         {
           if (newEvent.button.x < 32 * PIXEL_WIDTH && newEvent.button.y < 32 * PIXEL_HEIGHT)
           {
             if (newEvent.button.button == SDL_BUTTON_LEFT)
                mainCursor->changeValueUp(mouseOver[CURSOR_X], mouseOver[CURSOR_Y]);
             if (newEvent.button.button == SDL_BUTTON_RIGHT)
                mainCursor->changeValueDown(mouseOver[CURSOR_X], mouseOver[CURSOR_Y]);
           }
         }
         else
         {
            mainCursor->setHotSpot(mouseOver[CURSOR_X], mouseOver[CURSOR_Y]);
            settingHotSpot = false;
         }
      }
      else if (newEvent.type == SDL_KEYDOWN)
      {
        if (newEvent.key.keysym.sym == SDLK_F2)
        {
          mainCursor->save("default.cur");
        }
        else if (newEvent.key.keysym.sym == SDLK_F3)
        {
          mainCursor->load("default.cur");
        }
        else if (newEvent.key.keysym.sym == SDLK_F4)
        {
          if (theCursor != NULL)
          {
            SDL_FreeCursor(theCursor);
            theCursor = NULL;
          }
          theCursor = mainCursor->convert();
          SDL_SetCursor(theCursor);
        }
        else if (newEvent.key.keysym.sym == SDLK_F5)
        {
          SDL_FreeCursor(theCursor);
          theCursor = NULL;
        }
        else if (newEvent.key.keysym.sym == SDLK_F6)
        {
          settingHotSpot = true;
        }
      }
      
      SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
      
      for (int i = 0; i < 5; ++i)
      {
        if (buttons[i].isClicked() == true)
        {
          switch (i)
          {
            case 0:
              mainCursor->save("default.cur");
              break;
            case 1:
              mainCursor->load("default.cur");
              break;
            case 2:
              if (theCursor != NULL)
              {
                SDL_FreeCursor(theCursor);
                theCursor = NULL;
              }
              theCursor = mainCursor->convert();
              SDL_SetCursor(theCursor);
              break;
            case 3:
              SDL_FreeCursor(theCursor);
              theCursor = NULL;
              break;
            case 4:
              settingHotSpot = true;
              break;
            default:
              cerr << "Something's wrong...\n";
              break;
          }
        }
        buttons[i].draw();
      }
      mainCursor->draw(mouseOver);
      SDL_Flip(screen);
   }
   
   SDL_FreeCursor(theCursor);
   SDL_Quit();
   return 0;
}
