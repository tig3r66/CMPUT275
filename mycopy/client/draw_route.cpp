#include "include/draw_route.h"
#include "include/map_drawing.h"

extern shared_vars shared;

void draw_route() {
    for (int i = 0; i < shared.num_waypoints - 1; i++) {
        // getting the two adjacent points
        lon_lat_32 pt1 = shared.waypoints[i];
        lon_lat_32 pt2 = shared.waypoints[i+1];
        // getting first point
        xy_pos pt1_loc = xy_pos(longitude_to_x(shared.map_number, pt1.lon),
            latitude_to_y(shared.map_number, pt1.lat));
        pt1_loc.x -= shared.map_coords.x;
        pt1_loc.y -= shared.map_coords.y;
        // getting second point
        xy_pos pt2_loc = xy_pos(longitude_to_x(shared.map_number, pt2.lon),
            latitude_to_y(shared.map_number, pt2.lat));
        pt2_loc.x -= shared.map_coords.x;
        pt2_loc.y -= shared.map_coords.y;

        shared.tft->drawLine(pt1_loc.x, pt1_loc.y, pt2_loc.x, pt2_loc.y,
            TFT_RED);
    }
}
