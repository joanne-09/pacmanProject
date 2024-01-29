#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"


// TODO-HACKATHON 2-0: Create one ghost //done
// Just modify the GHOST_NUM to 1
#define GHOST_NUM 4
// TODO-GC-ghost: create a least FOUR ghost! //done
/* global variables*/
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
int game_main_Score = 0;
bool game_over = false;
// select if allow cheat mode
extern bool allowCheat;
// select if mute the game
extern bool mute;
// change music
extern bool changeMusic;
// set different key
extern int keyset;
extern int key[4];
// set different character
extern int character;

/* Internal variables*/
static ALLEGRO_TIMER* power_up_timer;
static const int power_up_duration = 10;
static Pacman* pman;
static Pacman* pman2;
static Map* basic_map;
static Ghost** ghosts;
int wasd[4] = { ALLEGRO_KEY_W, ALLEGRO_KEY_A, ALLEGRO_KEY_S, ALLEGRO_KEY_D };
int arrow[4] = { ALLEGRO_KEY_UP, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_RIGHT };
bool debug_mode = false;
bool cheat_mode = false;
bool allowBlockCross = false;
// check if user have entered the name
bool allowChangeScene = false;
// variables for notification
static int64_t notifyTimer;
bool ifNotify = false;
char* notifyStr;
// varaible for second_pman
bool enablePman2 = false;
bool pman2Die = false;

// variables for text box
char nameSet[21];
static int offset = 0;
//FILE* highScoreFile;

/* Declare static function prototypes */
static void init(void);
static void step(void);
static void pmanCheckItem(Pacman*);
static void status_update(void);
static void update(void);
static void draw(void);
static void notify(char* nStr);
static void printinfo(void);
static void destroy(void);
static void on_key_down(int key_code);
static void on_mouse_down(int btn, int x, int y, int dz);
static void render_init_screen(void);
static void draw_hitboxes(void);
// for high score board
static void drawTextBox(void);
static void writeHighScore(void);
static void sortScore(void);

static void init(void) {
	game_over = false;
	cheat_mode = false;
	game_main_Score = 0;

	if (keyset) for(int i=0; i<4; i++) key[i]=arrow[i];
	else for(int i=0; i<4; i++) key[i]=wasd[i];

	// create map
	//basic_map = create_map(NULL);
	// TODO-GC-read_txt: Create map from .txt file so that you can design your own map!! //done
	basic_map = create_map("Assets/map_nthu.txt");
	if (!basic_map) {
		game_abort("error on creating map");
	}

	// create pacman
	pman = pacman_create();
	if (!pman) game_abort("error on creating pacamn\n");
	pman2 = pacman_create();
	pman2->addtional = true;
	if (!pman2) game_abort("error on creating pacman2\n");
	
	// allocate ghost memory
	// TODO-HACKATHON 2-1: Allocate dynamic memory for ghosts array. //done
	ghosts = (Ghost**)malloc(sizeof(Ghost*) * GHOST_NUM);
	if(!ghosts){
		game_log("We haven't create any ghosts!\n");
	}
	else {
		// TODO-HACKATHON 2-2: create a ghost. //done
		// Try to look the definition of ghost_create in ghost.h and figure out what should be placed here.
		for (int i = 0; i < GHOST_NUM; i++) {
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);  
			if (!ghosts[i])
				game_abort("error creating ghost\n");
		}
	}
	GAME_TICK = 0;

	render_init_screen();
	power_up_timer = al_create_timer(1.0f); // 1 tick per second
	if (!power_up_timer)
		game_abort("Error on create timer\n");

	//reset some variables
	allowChangeScene = false;
	enablePman2 = false;
	pman2Die = false;
	nameSet[0] = '\0';
	offset = 0;
	return ;
}

