#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_CENTER_X (SCREEN_WIDTH / 2)
#define SCREEN_CENTER_Y (SCREEN_HEIGHT / 2)
#define FPS 90
#define TAM_MAX 500
#define BOOSTS_MAX 200
#define BLUE_RADIUS 5
#define BOOST_RADIUS 12
#define SHIELD_RADIUS 30
#define AURA_RADIUS 60
#define EQUALIZER 30.0
#define LVL_2 700
#define LVL_INCREMENT 300
#define POWER_TIME_INCREMENT 8.0

typedef struct enemies {
	float pX, pY, radius, speed;
} enemy;

typedef struct powerUps {
	int type;
	float pX, pY, speed;
} powerUp;

void loadingScreen(ALLEGRO_BITMAP* loading, ALLEGRO_FONT* title, ALLEGRO_FONT* subTitle, ALLEGRO_FONT* options, ALLEGRO_COLOR prim, ALLEGRO_COLOR sec, ALLEGRO_COLOR terc) {
	al_draw_bitmap(loading, 0, 0, NULL);
	al_draw_text(title, prim, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 100, ALLEGRO_ALIGN_CENTER, "Sinuous");
	al_draw_text(subTitle, sec, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 170, ALLEGRO_ALIGN_CENTER, "Avoid the Red Dots");
	al_draw_text(options, terc, SCREEN_CENTER_X, SCREEN_CENTER_Y + 150, ALLEGRO_ALIGN_CENTER, "Pressione Enter para Prosseguir");
}

void infoScreen(ALLEGRO_BITMAP* infoBack, ALLEGRO_FONT* text, ALLEGRO_FONT* subTitle, ALLEGRO_FONT* options, ALLEGRO_COLOR prim, ALLEGRO_COLOR sec, ALLEGRO_COLOR terc) {
	al_draw_bitmap(infoBack, 0, 0, NULL);
	al_draw_text(options, prim, SCREEN_CENTER_X, SCREEN_CENTER_Y + 150, ALLEGRO_ALIGN_CENTER, "Voltar");
	al_draw_text(subTitle, sec, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) - 100, ALLEGRO_ALIGN_CENTER, "Como Jogar:");
	al_draw_text(text, terc, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) - 30, ALLEGRO_ALIGN_CENTER, "Seja bem vindo a Sinuos: Avoid the Red Dots!");
	al_draw_text(text, terc, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 10, ALLEGRO_ALIGN_CENTER, "Para iniciar o jogo, basta pressionar Jogar no Menu Principal");
	al_draw_text(text, terc, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 50, ALLEGRO_ALIGN_CENTER, "Durante o jogo, voce controla a bolinha azul com o Mouse");
	al_draw_text(text, terc, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 90, ALLEGRO_ALIGN_CENTER, "Ganhe pontos ao longo do tempo e ao se mover, tambem existem PowerUps");
	al_draw_text(text, terc, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 130, ALLEGRO_ALIGN_CENTER, "Verifique suas Vidas, Pontos e Tempo na Aba superior da tela");
	al_draw_text(text, terc, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 170, ALLEGRO_ALIGN_CENTER, "Ao encostar em uma bolinha vermelha voce perde uma vida");
	al_draw_text(text, terc, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 210, ALLEGRO_ALIGN_CENTER, "Se suas vidas acabarem, Game Over");
	al_draw_text(text, terc, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 250, ALLEGRO_ALIGN_CENTER, "Esta preparado para este Desafio?");
}

void menuScreen(ALLEGRO_BITMAP* backGround, ALLEGRO_FONT* title, ALLEGRO_FONT* subTitle, ALLEGRO_FONT* options, ALLEGRO_COLOR prim, ALLEGRO_COLOR sec, ALLEGRO_COLOR terc, ALLEGRO_COLOR quar, ALLEGRO_COLOR quin) {
	al_draw_bitmap(backGround, 0, 0, NULL);
	al_draw_text(title, prim, SCREEN_CENTER_X, SCREEN_CENTER_Y / 2, ALLEGRO_ALIGN_CENTER, "Sinuous");
	al_draw_text(subTitle, sec, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 70, ALLEGRO_ALIGN_CENTER, "Avoid the Red Dots");
	al_draw_text(options, terc, SCREEN_CENTER_X, SCREEN_CENTER_Y + 50, ALLEGRO_ALIGN_CENTER, "Jogar");
	al_draw_text(options, quar, SCREEN_CENTER_X, SCREEN_CENTER_Y + 100, ALLEGRO_ALIGN_CENTER, "Sobre o Jogo");
	al_draw_text(options, quin, SCREEN_CENTER_X, SCREEN_CENTER_Y + 150, ALLEGRO_ALIGN_CENTER, "Sair");
}

