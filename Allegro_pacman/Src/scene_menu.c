#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <math.h>
#include "scene_menu_object.h"
#include "scene_settings.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"

/* Internal Variables*/
static ALLEGRO_BITMAP* gameTitle = NULL;
static ALLEGRO_SAMPLE_ID menuBGM;
static int gameTitleW ;
static int gameTitleH ;

// [HACKATHON 3]
// TARGET : use a clickable button to enter setting scene.
// For `Button` struct(object) is defined in `scene_menu_object.h` and `scene_menu_object.c`
// STRONGLY recommend you trace both of them first. 

// TODO-HACKATHON 3-1: Declare variable for button //done
// Uncomment and fill the code below
static Button btnSettings;
static Button btnhighScoreBoard;

// for high score board
char name[5][21];
int score[5];

static void init() {

	// TODO-HACKATHON 3-2: Create button to settings //done
	// Uncomment and fill the code below
	btnSettings = button_create(730, 20, 50, 50, "Assets/settings.png", "Assets/settings2.png", NULL);
	btnhighScoreBoard = button_create(730, 100, 50, 50, "Assets/scoreBoard.png", "Assets/scoreBoardH.png", NULL);

	gameTitle = load_bitmap("Assets/title.png");
	gameTitleW = al_get_bitmap_width(gameTitle);
	gameTitleH = al_get_bitmap_height(gameTitle);
	stop_bgm(menuBGM);
	if(!mute){
		if (changeMusic) themeMusic = load_audio("Assets/Music/menu_music2.ogg");
		menuBGM = play_bgm(themeMusic, music_volume);
	}
	
	if (allowCheat != true) allowCheat = false;

	// read info for high score board
	FILE* stream = NULL;
	errno_t err = fopen_s(&stream, "Assets/highScore.txt", "r");
	if (err) {
		game_log("file not open successfully");
		return;
	}
	for (int i = 0; i < 5; i++) {
		int iseof = fscanf(stream, "%s %d\n", name[i], &score[i]);
		//read till end of file
		if (iseof !=2) break;
	}
	fclose(stream);
}

//draw high score board, only display top 5 users
static void drawHighScoreBoard() {
	// 600*500 box for high score board
	al_draw_filled_rectangle(100, 150, 700, 650, al_map_rgb(0, 0, 0));
	al_draw_rectangle(100, 100, 700, 600, al_map_rgb(255, 255, 255), 15);
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		400, 150, ALLEGRO_ALIGN_CENTER, "HIGH SCORE BOARD");
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		400, 200, ALLEGRO_ALIGN_CENTER, "TOP 5 PLAYER");
	
	for (int i = 0; i < 5; i++) {
		if (score[i] != 0) {
			if (name[i])
				al_draw_textf(menuFont, al_map_rgb(255, 255, 255),
					200, 300 + 50 * i, ALLEGRO_ALIGN_LEFT, "%d: %-20s %05d", i + 1, name[i], score[i]);
			else
				al_draw_textf(menuFont, al_map_rgb(255, 255, 255),
					200, 300 + 50 * i, ALLEGRO_ALIGN_LEFT, "%d: %-20s %05d", i + 1, " \0" , score[i]);
		}

		else break;
	}
}

static void draw() {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	const float scale = 0.7;
	const float offset_w = (SCREEN_W >> 1) - 0.5 * scale * gameTitleW;
	const float offset_h = (SCREEN_H >> 1) - 0.5 * scale * gameTitleH;

	//draw title image
	al_draw_scaled_bitmap(
		gameTitle,
		0, 0,
		gameTitleW, gameTitleH,
		offset_w, offset_h,
		gameTitleW * scale, gameTitleH * scale,
		0
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"PRESS <ENTER>"
	);

	// TODO-HACKATHON 3-3: Draw button //done
	drawButton(btnSettings);
	drawButton(btnhighScoreBoard);

	if (btnhighScoreBoard.clicked) drawHighScoreBoard();

}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	//done
	//	TODO-HACKATHON 3-7: Update button's status(hovered), and utilize the function `pnt_in_rect`, which you just implemented
	//	Uncomment and fill the code below
	btnSettings.hovered = buttonHover(btnSettings, mouse_x, mouse_y);
	btnhighScoreBoard.hovered = buttonHover(btnhighScoreBoard, mouse_x, mouse_y);
}


//	TODO-HACKATHON 3-8: When btnSettings clicked, switch to settings scene //done
//  `game_change_scene` is defined in `game.h`.
//  You can check line 121 `scene_menu.c` to see how to use this function.
//  And find the corresponding function for `scene_setting`
// 	Utilize the member defined in struct `Button`.
//  The logic here is 
//  `if clicked the mouse` && `mouse position is in the area of button`
// 	      `Enter the setting scene`
//	Uncomment and fill the code below

static void on_mouse_down() {
	if (btnSettings.hovered)
		game_change_scene(scene_settings_create());
	if (btnhighScoreBoard.hovered)
		btnhighScoreBoard.clicked = !btnhighScoreBoard.clicked;
}

// use for button destroy
static void destroyButton(static Button btn) {
	al_destroy_bitmap(btn.hovered_img);
	al_destroy_bitmap(btn.default_img);
	al_destroy_bitmap(btn.clicked_img);
}

static void destroy() {
	stop_bgm(menuBGM);
	al_destroy_bitmap(gameTitle);
	//	TODO-HACKATHON 3-10: Destroy button images //done
	destroyButton(btnSettings);
	destroyButton(btnhighScoreBoard);
}

static void on_key_down(int keycode) {
	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
			if (!btnhighScoreBoard.clicked) game_change_scene(scene_main_create());
			else btnhighScoreBoard.clicked = false;
			break;
		default:
			break;
	}
}

// TODO-IF: Add more event callback functions such as update, ...

// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_menu_create(void) {

	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Menu";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_move = &on_mouse_move;
	// TODO-HACKATHON 3-9: Register on_mouse_down. //done
	// Uncomment the code below.
	scene.on_mouse_down = &on_mouse_down;
	// -------------------------------------

	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Menu scene created");
	return scene;
}