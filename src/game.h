#include "levels.h"
#include "lvlfun.h"

static PlayerBall   ball;
static AccelData    accel;
AppTimer*           render_timer= NULL;
AppTimer*           lvlend_anim_timer= NULL;

int     level_counter= 0;
bool    redraw_level= true;

bool    lightctl= true;

#define APP_FRAMERATE   (1000/30)

extern Window*      win_enddemo;
extern Layer*       gfx_lyr;

static void ball_update()
{
    level_collide(&ball, CURRENT_LEVEL.lvlptr, CURRENT_LEVEL.lvl_datac);

    //Collide with screen perimeter
    // if (ball.pos.x+ball.prev_speed.x > PBL_DISPLAY_WIDTH-ball.radius)
    //     ball.prev_speed.x= 0, ball.pos.x= PBL_DISPLAY_WIDTH-ball.radius;
    // if (ball.pos.x+ball.prev_speed.x < ball.radius)
    //     ball.prev_speed.x= 0, ball.pos.x= ball.radius;
    // if (ball.pos.y+ball.prev_speed.y > PBL_DISPLAY_HEIGHT-ball.radius)
    //     ball.prev_speed.y= 0, ball.pos.y= PBL_DISPLAY_HEIGHT-ball.radius;
    // if (ball.pos.y+ball.prev_speed.y < ball.radius)
    //     ball.prev_speed.y= 0, ball.pos.y= ball.radius;

    ball.pos.y+= ball.prev_speed.y;
    ball.pos.x+= ball.prev_speed.x;

    if (accel.x>1200) accel.x=1200;
    if (accel.x<-1200) accel.x=-1200;
    if (accel.y>1200) accel.y=1200;
    if (accel.y<-1200) accel.y=-1200;

    ball.prev_speed.x= accel.x/(1000/ball.mass);
    ball.prev_speed.y= -accel.y/(1000/ball.mass);
}

static void gfx_invalidate();
static void gfx_update_proc(Layer* layer, GContext* ctx)
{
    if (accel_service_peek(&accel) < 0)
    {
        //unable to get accelerometer data
        // graphics_context_set_fill_color(ctx, GColorRed);
        // graphics_fill_rect(ctx,
        //     (GRect){(GPoint){.x=0,.y=0},(GSize){.w=PBL_DISPLAY_WIDTH,.h=PBL_DISPLAY_HEIGHT}}
        // , 0,0);
        return;
    }

    graphics_context_set_antialiased(ctx, true);

    //Clean the old ball
    //graphics_context_set_fill_color(ctx, GColorRajah);
    graphics_context_set_fill_color(ctx, GColorYellow);
    graphics_fill_rect(ctx,(GRect){
        .origin= (GPoint){ ball.pos.x-ball.radius, ball.pos.y-ball.radius },
        .size= (GSize){ ball.radius*2+ball.stroke, ball.radius*2+ball.stroke }
    }, 0, 0);

    if (!anim_lvlend_frames && !anim_die_frames)
        ball_update();

    if (redraw_level)
    {
        graphics_context_set_fill_color(ctx, GColorYellow);
        graphics_fill_rect(ctx,
            (GRect){(GPoint){.x=0,.y=0},(GSize){.w=PBL_DISPLAY_WIDTH,.h=PBL_DISPLAY_HEIGHT}}
            , 0,0);

        level_draw_static(ctx, CURRENT_LEVEL.lvlptr, CURRENT_LEVEL.lvl_datac);

        if (!lvl_draw_hidden_override)
        {
            graphics_context_set_fill_color(ctx, GColorWhite);
            gdraw_command_image_set_bounds_size(img_levelbg, (GSize){.w=PBL_DISPLAY_WIDTH,.h=PBL_DISPLAY_HEIGHT});
            gdraw_command_image_draw(ctx, img_levelbg, (GPoint){.x=0,.y=0});
        }

        if (frames > 10)
            redraw_level= false;
    }

    if (!anim_lvlend_frames && !anim_die_frames)
    {
        level_draw(ctx, CURRENT_LEVEL.lvlptr, CURRENT_LEVEL.lvl_datac, &ball);
        level_collide_projectiles(ctx, CURRENT_LEVEL.lvlptr, CURRENT_LEVEL.lvl_datac, &ball);
    }

    if (anim_die_frames < ball.radius)
    {
        if (death_by_falling)
        {
            //Draw player ball
player_draw_default:
            graphics_context_set_stroke_color(ctx, GColorBlack);
            graphics_context_set_fill_color(ctx, GColorRed);
            graphics_context_set_stroke_width(ctx, ball.stroke);
            graphics_fill_circle(ctx, ball.pos, ball.radius-anim_die_frames);
            graphics_draw_circle(ctx, ball.pos, ball.radius-1-anim_die_frames);
            graphics_draw_circle(ctx, ball.pos, ball.radius-anim_die_frames);
        }
        else
        {
            if (anim_die_frames > 0)
            {
                graphics_context_set_stroke_color(ctx, GColorBlack);
                graphics_context_set_fill_color(ctx, GColorLightGray);
                graphics_context_set_stroke_width(ctx, 1);
                GPoint shatterball_pos= (GPoint){
                    .x= ball.pos.x+anim_die_frames*4,
                    .y= ball.pos.y+anim_die_frames*4
                };
                graphics_fill_circle(ctx, shatterball_pos, ball.radius-anim_die_frames);
                graphics_draw_circle(ctx, shatterball_pos, ball.radius-anim_die_frames);
                shatterball_pos= (GPoint){
                    .x= ball.pos.x-anim_die_frames*4,
                    .y= ball.pos.y+anim_die_frames*4
                };
                graphics_fill_circle(ctx, shatterball_pos, ball.radius-anim_die_frames);
                graphics_draw_circle(ctx, shatterball_pos, ball.radius-anim_die_frames);
                shatterball_pos= (GPoint){
                    .x= ball.pos.x-anim_die_frames*4,
                    .y= ball.pos.y-anim_die_frames*4
                };
                graphics_fill_circle(ctx, shatterball_pos, ball.radius-anim_die_frames);
                graphics_draw_circle(ctx, shatterball_pos, ball.radius-anim_die_frames);
                shatterball_pos= (GPoint){
                    .x= ball.pos.x+anim_die_frames*4,
                    .y= ball.pos.y-anim_die_frames*4
                };
                graphics_fill_circle(ctx, shatterball_pos, ball.radius-anim_die_frames);
                graphics_draw_circle(ctx, shatterball_pos, ball.radius-anim_die_frames);
            }
            else goto player_draw_default;
        }
    }
    else
    {
        //Draw cartoony circle transition around the player
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_context_set_stroke_width(ctx, 50);
        graphics_draw_circle(ctx, ball.pos, (PBL_DISPLAY_WIDTH*4)/(anim_die_frames-ball.radius+1));
    }

    if (anim_lvlend_frames)
    {
        graphics_context_set_fill_color(ctx, GColorBlack);
        graphics_fill_circle(ctx, ball.pos, (PBL_DISPLAY_WIDTH*3)/anim_lvlend_frames);
        anim_lvlend_frames--;

        if (anim_lvlend_frames <= 0)
        {
            //psleep(100);
            anim_lvlend_frames= 0;
        }
    }

    if (anim_die_frames)
    {
        anim_die_frames++;

        if (anim_die_frames > 30)
        {
            anim_die_frames= 0;
            level_load(&ball, CURRENT_LEVEL.lvlptr, CURRENT_LEVEL.lvl_datac, false);
            gfx_invalidate();
        }
    }

    frames++;
}

