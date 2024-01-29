// TODO-HACKATHON 3-9: Create scene_settings.h and scene_settings.c.
// No need to do anything for this part. We've already done it for
// you, so this 2 files is like the default scene template.
#include "scene_settings.h"
#include "scene_menu_object.h"
#include "scene_game.h"

// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */
static Button btnCheat;
static Button btnMute;
static Button btnMusic;
static Button btnKeyArrow;
static Button btnKeyWASD;
static Button btnY;
static Button btnB;
static Button btnC;

// TODO-IF: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.
extern int key[4];

static void init(void) {
	btnCheat = button_create(100, 50, 50, 50, "Assets/checkBox.png", NULL, "Assets/checkBoxClicked.png");
	btnMute = button_create(100, 110, 50, 50, "Assets/checkBox.png", NULL, "Assets/checkBoxClicked.png");
	btnMusic = button_create(100, 170, 50, 50, "Assets/checkBox.png", NULL, "Assets/checkBoxClicked.png");
	btnKeyArrow = button_create(170, 230+60, 210, 142, "Assets/keyArrow.png", "Assets/keyArrowHovered.png", "Assets/keyArrowClicked.png");
	btnKeyWASD = button_create(470, 230+60, 210, 142, "Assets/keyWASD.png", "Assets/keyWASDHovered.png", "Assets/keyWASDClicked.png");
	btnY = button_create(170, 530-100+60, 94, 100, "Assets/pacman0.png", "Assets/pacman2.png", "Assets/pacman1.png");
	btnB = button_create(270, 530-100+60, 94, 100, "Assets/pacman3.png", "Assets/pacman5.png", "Assets/pacman4.png");
	btnC = button_create(370, 530-100+60, 94, 100, "Assets/pacman6.png", "Assets/pacman8.png", "Assets/pacman7.png");
	if (allowCheat) btnCheat.clicked = true;
	if (mute) btnMute.clicked = true;
	if (keyset) btnKeyArrow.clicked = true;
	if (changeMusic) btnMusic.clicked = true;
	else btnKeyWASD.clicked = true;
	//key[4] = { ALLEGRO_KEY_W, ALLEGRO_KEY_A, ALLEGRO_KEY_S, ALLEGRO_KEY_D };
	if (character == 1) btnB.clicked = true;
	else if (character == 2) btnC.clicked = true;
	else btnY.clicked = true;
}

static void draw(void ){
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"<ENTER> Back to menu"
	);

	// if allow cheat mode
	drawButton(btnCheat);
	al_draw_text(menuFont, al_map_rgb(255, 255, 255), 
		170, 65, ALLEGRO_ALIGN_LEFT, "Allow cheat mode");
	// if mute music
	drawButton(btnMute);
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		170, 125, ALLEGRO_ALIGN_LEFT, "Mute music");
	// if change music
	drawButton(btnMusic);
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		170, 185, ALLEGRO_ALIGN_LEFT, "Change music");
	// button for selecting key
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		170, 195+60, ALLEGRO_ALIGN_LEFT, "Select control key set");
	drawButton(btnKeyArrow);
	drawButton(btnKeyWASD);
	/*al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		170, 400, ALLEGRO_ALIGN_LEFT, "MOVE UP: W");
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		170, 440, ALLEGRO_ALIGN_LEFT, "MOVE DOWN: S");
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		470, 400, ALLEGRO_ALIGN_LEFT, "MOVE LEFT: A");
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		470, 440, ALLEGRO_ALIGN_LEFT, "MOVE RIGHT: D");*/
	// button for selecting character
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		170, 500-100+60, ALLEGRO_ALIGN_LEFT, "Select character");
	drawButton(btnY);
	drawButton(btnB);
	drawButton(btnC);
}

