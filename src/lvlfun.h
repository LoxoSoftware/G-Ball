#define CURRENT_LEVEL                   (levels[level_counter])
#define STRUCT_ARR(arr, index, type)    (*((type*)((uint32_t)&arr+index*sizeof(type*))))   //I hate this

bool lvl_draw_hidden_override= false;
GDrawCommandImage* img_levelbg= NULL;

extern int          level_counter;
extern Window*      win_enddemo;
extern Window*      win_endlevel;
extern Layer*       gfx_lyr;
extern AppTimer*    render_timer;
extern AppTimer*    lvlend_anim_timer;

#define D_STROKE_SZ         1

uint32_t    frames= 0;
uint32_t    total_frames= 0;
uint32_t    best_frames= 0;
short       anim_lvlend_frames= 0;  //Descending
short       anim_die_frames= 0;     //Ascending
bool        death_by_falling= false;

static const uint8_t endhole_colors[6]=
{
    // 0b11000010, 0b11000011, 0b11000111,
    // 0b11001010, 0b11000111, 0b11000011,
    0b11100011, 0b11010111, 0b11001011,
    0b11001111, 0b11001011, 0b11010111,
};

typedef struct
{
    GPoint  pos;
    GPoint  prev_speed;
    GPoint  next_speed;
    uint8_t radius;
    uint8_t stroke;
    uint8_t mass;
} PlayerBall;

typedef struct
{
    uint8_t     life; //0= projectile is inactive
    GPoint      pos;
    GSize       size;
    GPoint      speed;
} Projectile;

Projectile projectiles[32];

#include "collision.h"

void level_load(PlayerBall* ball, level_elem_t lvldata[], int datac, bool time_reset);

void lvlend_anim_timer_trigger()
{
    anim_lvlend_frames= 0;
    window_stack_push(win_endlevel, true);
    //level_goto_next(ball);
}

void level_collide_projectiles(GContext* ctx, level_elem_t lvldata[], int datac, PlayerBall* ball)
{
    for (uint8_t ip=0; ip<sizeof(projectiles)/sizeof(Projectile); ip++)
    {
        Projectile t= projectiles[ip];

        if (!t.life)
            continue;

        GRect proj_rect_prev= { .origin= t.pos, .size= t.size };
        GRect proj_rect_next= {
            .origin= {
                t.pos.x+t.speed.x,
                t.pos.y+t.speed.y },
            .size= t.size };
        GRect playa_rect= (GRect){
            .origin= (GPoint){ (*ball).pos.x-(*ball).radius-(*ball).stroke, (*ball).pos.y-(*ball).radius-(*ball).stroke },
            .size= (GSize){ (*ball).radius*2+(*ball).stroke*2, (*ball).radius*2+(*ball).stroke*2 }
        };

        //Check for walls
        for (int ie=0; ie<datac; ie++)
        {
            GPoint ent_pos= (GPoint){ .x= lvldata[ie].x, .y= lvldata[ie].y };
            GSize ent_size= (GSize){ .w= lvldata[ie].w, .h= lvldata[ie].h };
            GRect ent_rect= { .origin= ent_pos, .size= ent_size };

            switch (lvldata[ie].type)
            {
                case ent_solidWall:
                    if (!coll_box2box_check(ent_rect, proj_rect_next, NULL))
                        continue;
                    t.life= 0;
                    graphics_context_set_fill_color(ctx, GColorLightGray);
                    graphics_fill_rect(ctx, proj_rect_prev, 0, 0);
                    goto end_projectile_coll_elem_loop;
                default:
                    continue;
            }
        }
end_projectile_coll_elem_loop:

        if (coll_box2box_check(playa_rect, proj_rect_next, NULL))
        {
            death_by_falling= false;
            anim_die_frames= 1;
            return;
        }

        projectiles[ip]= t;
    }
}

