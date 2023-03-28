#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <map>
#include <iterator>
#include <random>
#include <vector>
#include <string>

using namespace std;

class CopyModel 
{
	private:
		/**
		*  params
		*/
		int k;
		int alpha;
		float threshold;
		const char *filename;

		/**
		*  structs
		*/
		ifstream file;
		vector<char> alphabet;
		multimap<string, char> sequences;
		int pointer;
		int file_length;
	
	public:
		CopyModel(const char *filename, int k, int alpha, float threshold)
		{
			this->filename = filename;
			this->k = k;
			this->alpha = alpha;
			this->alphabet = {};
			this->pointer = 0;

			this->file.open(filename);
			if (!this->file .is_open()) {
				cout << "[ERROR] Can't open file '" << filename << "'" << endl;
				exit(EXIT_FAILURE);
			}
			this->file.seekg(0, ios::end);
			this->file_length = this->file.tellg();
			this->file.seekg(0, ios::beg);
		}

		void addToAlphabet(char ch)
		{
			this->alphabet.push_back(ch);
		}

		/*
		void processSequence(vector<char> sequence, char ch)
		{
			this->sequences.insert({ sequence, ch });
		}
		*/

		char get_next_character_prediction(pair<multimap<string, char>::iterator, multimap<string, char>::iterator> iterator, string key, int size)
		{
			
			char possible_characters[size];
			char choosen_ch;
			int i = 0;

			cout << "possible characters: ";
			for (auto itr = iterator.first; itr != iterator.second; ++itr) {
				possible_characters[i] = itr->second;
				cout << itr->second << ",";
			}
			cout << "\n" << endl;

			srand(time(0)); // seed the random number generator with the current time
			
			choosen_ch = possible_characters[rand() % size];

			return choosen_ch;

		}

		void start()
		{

			char ch;
			vector<char> sliding_window;
			string seq;
			int i;
			char next_character;
			char next_character_prevision;
			int n_previous_encounters;
			
			int Nf = 0;
			int Nh = 0;

			while (!this->file.eof()) {
				
				this->file.get(ch);

				sliding_window.push_back(ch);
				if (sliding_window.size() == k)
				{
					for (char i: sliding_window)
				    	cout << i;
					sliding_window.erase(sliding_window.begin(), sliding_window.begin() + 1);

					string seq(sliding_window.begin(), sliding_window.end());

					n_previous_encounters = this->sequences.count(seq);

					if (!this->file.get(next_character)) {
						// reached end of file before k characters
						cerr << "Reached EOF" << endl;
						break;
					}

					cout << "(" << next_character << ")\n" << endl;

					//cout << seq;
					if (n_previous_encounters > 0) {

						next_character_prevision = get_next_character_prediction(this->sequences.equal_range(seq), seq, n_previous_encounters);
						
						//cout << "(((((()))))" << next_character_prevision << ")\n" << endl;

						if (next_character_prevision == next_character) Nh++;
						else Nf++;

						//cout << "(" << next_character_prevision << ")";
					}
					cout << endl;

					this->sequences.insert({seq, next_character});

				} 
				
				this->file.seekg(++this->pointer, ios::beg);	// Increments pointer for next iteration (sliding-window)


				
			}

			cout << "Nh = " << Nh << "\n";
			cout << "Nf = " << Nf << "\n";

		}
};



int main(int argc, char **argv) {
	
	// Command line arguments
	const char *filename = argv[1];						// file to predict and compare
	int k = atoi(argv[2]);						// size of the sliding window

	CopyModel cp(filename, k, 0.1, 0.5);

	cp.start();

	return 0;
}