void menuNotSelected(int* selectSound, ALLEGRO_COLOR prim, ALLEGRO_COLOR* playColor, ALLEGRO_COLOR* infoColor, ALLEGRO_COLOR* exitColor) {
	*selectSound = 0;
	*playColor = prim;
	*infoColor = prim;
	*exitColor = prim;
}

void gameOverNotSelected(int* selectSound, ALLEGRO_COLOR prim, ALLEGRO_COLOR* playAgainColor, ALLEGRO_COLOR* toMainMenuColor) {
	*selectSound = 0;
	*playAgainColor = prim;
	*toMainMenuColor = prim;
}

bool contact(float x1, float x2, float y1, float y2, float r1, float r2) {
	int centersDistancePow = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
	int radiusSumPow = ((r1 + r2) * (r1 + r2));
	if (centersDistancePow <= radiusSumPow) {
		return true;
	}
	else if (centersDistancePow > radiusSumPow) {
		return false;
	}
}

void reset(int *cont, int* lifes, double* moveSpeed, float* points, float* timing, int* level, int* currentLevel, int* ballMax, bool* showTime, bool* invincible, float* powerTime, int* powerCont, bool* showShield, bool* showGravity, bool* showSlow, bool* showSmall) {
	*cont = 0;
	*lifes = 3;
	*moveSpeed = 20;
	*points = 0;
	*timing = 0;
	*showTime = false;
	*level = 1;
	*currentLevel = LVL_2 + LVL_INCREMENT;
	*ballMax = 35;
	*invincible = false;
	*powerCont = 0;
	*powerTime = POWER_TIME_INCREMENT;
	*showShield = false;
	*showGravity = false;
	*showSlow = false;
	*showSmall = false;
}

void gameScreen(ALLEGRO_FONT* counters, ALLEGRO_COLOR prim, ALLEGRO_COLOR sec, ALLEGRO_COLOR terc, int lifes, float* points, int level, float* timing) {
	al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, 30, terc);
	al_draw_text(counters, prim, 10, 7, ALLEGRO_ALIGN_LEFT, "Vidas:");
	al_draw_textf(counters, sec, 60, 7, ALLEGRO_ALIGN_LEFT, "%d", lifes);
	al_draw_text(counters, prim, 90, 7, ALLEGRO_ALIGN_LEFT, "Tempo:");
	al_draw_textf(counters, sec, 140, 7, ALLEGRO_ALIGN_LEFT, "%.2f", *timing);
	al_draw_text(counters, prim, 200, 7, ALLEGRO_ALIGN_LEFT, "Pontos:");
	al_draw_textf(counters, sec, 260, 7, ALLEGRO_ALIGN_LEFT, "%.f", *points);
	al_draw_text(counters, prim, SCREEN_WIDTH - 50, 7, ALLEGRO_ALIGN_RIGHT, "Level:");
	al_draw_textf(counters, sec, SCREEN_WIDTH - 40, 7, ALLEGRO_ALIGN_LEFT, "%d", level);
	*timing += 1.0 / FPS;
	*points += (1.5 * level) / 10;
}