level_elem_t* level_check_switch(level_elem_t lvldata[], int datac, PlayerBall* ball, uint8_t switchid)
{
    //Returns NULL if switch is not being stepped on or it doesn't exist
    // Returns the switch element pointer otherwise

    for (int ie=0; ie<datac; ie++)
    {
        GPoint ent_pos= (GPoint){ .x= lvldata[ie].x, .y= lvldata[ie].y };
        GSize ent_size= (GSize){ .w= lvldata[ie].w, .h= lvldata[ie].h };
        GRect ent_rect= { .origin= ent_pos, .size= ent_size };
        GRect playa_rect= (GRect){
            .origin= (GPoint){ (*ball).pos.x-(*ball).radius-(*ball).stroke, (*ball).pos.y-(*ball).radius-(*ball).stroke },
            .size= (GSize){ (*ball).radius*2+(*ball).stroke*2, (*ball).radius*2+(*ball).stroke*2 }
        };

        switch (lvldata[ie].type)
        {
            case ent_switch:
                if (lvldata[ie].switchid != switchid)
                    continue;
                if (coll_box2box_check(playa_rect, ent_rect, NULL))
                    return &lvldata[ie];
                continue;
            default:
                continue;
        }
    }

    return NULL;
}

void level_draw(GContext* ctx, level_elem_t lvldata[], int datac, PlayerBall* ball)
{
    //Draw level elements
    for (int ie=0; ie<datac; ie++)
    {
        GPoint ent_pos= (GPoint){ .x= lvldata[ie].x, .y= lvldata[ie].y };
        GSize ent_size= (GSize){ .w= lvldata[ie].w, .h= lvldata[ie].h };
        GRect ent_rect= { .origin= ent_pos, .size= ent_size };
        GRect playa_rect= (GRect){
            .origin= (GPoint){ (*ball).pos.x-(*ball).radius-(*ball).stroke, (*ball).pos.y-(*ball).radius-(*ball).stroke },
            .size= (GSize){ (*ball).radius*2+(*ball).stroke*2, (*ball).radius*2+(*ball).stroke*2 }
        };

        switch (lvldata[ie].type)
        {
            case ent_end:
                ent_rect= (GRect){ .origin= {ent_pos.x-11,ent_pos.y-11}, .size= (GSize){24,24} };
                graphics_context_set_stroke_color(ctx, GColorBlack);
                graphics_context_set_fill_color(ctx, (GColor){endhole_colors[(frames/3)%6]});
                graphics_context_set_stroke_width(ctx, 3);
                //graphics_draw_circle(ctx, ent_pos, ent_rect.size.h/2);
                graphics_context_set_stroke_color(ctx, GColorBlack);
                graphics_draw_circle(ctx, ent_pos, ent_rect.size.w/2);
                //graphics_context_set_stroke_color(ctx, GColorLightGray);
                //graphics_draw_arc(ctx, ent_rect, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(0+45), DEG_TO_TRIGANGLE(180+45));
                graphics_context_set_stroke_color(ctx, GColorDarkGray);
                //graphics_draw_arc(ctx, ent_rect, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(180+45), DEG_TO_TRIGANGLE(360+45));
                graphics_fill_circle(ctx, ent_pos, ent_rect.size.w/2-1);
                break;
            case ent_hole:
                if (coll_box2box_check(playa_rect, ent_rect, NULL))
                {
                    graphics_context_set_fill_color(ctx, GColorBlack);
                    graphics_fill_rect(ctx, ent_rect, 0, 0);
                }
                break;
            case ent_decoration:
                ent_rect= (GRect){
                    .origin=ent_pos,
                    .size=gdraw_command_image_get_bounds_size(deco_res[lvldata[ie].image]) };
                if (coll_box2box_check(playa_rect, ent_rect, NULL))
                    gdraw_command_image_draw(ctx, deco_res[lvldata[ie].image], ent_pos);
                break;
            case ent_blaster:
                if (lvldata[ie].angle == angle_facingDown || lvldata[ie].angle == angle_facingUp)
                    ent_rect= (GRect){ .origin= ent_pos, .size= { 8, 16 } };
                else
                    ent_rect= (GRect){ .origin= ent_pos, .size= { 16, 8 } };
                graphics_context_set_fill_color(ctx, GColorRed);
                graphics_context_set_stroke_color(ctx, GColorBlack);
                graphics_context_set_stroke_width(ctx, D_STROKE_SZ);
                graphics_fill_rect(ctx, ent_rect, 0, 0);
                graphics_draw_rect(ctx, ent_rect);

                if (frames%lvldata[ie].interval == 0)
                {
                    //Spawn a projectile
                    graphics_context_set_fill_color(ctx, GColorChromeYellow);
                    graphics_fill_rect(ctx, ent_rect, 0, 0);

                    //Find an empty spot in the projectlies array
                    int16_t prj_id= -1;
                    for (uint8_t ip=0; ip<sizeof(projectiles)/sizeof(Projectile); ip++)
                        if (!projectiles[ip].life)
                        {
                            prj_id= ip;
                            break;
                        }
                    if (prj_id < 0) continue;

                    projectiles[prj_id].life= 90;
                    projectiles[prj_id].size= ent_rect.size;
                    switch (lvldata[ie].angle)
                    {
                        case angle_facingUp:
                            projectiles[prj_id].pos= (GPoint){ ent_pos.x, ent_pos.y-8};
                            projectiles[prj_id].speed= (GPoint){ 0, -2 };
                            break;
                        case angle_facingDown:
                            projectiles[prj_id].pos= (GPoint){ ent_pos.x, ent_pos.y+8};
                            projectiles[prj_id].speed= (GPoint){ 0, 2 };
                            break;
                        case angle_facingLeft:
                            projectiles[prj_id].pos= (GPoint){ ent_pos.x-8, ent_pos.y};
                            projectiles[prj_id].speed= (GPoint){ -2, 0 };
                            break;
                        case angle_facingRight:
                            projectiles[prj_id].pos= (GPoint){ ent_pos.x+8, ent_pos.y};
                            projectiles[prj_id].speed= (GPoint){ 2, 0 };
                            break;
                        default:
                            projectiles[prj_id].life= 0;
                            break;
                    }
                }
                break;
            case ent_switch:
                if (coll_box2box_check(playa_rect, ent_rect, NULL))
                {
                    graphics_context_set_fill_color(ctx, GColorDarkGreen);
                    graphics_fill_rect(ctx, ent_rect, 5, GCornersAll);
                    lvldata[ie].frames++;
                }
                else
                {
                    graphics_context_set_fill_color(ctx, GColorInchworm);
                    graphics_fill_rect(ctx, ent_rect, 5, GCornersAll);
                    graphics_context_set_stroke_color(ctx, GColorBlack);
                    graphics_context_set_stroke_width(ctx, 1);
                    ent_rect= (GRect){
                        .origin= {ent_pos.x+4,ent_pos.y+4},
                        .size= {ent_size.w-8,ent_size.h-8 } };
                    graphics_draw_rect(ctx, ent_rect);
                    lvldata[ie].frames= 0;
                }
                break;
            case ent_swblast:
                if (lvldata[ie].angle == angle_facingDown || lvldata[ie].angle == angle_facingUp)
                    ent_rect= (GRect){ .origin= ent_pos, .size= { 8, 16 } };
                else
                    ent_rect= (GRect){ .origin= ent_pos, .size= { 16, 8 } };

                level_elem_t* target_switch= level_check_switch(lvldata, datac, ball, lvldata[ie].switchid);

                if (!target_switch)
                    graphics_context_set_fill_color(ctx, GColorIslamicGreen);
                else
                    graphics_context_set_fill_color(ctx, GColorRed);
                graphics_context_set_stroke_color(ctx, GColorBlack);
                graphics_context_set_stroke_width(ctx, D_STROKE_SZ);
                graphics_fill_rect(ctx, ent_rect, 0, 0);
                graphics_draw_rect(ctx, ent_rect);

                if (target_switch) if ((*target_switch).frames%lvldata[ie].interval == 1)
                {
                    //Spawn a projectile
                    graphics_context_set_fill_color(ctx, GColorChromeYellow);
                    graphics_fill_rect(ctx, ent_rect, 0, 0);

                    //Find an empty spot in the projectlies array
                    int16_t prj_id= -1;
                    for (uint8_t ip=0; ip<sizeof(projectiles)/sizeof(Projectile); ip++)
                        if (!projectiles[ip].life)
                        {
                            prj_id= ip;
                            break;
                        }
                    if (prj_id < 0) continue;

                    projectiles[prj_id].life= 180;
                    projectiles[prj_id].size= ent_rect.size;
                    switch (lvldata[ie].angle)
                    {
                        case angle_facingUp:
                            projectiles[prj_id].pos= (GPoint){ ent_pos.x, ent_pos.y-8};
                            projectiles[prj_id].speed= (GPoint){ 0, -1 };
                            break;
                        case angle_facingDown:
                            projectiles[prj_id].pos= (GPoint){ ent_pos.x, ent_pos.y+8};
                            projectiles[prj_id].speed= (GPoint){ 0, 1 };
                            break;
                        case angle_facingLeft:
                            projectiles[prj_id].pos= (GPoint){ ent_pos.x-8, ent_pos.y};
                            projectiles[prj_id].speed= (GPoint){ -1, 0 };
                            break;
                        case angle_facingRight:
                            projectiles[prj_id].pos= (GPoint){ ent_pos.x+8, ent_pos.y};
                            projectiles[prj_id].speed= (GPoint){ 1, 0 };
                            break;
                        default:
                            projectiles[prj_id].life= 0;
                            break;
                    }
                }
                break;
            default:
                // graphics_context_set_fill_color(ctx, GColorRed);
                // graphics_fill_rect(ctx, ent_rect, 0, 0);
                continue;
        }
    }

    //Draw projectiles
    for (uint8_t ip=0; ip<sizeof(projectiles)/sizeof(Projectile); ip++)
    {
        Projectile t= projectiles[ip];

        if (!t.life)
            continue;

        GRect ent_rect_prev= { .origin= t.pos, .size= t.size };
        GRect ent_rect_next= {
            .origin= {
                t.pos.x+t.speed.x,
                t.pos.y+t.speed.y },
            .size= t.size };

        graphics_context_set_fill_color(ctx, GColorRajah);
        graphics_fill_rect(ctx, ent_rect_prev, 0, 0);
        graphics_context_set_fill_color(ctx, GColorBlack);
        graphics_fill_rect(ctx, ent_rect_next, 0, 0);

        t.pos= ent_rect_next.origin;
        t.life--;

        projectiles[ip]= t;
    }
}