static void step(void) {
	if (pman->objData.moveCD > 0)
		pman->objData.moveCD -= pman->speed;
	if (enablePman2 && pman2->objData.moveCD > 0)
		pman2->objData.moveCD -= pman2->speed;
	for (int i = 0; i < GHOST_NUM; i++) {
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}
static void pmanCheckItem(Pacman* pman) {
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	bool isItem = false;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;
	// TODO-HACKATHON 1-3: check which item you are going to eat and use `pacman_eatItem` to deal with it. //done
	switch (basic_map->map[Grid_y][Grid_x]){
		case '.':
			if(!mute) pacman_eatItem(pman, '.');
			game_main_Score++;
			isItem = true;
			break;
		case 'P':
			// TODO-GC-PB: ease power bean //probably_done
			pman->powerUp = true;
			//set notify
			notify("Power mode on");
			if(!mute) pacman_eatItem(pman, 'P');
			game_main_Score++;

			// stop and reset power_up_timer value
			al_stop_timer(power_up_timer);
			power_up_timer = al_create_timer(1.0f);
			if (!power_up_timer)
				game_abort("Error on create timer\n");
			// start the timer
			al_start_timer(power_up_timer);
			isItem = true;
			break;
		default:
			break;
	}
	
	// TODO-HACKATHON 1-4: erase the item you eat from map //done
	// Be careful, don't erase the wall block.
	if(isItem) basic_map->map[Grid_y][Grid_x]=' ';
	
}
static void status_update(void) {
	// TODO-PB: check powerUp duration //probably_done
	if (pman->powerUp || pman2->powerUp){
		// Check the value of power_up_timer //emmmm copilot does it, but i don't know if it works
		// If runs out of time reset all relevant variables and ghost's status
		// hint: ghost_toggle_FLEE
		if(al_get_timer_count(power_up_timer) > power_up_duration){
			pman->powerUp = false;
			pman2->powerUp = false;
			al_stop_timer(power_up_timer);
			for (int i = 0; i < GHOST_NUM; i++) {
				ghost_toggle_FLEE(ghosts[i], false);
			}
		}else{
			for (int i = 0; i < GHOST_NUM; i++) ghost_toggle_FLEE(ghosts[i], true);
		}
	}

	for (int i = 0; i < GHOST_NUM; i++) {
		if (ghosts[i]->status == GO_IN){
			continue;
		}else if (ghosts[i]->status == FREEDOM) {
			// TODO-GC-game_over: use `getDrawArea(..., GAME_TICK_CD)` and `RecAreaOverlap(..., GAME_TICK_CD)` functions to detect if pacman and ghosts collide with each other.
			// And perform corresponding operations.
			// [NOTE] You should have some if-else branch here if you want to implement power bean mode.
			const RecArea parea = getDrawArea((object*)pman, GAME_TICK_CD);
			const RecArea garea = getDrawArea((object*)ghosts[i], GAME_TICK_CD);
			
			if(/*!cheat_mode &&*/ RecAreaOverlap(&parea, &garea)) {
				game_log("collide with ghost\n");
				al_rest(1.0);
				// only to play sound
				if(!mute) pacman_die();
				game_over = true;
				break; // animation shouldn't be trigger twice.
			}
			// another_pman
			if (enablePman2) {
				const RecArea parea2 = getDrawArea((object*)pman2, GAME_TICK_CD);
				if (RecAreaOverlap(&parea2, &garea)) {
					al_rest(1.0);
					if(!mute) pacman_die();
					game_log("additional pman collide with ghost\n");
					pman2Die = true;
					break;
				}
			}
		}else if (ghosts[i]->status == FLEE){
			// TODO-GC-PB: if ghost is collided by pacman, it should go back to the room immediately and come out after a period.
			//pacman_eatItem(pman, 'G');
			const RecArea parea = getDrawArea((object*)pman, GAME_TICK_CD);
			const RecArea garea = getDrawArea((object*)ghosts[i], GAME_TICK_CD);

			if(/*!cheat_mode && */ RecAreaOverlap(&parea, &garea)) {
				game_main_Score += 10;
				ghost_collided(ghosts[i]);
			}
			if (enablePman2) {
				const RecArea parea2 = getDrawArea((object*)pman2, GAME_TICK_CD);
				if (RecAreaOverlap(&parea2, &garea)) {
					game_main_Score += 10;
					ghost_collided(ghosts[i]);
				}
			}
		}
	}
}

static void update(void) {
	//handle when main pman die
	if (game_over) {
		// TODO-GC-game_over: start pman->death_anim_counter and schedule a game-over event (e.g change scene to menu) after Pacman's death animation finished
		// hint: refer al_get_timer_started(...), al_get_timer_count(...), al_stop_timer(...), al_rest(...)
		
		// start the timer if it hasn't been started.
		// check timer counter.
		// stop the timer if counter reach desired time.
		if (!al_get_timer_started(pman->death_anim_counter)) {
			pman->death_anim_counter = al_create_timer(1.0f / 8.0f);
			if(enablePman2) pman2->death_anim_counter = al_create_timer(1.0f / 8.0f);
			if (!pman->death_anim_counter)
				game_abort("Error on create timer\n");
			al_start_timer(pman->death_anim_counter);
			if (enablePman2) al_start_timer(pman2->death_anim_counter);
		}
		
		//go back to menu scene
		if (al_get_timer_started(pman->death_anim_counter) && al_get_timer_count(pman->death_anim_counter) > 12) {
			if (allowChangeScene) {
				// write the name and the score in the file
				writeHighScore();
				//change scene to menu
				al_stop_timer(pman->death_anim_counter);
				game_change_scene(scene_menu_create());
			}
		}
		
		return;
	}
	//handle when additional pman die
	else if (pman2Die && enablePman2) {
		// start the timer if it hasn't been started.
		// check timer counter.
		// stop the timer if counter reach desired time.
		if (!al_get_timer_started(pman2->death_anim_counter)) {
			pman2->death_anim_counter = al_create_timer(1.0f / 8.0f);
			if (!pman2->death_anim_counter)
				game_abort("Error on create timer\n");
			al_start_timer(pman2->death_anim_counter);
		}
		if (al_get_timer_started(pman2->death_anim_counter) && al_get_timer_count(pman2->death_anim_counter) > 12) {
			al_stop_timer(pman2->death_anim_counter);
			pman2Die = false;
			enablePman2 = false;
		}

		return;
	}

	step();
	pmanCheckItem(pman);
	if(enablePman2) pmanCheckItem(pman2);
	status_update();
	pacman_move(pman, basic_map, allowBlockCross);
	if(enablePman2) pacman_move(pman2, basic_map, allowBlockCross);
	for (int i = 0; i < GHOST_NUM; i++) 
		ghosts[i]->move_script(ghosts[i], basic_map, pman);
}

// TODO-ADVANCED: add a textbox to allow user to enter their name //probably_done
static void drawTextBox() {
	// 600*400 box for entering name
	al_draw_filled_rectangle(100, 150, 700, 550, al_map_rgb(0, 0, 0));
	al_draw_rectangle(100, 150, 700, 550, al_map_rgb(255, 255, 255), 15);
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		400, 250, ALLEGRO_ALIGN_CENTER, "Please enter your name:");
	al_draw_text(menuFont, al_map_rgb(255, 255, 255),
		400, 450, ALLEGRO_ALIGN_CENTER, "Press <ENTER> to continue");
	// texting box
	al_draw_rectangle(150, 300, 650, 400, al_map_rgb(255, 255, 255), 10);
	// draw character
	if (nameSet[0]!='\0') {
		nameSet[offset] = '\0';
		al_draw_textf(menuFont, al_map_rgb(255, 255, 255),
			180, 340, ALLEGRO_ALIGN_LEFT, "%s", nameSet);
	}
}