void gameOverScreen(ALLEGRO_BITMAP* gameOverBack, ALLEGRO_FONT* title, ALLEGRO_FONT* text, ALLEGRO_FONT* subTitle, ALLEGRO_FONT* options, ALLEGRO_COLOR prim, ALLEGRO_COLOR sec, ALLEGRO_COLOR terc, ALLEGRO_COLOR quar, ALLEGRO_COLOR quin, ALLEGRO_COLOR sext, float points, float timing) {
	al_draw_bitmap(gameOverBack, 0, 0, NULL);
	al_draw_text(title, prim, SCREEN_CENTER_X, SCREEN_CENTER_Y / 4, ALLEGRO_ALIGN_CENTER, "GAME OVER");
	al_draw_text(text, sec, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 4) + 80, ALLEGRO_ALIGN_CENTER, "Voce jogou bem, mas acabaram suas vidas");
	al_draw_text(subTitle, terc, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 80, ALLEGRO_ALIGN_CENTER, "Pontos:");
	al_draw_textf(text, quar, SCREEN_CENTER_X, (SCREEN_CENTER_Y / 2) + 120, ALLEGRO_ALIGN_CENTER, "%.f", points);
	al_draw_text(subTitle, terc, SCREEN_CENTER_X, SCREEN_CENTER_Y + 20, ALLEGRO_ALIGN_CENTER, "Tempo Decorrido:");
	al_draw_textf(text, quar, SCREEN_CENTER_X, SCREEN_CENTER_Y + 60, ALLEGRO_ALIGN_CENTER, "%.1f", timing);
	al_draw_text(options, quin, SCREEN_CENTER_X, SCREEN_CENTER_Y + 130, ALLEGRO_ALIGN_CENTER, "Jogar Novamente");
	al_draw_text(options, sext, SCREEN_CENTER_X, SCREEN_CENTER_Y + 180, ALLEGRO_ALIGN_CENTER, "Menu Principal");
}

void initializeEnemies(enemy* en, int* cont, int ballMax, int dropX, double moveSpeed) {
	for (*cont; *cont < ballMax; *cont = *cont + 1) {
		dropX = rand() % 2;
		if (dropX) {
			en[*cont].pX = rand() % SCREEN_WIDTH;
			en[*cont].pY = 0;
		}
		else if (!dropX) {
			en[*cont].pY = rand() % SCREEN_HEIGHT;
			en[*cont].pX = SCREEN_WIDTH;
		}
		if (*cont % 2 == 0) {
			en[*cont].radius = BLUE_RADIUS - (rand() % 30) / EQUALIZER;
		}
		else {
			en[*cont].radius = BLUE_RADIUS + (rand() % 30) / EQUALIZER;
		}
		en[*cont].speed = (moveSpeed + (rand() % 20)) / EQUALIZER;
	}
}

void initializePowers(powerUp* up, float timing, float* powerTime, int* powerCont, int dropX, double moveSpeed) {
	if (timing >= *powerTime) {
		up[*powerCont].type = rand() % 8;
		if (up[*powerCont].type <= 4) {
			dropX = rand() % 2;
			if (dropX) {
				up[*powerCont].pX = SCREEN_WIDTH - (rand() % (SCREEN_WIDTH / 2));
				up[*powerCont].pY = -10;
			}
			else if (!dropX) {
				up[*powerCont].pY = rand() % (SCREEN_HEIGHT / 2);
				up[*powerCont].pX = SCREEN_WIDTH + 10;
			}
			up[*powerCont].speed = (moveSpeed + (rand() % 20)) / EQUALIZER;
		}
		else {
			up[*powerCont].pX = -10;
			up[*powerCont].pY = SCREEN_HEIGHT + 10;
			up[*powerCont].type = -1;
			up[*powerCont].speed = -1;
		}
		*powerCont = *powerCont + 1;
		*powerTime += POWER_TIME_INCREMENT;
	}
}