void level_draw_static(GContext* ctx, level_elem_t lvldata[], int datac)
{
    for (int ie=0; ie<datac; ie++)
    {
        GPoint ent_pos= (GPoint){ .x= lvldata[ie].x, .y= lvldata[ie].y };
        GSize ent_size= (GSize){ .w= lvldata[ie].w, .h= lvldata[ie].h };
        GRect ent_rect= { .origin= ent_pos, .size= ent_size };

        switch (lvldata[ie].type)
        {
            case ent_solidWall:
                if (!lvl_draw_hidden_override)
                    continue;
                graphics_context_set_stroke_color(ctx, GColorBlack);
                graphics_context_set_fill_color(ctx, GColorWhite);
                graphics_context_set_stroke_width(ctx, D_STROKE_SZ);
                graphics_fill_rect(ctx, ent_rect, 8, GCornersAll);
                graphics_draw_round_rect(ctx, ent_rect, 8);
                break;
            case ent_hole:
                graphics_context_set_fill_color(ctx, GColorBlack);
                graphics_fill_rect(ctx, ent_rect, 0, 0);
                break;
            case ent_decoration:
                gdraw_command_image_draw(ctx, deco_res[lvldata[ie].image], ent_pos);
                break;
            default:
                continue;
        }
    }
}

