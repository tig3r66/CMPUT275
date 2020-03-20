#include "include/draw_route.h"
#include "include/map_drawing.h"

extern shared_vars shared;

void draw_route() {
	for (int i = 0; i < numwaypoints-1; i++) {
		lon_lat_32 waypointOne = shared.waypoints[i];
		lon_lat_32 waypointTwo = shared.waypoint[i+1];
		long long x1 = longitude_to_x(shared.map_number, waypointOne.lon);
		long long x2 = longitude_to_x(shared.map_number, waypointTwo.lon);
		long long y1 = latitude_to_y(shared.map_number, waypointOne.lat);
		long long y2 = latitude_to_y(shared.map_number, waypointTwo.lat);
		*(shared.tft).drawline(x1,y1,x2,y2, TFT_BLUE);
	}
}
