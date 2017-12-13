#include "quine.h"

int QuineMcCluskeySolver::OneCounter(int* binary, int NumberOfDigit)
{
	int i, count = 0;
	for (i = 0; i <= NumberOfDigit - 1; i++)
	{
		if (binary[i] == 1)
			count++;
	}
	return count;
}

int QuineMcCluskeySolver::Combination(int n, int ColumnNo, int k)
{
	int Comb, i, NtoK = 1, Kto1 = 1;
	for (i = n; i >= n - k + 1 - ColumnNo; i--)
	{
		NtoK = i * NtoK;
	}
	for (i = k; i >= 1; i--)
	{
		Kto1 = i * Kto1;
	}
	Comb = NtoK / Kto1;
	return Comb;
}

int QuineMcCluskeySolver::IsPowerOfTwo(int n)
{
	return (floor(log(n) / log(2)) == (log(n) / log(2)));
}

void QuineMcCluskeySolver::DecimalToBinary()
{
	int i, j, dividend;
	for (i = 0; i<NumberOfAllMinterm; i++)
	{
		dividend = MintermIndicesDecimal[i];
		for (j = NumberOfVariable - 1; j >= 0; j--)
		{
			Minterm_Binary[i][j] = dividend % 2;
			dividend = dividend / 2;
		}
	}
}



int QuineMcCluskeySolver::IsDontCare(int MT)
{
	int i;
	for (i = 0; i<NumberOfDontCare; i++)
		if (MT == MintermIndicesDecimal_DontCare[i])
			return 1;
	return 0;
}

void QuineMcCluskeySolver::Recursion_For_Loop(int m)
{
	int n = m;
	for (For[n] = 0; For[n]<NumberOfRemainingPI; For[n]++)
	{
		if (ReducedPIChart[NumberOfRemainingMT - 1 - n][For[n]])
		{
			if (n>0)
			{
				m = n;
				m--;
				Recursion_For_Loop(m);
			}
			else if (n == 0)
			{
				for (i = 0; i<NumberOfRemainingMT; i++)
				{
					Potential_EPI[PotEPINo][i] = For[NumberOfRemainingMT - 1 - i];
				}
				PotEPINo++;
			}
		}
	}
}

QuineMcCluskeySolver::QuineMcCluskeySolver(int variable_num, int minterm_num, int dontcare_num, int * minterm_decimal, int * dontcare_decimal)
{
	this->NumberOfVariable = variable_num;
	this->NumberOfAllMinterm = minterm_num;
	this->NumberOfDontCare = dontcare_num;
	this->MintermIndicesDecimal = minterm_decimal;
	MintermIndicesDecimal_DontCare = dontcare_decimal;
}

QuineMcCluskeySolver::~QuineMcCluskeySolver()
{
	if (Minterm_Binary != NULL)
	{
		for (i = 0; i <= NumberOfAllMinterm; i++)
		{
			free(Minterm_Binary[i]);
			Minterm_Binary[i] = NULL;
		}
//		free(Minterm_Binary);
		Minterm_Binary = NULL;		
	}


	if (Potential_EPI != NULL)
	{
		for (i = 0; i<NumberOfPossibleEPI; i++)
		{
			free(Potential_EPI[i]);
			Potential_EPI[i] = NULL;
		}
		free(Potential_EPI);
		Potential_EPI = NULL;
	}

	for (i = 0; i < NumberOfVariable + 1; i++)
	{
		for (j = 0; j < NumberOfVariable + 1 - i; j++)
		{
			free(Column[i][j]);
			Column[i][j] = NULL;
		}
		free(Column[i]);
		Column[i] = NULL;
	}
	free(Column);
	Column = NULL;

	free(NumberCounter);
	NumberCounter = NULL;
	
	for (i = 0; i<NumberOfAllMinterm; i++)
	{
		free(PI_Index[i]);
		PI_Index[i] = NULL;
	}
	free(PI_Index);
	PI_Index = NULL;

	free(EPI_Index);
	EPI_Index = NULL;

	if (NumberOfRemainingPI != 0)
	{
		for (i = 0; i < NumberOfRemainingMT; i++)
		{
			if (ReducedPIChart == NULL) break;
			free(ReducedPIChart[i]);
			ReducedPIChart[i] = NULL;
		}
		for (i = 0; i<NumberOfPossibleEPI; i++)
		{
			if (Potential_EPI == NULL) break;
			free(Potential_EPI[i]);
			Potential_EPI[i] = NULL;
		}
		free(Potential_EPI);
		free(For);
	}
	For = NULL;
	Potential_EPI = NULL;

	free(ReducedPIChart_X);
	ReducedPIChart_X = NULL;
	free(ReducedPIChart_Y);
	ReducedPIChart_Y = NULL;
	free(ReducedPIChart);
	ReducedPIChart = NULL;
	free(NoOfPIForEPI);
	NoOfPIForEPI = NULL;
	free(MintermIndicesDecimal);
	free(MintermIndicesDecimal_DontCare);
	MintermIndicesDecimal = NULL;
	MintermIndicesDecimal_DontCare = NULL;
}