void level_goto_next(PlayerBall* ball);
void level_collide(PlayerBall* ball, level_elem_t lvldata[], int datac)
{
    for (int ie=0; ie<datac; ie++)
    {
        GPoint ent_pos= (GPoint){ .x= lvldata[ie].x, .y= lvldata[ie].y };
        GSize ent_size= (GSize){ .w= lvldata[ie].w, .h= lvldata[ie].h };
        GRect ent_rect= (GRect){ .origin= ent_pos, .size= ent_size };
        GRect ent_rect_small= (GRect){
            .origin= (GPoint){ ent_pos.x+(*ball).radius+D_STROKE_SZ, ent_pos.y+(*ball).radius+D_STROKE_SZ },
            .size= (GSize){ ent_size.w-((*ball).radius+D_STROKE_SZ)*2, ent_size.h-((*ball).radius+D_STROKE_SZ)*2 },
        };
        GRect playa_rect= (GRect){
            .origin= (GPoint){ (*ball).pos.x-(*ball).radius-(*ball).stroke, (*ball).pos.y-(*ball).radius-(*ball).stroke },
            .size= (GSize){ (*ball).radius*2+(*ball).stroke*2, (*ball).radius*2+(*ball).stroke*2 }
        };

        switch (lvldata[ie].type)
        {
            case ent_solidWall:
                coll_playa_handle(ball, &ent_rect);
                break;
            case ent_end:
                ent_rect= (GRect){  .origin= (GPoint){ent_pos.x-4, ent_pos.y-4},
                                    .size= (GSize){8, 8} };
                if (coll_box2box_check(playa_rect, ent_rect, NULL))
                {
                    anim_lvlend_frames= 24;
                    lvlend_anim_timer= app_timer_register(800, (AppTimerCallback)lvlend_anim_timer_trigger, NULL);
                    return;
                }
                break;
            case ent_hole:
                if (anim_die_frames > 0)
                    break;

                if (coll_box2box_check(playa_rect, ent_rect_small, NULL))
                {
                    death_by_falling= true;
                    anim_die_frames= 1;
                    //psleep(1000);
                    //level_load(ball, CURRENT_LEVEL.lvlptr, CURRENT_LEVEL.lvl_datac, false);
                    return;
                }
                break;
            default:
                continue;
        }
    }
}

