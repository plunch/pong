#ifndef AUDIOPROXY_H
#define AUDIOPROXY_H

void proxy_init(void*);

void proxy_setvolume(float);
float proxy_getvolume(void);

void play_paddle_hit(void);
void play_player_point(void);

void play_menu_move(void);
void play_menu_back(void);
void play_menu_confirm(void);

#endif /* AUDIOPROXY_H */
