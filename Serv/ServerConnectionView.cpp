/*
 * ServerConnectionView.cpp
 *
 *  Created on: 14/11/2015
 *      Author: ger
 */

#include "ServerConnectionView.h"
#define TEXT_SPACING 20
#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 400
namespace std {

ServerConnectionView::ServerConnectionView() {
	objetivo = 0;
	this->font = NULL;
	this->renderer = NULL;
	this->window = NULL;
	this->shouldQuit = false;
	this->showingWait = false;

	this->backgroundTexture = NULL;
}

bool ServerConnectionView::init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return false;
	}

	// Create window
	window = SDL_CreateWindow("Age of the Rings", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN);
	if (window == NULL) {
		SDL_Quit();
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}

	SDL_Surface *background = IMG_Load("img/objectives.jpg");

	if (background == NULL) {
		printf("no carga imagen \n");
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}

	backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);

	if (backgroundTexture == NULL) {
		printf("no carga imagen \n");
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}

	if (TTF_Init() == -1) {
		SDL_DestroyTexture(backgroundTexture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}

	font = TTF_OpenFont("font.ttf", 16);
	if (font == NULL) {
		TTF_Quit();
		SDL_DestroyTexture(backgroundTexture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}
	return true;
}

void ServerConnectionView::quit() {
	SDL_DestroyTexture(backgroundTexture);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	font = NULL;
	renderer = NULL;
	window = NULL;
}
void ServerConnectionView::showForm() {
	if (!init()) {
		return;
	}

	SDL_Rect formTitleRect = { 0, TEXT_SPACING, 0, 0 };
	SDL_Texture* formTitle = renderText("Eligiendo objetivo de la partida...", &formTitleRect);
	formTitleRect.x = (WINDOW_WIDTH - formTitleRect.w) / 2;

	SDL_Rect formButtonkillAllRect = { TEXT_SPACING, formTitleRect.y + formTitleRect.h + TEXT_SPACING * 2, 0, 0 };
	SDL_Texture* formButtonkillAll = renderText("Kill All", &formButtonkillAllRect);
	formButtonkillAllRect.x = formButtonkillAllRect.x + formButtonkillAllRect.w;
	killAllButton = {
		formButtonkillAllRect.x - TEXT_SPACING / 2,
		formButtonkillAllRect.y - TEXT_SPACING / 2,
		formButtonkillAllRect.w + TEXT_SPACING ,
		formButtonkillAllRect.h + TEXT_SPACING
	};

	SDL_Rect formButtonFlagRect = { formButtonkillAllRect.x + formButtonkillAllRect.w, formTitleRect.y + formTitleRect.h + TEXT_SPACING * 2,
			0, 0 };
	SDL_Texture* formButtonFlag = renderText("Captura la bandera", &formButtonFlagRect);
	formButtonFlagRect.x = formButtonFlagRect.x + formButtonkillAllRect.w;
	flagButton = {
		formButtonFlagRect.x - TEXT_SPACING / 2,
		formButtonFlagRect.y - TEXT_SPACING / 2,
		formButtonFlagRect.w + TEXT_SPACING ,
		formButtonFlagRect.h + TEXT_SPACING
	};

	SDL_Rect formButtonKingRect =
			{ formButtonFlagRect.x + formButtonFlagRect.w, formTitleRect.y + formTitleRect.h + TEXT_SPACING * 2, 0, 0 };
	SDL_Texture* formButtonKing = renderText("Matar al rey", &formButtonKingRect);
	formButtonKingRect.x = formButtonKingRect.x + +formButtonkillAllRect.w;
	kingButton = {
		formButtonKingRect.x - TEXT_SPACING / 2,
		formButtonKingRect.y - TEXT_SPACING / 2,
		formButtonKingRect.w + TEXT_SPACING ,
		formButtonKingRect.h + TEXT_SPACING
	};

	SDL_Rect formButtonRect = { 0, formButtonKingRect.y + formButtonKingRect.h + 2 * TEXT_SPACING, 0, 0 };
	SDL_Texture* formButton = renderText("Levantar Servidor", &formButtonRect);
	formButtonRect.x = (WINDOW_WIDTH - formButtonRect.w) / 2;
	submitButton = {
		formButtonRect.x - TEXT_SPACING / 2,
		formButtonRect.y - TEXT_SPACING / 2,
		formButtonRect.w + TEXT_SPACING,
		formButtonRect.h + TEXT_SPACING
	};
	while (!shouldQuit) {
		// Update texts
		pullChanges();

		// Clear screen
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		// Draw
		SDL_RenderCopy(renderer, this->backgroundTexture, NULL, NULL);

		SDL_RenderCopy(renderer, formTitle, NULL, &formTitleRect);

		SDL_RenderCopy(renderer, formButtonkillAll, NULL, &formButtonkillAllRect);
		if (this->objetivo == KILL_ALL) {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x66, 0x00, 0x00);
			SDL_RenderDrawRect(renderer, &killAllButton);
		} else {
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderDrawRect(renderer, &killAllButton);
		}
		SDL_RenderCopy(renderer, formButtonFlag, NULL, &formButtonFlagRect);
		if (this->objetivo == FLAG) {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x66, 0x00, 0x00);
			SDL_RenderDrawRect(renderer, &flagButton);
		} else {
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderDrawRect(renderer, &flagButton);
		}
		SDL_RenderCopy(renderer, formButtonKing, NULL, &formButtonKingRect);
		if (this->objetivo == KING) {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x66, 0x00, 0x00);
			SDL_RenderDrawRect(renderer, &kingButton);
		} else {
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderDrawRect(renderer, &kingButton);
		}

		SDL_RenderCopy(renderer, formButton, NULL, &formButtonRect);
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderDrawRect(renderer, &submitButton);

		// Update screen
		SDL_RenderPresent(renderer);

		SDL_Delay(50);
	}

	// Free memory

	freeTexture(formButtonkillAll);
	freeTexture(formButtonFlag);
	freeTexture(formButtonKing);
	freeTexture(formButton);
	// Stop capturing the keyboard events
	SDL_StopTextInput();

	quit();
}
ServerConnectionView::~ServerConnectionView() {
}
void ServerConnectionView::pullChanges() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {

		if (e.type == SDL_QUIT) {
			shouldQuit = true;
			return;
		}
		if (!showingWait) {
			// Detect what is clicking
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				// Get mouse position
				SDL_Point clickPoint = { 0, 0 };
				SDL_GetMouseState(&clickPoint.x, &clickPoint.y);

				if (pointInsideRect(clickPoint, killAllButton)) {
					this->objetivo = KILL_ALL;
				} else if (pointInsideRect(clickPoint, flagButton)) {
					this->objetivo = FLAG;
				} else if (pointInsideRect(clickPoint, kingButton)) {
					this->objetivo = KING;
				} else if (pointInsideRect(clickPoint, submitButton)) {
					shouldQuit = true;
				}
			}
		}
	}
}
void ServerConnectionView::freeTexture(SDL_Texture* texture) {
	if (texture != NULL) {
		SDL_DestroyTexture(texture);
	}
}

