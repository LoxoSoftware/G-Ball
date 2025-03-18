#include <pebble.h>
#include <string.h>
#include "game.h"

Window*     win_title;
Window*     win_game;
Layer*      gfx_lyr;
Window*     win_enddemo;
Window*     win_endlevel;
AppTimer*   enddemo_redraw_tmr;

GBitmap*    bmp_title;

static void title_gfx(Layer* layer, GContext* ctx)
{
    int footer_h= 32;

    graphics_context_set_compositing_mode(ctx, GCompOpSet);

    graphics_context_set_fill_color(ctx, GColorYellow);
    graphics_fill_rect(ctx,(GRect){
        .origin= (GPoint){ 0, 0-footer_h },
        .size= (GSize){ PBL_DISPLAY_WIDTH, 100 }
    }, 0, 0);
    graphics_context_set_fill_color(ctx, GColorIcterine);
    graphics_fill_rect(ctx,(GRect){
        .origin= (GPoint){ 0, 100-footer_h },
        .size= (GSize){ PBL_DISPLAY_WIDTH, 30 }
    }, 0, 0);
    graphics_context_set_fill_color(ctx, GColorPastelYellow);
    graphics_fill_rect(ctx,(GRect){
        .origin= (GPoint){ 0, 130-footer_h },
        .size= (GSize){ PBL_DISPLAY_WIDTH, 20 }
    }, 0, 0);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx,(GRect){
        .origin= (GPoint){ 0, 150-footer_h },
        .size= (GSize){ PBL_DISPLAY_WIDTH, 18 }
    }, 0, 0);
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx,(GRect){
        .origin= (GPoint){ 0, PBL_DISPLAY_HEIGHT-footer_h },
        .size= (GSize){ PBL_DISPLAY_WIDTH, footer_h }
    }, 0, 0);
    graphics_fill_circle(ctx, (GPoint){ PBL_DISPLAY_WIDTH+8, PBL_DISPLAY_HEIGHT/2 }, 12);

    graphics_context_set_fill_color(ctx, GColorRed);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_stroke_width(ctx, 3);
    graphics_fill_circle(ctx, (GPoint){ 28,38 }, 24);
    graphics_draw_circle(ctx, (GPoint){ 28,38 }, 24);

    graphics_draw_bitmap_in_rect(ctx, bmp_title,
        (GRect){ .origin=(GPoint){21,30}, .size= gbitmap_get_bounds(bmp_title).size });

    char hsstr[64];
    if (best_frames)
        snprintf(hsstr, sizeof(hsstr), "Best time: %d:%d.%d",
                 (int)(best_frames/1800), (int)(best_frames/30)%60, (int)(best_frames*33)%1000);
    else
        snprintf(hsstr, sizeof(hsstr), "Best time: none");
    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_draw_text(ctx, hsstr,
        fonts_get_system_font(FONT_KEY_GOTHIC_18),
        (GRect){(GPoint){16,PBL_DISPLAY_HEIGHT-footer_h+8}, (GSize){PBL_DISPLAY_WIDTH-32,24}},
        GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    graphics_context_set_text_color(ctx, GColorBlack);
    graphics_draw_text(ctx, "Start  ",
        fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
        (GRect){(GPoint){ 0,PBL_DISPLAY_HEIGHT/2-12}, (GSize){PBL_DISPLAY_WIDTH,24}},
        GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);

    graphics_context_set_text_color(ctx, GColorBlack);
    graphics_draw_text(ctx, "v1.1 - (c) Loxo Software",
        fonts_get_system_font(FONT_KEY_GOTHIC_14),
        (GRect){(GPoint){ 2,PBL_DISPLAY_HEIGHT-footer_h-16}, (GSize){PBL_DISPLAY_WIDTH,24}},
        GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void enddemo_gfx(Layer* layer, GContext* ctx)
{
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx,(GRect){
        .origin= (GPoint){ 0, 0 },
        .size= (GSize){ PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT }
    }, 0, 0);

    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_draw_text(ctx, "Game over\n\nMore to come in the future!",
        fonts_get_system_font(FONT_KEY_GOTHIC_28),
        (GRect){(GPoint){16,16}, (GSize){PBL_DISPLAY_WIDTH-32,PBL_DISPLAY_HEIGHT-32}},
        GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void endlevel_gfx(Layer* layer, GContext* ctx)
{
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx,(GRect){
        .origin= (GPoint){ 0, 0 },
        .size= (GSize){ PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT }
    }, 0, 0);

    char msg[64];
    snprintf(msg, sizeof(msg), "Time:\n%d.%03d sec.", (int)(frames/30), (int)(frames*33)%1000);

    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_draw_text(ctx, msg,
        fonts_get_system_font(FONT_KEY_GOTHIC_28),
        (GRect){(GPoint){16,50}, (GSize){PBL_DISPLAY_WIDTH-32,64}},
        GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void endlevel_trigger()
{
    total_frames+= frames;

    if (level_counter >= NLEVELS || level_counter < 0)
        return;
    level_goto_next(&ball);
    gfx_invalidate();
    if (level_counter >= 0)
        window_stack_pop(true);
}

static void enddemo_window_load(Window* window)
{
    //Woah! it's a new record!
    if (total_frames < best_frames || !best_frames)
        best_frames= total_frames;

    layer_set_update_proc(window_get_root_layer(window), (LayerUpdateProc)enddemo_gfx);
    window_stack_remove(win_game, false);
    window_stack_remove(win_endlevel, false);
}

static void endlevel_window_load(Window* window)
{
    layer_set_update_proc(window_get_root_layer(window), (LayerUpdateProc)endlevel_gfx);
    app_timer_register(2000, (AppTimerCallback)endlevel_trigger, NULL);
}

static void endlevel_window_unload(Window* window)
{

}

static void click_startgame()
{
    level_counter= 0;
    total_frames= 0;
    window_stack_push(win_game, true);
}

static void ccp_title(void* context)
{
    window_single_click_subscribe(BUTTON_ID_SELECT, click_startgame);
}

static void title_window_load(Window* window)
{
    bmp_title= gbitmap_create_with_resource(RESOURCE_ID_BMP_TITLE);
    //GColor* title_pal= gbitmap_get_palette(bmp_title);
    GColor* title_pal_mod= (GColor*)malloc(16*sizeof(GColor));
    title_pal_mod[0]= GColorClear;
    title_pal_mod[1]= GColorBlack;
    title_pal_mod[2]= GColorWhite;
    gbitmap_set_palette(bmp_title, title_pal_mod, true);
    layer_set_update_proc(window_get_root_layer(window), (LayerUpdateProc)title_gfx);
    window_set_click_config_provider(window, ccp_title);
}

static void title_window_unload(Window* window)
{

}

static void init()
{
    // Create main Window element and assign to pointer
    win_title = window_create();
    win_game = window_create();
    win_enddemo = window_create();
    win_endlevel = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(win_title, (WindowHandlers) {
        .load = title_window_load,
        .unload = title_window_unload
    });
    window_set_window_handlers(win_game, (WindowHandlers) {
        .load = game_window_load,
        .unload = game_window_unload
    });
    window_set_window_handlers(win_endlevel, (WindowHandlers) {
        .load = endlevel_window_load,
        .unload = endlevel_window_unload
    });
    window_set_window_handlers(win_enddemo, (WindowHandlers) {
        .load = enddemo_window_load,
    });

    // Show the Window on the watch, with animated=true
    window_stack_push(win_title, true);

    light_enable(true);
}

static void deinit()
{
    // Destroy Window
    window_destroy(win_game);
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
}
