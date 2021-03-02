#ifndef _WENGINE_H
#define _WENGINE_H

#include <inttypes.h>

typedef enum {
	WINFLEX_FREE,
	WINFLEX_ABSOLUTE,
	WINFLEX_RELATIVE,
} winflex_t;

typedef struct {
	winflex_t flex;
	union {
		uint32_t absolute;
		float relative;
	} value;
} windim_t;

typedef struct {
} charstyle_t;

typedef struct {
	int character;
	charstyle_t style;
} winchar_t;

typedef struct {

	struct {
		windim_t top;
		windim_t right;
		windim_t bottom;
		windim_t left;
	} margin;
	
	struct {
		windim_t width;
		windim_t height;
	} dimension;

	uint8_t line_wrap;

	winchar_t background;

} winstyle_t;

typedef struct window window_t;
typedef struct window {

	winstyle_t *style;

	struct {
		uint16_t x;
		uint16_t y;
	} position;

	struct {
		uint16_t x;
		uint16_t y;
		uint8_t f_updated;
	} dimension;

	struct {
		winchar_t *mem;
		uint16_t space;
		uint16_t imin;
		uint16_t imax;
	} content;

	struct {
		uint16_t xmin, ymin;
		uint16_t xmax, ymax;
	} update_area;

	void (*callback_resize)(window_t *win);
	void (*callback_tick)(window_t *win, uint64_t tick_count);

	void *data;

} window_t;

window_t *
window_create(winstyle_t *style);

void
window_style(window_t *win, winstyle_t *style);

void
window_clear(window_t *win);

void
window_write(window_t *win, const char *src, uint16_t size, charstyle_t style);

void
window_destroy(window_t *win);

void
wm_window_show(window_t *win);

void
wm_window_tofront(window_t *win);

void
wm_window_hide(window_t *win);

void
engine_setup(void);

void
engine_start(uint16_t tick_frequency);

void
engine_stop(void);

void
engine_clean(void);

#endif /* _WENGINE_H */
