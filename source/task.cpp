// this file you need to fill
// этот файл вам нужно заполнить
#include "task.h"
#include <cmath>
#include <thread>
#include <functional>

#define PI 3.141592653f

void Task::checkVisible(const std::vector<unit>& input_units, std::vector<int>& result)
{
	//проверка на вхождение в возможную область видимости с погрешностью
	bool (*check_distance_with_err)(const vec2&, const vec2&, const float&) =
		[](const vec2& p1, const vec2& p2, const float& sight_dist)
	{
		return std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2) <= std::pow(sight_dist, 2) + 0.000000001f;
	};
	//проверка на вхождение в возможную область видимости без погрешности
	bool (*check_distance)(const vec2&, const vec2&, const float&) =
		[](const vec2& p1, const vec2& p2, const float& sight_dist)
	{
		return std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2) <= std::pow(sight_dist, 2);
	};

	//векторное произведение
	void (*vm)(const vec2&, const vec2&, const vec2&, float&) =
		[](const vec2& start, const vec2& sight_vec, const vec2& point, float& ans)
	{
		ans = sight_vec.x * (point.y - start.y) - (point.x - start.y) * sight_vec.y;
	};

	//поворот вектора 
	/*
	x1 = x * cosA - y * sinA
	y1 = x * sinA + y * cosA

	координаты направляющего вектора после поворота
	*/
	void (*rotate)(const vec2&, vec2&, const float&) =
		[](const vec2& input_vect, vec2& output_vect, const float& angle)
	{
		output_vect.x = input_vect.x * std::cos(angle * PI / 180.f) - input_vect.y * std::sin(angle * PI / 180.f);
		output_vect.y = input_vect.x * std::sin(angle * PI / 180.f) + input_vect.y * std::cos(angle * PI / 180.f);
	};

	//проверка на равенство с погрешностью
	bool (*nearly_equal)(const float&, const float&, const float&) =
		[](const float& f1, const float& f2, const float& error_rate)
	{
		return f1 + error_rate >= f2 && f1 - error_rate <= f2;
	};

	std::size_t input_size = input_units.size();
	// fill vector result
	result.reserve(input_size);
	//Для начала будет проверка на вхождение в область видимости
	for (std::size_t i = 0; i < input_size; i++)
	{
		vec2 current_position = input_units[i].position;
		vec2 direction = input_units[i].direction;
		vec2 sight_vec1, sight_vec2;
		float angle = input_units[i].fov_deg / 2.f;

		//поворачиваем на половину угла обзора 
		//выделяя конус взора
		std::thread r1(rotate, std::ref(direction), std::ref(sight_vec1), std::ref(angle));
		std::thread r2(rotate, std::ref(direction), std::ref(sight_vec2), std::ref(angle) * -1);

		r1.join();
		r2.join();

		//вряд ли влияет
		sight_vec1.x *= input_units[i].distance;
		sight_vec2.x *= input_units[i].distance;
		sight_vec1.y *= input_units[i].distance;
		sight_vec2.y *= input_units[i].distance;


		int res = 0;

		for (std::size_t j = 0; j < input_size; j++)
		{
			vec2 checked_position = input_units[j].position;

			//может ли юнит увидеть другого на дистанции
			if (i != j && check_distance_with_err(current_position, checked_position, input_units[i].distance))
			{
				float vm1, vm2;
				std::thread v1(vm, std::ref(current_position), std::ref(sight_vec1), std::ref(checked_position), std::ref(vm1));
				std::thread v2(vm, std::ref(current_position), std::ref(sight_vec2), std::ref(checked_position), std::ref(vm2));
				v1.join();
				v2.join();
				//попадает ли юнит в сектор видимости круга
				if (vm1 <= 0.f && vm2 >= 0.f)
				{
					res++;
				}
			}
		}
		result.push_back(res);
	}
}