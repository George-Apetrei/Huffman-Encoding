#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

struct data {
	std::string sentence;							//Holds the sentence to be processed
	std::string encodedSentence;					//Holds the encoded sentence
	std::vector<std::string> symbol;				//Holds the unique symbols which the program will encode
	std::vector<int> symbol_counter;				//Counts the number of times each symbol is found within the sentence
	std::vector<double> symbol_probabilities;		//Holds the probabilities of each symbol occuring	(symbol_counter[i]/total symbols)
	std::vector<std::string> symbol_code;			//Holds the encoded value for each symbol
	std::vector<std::string> symbol_to_encode;		//Holds the symbols to encode with
	int distribution;								//1 - maximum distribution (value calculated goes above elements with the same value), 0 - minimum(element calculated goes under elements of the same value)
};
using DATA = struct data;

DATA Analysis(DATA to_analyse)
{
	int exists = 0;
	int total = 0;
	std::string word;
	int i = 0, pos = 0;
	//Count all the individual symbol used in the sentence, register each unique symbol in a vector, and count the number of times they are encountered in a vector of int. The positions of symbol and symbol counter are 
	while (pos < to_analyse.sentence.size()) {
		while (to_analyse.sentence[i] != ' '&&to_analyse.sentence[i] != '\0'&&to_analyse.sentence[i] != '\n') {
			i++;
		}
		for (int j = 0; j < to_analyse.symbol.size(); ++j) {
			if (to_analyse.sentence.compare(pos + 1, i - pos - 1, to_analyse.symbol[j]) == 0) {
				to_analyse.symbol_counter[j]++;
				exists = 1;
				break;
			}
			else {
				exists = 0;
			}
		}
		if (exists == 0) {
			to_analyse.symbol_counter.push_back(1);
			if (pos == 0) {
				to_analyse.symbol.push_back(to_analyse.sentence.substr(pos, i - pos));
			}
			else {
				to_analyse.symbol.push_back(to_analyse.sentence.substr(pos + 1, i - pos - 1));
			}

		}

		pos = i;
		i++;
	}

	//Calculate the probabilities of each symbol appearing
	for (int i = 0; i < to_analyse.symbol_counter.size(); ++i) {
		to_analyse.symbol_probabilities.push_back(static_cast<double>(to_analyse.symbol_counter[i]) / to_analyse.sentence.size());
	}


	return to_analyse;
}
//Bubble sort
DATA sorting(DATA info) {
	int check = 0, t, pass = 0, swap = 0, none = 0;
	std::string swapping;
	double the_d;
	while (check != 1)
	{
		for (int i = 0; i < info.symbol_probabilities.size() - 1; ++i)
		{
			if (info.symbol_probabilities[i] < info.symbol_probabilities[i + 1])
			{
				swapping = info.symbol[i];
				info.symbol[i] = info.symbol[i + 1];
				info.symbol[i + 1] = swapping;
				the_d = info.symbol_probabilities[i];
				info.symbol_probabilities[i] = info.symbol_probabilities[i + 1];
				info.symbol_probabilities[i + 1] = the_d;
				swap++;
				none = 1;
			}
		}
		if (none == 1)
		{
			none = 0;
		}
		else
		{
			check = 1;
		}
		pass++;
	}
	return info;
}

//Determine if the size of the probabilities array allows for encoding with the number of symbols to encode with, if yes returns 1, else returns 0
int check_size(int number_of_probabilities, int number_of_symbols, int * steps)
{
	while (number_of_probabilities >= number_of_symbols) {
		*steps += 1;
		if (number_of_probabilities == number_of_symbols) {
			return 1;
		}
		number_of_probabilities -= number_of_symbols - 1;

	}
	return 0;
}

