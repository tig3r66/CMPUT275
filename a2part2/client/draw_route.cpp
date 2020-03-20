#include "include/draw_route.h"
#include "include/map_drawing.h"

extern shared_vars shared;

void draw_route() {
	for (int i = 0; i < numwaypoints; i++) {
		lon_lat_32 waypoint = shared.waypoints[i];
		long long lat = waypoint.lat;
		long long lon = waypoint.lon;
		
	}
}
