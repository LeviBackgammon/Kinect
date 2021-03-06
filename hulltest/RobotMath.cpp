#include "RobotMath.h"
#define DEBUG
using namespace std;

const double RobotMath::k_diff_height[4] = {85 / 12.0, 48 / 12.0, 48 / 12.0, 15 / 12.0};

RobotMath::RobotMath()
{
	// Empty
}

double RobotMath::GetDistance(CvPoint leftpt, CvPoint rightpt, int basket)
{
	if (basket > 3) basket = 3;
	if (basket < 0) basket = 0;

	angle_elevation_left = atan((240 - leftpt.y) / k_zeroy) + k_cameraoffset;
	angle_elevation_right = atan((240 - rightpt.y) / k_zeroy) + k_cameraoffset;
	
	dist_flat_left = (k_diff_height[basket] * (cos(angle_elevation_left)) / sin(angle_elevation_left));
	dist_flat_right = (k_diff_height[basket] * (cos(angle_elevation_right)) / sin(angle_elevation_right));
	
	if (dist_flat_left < dist_flat_right) // On left side of field
	{
		// acos may return negative
		angle_transition_inner = acos((pow(k_bkbd_width, 2) + pow(dist_flat_left, 2) - pow(dist_flat_right, 2)) / (2 * k_bkbd_width * dist_flat_left));
		angle_transition_outer = PI - angle_transition_inner;

		dist_bkbd_perpendicular = dist_flat_left * sin(angle_transition_outer);

		dist_perpendicular_to_center = ((dist_bkbd_perpendicular / sin(angle_transition_outer)) * cos(angle_transition_outer)) + (k_bkbd_width / 2);

		dist_reflect_perpendicular = dist_bkbd_perpendicular + 1.25;

		dist_output = sqrt(pow(dist_perpendicular_to_center, 2) + (pow(dist_reflect_perpendicular, 2)));
	}
	else if (dist_flat_left > dist_flat_right) // On right side of field
	{
		angle_transition_inner = acos((pow(k_bkbd_width, 2) + pow(dist_flat_right, 2) - pow(dist_flat_left, 2)) / (2 * k_bkbd_width * dist_flat_right));
		angle_transition_outer = PI - angle_transition_inner;

		dist_bkbd_perpendicular = dist_flat_right * sin(angle_transition_outer);

		dist_perpendicular_to_center = ((dist_bkbd_perpendicular / sin(angle_transition_outer)) * cos(angle_transition_outer)) + (k_bkbd_width / 2);

		dist_reflect_perpendicular = dist_bkbd_perpendicular + 1.25;

		dist_output = sqrt(pow(dist_perpendicular_to_center, 2) + (pow(dist_reflect_perpendicular, 2)));
	}
	else if (dist_flat_left == dist_flat_right) // Will never happen, but catching middle line
	{
		dist_output = sqrt(pow(dist_flat_left, 2) - pow(k_bkbd_width / 2, 2)) + (15.0 / 12.0); // Dist. between bkbd and hoop center
	}

	#ifdef DEBUG
	printf("leftpt.y: %d\n", leftpt.y);
	printf("rightpt.y: %d\n", rightpt.y);
	printf("k_diff_height: %f; basket: %d\n", k_diff_height[basket], basket);
	printf("dist_output: %f\n\n", dist_output);
	#endif

	return dist_output; // Yes, we could just access the variable, but this is cleaner
}

// Should be doable with only viewpicture info and nothing from GetDistance.
// Returns angle offset from center of screen in degrees.
// POSITIVE ANGLE IS TURN CLOCKWISE
double RobotMath::GetAngle(CvPoint leftpt, CvPoint rightpt)
{
	angle_offset_left = atan((leftpt.x - 320) / k_zerox);
	angle_offset_right = atan((rightpt.x - 320) / k_zerox);

	angle_offset_mid = (angle_offset_left + angle_offset_right) / 2;

	// angle_reflect_correction = atan(dist_perpendicular_to_center / dist_bkbd_perpendicular) - atan(dist_perpendicular_to_center / dist_reflect_perpendicular);
	angle_reflect_correction = 0.0;

	if (dist_flat_left < dist_flat_right && angle_offset_mid <= 0)
	{
		angle_output = angle_reflect_correction + angle_offset_mid;
	}
	else if (dist_flat_left < dist_flat_right && angle_offset_mid > 0)
	{
		angle_output = angle_offset_mid - angle_reflect_correction;
	}
	else if (dist_flat_left > dist_flat_right && angle_offset_mid <= 0)
	{
		angle_output = angle_offset_mid + angle_reflect_correction;
	}
	else if (dist_flat_left > dist_flat_right && angle_offset_mid > 0)
	{
		angle_output = angle_offset_mid - angle_reflect_correction;
	}
	else // Shouldn't happen.
	{
		angle_output = angle_offset_mid;
	}

	// CONVERT TO DEGREES
	angle_output = angle_output * 180 / PI;

	#ifdef DEBUG
	printf("leftpt.x: %d\n", leftpt.x);
	printf("rightpt.x: %d\n", rightpt.x);
	printf("angle_offset_left: %f\n", angle_offset_left);
	printf("angle_offset_right: %f\n", angle_offset_right);
	printf("angle_offset_mid: %f\n", angle_offset_mid);
	printf("angle_reflect_correction: %f\n", angle_reflect_correction);
	printf("angle_output: %f\n\n", angle_output);
	#endif

	return angle_output; // As above.
}
