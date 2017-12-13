#pragma once
#include <cmath>
#include <memory>
#include <vector>

class QuineMcCluskeySolver
{
private:
	int i = 0, j = 0, temp = 0, NumberOfDontCare, NumberOfAllMinterm, NumberOfVariable,
		NumberOfEPI = 0,
		NumberOfRemainingMT = 0, NumberOfRemainingPI = 0,
		NumberOfPI = 0,
		PotEPINo = 0,
		NumberOfPossibleEPI = 1,
		MinimumNo = 0,
		Groupable = 1;

	int *MintermIndicesDecimal = NULL, *MintermIndicesDecimal_DontCare = NULL, **Minterm_Binary = NULL, ****Column = NULL, **PI_Index = NULL, **EPI_Index = NULL, *NumberCounter = NULL, *ReducedPIChart_X = NULL, **ReducedPIChart_Y = NULL, **ReducedPIChart = NULL, *For = NULL, **Potential_EPI = NULL, *NoOfPIForEPI = NULL;

	void DecimalToBinary();
	int IsDontCare(int MT);
	void Recursion_For_Loop(int m);

	int OneCounter(int* binary, int NumberOfDigit);

	int Combination(int n, int ColumnNo, int k);

	int IsPowerOfTwo(int n);

public:
	int numberOfTerms();
	void printCharOutput() const;
	QuineMcCluskeySolver(int variable_num, int minterm_num, int dontcare_num, int * minterm_decimal, int * dontcare_decimal);
	~QuineMcCluskeySolver();
};
