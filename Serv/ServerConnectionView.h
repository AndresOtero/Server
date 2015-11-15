/*
 * ServerConnectionView.h
 *
 *  Created on: 14/11/2015
 *      Author: ger
 */

#ifndef SERVERCONNECTIONVIEW_H_
#define SERVERCONNECTIONVIEW_H_

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define KILL_ALL 0
#define FLAG 1
#define KING 2
namespace std {

class ServerConnectionView {
public:
	ServerConnectionView();
	void showForm();
	void showWaitingRoom();
	int getObjetivo();
	virtual ~ServerConnectionView();

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_Font* font;

	SDL_Texture *backgroundTexture;
	SDL_Rect killAllButton;
	SDL_Rect flagButton;
	SDL_Rect kingButton;
	SDL_Rect submitButton;

	int objetivo;
	bool shouldQuit;
	bool showingWait;

	bool init();
	void quit();
	void pullChanges();

	// Click helper
	bool pointInsideRect(SDL_Point point, SDL_Rect rect);
	SDL_Texture* renderText(string text, SDL_Rect* textSize);
	void freeTexture(SDL_Texture* texture);
};

} /* namespace std */

#endif /* SERVERCONNECTIONVIEW_H_ */