static void gfx_invalidate()
{
    layer_mark_dirty(gfx_lyr);
    render_timer= app_timer_register(APP_FRAMERATE, (AppTimerCallback)gfx_invalidate, NULL);
}

static void click_redrawlvl(ClickRecognizerRef recognizer, void *context)
{
    redraw_level= true;
}
static void click_lightctl(ClickRecognizerRef recognizer, void *context)
{
    lightctl= !lightctl;
    light_enable(lightctl);
}
static void click_nextlvl(ClickRecognizerRef recognizer, void *context)
{
    level_goto_next(&ball);
    gfx_invalidate();
}

static void ccp_gamewindow(void* context)
{
    window_single_click_subscribe(BUTTON_ID_SELECT, click_redrawlvl);
    window_single_click_subscribe(BUTTON_ID_DOWN, click_lightctl);
    //window_single_click_subscribe(BUTTON_ID_UP, click_nextlvl);
}

static void game_window_load(Window *window)
{
    gfx_lyr= window_get_root_layer(window);
    layer_set_update_proc(gfx_lyr, (LayerUpdateProc)gfx_update_proc);

    ball.pos= (GPoint){.x=PBL_DISPLAY_WIDTH/2, .y=PBL_DISPLAY_HEIGHT/4};
    ball.radius= 6;
    ball.stroke= 1;
    ball.mass= 20;
    ball.prev_speed.x= 0;
    ball.prev_speed.y= 0;

    deco_res[deco_img_skull]= gdraw_command_image_create_with_resource(RESOURCE_ID_PDC_SKULL);
    deco_res[deco_img_warning]= gdraw_command_image_create_with_resource(RESOURCE_ID_PDC_WARNING);

    window_set_click_config_provider(window, ccp_gamewindow);

    redraw_level= true;
    level_load(&ball, CURRENT_LEVEL.lvlptr, CURRENT_LEVEL.lvl_datac, true);

    frames= 0;
    anim_lvlend_frames= 0;
    gfx_invalidate();
}

static void game_window_unload(Window *window)
{

}
