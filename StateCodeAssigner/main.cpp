#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>

unsigned int CountOnesFromInteger(unsigned int value) {
	unsigned int count;
	for (count = 0; value != 0; count++, value &= value - 1);
	return count;
}

unsigned int count_back_adjacencies(std::vector<unsigned int> combination, char ** table, unsigned int rows, unsigned int columns)
{
	if (rows == 0 || columns == 0)
		return UINT_MAX;
	unsigned int count = 0;
	for (unsigned int i = 0; i < rows - 1; i++)
	{
		for (auto j = rows - 1; j > i; j--)
		{
			if (table[i][1] == table[j][1])
			{
				count += CountOnesFromInteger(combination[table[i][0]] ^ combination[table[i][1]]) + CountOnesFromInteger(combination[table[j][0]] ^ combination[table[j][1]]);
			}
			if (table[i][2] == table[j][2])
			{
				count += CountOnesFromInteger(combination[table[i][0]] ^ combination[table[i][2]]) + CountOnesFromInteger(combination[table[j][0]] ^ combination[table[j][2]]);
			}
		}
	}
	return count;
}

template <typename Iterator>
inline bool next_combination(const Iterator first, Iterator k, const Iterator last)
{
	/* Credits: Thomas Draper */
	if ((first == last) || (first == k) || (last == k))
		return false;
	Iterator itr1 = first;
	Iterator itr2 = last;
	++itr1;
	if (last == itr1)
		return false;
	itr1 = last;
	--itr1;
	itr1 = k;
	--itr2;
	while (first != itr1)
	{
		if (*--itr1 < *itr2)
		{
			Iterator j = k;
			while (!(*itr1 < *j)) ++j;
			std::iter_swap(itr1, j);
			++itr1;
			++j;
			itr2 = k;
			std::rotate(itr1, j, last);
			while (last != j)
			{
				++j;
				++itr2;
			}
			std::rotate(k, itr2, last);
			return true;
		}
	}
	std::rotate(first, k, last);
	return false;
}

int main()
{
	std::string letters;
	std::cout << "input the 1 input state table with no separation. e.g. present state then the next states" << std::endl;
		std::cin >> letters;
	unsigned int states = letters.length() / 3;

	char ** table = new char*[states];

	for (unsigned int i = 0; i < states; i++)
		table[i] = new char[3];

	if (states > 8)
	{
		std::cout << "It would take to long to brute force, sorry" << std::endl;
		return 0;
	}

	for (unsigned int i = 0; i < letters.length(); i++)
		table[i / 3][i % 3] = letters[i] - 0x61;

	auto bitnum = states;
	// Round up to the next power of 2.
	bitnum--;
	bitnum |= bitnum >> 1;
	bitnum |= bitnum >> 2;
	bitnum |= bitnum >> 4;
	bitnum |= bitnum >> 8;
	bitnum |= bitnum >> 16;
	bitnum++;

	std::vector<unsigned int> numbers;
	for (unsigned int i = 0; i < bitnum; numbers.push_back(i++));
	std::vector<unsigned int> best_found_solution;
	auto best_solution_adjacency_count = UINT_MAX;
//	auto best_solution_back_adjacency_count = UINT_MAX;
	unsigned int iterations = 0;
	do {
		std::vector<unsigned int> temp_nums = numbers;
		do {
			unsigned int current_solution_adjacency_count = 0;
			for (unsigned int i = 0; i < states; i++)
				current_solution_adjacency_count += CountOnesFromInteger(temp_nums[table[i][0]] ^ temp_nums[table[i][1]]) + CountOnesFromInteger(temp_nums[table[i][0]] ^ temp_nums[table[i][2]]);
			
			if (current_solution_adjacency_count < best_solution_adjacency_count)
			{
				best_solution_adjacency_count = current_solution_adjacency_count;
				best_found_solution = temp_nums;
//				best_solution_back_adjacency_count = count_back_adjacencies(temp_nums, table, states, 3);
			}

//			if (current_solution_adjacency_count == best_solution_adjacency_count)
//			{
//				auto back = count_back_adjacencies(temp_nums, table, states, 3);
//				if (back < best_solution_back_adjacency_count)
//				{
//					best_solution_adjacency_count = current_solution_adjacency_count;
//					best_found_solution = temp_nums;
//					best_solution_back_adjacency_count = back;
//				}
//			}
			iterations++;
		} while (next_permutation(temp_nums.begin(), temp_nums.end()));
	} while (next_combination(numbers.begin(), numbers.begin() + states, numbers.end()));

	std::cout << std::endl << "Best Solution with " << iterations << " iterations:" << std::endl;
	std::cout << "Adjacency score: " << best_solution_adjacency_count << std::endl;
	for (unsigned int i = 0; i < states; i++)
		std::cout << (char)(table[i][0] + 0x61) << " = " << best_found_solution[i] << std::endl;

	for (unsigned int i = 0; i < states; i++)
		delete[] table[i];
	delete [] table;

	system("pause");
}
