#include "wengine.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static struct {
	uint16_t cols;
	uint16_t lines;
	uint8_t f_clear;
	uint8_t f_resized;
} winroot;

static inline void
winroot_init(void)
{
	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	curs_set(0);

	winroot.cols = 0;
	winroot.lines = 0;
	winroot.f_resized = 0;
}

static inline void
winroot_clear()
{
	clear();
}

static inline void
winroot_resize(void)
{
	const uint16_t cols_tmp = winroot.cols;
	const uint16_t lines_tmp = winroot.lines;

	winroot.cols = COLS;
	winroot.lines = LINES;

	if (cols_tmp != winroot.cols || lines_tmp != winroot.lines)
	{
		winroot.f_resized = 1;
	}
}

static inline void
winroot_clean(void)
{
	endwin();
}

window_t *
window_create(winstyle_t *style)
{
	window_t *newwin;
	winchar_t *newcontent;

	newwin = (window_t *)malloc(sizeof(window_t));
	if (newwin == NULL)
	{
		fprintf(stderr, "Failed to allocate window.");
		exit(0);
	}

	newcontent = (winchar_t *)malloc(sizeof(winchar_t) * UINT16_MAX);
	if (newwin == NULL)
	{
		fprintf(stderr, "Failed to allocate window content.");
		exit(0);
	}

	newwin->style = style;
	
	newwin->position.x = 0;
	newwin->position.y = 0;

	newwin->dimension.x = 0;
	newwin->dimension.y = 0;
	newwin->dimension.f_updated = 0;

	newwin->content.mem = newcontent;
	newwin->content.space = UINT16_MAX;
	newwin->content.used = 0;

	/*
	newwin->draw.mem_start = 0;
	newwin->draw.mem_end = 0;
	newwin->draw.x_start = 0;
	newwin->draw.y_start = 0;
	newwin->draw.x_end = 0;
	newwin->draw.y_end = 0;
	*/

	newwin->callback_resize = NULL;
	newwin->callback_tick = NULL;

	return newwin;
}

static inline void
window_resize(window_t *win)
{
	window_style(win, win->style);
}

void
window_style(window_t *win, winstyle_t *style)
{
	int new_dimx = winroot.cols;
	int new_dimy = winroot.lines;
	int new_posx = 0;
	int new_posy = 0;

	win->style = style;

	switch (style->position.top.flex)
	{
		case WINFLEX_FREE:
			break;
		case WINFLEX_ABSOLUTE:
			new_posy += style->position.top.value.absolute;
			new_dimy -= style->position.top.value.absolute;
			break;
		case WINFLEX_RELATIVE:
			new_posy += style->position.top.value.relative * winroot.lines;
			new_dimy -= style->position.top.value.relative * winroot.lines;
			break;
	}

	switch (style->position.right.flex)
	{
		case WINFLEX_FREE:
			break;
		case WINFLEX_ABSOLUTE:
			new_dimx -= style->position.right.value.absolute;
			break;
		case WINFLEX_RELATIVE:
			new_dimx -= style->position.right.value.relative * winroot.cols;
			break;
	}

	switch (style->position.bottom.flex)
	{
		case WINFLEX_FREE:
			break;
		case WINFLEX_ABSOLUTE:
			new_dimy -= style->position.bottom.value.absolute;
			break;
		case WINFLEX_RELATIVE:
			new_dimy -= style->position.bottom.value.relative * winroot.lines;
			break;
	}

	switch (style->position.left.flex)
	{
		case WINFLEX_FREE:
			break;
		case WINFLEX_ABSOLUTE:
			new_posx += style->position.left.value.absolute;
			new_dimx -= style->position.left.value.absolute;
			break;
		case WINFLEX_RELATIVE:
			new_posx += style->position.left.value.relative * winroot.cols;
			new_dimx -= style->position.left.value.relative * winroot.cols;
			break;
	}

	switch (style->dimension.width.flex)
	{
		case WINFLEX_FREE:
			break;
		case WINFLEX_ABSOLUTE:
			new_dimx = style->dimension.width.value.absolute;
			break;
		case WINFLEX_RELATIVE:
			new_dimx = style->dimension.width.value.relative * winroot.cols;
			break;
	}

	switch (style->dimension.height.flex)
	{
		case WINFLEX_FREE:
			break;
		case WINFLEX_ABSOLUTE:
			new_dimy = style->dimension.height.value.absolute;
			break;
		case WINFLEX_RELATIVE:
			new_dimy = style->dimension.height.value.relative * winroot.lines;
			break;
	}
	
	if (new_dimx < 0)
	{
		new_dimx = 0;
	}

	if (new_dimy < 0)
	{
		new_dimy = 0;
	}

	if (style->position.top.flex == WINFLEX_FREE)
	{
		switch (style->position.bottom.flex)
		{
			case WINFLEX_FREE:
				break;
			case WINFLEX_ABSOLUTE:
				new_posy = winroot.lines - new_dimy
					- style->position.bottom.value.absolute;
				break;
			case WINFLEX_RELATIVE:
				new_posy = winroot.lines - new_dimy
					- style->position.bottom.value.relative * winroot.lines;
				break;
		}
	}
	
	if (style->position.left.flex == WINFLEX_FREE)
	{
		switch (style->position.right.flex)
		{
			case WINFLEX_FREE:
				break;
			case WINFLEX_ABSOLUTE:
				new_posx = winroot.cols - new_dimx
					- style->position.right.value.absolute;
				break;
			case WINFLEX_RELATIVE:
				new_posx = winroot.cols - new_dimx
					- style->position.right.value.relative * winroot.cols;
				break;
		}
	}

	if (new_dimx != win->dimension.x || new_dimy != win->dimension.y)
	{
		win->dimension.x = new_dimx;
		win->dimension.y = new_dimy;
		win->dimension.f_updated = 1;
	}

	if (new_posx != win->position.x || new_posy != win->position.y)
	{
		win->position.x = new_posx;
		win->position.y = new_posy;
	}
}