void loadEnemies(enemy* en, int* cont, int i, int dropX, int mX, int mY, double moveSpeed, int* lifes, float shieldRadius, float* timeInvincible, float timing, bool showShield, bool showGravity, bool* invincible, bool* gameOver, ALLEGRO_COLOR prim, ALLEGRO_SAMPLE* damage, ALLEGRO_SAMPLE* explosion) {
	for (i = 0; i < *cont; i++) {
		al_draw_filled_circle(en[i].pX, en[i].pY, en[i].radius, prim);
		en[i].pX = en[i].pX - en[i].speed;
		en[i].pY = en[i].pY + en[i].speed;
		if (contact(mX, en[i].pX, mY, en[i].pY, BLUE_RADIUS, en[i].radius) && !*invincible && !showShield && !showGravity) {
			al_play_sample(damage, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
			*lifes = *lifes - 1;
			*timeInvincible = timing;
			*invincible = true;
			dropX = rand() % 2;
			if (dropX) {
				en[i].pX = rand() % SCREEN_WIDTH;
				en[i].pY = 0;
			}
			else if (!dropX) {
				en[i].pY = rand() % SCREEN_HEIGHT;
				en[i].pX = SCREEN_WIDTH;
			}
		}
		if (en[i].pX < -5 || en[i].pY > SCREEN_HEIGHT + 5 || (contact(mX, en[i].pX, mY, en[i].pY, BLUE_RADIUS, en[i].radius) && !invincible && !showGravity) || (contact(mX, en[i].pX, mY, en[i].pY, BLUE_RADIUS, shieldRadius) && showShield)) {
			if (contact(mX, en[i].pX, mY, en[i].pY, BLUE_RADIUS, shieldRadius) && showShield) al_play_sample(explosion, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
			dropX = rand() % 2;
			if (dropX) {
				en[i].pX = rand() % SCREEN_WIDTH;
				en[i].pY = 0;
			}
			else if (!dropX) {
				en[i].pY = rand() % SCREEN_HEIGHT;
				en[i].pX = SCREEN_WIDTH;
			}
			en[i].speed = (moveSpeed + (rand() % 20)) / EQUALIZER;
		}
		if (*lifes == 0) {
			*gameOver = true;
		}
	}
}

void loadPowers(powerUp* up, int powerCont, int i, int mX, int mY, float* points, int* lifes, float timing, float* shieldRadius, double* shieldTime, double* gravityTime, double* slowTime, double* smallTime, bool* showShield, bool* showGravity, bool* showSlow, bool* showSmall, ALLEGRO_COLOR prim, ALLEGRO_COLOR sec, ALLEGRO_COLOR terc, ALLEGRO_COLOR quar, ALLEGRO_COLOR quin, ALLEGRO_COLOR sext, ALLEGRO_FONT* text, ALLEGRO_SAMPLE* boost) {
	for (i = 0; i < powerCont; i++) {
		if (up[i].speed != -1) {
			up[i].pX = up[i].pX - up[i].speed;
			up[i].pY = up[i].pY + up[i].speed;
		}
		switch (up[i].type) {
		case 0:
			al_draw_filled_circle(up[i].pX, up[i].pY, BOOST_RADIUS, prim);
			al_draw_text(text, sec, up[i].pX, up[i].pY - 12, ALLEGRO_ALIGN_CENTER, "S");
			break;
		case 1:
			al_draw_filled_circle(up[i].pX, up[i].pY, BOOST_RADIUS, terc);
			al_draw_text(text, sec, up[i].pX, up[i].pY - 12, ALLEGRO_ALIGN_CENTER, "G");
			break;
		case 2:
			al_draw_filled_circle(up[i].pX, up[i].pY, BOOST_RADIUS, quar);
			al_draw_text(text, sec, up[i].pX, up[i].pY - 10, ALLEGRO_ALIGN_CENTER, "T");
			break;
		case 3:
			al_draw_filled_circle(up[i].pX, up[i].pY, BOOST_RADIUS, quin);
			al_draw_text(text, sec, up[i].pX, up[i].pY - 12, ALLEGRO_ALIGN_CENTER, "M");
			break;
		case 4:
			al_draw_filled_circle(up[i].pX, up[i].pY, BOOST_RADIUS, sext);
			al_draw_text(text, sec, up[i].pX, up[i].pY - 12, ALLEGRO_ALIGN_CENTER, "1");
			break;
		default:
			break;
		}
		if (contact(mX, up[i].pX, mY, up[i].pY, BLUE_RADIUS, BOOST_RADIUS)) {
			al_play_sample(boost, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
			switch (up[i].type) {
			case 0:
				*shieldTime = timing;
				*shieldRadius = SHIELD_RADIUS;
				*showShield = true;
				break;
			case 1:
				*gravityTime = timing;
				*showGravity = true;
				break;
			case 2:
				*slowTime = timing;
				*showSlow = true;
				break;
			case 3:
				*smallTime = timing;
				*showSmall = true;
				break;
			case 4:
				*lifes = *lifes + 1;
				break;
			default:
				break;
			}
			*points += 30;
			up[i].pX = -10;
			up[i].pY = SCREEN_HEIGHT + 10;
			up[i].type = -1;
			up[i].speed = -1;
		}
		if (up[i].pX < -10 || up[i].pY > SCREEN_HEIGHT + 10) {
			up[i].pX = -10;
			up[i].pY = SCREEN_HEIGHT + 10;
			up[i].type = -1;
			up[i].speed = -1;
		}
	}
}

void leveling(float points, int* nextLevel, int* level, int* ballMax, double* moveSpeed, float* timeLvl, float timing, bool* showTime, ALLEGRO_SAMPLE* levelUp, ALLEGRO_FONT* subTitle, ALLEGRO_COLOR prim) {
	if (points >= *nextLevel) {
		al_play_sample(levelUp, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
		*level += 1;
		*nextLevel = LVL_2 + points + LVL_INCREMENT * (*level);
		*ballMax += 5;
		if (*level <= 12) {
			*moveSpeed += 5;
		}
		*timeLvl = timing;
		*showTime = true;
	}
	if (*showTime) {
		al_draw_text(subTitle, prim, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 50, ALLEGRO_ALIGN_RIGHT, "Level Up!");
		if (timing - *timeLvl >= 3) *showTime = false;
	}
}

void invincibleEffect(float timing, float* timeInvincible, bool* invincible, ALLEGRO_COLOR* playerColor, ALLEGRO_COLOR prim, ALLEGRO_COLOR sec) {
	if (*invincible) {
		if (rand() % 2) {
			*playerColor = prim;
		}
		else {
			*playerColor = sec;
		}
		if (timing - *timeInvincible >= 3) {
			*invincible = false;
			*playerColor = sec;
		}
	}
}

void loadShield(bool* showShield, int mX, int mY, float* shieldRadius, float timing, double shieldTime, ALLEGRO_COLOR prim, ALLEGRO_COLOR sec) {
	if (*showShield) {
		al_draw_filled_circle(mX, mY, *shieldRadius, prim);
		al_draw_circle(mX, mY, *shieldRadius, sec, 4);
		if (timing - shieldTime >= 4) {
			*shieldRadius -= 0.4;
		}
		if (*shieldRadius <= BLUE_RADIUS) *showShield = false;
	}
}

void loadGravity(enemy* en, bool* showGravity, int mX, int mY, int cont, int j, float timing, double gravityTime, ALLEGRO_COLOR prim, ALLEGRO_COLOR sec) {
	if (*showGravity) {
		al_draw_filled_circle(mX, mY, AURA_RADIUS, prim);
		al_draw_circle(mX, mY, AURA_RADIUS, sec, 3);
		for (j = 0; j < cont; j++) {
			while (contact(mX, en[j].pX, mY, en[j].pY, AURA_RADIUS, en[j].radius)) {
				if (en[j].pX > mX && en[j].pY < mY) {
					en[j].pX = en[j].pX + 0.1;
					en[j].pY = en[j].pY - 0.1;
				}
				else if (en[j].pX > mX && en[j].pY > mY) {
					en[j].pX = en[j].pX + 0.1;
					en[j].pY = en[j].pY + 0.1;
				}
				else if (en[j].pX < mX && en[j].pY < mY) {
					en[j].pX = en[j].pX - 0.1;
					en[j].pY = en[j].pY - 0.1;
				}
				else if (en[j].pX < mX && en[j].pY > mY) {
					en[j].pX = en[j].pX - 0.1;
					en[j].pY = en[j].pY + 0.1;
				}
			}
		}
		if (timing - gravityTime >= 5) *showGravity = false;
	}
}

void loadSlow(enemy* en, bool* showSlow, int cont, int j, double moveSpeed, float timing, double slowTime) {
	if (*showSlow) {
		for (j = 0; j < cont; j++) {
			if (en[j].speed > ((moveSpeed + 20) / EQUALIZER) / 5) {
				en[j].speed = (en[j].speed / 5);
			}
		}
		if (timing - slowTime >= 4.5) {
			for (j = 0; j < cont; j++) {
				en[j].speed = (en[j].speed * 5);
			}
			*showSlow = false;
		}
	}
}

void loadSmall(enemy* en, bool* showSmall, int cont, int j, double moveSpeed, float timing, double smallTime) {
	if (*showSmall) {
		for (j = 0; j < cont; j++) {
			if (en[j].radius > ((BLUE_RADIUS + 30 / EQUALIZER) / 2)) {
				en[j].radius = (en[j].radius / 2);
			}
		}
		if (timing - smallTime >= 5) {
			for (j = 0; j < cont; j++) {
				en[j].radius = (en[j].radius * 2);
			}
			*showSmall = false;
		}
	}
}

int main() {

	ALLEGRO_DISPLAY* display;

	if (!al_init()) {
		al_show_native_message_box(NULL, NULL, NULL, "Falha ao inicializar o Allegro", NULL, NULL);
	}

	display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
	al_set_window_position(display, 500, 200);
	al_set_window_title(display, "Sinuous");

	al_init_primitives_addon();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_acodec_addon();
	al_install_mouse();
	al_install_keyboard();
	al_install_audio();
	al_reserve_samples(7);

	ALLEGRO_BITMAP* loading = al_load_bitmap("StartImage.jpg");
	ALLEGRO_BITMAP* infoBack = al_load_bitmap("Info.png");
	ALLEGRO_BITMAP* cursor = al_load_bitmap("Cursor.png");
	ALLEGRO_BITMAP* backGround = al_load_bitmap("Background.jpg");
	ALLEGRO_BITMAP* gameOverBack = al_load_bitmap("GameOver.png");
	ALLEGRO_FONT* title = al_load_font("OrbitronMedium.ttf", 72, NULL);
	ALLEGRO_FONT* subTitle = al_load_font("OrbitronMedium.ttf", 34, NULL);
	ALLEGRO_FONT* options = al_load_font("Dimitri.ttf", 34, NULL);
	ALLEGRO_FONT* text = al_load_font("OrbitronMedium.ttf", 18, NULL);
	ALLEGRO_FONT* counters = al_load_font("OrbitronMedium.ttf", 12, NULL);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
	ALLEGRO_SAMPLE* select = al_load_sample("Select.mp3");
	ALLEGRO_SAMPLE* click = al_load_sample("Click.mp3");
	ALLEGRO_SAMPLE* enter = al_load_sample("Enter.mp3");
	ALLEGRO_SAMPLE* explosion = al_load_sample("Explosion.mp3");
	ALLEGRO_SAMPLE* damage = al_load_sample("Damage.mp3");
	ALLEGRO_SAMPLE* boost = al_load_sample("Boost.mp3");
	ALLEGRO_SAMPLE* levelUp = al_load_sample("LevelUp.mp3");

	ALLEGRO_COLOR grey = al_map_rgb(80, 80, 80);
	ALLEGRO_COLOR lightGrey = al_map_rgb(150, 150, 150);
	ALLEGRO_COLOR black = al_map_rgb(30, 30, 30);
	ALLEGRO_COLOR translucidBlack = al_map_rgba(10, 10, 10, 160);
	ALLEGRO_COLOR translucidWhite = al_map_rgba(240, 240, 240, 240);
	ALLEGRO_COLOR white = al_map_rgb(230, 230, 230);
	ALLEGRO_COLOR lightBlue = al_map_rgb(0, 255, 255);
	ALLEGRO_COLOR translucidLightBlue = al_map_rgba(102, 205, 170, 30);
	ALLEGRO_COLOR red = al_map_rgb(255, 80, 80);
	ALLEGRO_COLOR darkRed = al_map_rgb(220, 20, 60);
	ALLEGRO_COLOR crimson = al_map_rgb(255, 20, 60);
	ALLEGRO_COLOR teal = al_map_rgb(0, 128, 128);
	ALLEGRO_COLOR darkBlue = al_map_rgb(72, 61, 139);
	ALLEGRO_COLOR lightPurple = al_map_rgb(138, 43, 226);
	ALLEGRO_COLOR yellowGreen = al_map_rgb(154, 205, 50);
	ALLEGRO_COLOR seaGreen = al_map_rgb(60, 179, 113);
	ALLEGRO_COLOR playerColor = lightBlue;
	ALLEGRO_COLOR playColor = white;
	ALLEGRO_COLOR infoColor = white;
	ALLEGRO_COLOR exitColor = white;
	ALLEGRO_COLOR returnColor = white;
	ALLEGRO_COLOR playAgainColor = white;
	ALLEGRO_COLOR toMainMenuColor = white;

	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_display_event_source(display));

	bool load = false;
	bool exit = false;
	bool menu = true;
	bool start = false;
	bool info = false;
	bool move = false;
	bool gameOver = false;
	bool showTime, invincible;
	bool showShield, showGravity, showSlow, showSmall;
	int mX = 10, mY = 10;
	int cont, powerCont, ballMax, dropX = 0, i = 0, j = 0;
	int lifes, level, nextLevel;
	int selectSound = 0;
	double shieldTime, gravityTime, slowTime, smallTime;
	float shieldRadius = SHIELD_RADIUS;
	float timing, timeLvl, timeInvincible, powerTime, points;
	double moveSpeed;
	enemy en[TAM_MAX];
	powerUp up[BOOSTS_MAX];

	srand(time(NULL));

	al_hide_mouse_cursor(display);

	al_start_timer(timer);

	while (!exit) {

		ALLEGRO_EVENT events;
		al_wait_for_event(queue, &events);

		if (events.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			exit = true;
		}
		else if (events.type == ALLEGRO_EVENT_MOUSE_AXES) {
			if (events.mouse.x < 0) {
				mX = 0;
			}
			else if (events.mouse.x > SCREEN_WIDTH) {
				mX = SCREEN_WIDTH;
			}
			else {
				mX = events.mouse.x;
			}
			if (events.mouse.y < 0) {
				mY = 0;
			}
			else if (events.mouse.y > SCREEN_HEIGHT) {
				mY = SCREEN_HEIGHT;
			}
			else {
				mY = events.mouse.y;
			}

			if (start && !gameOver) {
				points += 0.2;
			}
		}

		if (events.type == ALLEGRO_EVENT_TIMER) {
			if (start && !gameOver) {
				move = true;
			}
			al_flip_display();
		}

		if (load && !start) {
			if (mX >= (SCREEN_CENTER_X - 50) && mX <= (SCREEN_CENTER_X + 44) && mY >= (SCREEN_CENTER_Y + 50) && mY <= (SCREEN_CENTER_Y + 80) && !info) {
				playColor = darkRed;
				selectSound++;
				if (events.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
					al_play_sample(click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
					start = true;
				}
			}
			else if (mX >= (SCREEN_CENTER_X - 106) && mX <= (SCREEN_CENTER_X + 100) && mY >= (SCREEN_CENTER_Y + 100) && mY <= (SCREEN_CENTER_Y + 130) && !info) {
				selectSound++;
				infoColor = darkRed;
				if (events.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
					al_play_sample(click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
					info = true;
				}
			}
			else if (mX >= (SCREEN_CENTER_X - 36) && mX <= (SCREEN_CENTER_X + 30) && mY >= (SCREEN_CENTER_Y + 150) && mY <= (SCREEN_CENTER_Y + 180)) {
				selectSound++;
				exitColor = darkRed;
				if (events.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && !info) {
					exit = true;
				}
			}
			else {
				menuNotSelected(&selectSound, white, &playColor, &infoColor, &exitColor);
			}
		}

		if (selectSound == 1) {
			al_play_sample(select, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
		}

		if (!start) {
			if (!load) {
				loadingScreen(loading, title, subTitle, options, lightGrey, grey, white);
				if (events.type == ALLEGRO_EVENT_KEY_UP) {
					if (events.keyboard.keycode == ALLEGRO_KEY_ENTER) {
						al_play_sample(enter, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
						load = true;
					}
				}
			}
			else if (info) {
				if (mX >= (SCREEN_CENTER_X - 58) && mX <= (SCREEN_CENTER_X + 50) && mY >= (SCREEN_CENTER_Y + 150) && mY <= (SCREEN_CENTER_Y + 180)) {
					returnColor = darkRed;
					if (events.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
						al_play_sample(click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
						info = false;
					}
				}
				else {
					returnColor = white;
				}
				infoScreen(infoBack, text, subTitle, options, returnColor, lightGrey, white);
			}
			else if (menu) {
				menuScreen(backGround, title, subTitle, options, lightGrey, grey, playColor, infoColor, exitColor);
				reset(&cont, &lifes, &moveSpeed, &points, &timing, &level, &nextLevel, &ballMax, &showTime, &invincible, &powerTime, &powerCont, &showShield, &showGravity, &showSlow, &showSmall);
			}
			al_draw_bitmap(cursor, mX, mY, ALLEGRO_FLIP_HORIZONTAL);
		}
		else if (start) {

			initializeEnemies(en, &cont, ballMax, dropX, moveSpeed);

			initializePowers(up, timing, &powerTime, &powerCont, dropX, moveSpeed);

			if (move) {
				move = false;

				al_clear_to_color(black);

				loadEnemies(en, &cont, i, dropX, mX, mY, moveSpeed, &lifes, shieldRadius, &timeInvincible, timing, showShield, showGravity, &invincible, &gameOver, red, damage, explosion);

				loadPowers(up, powerCont, i, mX, mY, &points, &lifes, timing, &shieldRadius, &shieldTime, &gravityTime, &slowTime, &smallTime, &showShield, &showGravity, &showSlow, &showSmall, teal, white, darkBlue, lightPurple, yellowGreen, seaGreen, text, boost);

				al_draw_filled_circle(mX, mY, BLUE_RADIUS, playerColor);

				gameScreen(counters, lightGrey, white, translucidBlack, lifes, &points, level, &timing);

				leveling(points, &nextLevel, &level, &ballMax, &moveSpeed, &timeLvl, timing, &showTime, levelUp, subTitle, white);

				invincibleEffect(timing, &timeInvincible, &invincible, &playerColor, black, lightBlue);

				loadShield(&showShield, mX, mY, &shieldRadius, timing, shieldTime, translucidLightBlue, lightBlue);

				loadGravity(en, &showGravity, mX, mY, cont, j, timing, gravityTime, translucidWhite, teal);

				loadSlow(en, &showSlow, cont, j, moveSpeed, timing, slowTime);

				loadSmall(en, &showSmall, cont, j, moveSpeed, timing, smallTime);
			}
			else if (gameOver) {
				if (mX >= (SCREEN_CENTER_X - 144) && mX <= (SCREEN_CENTER_X + 138) && mY >= (SCREEN_CENTER_Y + 130) && mY <= (SCREEN_CENTER_Y + 160)) {
					selectSound++;
					playAgainColor = darkRed;
					if (events.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && !info) {
						al_play_sample(click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
						playerColor = lightBlue;
						reset(&cont, &lifes, &moveSpeed, &points, &timing, &level, &nextLevel, &ballMax, &showTime, &invincible, &powerTime, &powerCont, &showShield, &showGravity, &showSlow, &showSmall);
						gameOver = false;
					}
				}
				else if (mX >= (SCREEN_CENTER_X - 122) && mX <= (SCREEN_CENTER_X + 115) && mY >= (SCREEN_CENTER_Y + 180) && mY <= (SCREEN_CENTER_Y + 210)) {
					selectSound++;
					toMainMenuColor = darkRed;
					if (events.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && !info) {
						al_play_sample(click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
						playerColor = lightBlue;
						gameOver = false;
						start = false;
					}
				}
				else {
					gameOverNotSelected(&selectSound, white, &playAgainColor, &toMainMenuColor);
				}
				gameOverScreen(gameOverBack, title, text, subTitle, options, grey, lightGrey, crimson, white, playAgainColor, toMainMenuColor, points, timing);
				al_draw_bitmap(cursor, mX, mY, ALLEGRO_FLIP_HORIZONTAL);
			}
		}
	}

	al_destroy_bitmap(gameOverBack);
	al_destroy_bitmap(infoBack);
	al_destroy_bitmap(backGround);
	al_destroy_bitmap(cursor);
	al_destroy_sample(select);
	al_destroy_sample(explosion);
	al_destroy_sample(click);
	al_destroy_sample(enter);
	al_destroy_sample(damage);
	al_destroy_sample(boost);
	al_destroy_sample(levelUp);
	al_uninstall_audio();
	al_uninstall_mouse();
	al_uninstall_keyboard();
	al_destroy_timer(timer);
	al_destroy_event_queue(queue);
	al_destroy_display(display);

	return 0;
}