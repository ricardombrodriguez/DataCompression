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
#include <getopt.h>
#include <math.h>

using namespace std;


class CopyModel 
{
	private:
		/**
		*  params
		*/
		string filename;
		int k;
		float alpha;
		float threshold;
		int file_length;

		/**
		*  structs
		*/
		ifstream file;
		vector<char> file_buffer;			
		multimap<string, int> sequences_lookahead;				// Key-value pairs. Key: string sequence, Value: current best lookahead character
		map<char, int[2]> character_counters;					// Key-value pairs. Key: string sequence, Value: [Nh, Nf]. Keep track of Nh and Nf of the sequence lookahead character
		int pointer;


	public:
		CopyModel(string filename, int k, float alpha, float threshold)
		{

			this->filename = filename;
			this->k = k;
			this->alpha = alpha;
			this->threshold = threshold;
			this->file_buffer = {};
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

		// void addToAlphabet(char ch)
		// {
		// 	this->alphabet.push_back(ch);
		// }


		/*
		void processSequence(vector<char> sequence, char ch)
		{
			this->sequences.insert({ sequence, ch });
		}
		*/

		char get_next_character_prediction(pair<multimap<string, int>::iterator, multimap<string, int>::iterator> iterator)
		{
			//char chosen_ch = this->file_buffer[iterator.first->second + this->k];
			char chosen_ch = this->file_buffer[iterator.first->second];
			return chosen_ch;

		}

		/**
		 * @brief Changes the lookahead character of the sequence by removing the first item on the list
		 * 
		 * @param seq 
		 */
		void replace_sequence_lookahead_character(string seq) 
		{

			if (this->sequences_lookahead.count(seq) > 1) {

				auto it = this->sequences_lookahead.find(seq);
				//int position_to_remove = it->second + this->k;
				int position_to_remove = it->second;
				char character_to_reset = this->file_buffer[position_to_remove];
				this->character_counters[character_to_reset][0] = 0;
				this->character_counters[character_to_reset][1] = 0;
				this->sequences_lookahead.erase(it);
				
			}

		}

		float calculate_probability(char ch)
		{
			int hits = this->character_counters[ch][0];
			int fails = this->character_counters[ch][1];
			
			return ((hits + this->alpha) / (hits + fails + 2 * this->alpha));
		}

		void start()
		{

			char ch;		
			char i;	
			vector<char> sliding_window;
			string seq;
			char next_character;
			char next_character_prevision;
			int n_previous_encounters;
			int Nh = 0, Nf = 0;
			float p_hit;

			while (!this->file.eof()) {
				
				this->file.get(ch);

				if (this->character_counters.count(ch) < 1) {
					this->character_counters[ch][0] = 0;		// Initialize Nh & Nf for the new character
					this->character_counters[ch][1] = 0;
				}
				
				file_buffer.insert(file_buffer.begin() + pointer, ch);		// Add *ch* to file buffer in index/position *pointer*

				sliding_window.push_back(ch);

				if (sliding_window.size() == k)
				{
					
					// for (char i: sliding_window)
					// 	cout << i;
					sliding_window.erase(sliding_window.begin(), sliding_window.begin() + 1);

					string seq(sliding_window.begin(), sliding_window.end());

					n_previous_encounters = this->sequences_lookahead.count(seq);

					if (!this->file.get(next_character)) {
						// reached end of file before k characters
						cerr << "Reached EOF" << endl;
						break;
					}

					//cout << "(" << next_character << ")\n" << endl;

					//cout << seq;
					if (n_previous_encounters > 0) {

						next_character_prevision = get_next_character_prediction(this->sequences_lookahead.equal_range(seq));
						
						if (this->character_counters.count(next_character_prevision) < 1) {
							this->character_counters[next_character_prevision][0] = 0;
							this->character_counters[next_character_prevision][1] = 0;
						}

						if (next_character_prevision == next_character) {
							this->character_counters[next_character_prevision][0]++;	// character hits
							Nh++;																												// overall hits
						} else {			
							cout << "Character: " << next_character << " | Predicted: " << next_character_prevision << endl;
							this->character_counters[next_character_prevision][1]++;	// character fails
							Nf++;																												// overall fails
						}

						// aualizar this->sequences_counters
						p_hit = calculate_probability(next_character_prevision);

						// Change lookahead character
						if (p_hit < this->threshold) {
							cout << "phit baixo, vou mudar o lookahead: " << endl;
							replace_sequence_lookahead_character(seq);
						}
						
					}
					//cout << endl;

					this->sequences_lookahead.insert({seq, this->pointer});

				} 
				
				this->file.seekg(++this->pointer, ios::beg);	// Increments pointer for next iteration (sliding-window)


				
			}

		
			float probability = ((float)Nh) / (Nh + Nf);
			float n_bits = -log(probability)/log(2);

			cout << "Nh = " << Nh << endl;
			cout << "Nf = " << Nf << endl;
			cout << "Probability " << probability << endl;
			cout << "Bits = " << n_bits << endl;

		}	
};





int main(int argc, char **argv) {
	
	// Command line arguments
	string filename; 									// file to predict and compare
	int k = 5;											// size of the sliding window
	float alpha = 0.1;									// alpha value for probability
	float threshold = 0.5;								// probability threshold


	int opt;
    while ((opt = getopt(argc, argv, "f:k:a:t:")) != -1) {
        switch (opt) {
            case 'f':
                filename = string(optarg);
                break;
            case 'k':
                k = atoi(optarg);
				if (k  < 1) {		
					cout << "[ERROR] Sliding window size must be greater than zero.\n" << endl;
					exit(EXIT_FAILURE);
				}
                break;
			case 'a':
                alpha = atof(optarg);
				if (alpha <= 0) {		
					cout << "[ERROR] Alpha must be bigger than zero.\n" << endl;
					exit(EXIT_FAILURE);
				}
                break;
			case 't':
                threshold = atof(optarg);
				if (threshold < 0 || threshold > 1) {		
					cout << "[ERROR] Probability threshold must be between 0 and 1.\n" << endl;
					exit(EXIT_FAILURE);
				}
                break;
            default:
                cerr << "Usage: " << argv[0] << " -f <filename> -k <window_size> -a <alpha> -t <threshold>\n";
                return 1;
        }
    }

	CopyModel cp(filename, k, alpha, threshold);

	cp.start();

	return 0;
}