static void on_key_down(int keycode) {
	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
			game_change_scene(scene_menu_create());
			break;
		default:
			break;
	}
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	// update button status and utilize the function 'put in rect'
	btnCheat.hovered = buttonHover(btnCheat, mouse_x, mouse_y);
	btnMute.hovered = buttonHover(btnMute, mouse_x, mouse_y);
	btnMusic.hovered = buttonHover(btnMusic, mouse_x, mouse_y);
	btnKeyArrow.hovered = buttonHover(btnKeyArrow, mouse_x, mouse_y);
	btnKeyWASD.hovered = buttonHover(btnKeyWASD, mouse_x, mouse_y);
	btnY.hovered = buttonHover(btnY, mouse_x, mouse_y);
	btnB.hovered = buttonHover(btnB, mouse_x, mouse_y);
	btnC.hovered = buttonHover(btnC, mouse_x, mouse_y);
}

static void on_mouse_down() {
	if (btnCheat.hovered) {
		btnCheat.clicked = !btnCheat.clicked;
		if (allowCheat == NULL) allowCheat = true;
		else allowCheat = !allowCheat;
	}
	if (btnMute.hovered) {
		btnMute.clicked = !btnMute.clicked;
		if (mute == NULL) mute = true;
		else mute = !mute;
	}
	if (btnMusic.hovered) {
		btnMusic.clicked = !btnMusic.clicked;
		if (changeMusic == NULL) changeMusic = true;
		else changeMusic = !changeMusic;
	}
	if (btnKeyArrow.hovered) {
		btnKeyArrow.clicked = true;
		btnKeyWASD.clicked = false;
		keyset = 1;
	}
	if (btnKeyWASD.hovered) {
		btnKeyWASD.clicked = true;
		btnKeyArrow.clicked = false;
		keyset = 0;
	}
	if (btnY.hovered) character = 0;
	if (btnB.hovered) character = 1;
	if (btnC.hovered) character = 2;
	switch (character) {
		case 1:
			btnB.clicked = true;
			btnY.clicked = false;
			btnC.clicked = false;
			break;
		case 2:
			btnC.clicked = true;
			btnY.clicked = false;
			btnB.clicked = false;
			break;
		default:
			btnY.clicked = true;
			btnB.clicked = false;
			btnC.clicked = false;
			break;
	}
}

static void destroyButton(static Button btn) {
	al_destroy_bitmap(btn.hovered_img);
	al_destroy_bitmap(btn.default_img);
	al_destroy_bitmap(btn.clicked_img);
}

static void destroy() {
	// Destroy button images
	destroyButton(btnCheat);
	destroyButton(btnMute);
	destroyButton(btnMusic);
	destroyButton(btnKeyArrow);
	destroyButton(btnKeyWASD);
	destroyButton(btnY);
	destroyButton(btnB);
	destroyButton(btnC);
}

// The only function that is shared across files.
Scene scene_settings_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	// register on_mouse_down and on_mouse_move
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Settings scene created");
	return scene;
}

// TODO-Graphical_Widget:
// Just suggestions, you can create your own usage.
	// Selecting BGM:
	// 1. Declare global variables for storing the BGM. (see `shared.h`, `shared.c`)
	// 2. Load the BGM in `shared.c`.
	// 3. Create dropdown menu for selecting BGM in setting scene, involving `scene_settings.c` and `scene_setting.h.
	// 4. Switch and play the BGM if the corresponding option is selected.
	// Adjusting Volume:
	// 1. Declare global variables for storing the volume. (see `shared.h`, `shared.c`)
	// 2. Create a slider for adjusting volume in setting scene, involving `scene_settings.c` and `scene_setting.h.
		// 2.1. You may use checkbox to switch between mute and unmute.
	// 3. Adjust the volume and play when the button is pressed.

	// Selecting Map:
	// 1. Preload the map to `shared.c`.
	// 2. Create buttons(or dropdown menu) for selecting map in setting scene, involving `scene_settings.c` and `scene_setting.h.
		// 2.1. For player experience, you may also create another scene between menu scene and game scene for selecting map.
	// 3. Create buttons(or dropdown menu) for selecting map in setting scene, involving `scene_settings.c` and `scene_setting.h.
	// 4. Switch and draw the map if the corresponding option is selected.
		// 4.1. Suggestions: You may use `al_draw_bitmap` to draw the map for previewing. 
							// But the map is too large to be drawn in original size. 
							// You might want to modify the function to allow scaling.
