// this file you need to fill
// этот файл вам нужно заполнить
#include "task.h"
#include <cmath>
#include <thread>
#include <functional>
#include <chrono>
#include <mutex>

#define PI 3.141592653f

std::mutex mutex;

//скалярное произведение
void scalarMultiply(const vec2& start, const vec2& sight_vec, const vec2& point, float& ans);
//векторное произведение
void vectorMultiply(const vec2& start, const vec2& sight_vec, const vec2& point, float& ans);
//проверка на вхождение в возможную область видимости
bool checkDistance(const vec2& p1, const vec2& p2, const float& sight_dist);
/*
	x1 = x * cosA - y * sinA
	y1 = x * sinA + y * cosA

	координаты направляющего вектора после поворота
*/
void rotateVector(const vec2& input_vect, vec2& output_vect, const float& angle);

void checkVisiblePart(const std::vector<unit>& input_units,
	std::vector<int>& result, const unsigned int& start, const unsigned int& end, const unsigned int& size);
void createThread(const std::vector<unit>& input_units,
	std::vector<int>& result, const unsigned int& start, const unsigned int& step, const unsigned int& size)
{
	std::thread thr; 
	if (start + step < size)
	{
		thr = std::thread(checkVisiblePart, std::ref(input_units), std::ref(result), start, start + step, size);
		createThread(input_units, result, start + step, step, size);
	}
	else
	{
		thr = std::thread(checkVisiblePart, std::ref(input_units), std::ref(result), start, size, size);
	}
	thr.join();
}
void Task::checkVisible(const std::vector<unit>& input_units, std::vector<int>& result)
{
	unsigned int input_size = input_units.size();
	// fill vector result
	result.resize(input_size);

	unsigned int thread_count = std::thread::hardware_concurrency();
	unsigned int count_to_thread;
	if (input_size / thread_count)
	{
		count_to_thread = input_size / thread_count;
	}
	else
	{
		count_to_thread = input_size;
	}

	createThread(input_units, result, 0, 1, input_size);
}


void scalarMultiply(const vec2& start, const vec2& sight_vec, const vec2& point, float& ans)
{
	ans = sight_vec.x * (point.x - start.x) + (point.y - start.y) * sight_vec.y;
};
void vectorMultiply(const vec2& start, const vec2& sight_vec, const vec2& point, float& ans)
{
	ans = sight_vec.x * (point.y - start.y) - (point.x - start.x) * sight_vec.y;
}
bool checkDistance(const vec2& p1, const vec2& p2, const float& sight_dist)
{
	return std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2) <= std::pow(sight_dist, 2);
}
void rotateVector(const vec2& input_vect, vec2& output_vect, const float& angle)
{
	output_vect.x = input_vect.x * std::cos(angle * PI / 180.f) - input_vect.y * std::sin(angle * PI / 180.f);
	output_vect.y = input_vect.x * std::sin(angle * PI / 180.f) + input_vect.y * std::cos(angle * PI / 180.f);
}
void checkVisiblePart(const std::vector<unit>& input_units, std::vector<int>& result, const unsigned int& start, const unsigned int& end, const unsigned int& size)
{
	for (unsigned int i = start; i < end; i++)
	{
		unit first = input_units[i], second;
		vec2 current_position = first.position;
		vec2 direction_first = first.direction;
		vec2 sight_vec1_first, sight_vec2_first;
		float angle_first = first.fov_deg / 2.f;

		//поворачиваем на половину угла обзора первого юнита
		//выделяя конус взора
		rotateVector(direction_first, sight_vec1_first, angle_first);
		rotateVector(direction_first, sight_vec2_first, -angle_first);

		for (unsigned int j = i + 1; j < size; j++)
		{
			second = input_units[j];
			vec2 checked_position = second.position;

			//может ли первый юнит увидеть второго на дистанции
			if (checkDistance(current_position, checked_position, first.distance))
			{
				float vm1, vm2;
				vectorMultiply(current_position, sight_vec1_first, checked_position, vm1);
				vectorMultiply(current_position, sight_vec2_first, checked_position, vm2);

				//попадает ли юнит в сектор видимости круга
				if (vm1 <= 0.f && vm2 >= 0.f)
				{
					mutex.lock();
					result[i]++;
					mutex.unlock();
				}
			}
			//может ли второй юнит увидеть первого на дистанции
			if (checkDistance(checked_position, current_position, second.distance))
			{
				vec2 direction_second = second.direction;
				vec2 sight_vec1_second, sight_vec2_second;
				float angle_second = second.fov_deg / 2.f;

				float vm1, vm2;

				rotateVector(direction_second, sight_vec1_second, angle_second);
				rotateVector(direction_second, sight_vec2_second, -angle_second);

				vectorMultiply(checked_position, sight_vec1_second, current_position, vm1);
				vectorMultiply(checked_position, sight_vec2_second, current_position, vm2);

				//попадает ли юнит в сектор видимости круга
				if (vm1 <= 0.f && vm2 >= 0.f)
				{
					mutex.lock();
					result[j]++;
					mutex.unlock();
				}
			}
		}
	}
}
