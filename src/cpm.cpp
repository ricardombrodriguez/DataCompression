#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <random>

using namespace std;

string read_file(string file_name) {
	ifstream file (file_name);

	if (!file.is_open()) {
			cout << "[ERROR] Can't open file '" << file_name << "'" << endl;
			exit(EXIT_FAILURE);
	}

	string buffer ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	return buffer;
}

char get_next_character_prediction(pair<multimap<string, char>::iterator, multimap<string, char>::iterator> iterator, string key, int size)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distr(0, size);

	int randIdx = distr(gen);

	while (randIdx > 1) {
			advance(iterator.first, 1);
			if ((*iterator.first).first == key) randIdx--;
	}

	return (*iterator.first).second;
}

int main(int argc, char **argv) {

	// Command line arguments
	const char *filename = argv[1];					// file to predict and compare
	const int k = atoi(argv[2]);						// size of the sliding window

	int Nh = 0;
	int Nf = 0;

	multimap<string, char> sequences;

	string buffer = read_file(filename);

	// Iterate through the file buffer using a k-size sliding window
	for (int i = 0; i <= buffer.length() - k - 1; i++) {

			string window = buffer.substr(i, k);

			int n_previous_encounters = sequences.count(window);

			char next_character = buffer.at(i + k);

			cout << window;
			if (n_previous_encounters > 0) {
				char next_character_prevision = get_next_character_prediction(sequences.equal_range(window), window, n_previous_encounters);
				
				if (next_character_prevision == next_character) Nh++;
				else Nf++;

				cout << "(" << next_character_prevision << ")";
			}
			cout << endl;

			sequences.insert({window, next_character});
	}

	return 0;
}