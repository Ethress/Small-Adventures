#include <allegro.h>
#include <math.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define FOV 500  // Field of View for 3D projection
#define MOVE_SPEED 5.0
#define ROTATION_SPEED 0.05

//====== CROSSHAIR ======//
#define CROSSHAIR_HEIGHT    10
#define CROSSHAIR_WIDTH     2

#define GRAVITY 9.8  // Gravity constant

typedef struct
{
    float x, y, z;
} Point3D;

float yaw = 0;   // Rotation around the y-axis (left/right)
float pitch = 0; // Rotation around the x-axis (up/down)
float cam_x = 0, cam_y = 0, cam_z = -300; // Initial camera position



// Function to rotate a point relative to the camera's yaw and pitch
Point3D transform_point(Point3D p, float cam_x, float cam_y, float cam_z, float yaw, float pitch)
{
    // Translate the point relative to the camera's position
    p.x -= cam_x;
    p.y -= cam_y;
    p.z -= cam_z;

    // Apply yaw rotation (around the y-axis)
    float cos_yaw = cos(yaw);
    float sin_yaw = sin(yaw);
    float x_rot = p.x * cos_yaw - p.z * sin_yaw;
    float z_rot = p.x * sin_yaw + p.z * cos_yaw;

    // Apply pitch rotation (around the x-axis)
    float cos_pitch = cos(pitch);
    float sin_pitch = sin(pitch);
    float y_rot = p.y * cos_pitch - z_rot * sin_pitch;
    float z_final = p.y * sin_pitch + z_rot * cos_pitch;

    Point3D transformed = {x_rot, y_rot, z_final};
    return transformed;
}


// Function to project 3D points onto a 2D plane
void project_point(Point3D p, float *screen_x, float *screen_y)
{
    if (p.z > 0)
    {
        float scale = FOV / p.z;
        *screen_x = SCREEN_W / 2 + p.x * scale;
        *screen_y = SCREEN_H / 2 - p.y * scale;
    }
    else
    {
        *screen_x = -1;
        *screen_y = -1;
    }
}



// Function to draw a projectile trajectory starting from the camera position and following the direction the camera is looking
void draw_trajectory(BITMAP *buffer, Point3D start, Point3D direction, float initial_velocity, float cam_x, float cam_y, float cam_z, float yaw, float pitch) {
    float time_step = 0.1;
    float t = 0.0;

    // Normalize the direction vector
    float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    direction.x /= magnitude;
    direction.y /= magnitude;
    direction.z /= magnitude;

    // Scale by initial velocity to get initial velocity components
    float v_x = initial_velocity * direction.x;
    float v_y = initial_velocity * direction.y;
    float v_z = initial_velocity * direction.z;

    for (int i = 0; i < 200; i++) { // Adjust loop count for trajectory length
        // Calculate the projectile's position at time t
        Point3D p;
        p.x = start.x + v_x * t;
        p.y = start.y + (v_y * t - 0.5 * GRAVITY * t * t);
        p.z = start.z + v_z * t;

        // Transform and project the point to the 2D screen
        p = transform_point(p, cam_x, cam_y, cam_z, yaw, pitch);
        float screen_x, screen_y;
        project_point(p, &screen_x, &screen_y);

        // Draw the point if it's visible
        if (screen_x >= 0 && screen_y >= 0 && screen_x < SCREEN_W && screen_y < SCREEN_H) {
            putpixel(buffer, (int)screen_x, (int)screen_y, makecol(255, 255, 0)); // Yellow color for the trajectory
        }

        t += time_step; // Increment time step
    }
}

void compute_camera_vectors(float yaw, float pitch, Point3D *forward, Point3D *right, Point3D *up)
{
    // Calculate the forward vector based on yaw and pitch
    forward->x = cos(pitch) * sin(yaw);
    forward->y = sin(pitch);
    forward->z = cos(pitch) * cos(yaw);

    // Calculate the right vector (perpendicular to forward, flat on the horizontal plane)
    right->x = cos(yaw);
    right->y = 0;  // Flat on the horizontal plane
    right->z = -sin(yaw);

    // Calculate the up vector (perpendicular to both forward and right)
    up->x = -sin(pitch) * sin(yaw);
    up->y = cos(pitch);
    up->z = -sin(pitch) * cos(yaw);
}