extern bool redraw_level;
void level_load(PlayerBall* ball, level_elem_t lvldata[], int datac, bool time_reset)
{
    app_timer_cancel(render_timer);

    for (uint8_t ip=0; ip<sizeof(projectiles)/sizeof(Projectile); ip++)
        projectiles[ip].life= 0;

    for (int ie=0; ie<datac; ie++)
    {
        GPoint ent_pos= (GPoint){ .x= lvldata[ie].x, .y= lvldata[ie].y };

        switch (lvldata[ie].type)
        {
            case ent_spawn:
                (*ball).pos= ent_pos;
                break;
            default:
                continue;
        }
    }

    redraw_level= true;

    img_levelbg = gdraw_command_image_create_with_resource(CURRENT_LEVEL.image_res);
    if (!img_levelbg)
        lvl_draw_hidden_override= true;

    if (time_reset)
        frames= 0;
}

void level_goto_next(PlayerBall* ball)
{
    level_counter++;

    if (level_counter >= NLEVELS || level_counter < 0)
    {
        window_stack_push(win_enddemo, true);
        level_counter= -1;
    }
    else
    {
        level_load(ball, CURRENT_LEVEL.lvlptr, CURRENT_LEVEL.lvl_datac, true);
        layer_mark_dirty(gfx_lyr);
    }
}
