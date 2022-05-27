#include <allegro.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <math.h>

float getPt( int n1, int n2, float perc )
{
    return n1 + ( (n2 - n1) * perc );
}

int main(int argc, char *argv[])
{
    allegro_init();
    install_mouse();
    install_keyboard();
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, 0);

    set_color_depth(desktop_color_depth());
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,1200, 800,0,0)!=0)
    {
        allegro_message("prb gfx mode");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    show_mouse(screen);


    std::string nomImage = "Plane.pcx";
    char *charNomImage = new char [nomImage.length()+1];
    std::strcpy (charNomImage, nomImage.c_str());

    BITMAP *bmp;
    bmp=load_bitmap(charNomImage,NULL);
    if (!bmp)
    {
        std::cout << "'" << nomImage << "' has not been found or is corrupted." << std::endl;
        exit(EXIT_FAILURE);
    }


    //////////////////////////////////

    BITMAP* buffer = create_bitmap(1200, 800);
    BITMAP* buffer2 = create_bitmap(1200, 800);
    int x1 = 300, y1 = 500;
    int x2 = 500, y2 = 400;
    int x3 = 700, y3 = 300;
    int xa, ya, xb, yb, x, y;

    circlefill(buffer2, x1, y1, 3, makecol(255, 255, 255));
    circlefill(buffer2, x2, y2, 3, makecol(255, 255, 255));
    circlefill(buffer2, x3, y3, 3, makecol(255, 255, 255));


    float step = 0;
    float simulationSpeed = 0.001;

    float d_x, d_y;


    float img_rot;
    int precPtx = x1, precPty = y1;

    while(true)
    {
        for( float i = 0 ; i < 1 ; i += 0.01 )
        {
            // The Green Line
            xa = getPt( x1, x2, i );
            ya = getPt( y1, y2, i );
            xb = getPt( x2, x3, i );
            yb = getPt( y2, y3, i );

            // The Black Dot
            x = getPt( xa, xb, i );
            y = getPt( ya, yb, i );


            line(buffer, precPtx, precPty, x, y, makecol(0, 96, 255));
            precPtx = x;
            precPty = y;
        }
        precPtx = x1;
        precPty = y1;


        // The Green Line
        xa = getPt( x1, x2, step );
        ya = getPt( y1, y2, step );
        xb = getPt( x2, x3, step );
        yb = getPt( y2, y3, step );

        // The Black Dot
        x = getPt( xa, xb, step );
        y = getPt( ya, yb, step );
        blit(buffer,buffer2,0,0,0,0,buffer->w,buffer->h);


        float Pt1_x = x; //center image
        float Pt1_y = y;
        float Pt3_x = getPt( getPt( x1, x2, step+100*simulationSpeed ), getPt( x2, x3, step+100*simulationSpeed ), step+100*simulationSpeed );
        float Pt3_y = getPt( getPt( y1, y2, step+100*simulationSpeed ), getPt( y2, y3, step+100*simulationSpeed ), step+100*simulationSpeed );
        circlefill(buffer2, Pt3_x, Pt3_y, 5, makecol(140, 140, 80));

        int center_x = x-(bmp->w*0.08)/2;
        int center_y = y-(bmp->h*0.08)/2;

        d_x = Pt3_x - Pt1_x;
        d_y = Pt3_y - Pt1_y;

        if((d_x*d_x + d_y*d_y) != 0)
        {
            img_rot = atan2(d_y, d_x)*128.0/M_PI;
        }

        rotate_scaled_sprite(buffer2, bmp, center_x, center_y, ftofix(float(img_rot+32)), ftofix(float(0.08)));    //0 to 259
        blit(buffer2,screen,0,0,0,0,buffer2->w,buffer2->h);
        usleep(600);

        step+=simulationSpeed;

        if(step >=1)
        {
            step=0;
        }


        x2 = 450;
        y2 = 300;

        blit(buffer2,screen,0,0,0,0,buffer2->w,buffer2->h);
        clear_bitmap(buffer);
    }


    return 0;
}
END_OF_MAIN();