// Draw a wireframe sphere
void draw_wireframe_sphere(BITMAP *buffer, float radius, int segments, float cam_x, float cam_y, float cam_z)
{
    float theta_step = M_PI / segments;
    float phi_step = 2 * M_PI / segments;

    for (float theta = 0; theta <= M_PI; theta += theta_step)
    {
        for (float phi = 0; phi < 2 * M_PI; phi += phi_step)
        {
            Point3D p1 = {radius * sin(theta) * cos(phi), radius * sin(theta) * sin(phi), radius * cos(theta)};
            Point3D p2 = {radius * sin(theta + theta_step) * cos(phi), radius * sin(theta + theta_step) * sin(phi), radius * cos(theta + theta_step)};
            Point3D p3 = {radius * sin(theta) * cos(phi + phi_step), radius * sin(theta) * sin(phi + phi_step), radius * cos(theta)};

            p1 = transform_point(p1, cam_x, cam_y, cam_z, yaw, pitch);
            p2 = transform_point(p2, cam_x, cam_y, cam_z, yaw, pitch);
            p3 = transform_point(p3, cam_x, cam_y, cam_z, yaw, pitch);

            float x1, y1, x2, y2, x3, y3;
            project_point(p1, &x1, &y1);
            project_point(p2, &x2, &y2);
            project_point(p3, &x3, &y3);

            if (x1 >= 0 && x2 >= 0) line(buffer, x1, y1, x2, y2, makecol(255, 255, 255));
            if (x1 >= 0 && x3 >= 0) line(buffer, x1, y1, x3, y3, makecol(255, 255, 255));
        }
    }
}

const char* get_compass_direction(float yaw) {
    // Normalize the yaw angle to be between 0 and 2*PI
    while (yaw < 0) yaw += 2 * M_PI;
    while (yaw >= 2 * M_PI) yaw -= 2 * M_PI;

    // Convert yaw to degrees for easier interpretation
    float yaw_degrees = yaw * 180 / M_PI;

    if (yaw_degrees >= 337.5 || yaw_degrees < 22.5) return "N";
    else if (yaw_degrees >= 22.5 && yaw_degrees < 67.5) return "NE";
    else if (yaw_degrees >= 67.5 && yaw_degrees < 112.5) return "E";
    else if (yaw_degrees >= 112.5 && yaw_degrees < 157.5) return "SE";
    else if (yaw_degrees >= 157.5 && yaw_degrees < 202.5) return "S";
    else if (yaw_degrees >= 202.5 && yaw_degrees < 247.5) return "SW";
    else if (yaw_degrees >= 247.5 && yaw_degrees < 292.5) return "W";
    else return "NW";
}

void draw_compass_bar(BITMAP *buffer, float yaw) {
    int bar_width = SCREEN_W/2;
    int bar_height = 30;
    int center_x = SCREEN_W / 2;

    // Convert yaw to degrees and normalize to 0–360
    float yaw_degrees = yaw * 180 / M_PI;
    if (yaw_degrees < 0) yaw_degrees += 360;  // Ensure positive degrees

    // Draw the background of the compass bar (optionnal)
    //rectfill(buffer, 0 + center_x - bar_width/2, 0, bar_width  + center_x - bar_width/2, bar_height, makecol(50, 50, 50));
    //line(buffer, 0 + center_x - bar_width/2, bar_height, bar_width + center_x - bar_width/2, bar_height, makecol(255, 255, 255)); // Bottom border

    // Draw the degree markers and labels
    for (int i = -center_x; i <= center_x; i++) {
        // Determine degree position for each pixel relative to center_x and wrap within 0–359
        int degree_number = ((int)yaw_degrees + i + 360) % 360;

        // Position of the degree marker on the screen
        int pos_x = center_x + i;

        // Only draw if pos_x is within the screen boundaries
        if (pos_x >= 0 && pos_x < bar_width) {
            if (degree_number % 30 == 0) {
                // Larger markers for every 30 degrees
                line(buffer, pos_x + center_x - bar_width/2, 5, pos_x + center_x - bar_width/2, bar_height - 5, makecol(255, 255, 255));
                textprintf_centre_ex(buffer, font, pos_x + center_x - bar_width/2, 10, makecol(255, 255, 255), -1, "%d", degree_number);
            } else if (degree_number % 10 == 0) {
                // Smaller markers for every 10 degrees
                line(buffer, pos_x + center_x - bar_width/2, 15, pos_x + center_x - bar_width/2, bar_height - 10, makecol(200, 200, 200));
            }
        }
    }

    // Draw the center indicator (representing the current direction)
    line(buffer, center_x, 0, center_x, bar_height, makecol(255, 0, 0));  // Red center line
}





