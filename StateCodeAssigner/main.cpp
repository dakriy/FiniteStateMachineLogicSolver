#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include "quine.h"
#include <limits.h>
#include <cmath>
#include <string>


unsigned int next_power_of_two(unsigned int base)
{
	auto next_power_of_two = base;
	next_power_of_two--;
	next_power_of_two |= next_power_of_two >> 1;
	next_power_of_two |= next_power_of_two >> 2;
	next_power_of_two |= next_power_of_two >> 4;
	next_power_of_two |= next_power_of_two >> 8;
	next_power_of_two |= next_power_of_two >> 16;
	next_power_of_two++;
	return next_power_of_two;
}

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

void GetMinTermsAndDontCares(int * &minterm_dec, int & minterm_size, int * &dontcare_dec, int& dontcare_size, std::vector<unsigned short> arrangement, char ** table, unsigned int t_rows, unsigned int var_num, std::vector<unsigned int> current_arrangement)
{
	std::vector<int> temporary_term_holder;
	std::vector<int> temporary_dont_care_holder;
	for (unsigned int i = 0; i < t_rows; i++)
	{
		// w=0 case find minterm decimal value
		if(arrangement[table[i][1]] == 1)
		{
			temporary_term_holder.push_back((current_arrangement[table[i][0]]) << 1);
		}
		// w=1 case find minterm decimal value
		if(arrangement[table[i][2]] == 1)
		{
			temporary_term_holder.push_back((current_arrangement[table[i][0]] << 1) + 0x01);
		}
	}

	// And the don't cares
	for (unsigned int i = 0; i < pow(2, var_num); i++)
	{
		bool in_current_assgn = false;
		for(unsigned int j = 0; j < t_rows; j++)
		{
			if (current_arrangement[j] == i)
			{
				in_current_assgn = true;
				break;
			}
		}
		if (!in_current_assgn)
		{
			temporary_dont_care_holder.push_back(i << 1);
			temporary_dont_care_holder.push_back((i << 1) + 0x01);
		}
	}

	// Append the dont cares onto the terms cause they need to be there.
	temporary_term_holder.insert(temporary_term_holder.end(), temporary_dont_care_holder.begin(), temporary_dont_care_holder.end());


	minterm_size = temporary_term_holder.size();
	
	minterm_dec = (int *)malloc(minterm_size * sizeof(int));

	for(unsigned int i = 0; i < minterm_size; i++)
		minterm_dec[i] = temporary_term_holder[i];
	
	dontcare_size = temporary_dont_care_holder.size();

	dontcare_dec = (int *)malloc(dontcare_size * sizeof(int));
	
	for(unsigned int i = 0; i < dontcare_size; i++)
		dontcare_dec[i] = temporary_dont_care_holder[i];


	std::sort(minterm_dec, minterm_dec + minterm_size);
	std::sort(dontcare_dec, dontcare_dec + dontcare_size);
}

std::vector<QuineMcCluskeySolver *> calculate_number_of_terms(unsigned int bitnum, char ** table, unsigned int table_rows, std::vector<unsigned int> arrangment, int & number_of_terms)
{
	number_of_terms = 0;
	// Get actual bitnumb, because before it was the numbeer of states, so now I got actual bit number needed for those states
	bitnum = log2(bitnum);
	std::vector<QuineMcCluskeySolver *> solvers;
	for (unsigned int i = 0; i < bitnum; i++)
	{
		std::vector<unsigned short> arr;
		for (unsigned short j = 0; j < table_rows; j++)
			arr.push_back((arrangment[j] >> i) & 0x01);
		
		int * minterm_decimal;
		int * dontcare_decimal;
		int minterm_size, dontcare_size;
		GetMinTermsAndDontCares(minterm_decimal, minterm_size, dontcare_decimal, dontcare_size, arr, table, table_rows, bitnum, arrangment);
		QuineMcCluskeySolver * s = new QuineMcCluskeySolver(bitnum + 1, minterm_size, dontcare_size, minterm_decimal, dontcare_decimal);
		number_of_terms += s->numberOfTerms();
		solvers.push_back(s);
	}
	return solvers;
}

int main()
{
	std::string letters = "abcbdeceddafefafbc";
//	std::cout << "input the 1 input state table with no separation. e.g. present state then the next states" << std::endl;
//		std::cin >> letters;
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

	// Bitnum is the number that can be represented by a certian number of bits.
	auto bitnum = next_power_of_two(states);

	
	std::vector<unsigned int> numbers;
	for (unsigned int i = 0; i < bitnum; numbers.push_back(i++));
	std::vector<unsigned int> best_found_solution;
	std::vector<char> string_best_solved_k_map;
	auto best_solution_adjacency_count = UINT_MAX;
	unsigned int best_number_of_terms = UINT_MAX;
	unsigned int iterations = 0;
	std::vector<QuineMcCluskeySolver *> best_solved;

	do {
		std::vector<unsigned int> temp_nums = numbers;
		do {
			unsigned int current_solution_adjacency_count = 0;
			for (unsigned int i = 0; i < states; i++)
				current_solution_adjacency_count += CountOnesFromInteger(temp_nums[table[i][0]] ^ temp_nums[table[i][1]]) + CountOnesFromInteger(temp_nums[table[i][0]] ^ temp_nums[table[i][2]]);
			
			if (current_solution_adjacency_count <= best_solution_adjacency_count)
			{
				int this_time_terms;
				std::vector<QuineMcCluskeySolver *> s = calculate_number_of_terms(bitnum, table, states, temp_nums, this_time_terms);
				if (this_time_terms < best_number_of_terms)
				{
					for (auto solver : best_solved)
						delete solver;
					best_solution_adjacency_count = current_solution_adjacency_count;
					best_found_solution = temp_nums;
					best_number_of_terms = this_time_terms;
					best_solved = s;
				}
				else
					for (auto solver : s)
						delete solver;

			}

			iterations++;
//			printf("%d\n", iterations);
		} while (next_permutation(temp_nums.begin(), temp_nums.end()));
	} while (next_combination(numbers.begin(), numbers.begin() + states, numbers.end()));

	std::cout << std::endl << "Best Solution with " << iterations << " iterations:" << std::endl;
	std::cout << "Adjacency score: " << best_solution_adjacency_count << std::endl;
	for (unsigned int i = 0; i < states; i++)
		std::cout << (char)(table[i][0] + 0x61) << " = " << best_found_solution[i] << std::endl;
	std::cout << "KMAP for each bit output (with w being the least siginificant bit, also using D flip flops, Also A-D1 and B-D2 and so on and so forth until the last one goes with w): " << std::endl;
	for (int i = 0; i < best_solved.size(); i++)
	{
		printf("D%d:\n", best_solved.size() - i);
		best_solved[i]->printCharOutput();
		delete best_solved[i];
	}
	printf("\n");
	for (unsigned int i = 0; i < states; i++)
		delete[] table[i];
	delete [] table;
		
#ifdef _WIN32
	system("pause");
#endif
}