void
window_clear(window_t *win)
{
	win->content.used = 0;
}

void
window_write(window_t *win, const char *src, uint16_t size, charstyle_t style)
{
	uint16_t cpy_size;
	uint16_t offset;

	offset = win->content.used;
	cpy_size = size;
	if (cpy_size > win->content.space - offset)
	{
		cpy_size = win->content.space - offset;
	}

	for (uint16_t i = 0; i < cpy_size; i++)
	{
		win->content.mem[i + offset].character = src[i];
		win->content.mem[i + offset].style = style;
	}

	win->content.used += cpy_size;
}

void
window_destroy(window_t *win)
{
	free(win->content.mem);
	free(win);
}

struct {
	window_t **wins;
	uint8_t space;
	uint8_t used;
} winstack;

static inline void
winstack_init(void)
{
	window_t **newwins = (window_t **)malloc(sizeof(window_t *) * 8);
	if (newwins == NULL)
	{
		fprintf(stderr, "Failed to allocate window stack.");
		exit(0);
	}

	winstack.wins = newwins;
	winstack.space = 8;
	winstack.used = 0;
}

static inline void
winstack_grow(void)
{
	window_t **newwins = (window_t **)realloc(winstack.wins,
			sizeof(window_t *) * 2 * winstack.space);
	if (newwins == NULL)
	{
		fprintf(stderr, "Failed to grow window stack.");
		exit(0);
	}

	winstack.wins = newwins;
	winstack.space *= 2;
}

static inline void
winstack_insert(window_t *win)
{
	if (winstack.used >= winstack.space)
	{
		winstack_grow();
	}

	winstack.wins[winstack.used++] = win;
}

static inline void
winstack_move(window_t *win, uint8_t new_index)
{
	for (uint8_t i = new_index; i < winstack.used; i++)
	{
		if (winstack.wins[i] == win)
		{
			for (uint8_t j = i; j > 0; j--)
			{
				winstack.wins[j] = winstack.wins[j-1];
			}

			winstack.wins[new_index] = win;
			break;
		}
	}
}

static inline void
winstack_remove(window_t *win)
{
	for (uint8_t i = 0; i < winstack.used; i++)
	{
		if (winstack.wins[i] == win)
		{
			for (uint8_t j = i; j < winstack.used - 1; j++)
			{
				winstack.wins[j] = winstack.wins[j+1];
			}

			winstack.used--;
			break;
		}
	}
}

static inline uint8_t
winstack_used(void)
{
	return winstack.used;
}

static inline window_t *
winstack_get(uint8_t i)
{
	return winstack.wins[i];
}

static inline void
winstack_free(void)
{
	free(winstack.wins);
	winstack.wins = NULL;
	winstack.space = 0;
	winstack.used = 0;
}

static inline void
wm_setup(void)
{
	winstack_init();
	winroot_init();
}

void
wm_window_show(window_t *win)
{
	winstack_insert(win);
	window_resize(win);
}

void
wm_window_tofront(window_t *win)
{
	winstack_move(win, 0);
}

void
wm_window_hide(window_t *win)
{
	winstack_remove(win);
}

static inline void
wm_resize(void)
{
	window_t *win;

	winroot_resize();
	if (!winroot.f_resized)
	{
		return;
	}

	for (uint16_t i = 0; i < winstack_used(); i++)
	{
		win = winstack_get(i);

		window_resize(win);
		if (!win->dimension.f_updated)
		{
			continue;
		}

		if (win->callback_resize != NULL)
		{
			win->callback_resize(win);
		}

		win->dimension.f_updated = 0;
	}

	winroot.f_resized = 0;
	winroot.f_clear = 1;
}