//Function to encode symbols, end result of which is codes for each symbol
DATA encoding(DATA info) {
	//Step 1: Reorder the probabilities from highest to lowest
	info = sorting(info);
	//Step 2: Determine if the size of the array of probabilities allows for encoding of X symbols, and the steps in which it will encode
	int check = 0, *steps = new int;
	*steps = 0;
		while (1) {
			check = check_size(info.symbol_probabilities.size(), info.symbol_to_encode.size(), steps);
			if (check == 1) {
				break;
			}
			else {
				info.symbol_probabilities.push_back(0);
				*steps = 0;
			}
		}

	//Step 3: Determine the code symbol for all of the symbols

	std::vector<std::vector<double>> probabilities;					//Holds all the probabilities during calculation
	std::vector < std::vector<std::string>> symbols;				//Holds all the symbols during calculation
	std::vector<char> symbols_used_in_calculations;					//Holds all the symbols that are used to mark the sum of the previous row
	//0 holds the empty spot, 

	for (int i = 0; i < *steps; ++i) {
		probabilities.push_back(std::vector<double>());
		symbols.push_back(std::vector<std::string>());
	}
	//Push back the initial list of probabilities and assign correct symbols
	char identifier = 'a';
	for (int i = 0; i < info.symbol_probabilities.size(); ++i) {	//Copy probabilities into new variable for ease of use
		probabilities[0].push_back(info.symbol_probabilities[i]);
		if (i < (info.symbol_probabilities.size() - info.symbol_to_encode.size())) {
			symbols[0].push_back(std::string());
			symbols[0][i].push_back(identifier);
			identifier++;
		}
		else {
			symbols[0].push_back(std::to_string(i - (info.symbol_probabilities.size() - info.symbol_to_encode.size())));
		}
	}

	char c = 'A';		//Used in identifying the sums
	char number = '0';	//Used in filling the symbols with a number corresponding to the symbols to encode by
	double aux = 0;			//Used in swapping positions for numbers
	std::string aux_symbol;			//Used in swapping positions for symbols
	//Start of encoding
	for (int i = 0; i < *steps - 1; ++i) {
		//Copy values of probabilities up to the value calculated into next set
		for (int j = 0; j < (probabilities[i].size() - info.symbol_to_encode.size()); ++j) {
			probabilities[i + 1].push_back(probabilities[i][j]);
		}
		//Add last element on the next set as 0, to be calculated
		probabilities[i + 1].push_back(0);
		//Calculate value of last element
		for (int j = 0; j < info.symbol_to_encode.size(); ++j) {
			probabilities[i + 1][probabilities[i + 1].size() - 1] += probabilities[i][(probabilities[i].size() - info.symbol_to_encode.size()) + j];
		}
		//Copy to the next row previous symbols up to the size of the probabilities vector
		for (int j = 0; j < probabilities[i + 1].size() - 1; ++j) {
			symbols[i + 1].push_back(symbols[i][j]);
		}
		//Push back empty spot for the sum element
		symbols[i + 1].push_back(std::string());
		//Associate the sum with a symbol
		symbols[i + 1][probabilities[i + 1].size() - 1].push_back(c);
		//Distribute the same symbol to all the elements summed up previously
		for (int j = 0; j < info.symbol_to_encode.size(); ++j) {
			symbols[i][(symbols[i].size() - info.symbol_to_encode.size()) + j].push_back(c);
		}
		//Change the symbol for the next loop
		c++;
		//Distribute the sum to it's correct position based on the distribution rule
		if (info.distribution == 0) {
			//If distribution is minimum
			for (int j = 0; j < probabilities[i + 1].size(); ++j) {
				if (((probabilities[i + 1].size() - j - 1) < 0) || (probabilities[i + 1][probabilities[i + 1].size() - j - 2] >= probabilities[i + 1][probabilities[i + 1].size() - j - 1])) {
					break;
				}
				else {
					//Swap probabilities
					aux = probabilities[i + 1][probabilities[i + 1].size() - 2 - j];
					probabilities[i + 1][probabilities[i + 1].size() - 2 - j] = probabilities[i + 1][probabilities[i + 1].size() - j - 1];
					probabilities[i + 1][probabilities[i + 1].size() - j - 1] = aux;
					//Swap symbols
					aux_symbol = symbols[i + 1][symbols[i + 1].size() - 2 - j];
					symbols[i + 1][symbols[i + 1].size() - 2 - j] = symbols[i + 1][symbols[i + 1].size() - j - 1];
					symbols[i + 1][symbols[i + 1].size() - j - 1] = aux_symbol;
				}
			}
		}
		else {
			//If distribution is maximum
			for (int j = 0; j < probabilities[i + 1].size(); ++j) {
				if (((probabilities[i + 1].size() - j - 2) == -1) || (probabilities[i + 1][probabilities[i + 1].size() - j - 2] > probabilities[i + 1][probabilities[i + 1].size() - j - 1])) {
					break;
				}
				else {
					//Swap probabilities
					aux = probabilities[i + 1][probabilities[i + 1].size() - 2 - j];
					probabilities[i + 1][probabilities[i + 1].size() - 2 - j] = probabilities[i + 1][probabilities[i + 1].size() - j - 1];
					probabilities[i + 1][probabilities[i + 1].size() - j - 1] = aux;
					//Swap symbols
					aux_symbol = symbols[i + 1][symbols[i + 1].size() - 2 - j];
					symbols[i + 1][symbols[i + 1].size() - 2 - j] = symbols[i + 1][symbols[i + 1].size() - j - 1];
					symbols[i + 1][symbols[i + 1].size() - j - 1] = aux_symbol;
				}
			}
		}
		//Fill the new row with the notations of the symbols to encode with
		for (int j = symbols[i + 1].size() - info.symbol_to_encode.size(); j < symbols[i + 1].size(); ++j) {
			symbols[i + 1][j].push_back(number);
			number++;
		}
		//Reset number for next row
		number = '0';
	}

	//2nd loop, distribute the symbols to their correct spot
	//For number of rows
	for (int i = 0; i < symbols.size(); ++i) {
		//If row exists below
		if (symbols.size() - i - 2 == -1) {
			//If true, it means the end of the distribution
			break;
		}
		else {
			//If false, distribute to the row below
			//For number of columns, starting from the last row
			for (int j = 0; j < symbols[symbols.size() - 1 - i].size(); ++j)



				//For comparing row below
				for (int m = 0; m < symbols[symbols.size() - 2 - i].size(); ++m) {
					//If the first char on the upper column is the same as the last char on the bottom column
					if ((symbols[symbols.size() - 1 - i][j][0]) == (symbols[symbols.size() - 2 - i][m][symbols[symbols.size() - 2 - i][m].size() - 1])) {
						//For the size of the rest of the string at the upper column being compared
						for (int p = 1; p < (symbols[symbols.size() - 1 - i][j].size()); ++p) {
							//Push back into the column below everything else
							symbols[symbols.size() - 2 - i][m].push_back(symbols[symbols.size() - 1 - i][j][p]);
						}
					}
				}
		}

	}
	//3rd loop, register the code value for each symbol
	for (int i = 0; i < info.symbol.size(); ++i) {
		info.symbol_code.push_back(std::string());
	}
	for (int i = 0; i < info.symbol.size(); ++i) {
		for (int j = 0; j < symbols[0][i].size(); ++j) {
			if ((symbols[0][i][symbols[0][i].size() - j - 1] > '/') && (symbols[0][i][symbols[0][i].size() - j - 1] < ':')) {
				info.symbol_code[i].append(info.symbol_to_encode[symbols[0][i][symbols[0][i].size() - j - 1] - '0']);
			}
		}
	}
	return info;
}

