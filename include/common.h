// THIS FILE IS ONLY FOR STRUCT DECLARATION
// DO NOT MODIFICATE IT - IT`S PART OF ANOTHER PROJECT

// это файл только для деклараций общих структур
// не модифицируйте его - возможно это часть другого проекта

#pragma once

struct vec2
{
	float x = 0.0f;
	float y = 0.0f;
};

struct unit
{
	vec2 position; // position of unit (-10^5...10^5, -10^5...10^5)
	vec2 direction; // normalized view direction
	float fov_deg = 0.0f; // horizontal field-of-view in degrees (0...180)
	float distance = 0.0f; // view distance (0...10^5)
};
