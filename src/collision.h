//#include <math.h>
#define PI 3.14159

#define __PLAYA_RECT GRect playa_rect= (GRect){ \
                .origin= (GPoint){ (*ball).pos.x+ix-(*ball).radius-(*ball).stroke, (*ball).pos.y+iy-(*ball).radius-(*ball).stroke }, \
                .size= (GSize){ (*ball).radius*2+(*ball).stroke*2, (*ball).radius*2+(*ball).stroke*2 } \
            }

#define COLLDIR_UP      0b0001
#define COLLDIR_DOWN    0b0010
#define COLLDIR_LEFT    0b0100
#define COLLDIR_RIGHT   0b1000

bool coll_box2box_check(GRect boxA, GRect boxB, uint8_t* colldir)
{
    if (colldir)
    {
        (*colldir) = 0;

        (*colldir) +=  (boxA.origin.x < boxB.origin.x + boxB.size.w &&
                    boxA.origin.x + boxA.size.w > boxB.origin.x &&
                    boxA.origin.y < boxB.origin.y + 1 &&
                    boxA.origin.y + boxA.size.h > boxB.origin.y)? COLLDIR_UP:0;
        (*colldir) +=  (boxA.origin.x < boxB.origin.x + boxB.size.w &&
                    boxA.origin.x + boxA.size.w > boxB.origin.x &&
                    boxA.origin.y < boxB.origin.y + boxB.size.h &&
                    boxA.origin.y + boxA.size.h > boxB.origin.y + boxB.size.h - 1)? COLLDIR_DOWN:0;
        (*colldir) +=  (boxA.origin.x < boxB.origin.x + 1 &&
                    boxA.origin.x + boxA.size.w > boxB.origin.x &&
                    boxA.origin.y < boxB.origin.y + boxB.size.h &&
                    boxA.origin.y + boxA.size.h > boxB.origin.y)? COLLDIR_LEFT:0;
        (*colldir) +=  (boxA.origin.x < boxB.origin.x + boxB.size.w &&
                    boxA.origin.x + boxA.size.w > boxB.origin.x + boxB.size.w - 1 &&
                    boxA.origin.y < boxB.origin.y + boxB.size.h &&
                    boxA.origin.y + boxA.size.h > boxB.origin.y)? COLLDIR_RIGHT:0;
    }

    return  boxA.origin.x < boxB.origin.x + boxB.size.w &&
            boxA.origin.x + boxA.size.w > boxB.origin.x &&
            boxA.origin.y < boxB.origin.y + boxB.size.h &&
            boxA.origin.y + boxA.size.h > boxB.origin.y;
}

void coll_playa_handle(PlayerBall* ball, GRect* box)
{
    uint8_t colldir= 0;

    //y>0 && x>0
    for (int iy=0; iy<=(*ball).prev_speed.y; iy++)
    {
        bool has_collided= false;

        for (int ix=0; ix<=(*ball).prev_speed.x; ix++)
        {
            __PLAYA_RECT;

            if (coll_box2box_check(playa_rect, *box, &colldir))
            {
                if (colldir&COLLDIR_LEFT)
                    (*ball).prev_speed.x= 0;
                if (colldir&COLLDIR_UP)
                    (*ball).prev_speed.y= 0;
                // (*ball).pos.x-= (*ball).prev_speed.x+ix;
                // (*ball).pos.y-= (*ball).prev_speed.y+iy;
                has_collided= true;
                break;
            }
        }

        if (has_collided) break;
    }
    //y<0 && x>0
    for (int iy=0; iy>=(*ball).prev_speed.y; iy--)
    {
        bool has_collided= false;

        for (int ix=0; ix<=(*ball).prev_speed.x; ix++)
        {
            __PLAYA_RECT;

            if (coll_box2box_check(playa_rect, *box, &colldir))
            {
                if (colldir&COLLDIR_LEFT)
                    (*ball).prev_speed.x= 0;
                if (colldir&COLLDIR_DOWN)
                    (*ball).prev_speed.y= 0;
                // (*ball).pos.x-= (*ball).prev_speed.x-ix;
                // (*ball).pos.y-= (*ball).prev_speed.y-iy;
                has_collided= true;
                break;
            }
        }

        if (has_collided) break;
    }
    //y>0 && x<0
    for (int iy=0; iy<=(*ball).prev_speed.y; iy++)
    {
        bool has_collided= false;

        for (int ix=0; ix>=(*ball).prev_speed.x; ix--)
        {
            __PLAYA_RECT;

            if (coll_box2box_check(playa_rect, *box, &colldir))
            {
                if (colldir&COLLDIR_RIGHT)
                    (*ball).prev_speed.x= 0;
                if (colldir&COLLDIR_UP)
                    (*ball).prev_speed.y= 0;
                // (*ball).pos.x-= (*ball).prev_speed.x-ix;
                // (*ball).pos.y-= (*ball).prev_speed.y-iy;
                has_collided= true;
                break;
            }
        }

        if (has_collided) break;
    }
    //y<0 && x<0
    for (int iy=0; iy>=(*ball).prev_speed.y; iy--)
    {
        bool has_collided= false;

        for (int ix=0; ix>=(*ball).prev_speed.x; ix--)
        {
            __PLAYA_RECT;

            if (coll_box2box_check(playa_rect, *box, &colldir))
            {
                if (colldir&COLLDIR_RIGHT)
                    (*ball).prev_speed.x= 0;
                if (colldir&COLLDIR_DOWN)
                    (*ball).prev_speed.y= 0;
                // (*ball).pos.x-= (*ball).prev_speed.x-ix;
                // (*ball).pos.y-= (*ball).prev_speed.y-iy;
                has_collided= true;
                break;
            }
        }

        if (has_collided) break;
    }
}
