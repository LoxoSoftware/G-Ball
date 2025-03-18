#define NLEVELS (int)(sizeof(levels)/sizeof(level_t))

GDrawCommandImage* deco_res[2];

// Entity types
enum entity_t
{
    ent_void= 0,
    ent_spawn, ent_end, ent_hole,
    ent_solidWall, ent_fragileWall,
    ent_decoration, ent_blaster,
    ent_switch, ent_swblast
};

enum entity_angle_t
{
    angle_facingUp, angle_facingRight,
    angle_facingDown, angle_facingLeft
};

enum decoration_image_t
{
    deco_img_skull=0,
    deco_img_warning=1,
};

typedef struct
{
    uint8_t         type;
    uint8_t         switchid;
    int16_t         x, y;
    union
    {
        uint16_t    w;
        uint16_t    scale;
        uint16_t    interval;
    };
    union
    {
        uint16_t    h;
        uint16_t    angle;
    };
    union
    {
        uint32_t    state;
        uint32_t    frames;
        uint32_t    image;
    };
} level_elem_t;

typedef struct
{
    level_elem_t*   lvlptr;
    uint16_t        lvl_datac;
    //GColor          bgcolor;
    uint16_t        image_res;
    //uint16_t        clear_time;
} level_t;

level_elem_t lvl_1_elements[]=
{
    { .type=ent_spawn,      .x=32,.y=120 },
    { .type=ent_solidWall,  .x=0,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=144-8,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=0,.y=0,.w=144,.h=8 },
    { .type=ent_solidWall,  .x=0,.y=168-8,.w=144,.h=8 },
    { .type=ent_solidWall,  .x=8,.y=80,.w=92,.h=16 },
    { .type=ent_end,        .x=24,.y=64 },
};
level_elem_t lvl_2_elements[]=
{
    { .type=ent_spawn,      .x=144-32,.y=120 },
    { .type=ent_decoration, .x=60,.y=86,.image=deco_img_warning },
    { .type=ent_solidWall,  .x=0,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=144-8,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=0,.y=0,.w=144,.h=8 },
    { .type=ent_solidWall,  .x=0,.y=168-8,.w=144,.h=8 },
    { .type=ent_hole,       .x=8,.y=80,.w=28,.h=82 },
    { .type=ent_hole,       .x=144-38,.y=50,.w=30,.h=50 },
    { .type=ent_end,        .x=72,.y=16 },
};
level_elem_t lvl_3_elements[]=
{
    { .type=ent_spawn,      .x=100,.y=32 },
    { .type=ent_solidWall,  .x=60,.y=8,.w=16,.h=30 },
    { .type=ent_solidWall,  .x=0,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=144-8,.y=0,.w=8,.h=110 },
    { .type=ent_solidWall,  .x=0,.y=0,.w=144,.h=8 },
    { .type=ent_solidWall,  .x=0,.y=168-8,.w=144,.h=8 },
    { .type=ent_hole,       .x=8,.y=40,.w=80,.h=40 },
    { .type=ent_hole,       .x=8,.y=80,.w=40,.h=30 },
    { .type=ent_hole,       .x=100,.y=110,.w=50,.h=50 },
    { .type=ent_hole,       .x=8,.y=8,.w=50,.h=50 },
    { .type=ent_end,        .x=40,.y=140 },
};
level_elem_t lvl_4_elements[]=
{
    { .type=ent_spawn,      .x=20,.y=120 },
    { .type=ent_blaster,    .x=90,.y=4,.angle=angle_facingDown,.interval=50 },
    //{ .type=ent_blaster,    .x=40,.y=60 },
    { .type=ent_solidWall,  .x=42,.y=100,.w=14,.h=30 },
    { .type=ent_solidWall,  .x=0,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=144-8,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=0,.y=0,.w=144,.h=8 },
    { .type=ent_solidWall,  .x=0,.y=168-8,.w=144,.h=8 },
    //Middle walls
    { .type=ent_solidWall,  .x=40,.y=60,.w=50,.h=8 },
    { .type=ent_solidWall,  .x=100,.y=60,.w=38,.h=8 },
    { .type=ent_solidWall,  .x=8,.y=100,.w=50,.h=8 },
    { .type=ent_solidWall,  .x=68,.y=100,.w=32,.h=8 },
    { .type=ent_end,        .x=132,.y=32 },
};
level_elem_t lvl_5_elements[]=
{
    { .type=ent_spawn,      .x=110,.y=30 },
    { .type=ent_hole,       .x=44,.y=58,.w=16,.h=70 },
    { .type=ent_solidWall,  .x=44,.y=8,.w=16,.h=50 },
    { .type=ent_solidWall,  .x=88,.y=58,.w=50,.h=14 },
    { .type=ent_solidWall,  .x=52,.y=58,.w=8,.h=14 },
    { .type=ent_solidWall,  .x=60,.y=108,.w=24,.h=20 },
    { .type=ent_decoration, .x=80,.y=16,.image=deco_img_skull },
    //walls
    { .type=ent_solidWall,  .x=0,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=144-8,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=0,.y=0,.w=144,.h=8 },
    { .type=ent_solidWall,  .x=0,.y=168-8,.w=144,.h=8 },
    { .type=ent_blaster,    .x=68,.y=100,.angle=angle_facingUp,.interval=90 },
    { .type=ent_blaster,    .x=68,.y=120,.angle=angle_facingDown,.interval=50 },
    { .type=ent_blaster,    .x=50,.y=114,.angle=angle_facingLeft,.interval=50 },
    { .type=ent_blaster,    .x=78,.y=114,.angle=angle_facingRight,.interval=50 },
    //{ .type=ent_blaster,    .x=40,.y=60 },
    { .type=ent_end,        .x=24,.y=32 },
};
level_elem_t lvl_6_elements[]=
{
    { .type=ent_spawn,      .x=24,.y=40},
    { .type=ent_hole,       .x=100,.y=8,.w=16,.h=92 },
    { .type=ent_hole,       .x=116,.y=146,.w=20,.h=16 },
    { .type=ent_solidWall,  .x=8,.y=0,.w=64,.h=24 },
    { .type=ent_solidWall,  .x=8,.y=60,.w=64,.h=8 },
    { .type=ent_solidWall,  .x=46,.y=100,.w=36,.h=16 },
    { .type=ent_solidWall,  .x=90,.y=100,.w=24,.h=16 },
    { .type=ent_switch,     .x=76,.y=64,.w=20,.h=20,.switchid=0 },
    { .type=ent_swblast,    .x=82,.y=0,.angle=angle_facingDown,.interval=15,.switchid=0 },
    //Corners
    { .type=ent_solidWall,  .x=0,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=144-8,.y=0,.w=8,.h=168 },
    { .type=ent_solidWall,  .x=70,.y=0,.w=70,.h=8 },
    { .type=ent_solidWall,  .x=0,.y=168-8,.w=144,.h=8 },
    { .type=ent_end,        .x=130,.y=24 },
};
level_elem_t lvl_debug_elements[]=
{
    { .type=ent_spawn,      .x=32,.y=32 },
    { .type=ent_solidWall,  .x=60,.y=0,.w=32,.h=10 },
    { .type=ent_blaster,    .x=72,.y=70,.angle=angle_facingUp,.interval=10 },
    { .type=ent_blaster,    .x=72,.y=90,.angle=angle_facingDown,.interval=25 },
    { .type=ent_blaster,    .x=60,.y=80,.angle=angle_facingLeft,.interval=40 },
    { .type=ent_blaster,    .x=80,.y=80,.angle=angle_facingRight,.interval=50 },
};

level_t levels[]=
{
    //{ lvl_debug_elements, sizeof(lvl_debug_elements)/sizeof(level_elem_t), 0 },
    { lvl_1_elements, sizeof(lvl_1_elements)/sizeof(level_elem_t), RESOURCE_ID_PDC_LVL_1 },
    { lvl_2_elements, sizeof(lvl_2_elements)/sizeof(level_elem_t), RESOURCE_ID_PDC_LVL_2 },
    { lvl_3_elements, sizeof(lvl_3_elements)/sizeof(level_elem_t), RESOURCE_ID_PDC_LVL_3 },
    { lvl_4_elements, sizeof(lvl_4_elements)/sizeof(level_elem_t), RESOURCE_ID_PDC_LVL_4 },
    { lvl_5_elements, sizeof(lvl_5_elements)/sizeof(level_elem_t), RESOURCE_ID_PDC_LVL_5 },
    { lvl_6_elements, sizeof(lvl_6_elements)/sizeof(level_elem_t), RESOURCE_ID_PDC_LVL_6 },
};