char name[1000][21];
int score[1000];
int lines;
static void writeHighScore() {
	// write the name and the score in the file
	FILE* highScoreFile=NULL;
	
	int err = fopen_s(&highScoreFile, "Assets/highScore.txt", "r+");
	if (err==0) {
		//TODO-ADVANCED-high_score_board //probably_done //still have a lot to improve
		if (offset==0) strcpy_s(name[0], 21, nameSet);
		else strcpy_s(name[0], 21, "Player");
		score[0] = game_main_Score;
		for (int i = 1;  i < 1000; ++i) {
			int iseof = fscanf(highScoreFile, "%s %d\n", name[i], &score[i]);
			lines = i;
			//read till end of file
			if (score[i]==0) break;
		}
		sortScore();
		fclose(highScoreFile);

		int err = fopen_s(&highScoreFile, "Assets/highScore.txt", "w");
		for (int i = 0; i < 1000; i++) {
			if (score[i] == 0) break;
			fprintf(highScoreFile, "%s %d\n", name[i], score[i]);
		}
	}else {
		errno_t err = fopen_s(&highScoreFile, "Assets/highScore.txt", "w+");
		if(nameSet) fprintf(highScoreFile, "%s %d\n", nameSet , game_main_Score);
		else fprintf(highScoreFile, "Player %d\n", game_main_Score);
	}
	fclose(highScoreFile);
}