static inline void
wm_tick(uint64_t tick_count)
{
	window_t *win;
	
	for (uint16_t i = 0; i < winstack_used(); i++)
	{
		win = winstack_get(i);
		
		if (win->callback_tick != NULL)
		{
			win->callback_tick(win, tick_count);
		}
	}
}

static inline void
wm_draw(void)
{
	window_t *win;
	uint16_t posx, maxx;
	uint16_t posy, maxy;
	uint16_t content_it;
	uint8_t content_w;
	int content_ch;
	winchar_t win_bg;

	if (winroot.f_clear)
	{
		winroot_clear();
		winroot.f_clear = 0;
	}

	for (int16_t i = winstack_used() - 1; i >= 0; i--)
	{
		win = winstack_get(i);

		posx = win->position.x;
		posy = win->position.y;
		if (move(posy, posx) == ERR)
		{
			continue;
		}

		maxx = posx + win->dimension.x;
		if (maxx > winroot.cols)
		{
			maxx = winroot.cols;
		}

		maxy = posy + win->dimension.y;
		if (maxy > winroot.lines)
		{
			maxy = winroot.lines;
		}

		win_bg = win->style->background;
		content_it = 0;
		for (uint16_t y = posy; y < maxy; y++)
		{
			move(y, posx);
			content_w = 1;
			for (uint16_t x = posx; x < maxx; x++)
			{
				if (content_w && content_it < win->content.used)
				{
					content_ch = win->content.mem[content_it++].character;
					switch (content_ch)
					{
						case '\n':
							content_w = 0;
							x--;
							break;

						case '\r':
							x--;
							break;

						default:
							addch(content_ch);
							break;
					}
				}
				else
				{
					addch(win_bg.character);
				}
			}
		}
	}
	
	refresh();
}

static inline void
wm_clean(void)
{
	winroot_clean();
	winstack_free();
}

void
engine_setup(void)
{
	wm_setup();
}

static void
timespec_add_timespec(struct timespec *t, struct timespec *tadd)
{
	t->tv_sec += tadd->tv_sec;
	t->tv_nsec += tadd->tv_nsec;
	while (t->tv_nsec >= 1000000000)
	{
		t->tv_nsec -= 1000000000;
		t->tv_sec += 1;
	}
	while (t->tv_nsec < 0)
	{
		t->tv_nsec += 1000000000;
		t->tv_sec -= 1;
	}
}

static void
timespec_add_float(struct timespec *t, double time)
{
	struct timespec tmp = {
		.tv_sec = (time_t)time,
		.tv_nsec = (time - (time_t)time) * 1000000000,
	};

	timespec_add_timespec(t, &tmp);
}

static struct timespec
timespec_diff(struct timespec t1, struct timespec t2)
{
	t1.tv_sec *= -1;
	t1.tv_nsec *= -1;
	timespec_add_timespec(&t2, &t1);
	return t2;
}

typedef struct {
	struct timespec time;
	struct timespec interval;
} tick_t;

static inline void
tick_init(tick_t *t, uint16_t frequency)
{
	t->time.tv_sec = 0;
	t->time.tv_nsec = 0;
	t->interval.tv_sec = 0;
	t->interval.tv_nsec = 0;
	timespec_add_float(&(t->interval), 1.0 / frequency);
}

static inline int
tick_sleep(tick_t *t)
{
	int ret = 0;
	struct timespec tnow, tdiff;
	
	clock_gettime(CLOCK_REALTIME, &tnow);
	tdiff = timespec_diff(tnow, t->time);
	if (tdiff.tv_sec >= 0 && tdiff.tv_nsec > 0)
	{
		ret = nanosleep(&tdiff, NULL);
	}
	
	if (ret == 0)
	{
		clock_gettime(CLOCK_REALTIME, &(t->time));
		timespec_add_timespec(&(t->time), &(t->interval));
	}

	return ret;
}

static uint8_t engine_running = 0;

void
engine_start(uint16_t tick_frequency)
{
	uint64_t tick_count = 0;
	int tick_ret = 0;
	tick_t tick;
	
	tick_init(&tick, tick_frequency);

	engine_running = 1;
	while (engine_running)
	{
		tick_ret = tick_sleep(&tick);
		
		wm_resize();
		
		if (tick_ret == 0)
		{
			wm_tick(tick_count++);
		}

		wm_draw();
	}
}

void
engine_stop(void)
{
	engine_running = 0;
}

void
engine_clean(void)
{
	wm_clean();
}