//Function with the purpose of converting a sentence from words to their symbols, encodes the symbol-sentence to "encoded
DATA write_sentence_as_code(DATA info) {

	std::string sentence = info.sentence;

	int exists = 0;
	int pos = 0, i = 0;
	std::string test;
	while (pos < info.sentence.size()) {
		while (info.sentence[i] != ' '&&info.sentence[i] != '\0'&&info.sentence[i] != '\n') {
			i++;
		}
		for (int j = 0; j < info.symbol.size(); ++j) {
			if (pos > 0) {
				if (info.sentence.compare(pos + 1, i - pos - 1, info.symbol[j]) == 0) {
					test.append(info.symbol_code[j]);
					exists = 1;
					break;
				}
				else {
					exists = 0;
				}
			}
			else {
				if (info.sentence.compare(pos, i - pos, info.symbol[j]) == 0) {
					test.append(info.symbol_code[j]);
					exists = 1;
					break;
				}
				else {
					exists = 0;
				}
			}

		}
		if (exists == 0) {
			exit;
		}

		pos = i;
		i++;
	}
	info.encodedSentence = test;
	return info;
}
//Function to convert encodedSentence into sentence given the words
DATA decode(DATA info) {
	int exists = 0;
	int pos = 0;
	std::string test;
	std::string sentence = info.encodedSentence;
	while (pos < sentence.size()) {

		for (int j = 0; j < info.symbol.size(); ++j) {

			if (sentence.compare(pos, info.symbol_code[j].size(), info.symbol_code[j]) == 0) {
				sentence.replace(pos, info.symbol_code[j].size(), info.symbol[j]);
				test.append(info.symbol[j]);
				test.append(" ");
				pos += info.symbol[j].size();
				exists = 1;
				break;
			}
			else {
				exists = 0;
			}
		}
		if (exists == 0) {
			std::cout << "Sentence or words input are invalid";
			break;
		}
	}
	info.sentence = test;
	return info;
}

//Function to clear std::cin
void clear() {
	std::string ClearBuffer;
	std::getline(std::cin, ClearBuffer);
}