int QuineMcCluskeySolver::numberOfTerms()
{
	int k, l, m, n, x, y, LogicProbe;

	/***********Transform the decimal indices into Binary format and obtain relative
	information.***********/
	Minterm_Binary = (int **)malloc(NumberOfAllMinterm * sizeof(int*));
	for (i = 0; i <= NumberOfAllMinterm; i++)
	{
		Minterm_Binary[i] = (int *)malloc((NumberOfVariable + 4) * sizeof(int));
	}
	DecimalToBinary();
	for (i = 0; i<NumberOfAllMinterm; i++)
	{
		Minterm_Binary[i][NumberOfVariable] = OneCounter(Minterm_Binary[i], NumberOfVariable);
		Minterm_Binary[i][NumberOfVariable + 1] = 0;
		/* '0' means it hasn't been grouped, '1' means it has been
		grouped with other terms */
		Minterm_Binary[i][NumberOfVariable + 2] = MintermIndicesDecimal[i];
		/* this is its original minterm */
		Minterm_Binary[i][NumberOfVariable + 3] = MintermIndicesDecimal[i];
		/* this is all the minterms it consists of */
	}
	/***********Prepare the first column for grouping***********/
	Column = (int ****)malloc((NumberOfVariable + 1) * sizeof(int***));
	for (i = 0; i<NumberOfVariable + 1; i++)
	{
		Column[i] = (int ***)malloc((NumberOfVariable + 1 - i) * sizeof(int**));
		/* Column[i] contains all the terms in the (i+1)th column
		*/
	}
	for (i = 0; i<NumberOfVariable + 1; i++)
	{
		for (j = 0; j<NumberOfVariable + 1 - i; j++)
		{
			Column[i][j] = (int**)malloc(Combination(NumberOfVariable, i, j) * sizeof(int*)); /* Column[i][j]
																							  contains all the terms with j '1's in their binary indices in the (i+1)th column */
			for (k = 0; k<Combination(NumberOfVariable, i, j); k++)
			{
				Column[i][j][k] = NULL;
				/* Column[i][j][k]
				represents a term with in the j '1's in their binary indices in the (i+1)th column
				*/
			}
		}
	}
	for (i = 0; i<NumberOfVariable + 1; i++)
	{
		for (j = 0, k = 0; j<NumberOfAllMinterm; j++)
		{
			if (Minterm_Binary[j][NumberOfVariable] == i)
			{
				Column[0][i][k++] = Minterm_Binary[j];
				/* Prepare the first grouping column */
			}
		}
	}
	/***********Perform the grouping***********/
	for (i = 0; i<NumberOfVariable + 1; i++)
	{
		if (Groupable)
		{
			Groupable = 0;
			for (j = 0; j<NumberOfVariable - i; j++)
			{
				int p, position;
				m = 0;
				for (k = 0; k < Combination(NumberOfVariable, i, j); k++)
				{
					if (Column[i][j][k] != NULL)
					{
						for (l = 0; l < Combination(NumberOfVariable, i, j + 1); l++)
						{
							if (Column[i][j + 1][l] != NULL
								&& Column[i][j + 1][l][NumberOfVariable + 2 + i] > Column[i][j][k][NumberOfVariable + 2 + i] &&
								IsPowerOfTwo(Column[i][j + 1][l][NumberOfVariable + 2 + i] -
									Column[i][j][k][NumberOfVariable + 2 + i]))
							{
								LogicProbe = 0 - i;
								/*This LogicProbe is used to check whether this two terms has the same positions of
								'-'(which is represented by '2')*/
								for (n = 1; n <= i; n++)
									for (p = 1; p <= i; p++)
										if (Column[i][j + 1][l][NumberOfVariable + 1 + n] == Column[i][j][k][NumberOfVariable + 1 +
											p])
										{
											LogicProbe++;
										}
								if (LogicProbe == 0)
								{
									Groupable = 1;
									Column[i][j][k][NumberOfVariable + 1] = 1;
									Column[i][j + 1][l][NumberOfVariable + 1] = 1;
									Column[i + 1][j][m] = (int *)malloc((NumberOfVariable + 4 + i + pow(2, i + 1)) * sizeof(int));
									for (n = 0; n <= NumberOfVariable + 1 + i; n++)
									{
										Column[i + 1][j][m][n] = Column[i][j][k][n];
									}
									Column[i + 1][j][m][NumberOfVariable + 3 + i] = Column[i][j][k][NumberOfVariable + 2 + i];
									for (n = NumberOfVariable + 4 + i; n < NumberOfVariable + 4 + i + pow(2, i + 1); n++)
										Column[i + 1][j][m][n] = 0;
									position = log((Column[i][j + 1][l][NumberOfVariable + 2 + i] -
										Column[i][j][k][NumberOfVariable + 2 + i])) / log(2);
									Column[i + 1][j][m][NumberOfVariable - 1 - position] = 2;
									Column[i + 1][j][m][NumberOfVariable + 1] = 0;
									Column[i + 1][j][m][NumberOfVariable + 2 + i] = position;
									for (p = 0; p < pow(2, i); p++)
									{
										Column[i + 1][j][m][NumberOfVariable + 4 + i + p] = Column[i][j][k][NumberOfVariable + 3 + i +
											p];
									}
									for (p = pow(2, i); p < pow(2, i + 1); p++)
									{
										Column[i + 1][j][m][NumberOfVariable + 4 + i + p] = Column[i][j + 1][l][NumberOfVariable + 3 +
											i + p - (int)pow(2, i)];
									}
									m++;
								}
							}
						}
					}
				}
			}
		}
	}
	/***********NumberCounter count how many times each decimal index occurs***********/
	NumberCounter = (int *)malloc(pow(2, NumberOfVariable) * sizeof(int));
	for (i = 0; i<pow(2, NumberOfVariable); i++)
		NumberCounter[i] = 0;
	/***********Record the Prime Implicants(duplicates will be removed)***********/
	PI_Index = (int **)malloc(NumberOfAllMinterm * sizeof(int*));
	for (i = 0; i<NumberOfAllMinterm; i++)
	{
		PI_Index[i] = (int *)malloc(3 * sizeof(int));
	}
	for (i = 0; i<NumberOfVariable + 1; i++)
		for (j = 0; j<NumberOfVariable + 1 - i; j++)
			for (k = 0; k<Combination(NumberOfVariable, i, j); k++)
			{
				if (Column[i][j][k] != NULL && Column[i][j][k][NumberOfVariable + 1] == 0)
				{
					LogicProbe = 0 - pow(2, i); /*LogicProbe is used to
												check whether this PI is a duplicate*/
					for (l = k - 1; l >= 0; l--)
						if (LogicProbe != 0)
						{
							LogicProbe = 0 - pow(2, i);
							for (m = 0; m<pow(2, i); m++)
								for (n = 0; n<pow(2, i); n++)
									if (Column[i][j][l][NumberOfVariable + 3 + i + m] == Column[i][j][k][NumberOfVariable + 3 +
										i + n])
									{
										LogicProbe++;
									}
						}
					if (LogicProbe != 0)
					{
						PI_Index[NumberOfPI][0] = i;
						PI_Index[NumberOfPI][1] = j;
						PI_Index[NumberOfPI][2] = k;
						NumberOfPI++;
						for (l = 0; l<pow(2, i); l++)
						{
							NumberCounter[Column[i][j][k][NumberOfVariable + 3 + i + l]]++;
						}
					}
				}
			}
	/***********Remove the DontCare minterms***********/
	for (i = 0; i<NumberOfDontCare; i++)
		NumberCounter[MintermIndicesDecimal_DontCare[i]] = 0;
	EPI_Index = (int **)malloc(NumberOfAllMinterm * sizeof(int*));
	/***********In the PI Chart, find the minterms which only occurs once, and select
	the PIs which contain these minterms as EPIs and record them. Then set NumberCounter
	of this minterms to 0***********/
	for (i = 0; i<pow(2, NumberOfVariable); i++)
		if (NumberCounter[i] == 1)
			for (j = 0; j<NumberOfPI; j++)
				for (k = 0; k<pow(2, PI_Index[j][0]); k++)
				{
					if (Column[PI_Index[j][0]][PI_Index[j][1]][PI_Index[j][2]][NumberOfVariable + 3 + PI_Index[j][0] + k] == i)
					{
						EPI_Index[NumberOfEPI] = PI_Index[j];
						for (l = 0; l<pow(2, PI_Index[j][0]); l++)
							NumberCounter[Column[PI_Index[j][0]][PI_Index[j][1]][PI_Index[j][2]][NumberOfVariable + 3 + PI_Index[j][0] + l]] = 0;
						NumberOfEPI++;
						k = pow(2, PI_Index[j][0]);
					}
				}
	/***********Make the Reduced PI Chart***********/
	NumberOfRemainingMT = 0;
	for (i = 0; i<pow(2, NumberOfVariable); i++)
		if (NumberCounter[i] != 0)
			NumberOfRemainingMT++;
	ReducedPIChart_X = (int *)malloc(NumberOfRemainingMT * sizeof(int));
	for (i = 0; i<NumberOfRemainingMT; i++)
		ReducedPIChart_X[i] = -1;
	ReducedPIChart_Y = (int **)malloc(NumberOfPI * sizeof(int*));
	for (i = 0; i<NumberOfPI; i++)
		ReducedPIChart_Y[i] = NULL;
	ReducedPIChart = (int **)malloc(NumberOfRemainingMT * sizeof(int*));
	/***********This is the First Row, consist of the remaining minterms decimal
	indices***********/
	for (i = 0, j = 0; j<pow(2, NumberOfVariable); j++)
		if (NumberCounter[j] != 0)
		{
			ReducedPIChart_X[i] = j;
			i++;
		}
	/***********This is the First Column, consist of the remaining PIs***********/
	NumberOfRemainingPI = 0;
	for (i = 0; i<NumberOfPI; i++)
		for (j = 0; j<pow(2, PI_Index[i][0]); j++)
		{
			if (NumberCounter[Column[PI_Index[i][0]][PI_Index[i][1]][PI_Index[i][2]][NumberOfVariable + 3 + PI_Index[i][0] + j]] != 0)
			{
				j = pow(2, PI_Index[i][0]);
				ReducedPIChart_Y[NumberOfRemainingPI] = PI_Index[i];
				NumberOfRemainingPI++;
			}
		}
	/***********ReducedPIChart[i][j] represent the information of Reduced PI Chart('1'
	means picked, '0' means unpicked)***********/
	if (NumberOfRemainingPI != 0)
	{
		for (i = 0; i<NumberOfRemainingMT; i++)
			ReducedPIChart[i] = (int *)malloc(NumberOfRemainingPI * sizeof(int));
		for (i = 0; i<NumberOfRemainingMT; i++)
			for (j = 0; j<NumberOfRemainingPI; j++)
				ReducedPIChart[i][j] = 0;
		for (i = 0; i<NumberOfRemainingPI; i++)
			for (j = 0; j<pow(2, ReducedPIChart_Y[i][0]); j++)
				for (k = 0; k<NumberOfRemainingMT; k++)
					if (Column[ReducedPIChart_Y[i][0]][ReducedPIChart_Y[i][1]][ReducedPIChart_Y[i][2
					]][NumberOfVariable + 3 + ReducedPIChart_Y[i][0] + j] == ReducedPIChart_X[k])
						ReducedPIChart[k][i] = 1;
					
		/***********Select the EPIs from the Reduced PI Chart***********/
		For = (int *)malloc(NumberOfRemainingMT * sizeof(int)); /* For[i] will be
																used in the function 'Recursion_For_Loop(int m)' */
		for (i = 0; i<NumberOfRemainingMT; i++)
		{
			For[i] = -1;
		}
		for (i = 0; i<NumberOfRemainingMT; i++)
			NumberOfPossibleEPI = NumberOfPossibleEPI*NumberCounter[ReducedPIChart_X[i]];
		Potential_EPI = (int **)malloc(NumberOfPossibleEPI * sizeof(int*));
		for (i = 0; i<NumberOfPossibleEPI; i++)
		{
			Potential_EPI[i] = (int *)malloc(NumberOfRemainingMT * sizeof(int));
		}
		Recursion_For_Loop(NumberOfRemainingMT - 1);
		NoOfPIForEPI = (int *)malloc(NumberOfPossibleEPI * sizeof(int)); /*
																		 NoOfPIForEPI[i] will count how many PIs are in each combination that covers all
																		 minterms */
		for (i = 0; i<NumberOfPossibleEPI; i++)
			NoOfPIForEPI[i] = 0;
		for (i = 0; i<NumberOfPossibleEPI; i++)
			for (j = 0; j<NumberOfRemainingMT; j++)
				if (Potential_EPI[i][j] != -1)
				{
					NoOfPIForEPI[i]++;
					for (k = j + 1; k<NumberOfRemainingMT; k++)
						if (Potential_EPI[i][k] == Potential_EPI[i][j])
							Potential_EPI[i][k] = -1;
				}
		/***********Find the combination which require the least number of PIs to cover all
		minterms***********/
		for (i = 1; i<NumberOfPossibleEPI; i++)
			if (NoOfPIForEPI[i]<NoOfPIForEPI[MinimumNo])
				MinimumNo = i;
		for (i = 0; i<NumberOfRemainingMT; i++)
			if (Potential_EPI[MinimumNo][i] != -1)
				EPI_Index[NumberOfEPI++] = ReducedPIChart_Y[Potential_EPI[MinimumNo][i]];
	}

	/***********Print the final result of minimal SOP expression***********/
	int num_of_terms = 0;
	for (x = 0; x<NumberOfEPI; x++)
	{
		int vars = 0;
		for (y = 0; y<NumberOfVariable; y++)
		{
			if (Column[EPI_Index[x][0]][EPI_Index[x][1]][EPI_Index[x][2]][y] == 1) vars++;
			else
				if (Column[EPI_Index[x][0]][EPI_Index[x][1]][EPI_Index[x][2]][y] == 0) vars++;
		}
		if (vars > 0) num_of_terms++;
	}
	return num_of_terms;
}

void QuineMcCluskeySolver::printCharOutput() const
{
	for (int x = 0; x<NumberOfEPI; x++)
	{
		for (int y = 0; y<NumberOfVariable; y++)
		{
			if (Column[EPI_Index[x][0]][EPI_Index[x][1]][EPI_Index[x][2]][y] == 1) printf("%c", y + 65);
			else
				if (Column[EPI_Index[x][0]][EPI_Index[x][1]][EPI_Index[x][2]][y] == 0)
				{
					printf("%c'", y + 65);
				}
		}
		if (x < NumberOfEPI - 1)
			printf("+");
	}
}