void display_UI(BITMAP *buffer, float cam_x, float cam_y, float cam_z, float yaw, float pitch){
    //Coordinate display
    textout_ex(buffer, font, "Coordinates :", 5, 5, makecol(255, 255, 255), -1);
    textprintf_ex(buffer, font, 5, 15, makecol(255, 255, 255), -1, "x: %f", cam_x);
    textprintf_ex(buffer, font, 5, 25, makecol(255, 255, 255), -1, "y: %f", cam_y);
    textprintf_ex(buffer, font, 5, 35, makecol(255, 255, 255), -1, "z: %f", cam_z);
    textprintf_ex(buffer, font, 5, 45, makecol(255, 255, 255), -1, "yaw: %f", yaw);
    textprintf_ex(buffer, font, 5, 55, makecol(255, 255, 255), -1, "pitch: %f", pitch);

    /*
    // Display compass direction
    const char* compass_direction = get_compass_direction(yaw);
    textout_centre_ex(buffer, font, compass_direction, SCREEN_W / 2, 5, makecol(255, 255, 255), -1);
    */

    draw_compass_bar(buffer, yaw);

    //Crosshair
    int crosshair_width = CROSSHAIR_WIDTH;
    int crosshair_size = CROSSHAIR_HEIGHT;
    rectfill(buffer, int(SCREEN_W/2 - crosshair_width/2), int(SCREEN_H/2 - crosshair_size/2), int(SCREEN_W/2 + crosshair_width/2), int(SCREEN_H/2 + crosshair_size/2), makecol(255, 255, 255));
    rectfill(buffer, int(SCREEN_W/2 - crosshair_size/2), int(SCREEN_H/2 - crosshair_width/2), int(SCREEN_W/2 + crosshair_size/2), int(SCREEN_H/2 + crosshair_width/2), makecol(255, 255, 255));
}

int main()
{
    allegro_init();
    install_keyboard();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_W, SCREEN_H, 0, 0);

    bool exit_program = false;
    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);

    float initial_velocity = 300.0; // Example initial velocity
    float angle = M_PI / 4; // 45 degrees in radians

    while (!exit_program)
    {
        if (key[KEY_ESC]) exit_program = true;

        Point3D forward, right, up;
        compute_camera_vectors(yaw, pitch, &forward, &right, &up);

        if (key[KEY_W])
        {
            cam_x += forward.x * MOVE_SPEED;
            cam_y += forward.y * MOVE_SPEED;
            cam_z += forward.z * MOVE_SPEED;
        }
        if (key[KEY_S])
        {
            cam_x -= forward.x * MOVE_SPEED;
            cam_y -= forward.y * MOVE_SPEED;
            cam_z -= forward.z * MOVE_SPEED;
        }
        if (key[KEY_A])
        {
            cam_x -= right.x * MOVE_SPEED;
            cam_z -= right.z * MOVE_SPEED;
        }
        if (key[KEY_D])
        {
            cam_x += right.x * MOVE_SPEED;
            cam_z += right.z * MOVE_SPEED;
        }
        if (key[KEY_SPACE])
        {
            cam_y += MOVE_SPEED;
        }
        if (key[KEY_LSHIFT])
        {
            cam_y -= MOVE_SPEED;
        }

        if (key[KEY_LEFT]) yaw -= ROTATION_SPEED;
        if (key[KEY_RIGHT]) yaw += ROTATION_SPEED;
        if (key[KEY_DOWN]) pitch -= ROTATION_SPEED;
        if (key[KEY_UP]) pitch += ROTATION_SPEED;

        if (pitch > M_PI / 2) pitch = M_PI / 2;
        if (pitch < -M_PI / 2) pitch = -M_PI / 2;

        clear_to_color(buffer, makecol(0, 0, 0));
        draw_wireframe_sphere(buffer, 100, 20, cam_x, cam_y, cam_z);

        // Display the projectile trajectory when 'E' is pressed
        if (key[KEY_E]) {
            Point3D start = {cam_x, cam_y, cam_z};
            draw_trajectory(buffer, start, forward, initial_velocity, cam_x, cam_y, cam_z, yaw, pitch);
        }



        display_UI(buffer, cam_x, cam_y, cam_z, yaw, pitch);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        rest(20);
    }

    return 0;
}
END_OF_MAIN();