//Function to generate text file with all of the info
void Output_File(DATA info) {
	int n = 1;;
	std::ofstream output;
	output.open("data.txt");
	//Sentence
	output << n << ":Sentence, size: " << info.sentence.size() << std::endl << info.sentence << std::endl;
	n++;

	//Encoded sentence
	output << n << ":Encoded Sentence, size: " << info.encodedSentence.size() << std::endl << info.encodedSentence << std::endl;
	n++;

	//Symbols used
	output << n << ":Symbols, size: " << info.symbol.size() << std::endl;
	for (int i = 0; i < info.symbol.size(); ++i) {
		output << info.symbol[i] << std::endl;
	}
	n++;
	//Symbol Probabilities
	output << n << ":Probabilities\n";
	for (int i = 0; i < info.symbol_probabilities.size(); ++i) {
		output << info.symbol_probabilities[i] << std::endl;
	}
	n++;
	//Symbol Code
	output << n << ":Symbol Code\n";
	for (int i = 0; i < info.symbol_code.size(); ++i) {
		output << info.symbol_code[i] << std::endl;
	}
	n++;
	//Symbol to encode
	output << n << ":Symbol To Encode\n";
	for (int i = 0; i < info.symbol_to_encode.size(); ++i) {
		output << info.symbol_to_encode[i] << std::endl;
	}
	n++;

	//Distribution
	output << n << ":Distribution\n" << info.distribution;

	output.close();
}

//Function that handles all inputs necessary
DATA input(DATA info, int choice) {
	int i;
	double probabilities;
	std::string input;
	switch (choice) {
	case 1:

		std::cout << "Input sentence\n";
		getline(std::cin, info.sentence);
		std::cout << "Input number of symbols to encode with\n";
		std::cin >> i;
		clear();
		std::cout << "Input symbols to encode with, everything you have written up until you press enter is counted as a symbol\n";
		for (int j = 0; j < i; ++j) {
			std::cout << j + 1 << ": ";
			getline(std::cin, input);
			info.symbol_to_encode.push_back(input);
		}
		std::cout << "Input type of distribution\n1 - Maximum\n0 - Minimum\n";
		std::cin >> info.distribution;
		break;
	case 2:
		std::cout << "Input number of symbols to encode\n";
		std::cin >> i;
		clear();
		std::cout << "Input symbols to encode\n";
		for (int j = 0; j < i; ++j) {
			std::cout << j + 1 << ": ";
			getline(std::cin, input);
			info.symbol.push_back(input);
		}
		std::cout << "Input probabilities for each symbol\n";
		for (int j = 0; j < i; ++j) {
			std::cout << j + 1 << ": ";
			std::cin >> probabilities;
			info.symbol_probabilities.push_back(probabilities);
		}
		std::cout << "Input number of symbols to encode with\n";
		std::cin >> i;
		clear();
		std::cout << "Input symbols to encode with, everything you have written up until you press enter is counted as a symbol\n";
		for (int j = 0; j < i; ++j) {
			std::cout << j + 1 << ": ";
			getline(std::cin, input);
			info.symbol_to_encode.push_back(input);
		}
		std::cout << "Input type of distribution\n1 - Maximum\n0 - Minimum\n";
		std::cin >> info.distribution;
		break;
	case 3:
		std::cout << "Input the encoded sentence\n";
		getline(std::cin, info.encodedSentence);
		std::cout << "Input number of symbols in sentence\n";
		std::cin >> i;
		clear();
		std::cout << "Input the symbols\n";
		for (int j = 0; j < i; ++j) {
			std::cout << j + 1 << ": ";
			getline(std::cin, input);
			info.symbol.push_back(input);
		}
		std::cout << "Input the code for each symbol\n";
		for (int j = 0; j < i; ++j) {
			std::cout << j + 1 << ": ";
			getline(std::cin, input);
			info.symbol_code.push_back(input);
		}
		break;
	}
	return info;
}

int main()
{
	DATA coding;
	int choice = 0;
	std::cout << "Huffman coding!" << std::endl;
	std::cout << "Input:\n1 - Encode a sentence, given the sentence, symbols to encode with and distribution\n2 - Encode a set of symbols, given the symbols, probabilities, symbols to encode with and distribution\n3 - Decode a sentence, given the sentence, the symbols and their encoded value" << std::endl;
	std::cin >> choice;
	clear();
	switch (choice) {
	case 1:
		coding = input(coding, choice);
		coding = Analysis(coding);
		coding = encoding(coding);
		coding = write_sentence_as_code(coding);
		break;
	case 2:
		coding = input(coding, choice);
		coding = encoding(coding);
		break;
	case 3:
		coding = input(coding, choice);
		coding = decode(coding);
		std::cout << "The decoded sentence is:\n" << coding.sentence << std::endl << std::endl;
		break;
	}
	clear();
	Output_File(coding);

	std::cout << "Data has been processed and output to the \"Data.txt\" file in the same folder as the executable\nPress Enter to exit program\n";
	getline(std::cin, coding.sentence);
	return 0;
}
