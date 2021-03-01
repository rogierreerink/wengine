#include "ansi.h"
#include "test.h"
#include "wengine.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define TICK_FREQUENCY 10000

static window_t *win1, *win2, *win3, *win4;
static charstyle_t cstyle;

static winstyle_t style1 = {
	.position = {
		.top = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 2,
		},
		.right = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 4,
		},
		.bottom = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 2,
		},
		.left = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 4,
		},
	},
	.background = {
		.character = '#',
	},
};

static winstyle_t style2 = {
	.position = {
		.top = {
			.flex = WINFLEX_RELATIVE,
			.value.relative = 0.5,
		},
		.right = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 10,
		},
		.bottom = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 8,
		},
		.left = {
			.flex = WINFLEX_RELATIVE,
			.value.relative = 0.5,
		},
	},
	.background = {
		.character = '=',
	},
};

static winstyle_t style3 = {
	.position = {
		.top = {
			.flex = WINFLEX_FREE,
		},
		.right = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 4,
		},
		.bottom = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 2,
		},
		.top = {
			.flex = WINFLEX_FREE,
		},
	},
	.dimension = {
		.width = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 24,
		},
		.height = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 4,
		},
	},
	.background = {
		.character = '-',
	},
};

static winstyle_t style4 = {
	.position = {
		.top = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 0,
		},
		.right = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 0,
		},
		.bottom = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 0,
		},
		.left = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 0,
		},
	},
	.background = {
		.character = '+',
	},
};

static void
winsizepos_tick(window_t *win, uint64_t tick_count)
{
	static uint8_t invoke_count = 0;
	static uint64_t last_invoke_tick_count = 0;
	char buffer[128];
	size_t buflen;
	
	if (tick_count - last_invoke_tick_count >= TICK_FREQUENCY / 2)
	{
		last_invoke_tick_count = tick_count;
		invoke_count++;
		if (invoke_count > 3)
		{
			invoke_count = 0;
		}
	}
	
	window_clear(win);

	for (uint8_t i = 0; i < invoke_count; i++)
	{
		window_write(win, "\n", 1, cstyle);
	}

	buflen = snprintf(buffer, 128, "%d, %d, %d, %d, %lu",
			win->dimension.x, win->dimension.y,
			win->position.x, win->position.y, tick_count);

	window_write(win, buffer, buflen, cstyle);
}

test_return_t
test_wengine(void)
{
	win1 = window_create(&style1);
	win1->callback_tick = winsizepos_tick;
	win2 = window_create(&style2);
	win2->callback_tick = winsizepos_tick;
	win3 = window_create(&style3);
	win3->callback_tick = winsizepos_tick;
	win4 = window_create(&style4);
	win4->callback_tick = winsizepos_tick;
	
	engine_setup();

	wm_window_show(win1);
	wm_window_show(win2);
	wm_window_show(win3);
	wm_window_show(win4);

	wm_window_tofront(win3);
	wm_window_tofront(win2);
	
	/*
	window_write(win1, "Hello, world!", 13, cstyle);
	window_write(win2, "Another message :)!", 19, cstyle);
	window_write(win3, "A long message this is.\nMemory is limited. Wisely, you must use it.", 67, cstyle);
	window_write(win4, "Hey world.", 10, cstyle);
	*/
	
	engine_start(TICK_FREQUENCY);
	engine_stop();
	engine_clean();

	window_destroy(win1);
	window_destroy(win2);
	window_destroy(win3);
	window_destroy(win4);
	
	return TEST_OK;
}

void
test(test_t test, const char *name)
{
	printf(ANSI_STYLE_BOLD "Testing %s... " ANSI_RESET, name);
	fflush(stdout);
	switch (test())
	{
		case TEST_OK:
			printf(ANSI_COLOR_GREEN "OK!\n");
			break;
		case TEST_FAIL:
			printf(ANSI_COLOR_RED "Failed!\n");
			break;
		case TEST_CONFIRM:
			printf(ANSI_COLOR_YELLOW "Needs confirmation.\n");
			break;
		case TEST_NOTIMPLEMENTED:
			printf(ANSI_COLOR_BLUE "Not implemented.\n");
			break;
	}
	printf(ANSI_RESET);
}

int
main(int argc, char *argv[])
{
	test(test_wengine, "wengine");
	return EXIT_SUCCESS;
}