static void sortScore() {
	for (int i = 0; i < 1000; i++) {
		if(score[i]==0) break;
		for (int j = i + 1; j < 1000; j++) {
			if (score[i] < score[j]) {
				int temp = score[i];
				score[i] = score[j];
				score[j] = temp;
				char tempName[21];
				strcpy_s(tempName, 21, name[i]);
				strcpy_s(name[i], 21, name[j]);
				strcpy_s(name[j], 21, tempName);
			}
		}
	}
}

static void draw(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	// TODO-GC-scoring: Draw scoreboard, something your may need is sprinf();
	al_draw_textf(menuFont, al_map_rgb(255, 255, 255),
		40, 30, ALLEGRO_ALIGN_LEFT, "SCORE: %05d", game_main_Score);

	draw_map(basic_map);

	pacman_draw(pman);
	if (enablePman2) pacman_draw(pman2);

	//game notification
	if (al_get_timer_count(game_tick_timer) - notifyTimer < 256 && ifNotify) {
		al_draw_text(menuFont, al_map_rgb(255, 255, 0),
			SCREEN_W - 30, 30,
			ALLEGRO_ALIGN_RIGHT, notifyStr
		);
	}else ifNotify = false;

	//TODO-ADVANCED: add a textbox to allow user to enter their name
	if (game_over && al_get_timer_started(pman->death_anim_counter) && al_get_timer_count(pman->death_anim_counter) > 12)
		drawTextBox();

	if (game_over)
		return;

	// no drawing below when game over
	for (int i = 0; i < GHOST_NUM; i++)
		ghost_draw(ghosts[i]);
	
	//debugging mode
	if (debug_mode) {
		draw_hitboxes();
	}
}

//set notification mode and set up string
static void notify(char *nStr) {
	notifyTimer = al_get_timer_count(game_tick_timer);
	ifNotify = true;
	notifyStr = nStr;
}

static void draw_hitboxes(void) {
	RecArea pmanHB = getDrawArea((object *)pman, GAME_TICK_CD);
	al_draw_rectangle(
		pmanHB.x, pmanHB.y,
		pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
		al_map_rgb_f(1.0, 0.0, 0.0), 2
	);

	for (int i = 0; i < GHOST_NUM; i++) {
		RecArea ghostHB = getDrawArea((object *)ghosts[i], GAME_TICK_CD);
		al_draw_rectangle(
			ghostHB.x, ghostHB.y,
			ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2
		);
	}

}

static void printinfo(void) {
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}

static void destroy(void) {
	pacman_destroy(pman);
	pacman_destroy(pman2);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_destroy(ghosts[i]);
	}
	delete_map(basic_map);
	//free(notifyStr);
}

