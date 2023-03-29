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

struct char_data_t {
	int numHits;
	int numFails;
	float prob;
};

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
		string output_file;

		/**
		*  structs
		*/
		ifstream file;
		vector<char> file_buffer;			
		map<string, map<char, struct char_data_t>> sequences_lookahead;				// Key-value pairs. Key: string sequence, Value: current best lookahead character
		int pointer;

		/**
		*	{'AAAA': { 'B' : [Nhits, nFails, Prob], 'C': [Nhits, nFials, Prob]}}
		*
		*/


	public:
		CopyModel(string filename, int k, float alpha, float threshold, string output_file)
		{

			this->filename = filename;
			this->k = k;
			this->alpha = alpha;
			this->threshold = threshold;
			this->output_file = output_file;
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

		char get_next_character_prediction(string seq)
		{
			cout << "yo" << endl;
			int maxProb;

			for ( auto const&p : this->sequences_lookahead[seq]) {
				cout << p.first << '\n';
			}

			return 'a';

		}

		/**
		 * @brief Changes the lookahead character of the sequence by removing the first item on the list
		 * 
		 * @param seq 
		 */
		void replace_sequence_lookahead_character(string seq) 
		{
			/*
			if (this->sequences_lookahead.count(seq) > 1) {

				auto it = this->sequences_lookahead.find(seq);
				//int position_to_remove = it->second + this->k;
				int position_to_remove = it->second;
				char character_to_reset = this->file_buffer[position_to_remove];
				this->sequences_lookahead[character_to_reset][0] = 0;
				this->sequences_lookahead[character_to_reset][1] = 0;
				this->sequences_lookahead.erase(it);
				
			}
			*/

		}

		float calculate_probability(string seq, char ch)
		{
			int hits = this->sequences_lookahead[seq][ch].numHits;
			int fails = this->sequences_lookahead[seq][ch].numFails;

			this->sequences_lookahead[seq][ch].prob = ((hits + this->alpha) / (hits + fails + 2 * this->alpha));
			return float(this->sequences_lookahead[seq][ch].prob);
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
			float total_num_bits;

			while (!this->file.eof()) {
				this->file.get(ch);
				
				file_buffer.insert(file_buffer.begin() + pointer, ch);		// Add *ch* to file buffer in index/position *pointer*

				sliding_window.push_back(ch);

				if (sliding_window.size() == k)
				{

					seq(sliding_window.begin(), sliding_window.end());

					if (!this->file.get(next_character)) {
						// reached end of file before k characters
						cerr << "Reached EOF" << endl;
						break;
					}

					if (this->sequences_lookahead.count(seq) < 1) { //sliding windows not in dictionary.keys
						this->sequences_lookahead.insert({seq, {}});	
					}
					
					if (this->sequences_lookahead[seq].count(next_character) < 1) {
						
						char_data_t charInit = { 0, 0, 0 };
						this->sequences_lookahead[seq][next_character] = charInit;
					} else {
						next_character_prevision = get_next_character_prediction(seq);
						calculate_probability(seq, next_character);
					}

				} 
				
				this->file.seekg(++this->pointer, ios::beg);	// Increments pointer for next iteration (sliding-window)	
			}

		}	
};





int main(int argc, char **argv) {
	
	// Command line arguments
	string filename; 									// file to predict and compare
	int k = 5;											// size of the sliding window
	float alpha = 0.1;									// alpha value for probability
	float threshold = 0.5;								// probability threshold
	string output_file = "output.txt";


	int opt;
    while ((opt = getopt(argc, argv, "f:k:a:t:o:")) != -1) {
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
			case 'o':
                output_file = string(optarg);
                break;
            default:
                cerr << "Usage: " << argv[0] << " -f <filename> -k <window_size> -a <alpha> -t <threshold> -o <previsions_output_file>\n";
                return 1;
        }
    }

	CopyModel cp(filename, k, alpha, threshold, output_file);

	cp.start();

	return 0;
}