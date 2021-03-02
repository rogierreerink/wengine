#include "ansi.h"
#include "test.h"
#include "wengine.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <wchar.h>

#define TICK_FREQUENCY 25

static window_t *win1, *win2, *win3, *win4, *win5, *win6;
static charstyle_t cstyle;

static winstyle_t style1 = {
	.margin = {
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
	.margin = {
		.top = {
			.flex = WINFLEX_RELATIVE,
			.value.relative = 0.5,
		},
		.right = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 12,
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
	.margin = {
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
	.margin = {
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

static winstyle_t style5 = {
	.margin = {
		.top = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 4,
		},
		.right = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 8,
		},
		.bottom = {
			.flex = WINFLEX_FREE,
		},
		.left = {
			.flex = WINFLEX_FREE,
		},
	},
	.dimension = {
		.width = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 32,
		},
		.height = {
			.flex = WINFLEX_RELATIVE,
			.value.relative = 0.25,
		},
	},
	.background = {
		.character = '*',
	},
};

static winstyle_t style6 = {
	.margin = {
		.top = {
			.flex = WINFLEX_FREE,
			.value.absolute = 2,
		},
		.right = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 32,
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
	.dimension = {
		.height = {
			.flex = WINFLEX_ABSOLUTE,
			.value.absolute = 8,
		},
	},
	.line_wrap = 1,
	.background = {
		.character = ' ',
	},
};

static void
winsizepos_tick(window_t *win, uint64_t tick_count)
{
	wchar_t buffer[128];
	size_t buflen;
	
	if (win->data != NULL)
	{
		uint64_t *last_invoke_tick_count = (uint64_t *)(win->data);
		uint64_t *number_of_newlines = (uint64_t *)(win->data + sizeof(uint64_t));
		
		if (tick_count - *last_invoke_tick_count >= TICK_FREQUENCY / 25)
		{
			*last_invoke_tick_count = tick_count;
			(*number_of_newlines)++;
			if (*number_of_newlines > win->dimension.y * 2)
			{
				*number_of_newlines = 0;
			}
		}
		
		window_clear(win);

		for (uint8_t i = 0; i < *number_of_newlines; i++)
		{
			window_write(win, L"\n", 1, cstyle);
		}

		buflen = swprintf(buffer, 128, L"%d, %d, %d, %d, %lu, %lu",
				win->dimension.x, win->dimension.y,
				win->position.x, win->position.y, tick_count,
				*number_of_newlines);

		window_write(win, buffer, buflen, cstyle);
	}
}

void
callback_input(const wchar_t *buffer, size_t size)
{
	window_write(win6, buffer, size, cstyle);
}

test_return_t
test_wengine(void)
{
	win1 = window_create(&style1);
	win1->callback_tick = winsizepos_tick;
	win1->data = (uint64_t *)calloc(2, sizeof(uint64_t));

	win2 = window_create(&style2);
	win2->callback_tick = winsizepos_tick;
	win2->data = (uint64_t *)calloc(2, sizeof(uint64_t));

	win3 = window_create(&style3);
	win3->callback_tick = winsizepos_tick;
	win3->data = (uint64_t *)calloc(2, sizeof(uint64_t));

	win4 = window_create(&style4);
	win4->callback_tick = winsizepos_tick;
	win4->data = (uint64_t *)calloc(2, sizeof(uint64_t));

	win5 = window_create(&style5);
	win5->callback_tick = winsizepos_tick;
	win5->data = (uint64_t *)calloc(2, sizeof(uint64_t));
	
	win6 = window_create(&style6);
	
	engine_setup();

	wm_callback_input = callback_input;

	wm_window_show(win6);
	wm_window_show(win1);
	wm_window_show(win2);
	wm_window_show(win3);
	wm_window_show(win4);
	wm_window_show(win5);

	wm_window_tofront(win3);
	wm_window_tofront(win2);
	wm_window_tofront(win5);
	
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
	window_destroy(win5);
	window_destroy(win6);
	
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