int preKey;
static void on_key_down(int key_code) {
	allowBlockCross = false;
	bool ctrlPressed = (preKey==ALLEGRO_KEY_LCTRL) || (preKey==ALLEGRO_KEY_RCTRL);
	bool shiftPressed = (preKey==ALLEGRO_KEY_LSHIFT) || (preKey==ALLEGRO_KEY_RSHIFT);

	if (!game_over) {
		if (key_code == key[0]) {
			pacman_NextMove(pman, UP);
			if (enablePman2) pacman_NextMove(pman2, UP);
		}else if (key_code == key[1]) {
			pacman_NextMove(pman, LEFT);
			if (enablePman2) pacman_NextMove(pman2, LEFT);
		}else if (key_code == key[2]) {
			pacman_NextMove(pman, DOWN);
			if (enablePman2) pacman_NextMove(pman2, DOWN);
		}else if (key_code == key[3]) {
			pacman_NextMove(pman, RIGHT);
			if (enablePman2) pacman_NextMove(pman2, RIGHT);
		}else {
			switch (key_code) {
				// TODO-HACKATHON 1-1: Use allegro pre-defined enum ALLEGRO_KEY_<KEYNAME> to controll pacman movement //done
				// we provided you a function `pacman_NextMove` to set the pacman's next move direction.
				case ALLEGRO_KEY_C:
					if (allowCheat) {
						cheat_mode = !cheat_mode;
						if (cheat_mode) {
							game_log("cheat mode on");
							notify("cheat mode on");
						}else {
							game_log("cheat mode off");
							notify("cheat mode off");
						}
					}else notify("cheat mode is not allowed");
					break;
				case ALLEGRO_KEY_G:
					debug_mode = !debug_mode;
					break;
					// allow pman2 to draw on screen
				case ALLEGRO_KEY_P:
					if (!enablePman2) {
						notify("activate pman2");
						enablePman2 = true;
					}
					break;
				default:
					break;
			}
		}
		
		// TODO-HACKATHON 1-2: Use `pacman_movable` to check if the direction is movable. //done	
		// TODO-ADVANCED-cheat_mode //probably_done
		if (cheat_mode) {
			switch(key_code) {
				case ALLEGRO_KEY_K:
					for(int i=0; i<GHOST_NUM; i++)
						ghosts[i]->status = GO_IN;
					break;
				case ALLEGRO_KEY_S:
					if (ctrlPressed) {
						notify("Ghost stopped");
						for(int i=0; i<GHOST_NUM; i++)
							ghosts[i]->status = STOP;
					}
					break;
				case ALLEGRO_KEY_L:
					if (ctrlPressed) {
						notify("Available to cross blocks");
						allowBlockCross = true;
					}
					break;
			}
		}
	}else{
		if (key_code >= 1 && key_code <= 26 && offset < 20) {
			if(shiftPressed) nameSet[offset++] = key_code + 'A' - 1;
			else nameSet[offset++] = key_code + 'a' - 1;
		}
		else if (key_code >= 1 && key_code <= 26 && offset >= 20) notify("You can only enter 20 characters");
		else if (key_code == ALLEGRO_KEY_ENTER) allowChangeScene = true;
		else if (key_code == ALLEGRO_KEY_BACKSPACE && offset>0) offset--;
	}

	preKey = key_code;
}

static void on_mouse_down(int btn, int x, int y, int dz) {

	// nothing here

}

static void render_init_screen(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		400, 400,
		ALLEGRO_ALIGN_CENTER,
		"READY!"
	);

	al_flip_display();
	al_rest(2.0);

}
// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_main_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Start scene created");
	return scene;
}

int64_t get_power_up_timer_tick(){
	return al_get_timer_count(power_up_timer);
}

int64_t get_power_up_duration(){
	return power_up_duration;
}

int64_t get_game_timer() {
	return al_get_timer_count(game_tick_timer);
}