SDL_Texture* ServerConnectionView::renderText(string text, SDL_Rect* textSize) {
	SDL_Color textColor = { 0, 0, 0 };
	SDL_Surface* surfaceText = TTF_RenderText_Solid(font, text.c_str(), textColor);
	if (surfaceText == NULL) {
		// Error => sin surface
		return NULL;
	}
	textSize->w = surfaceText->w;
	textSize->h = surfaceText->h;

	SDL_Texture* renderedText = SDL_CreateTextureFromSurface(renderer, surfaceText);
	SDL_FreeSurface(surfaceText);
	if (renderedText == NULL) {
		// Error => sin texto
		textSize->w = 0;
		textSize->h = 0;
	}
	return renderedText;
}
bool ServerConnectionView::pointInsideRect(SDL_Point point, SDL_Rect rect) {
	return (rect.x <= point.x && rect.x + rect.w >= point.x && rect.y <= point.y && rect.y + rect.h >= point.y);
}
int ServerConnectionView::getObjetivo() {
	return this->objetivo;
}

void ServerConnectionView::showWaitingRoom() {
	if (!init()) {
		return;
	}

	SDL_Rect formTitleRect = { 0, TEXT_SPACING, 0, 0 };
	SDL_Texture* formTitle = renderText("Esperando conexion de los clientes...", &formTitleRect);
	formTitleRect.x = (WINDOW_WIDTH - formTitleRect.w) / 2;



	SDL_Rect formButtonRect = { 0, formTitleRect.y + formTitleRect.h + 2 * TEXT_SPACING, 0, 0 };
		SDL_Texture* formButton = renderText("Comenzar Partida", &formButtonRect);
		formButtonRect.x = (WINDOW_WIDTH - formButtonRect.w) / 2;
		submitButton = {
			formButtonRect.x - TEXT_SPACING / 2,
			formButtonRect.y - TEXT_SPACING / 2,
			formButtonRect.w + TEXT_SPACING,
			formButtonRect.h + TEXT_SPACING
		};
	while (!shouldQuit) {
			// Update texts
			pullChanges();

			// Clear screen
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(renderer);

			// Draw
			SDL_RenderCopy(renderer, this->backgroundTexture, NULL, NULL);

			SDL_RenderCopy(renderer, formTitle, NULL, &formTitleRect);



			SDL_RenderCopy(renderer, formButton, NULL, &formButtonRect);
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderDrawRect(renderer, &submitButton);

			// Update screen
			SDL_RenderPresent(renderer);

			SDL_Delay(50);
		}

		// Free memory
		freeTexture(formButton);
		// Stop capturing the keyboard events
		SDL_StopTextInput();

		quit();
}

}
