#include <cairo.h>

#include "lib/graphics.h"
#include "lib/shmemfonts.h"
#include "lib/decorations.h"
#include "lib/kbd.h"
#include "lib/yutani.h"
#include "lib/toyos_auth.h"
#include "lib/confreader.h"

yutani_t * yctx = NULL;
yutani_window_t * window       = NULL; /* GUI window */
gfx_context_t * ctx;

uint32_t window_width  = 640;
uint32_t window_height = 408;

static void render_decors(void) {
	/* XXX Make the decorations library support Yutani windows */
	render_decorations(window, ctx, "Settings");
}

static void refresh(void) {
	draw_fill(ctx, rgba(255, 255, 255, 0));
	render_decors();
}

int main(int argc, char ** argv) {
	yctx = yutani_init();
	window = yutani_window_create(yctx, window_width, window_height);
	window->focused = 1;
	ctx = init_graphics_yutani(window);
	draw_fill(ctx, rgba(255, 255, 255, 0));
	yutani_window_move(yctx, window, yctx->display_width / 2 - window->width / 2, yctx->display_height / 2 - window->width / 2);
	yutani_window_advertise_icon(yctx, window, "Settings", "gears");
	init_decorations();
	int should_exit = 0;
	while (!should_exit) {
		yutani_msg_t * m = yutani_poll_async(yctx);
		if (m) {
			switch (m->type) {
				case YUTANI_MSG_KEY_EVENT:
					{
						struct yutani_msg_key_event * ke = (void*)m->data;
						if (ke->event.action == KEY_ACTION_DOWN) {
							switch (ke->event.keycode) {
								case 'q':
									should_exit = 1;
									free(m);
									goto finish;
								default:
									break;
							}
						}
					}
					break;
				case YUTANI_MSG_WINDOW_MOUSE_EVENT:
					{
						struct yutani_msg_window_mouse_event * me = (void*)m->data;
						if (me->command == YUTANI_MOUSE_EVENT_DOWN && me->buttons & YUTANI_MOUSE_BUTTON_LEFT) {
							yutani_window_drag_start(yctx, window);
						}
					}
					break;
				case YUTANI_MSG_SESSION_END:
					should_exit = 1;
					break;
				case YUTANI_MSG_RESIZE_OFFER:
					{
						struct yutani_msg_window_resize * wr = (void*)m->data;
						yutani_window_resize_accept(yctx, window, wr->width, wr->height);
						reinit_graphics_yutani(ctx, window);
						refresh();
						yutani_window_resize_done(yctx, window);
						flip(ctx);
						yutani_flip(yctx, window);
					}
				default:
					break;
			}
			free(m);
		}
	}
	refresh();
	flip(ctx);
	yutani_flip(yctx, window);
	syscall_yield();
finish:
	yutani_close(yctx, window);
}
