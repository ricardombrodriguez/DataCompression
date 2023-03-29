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
#include <unordered_map>

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
		string filename;		// text source file name
		int k;					// sliding window size
		float alpha;			// smoothning factor
		float threshold;		// minimum performance of the model
		int file_length;		// length of the file
		string output_file;		// file to output the runs

		/**
		*  structs
		*/
		ifstream file;																			// file stream
		unordered_map<string, unordered_map<char, struct char_data_t>> sequences_lookahead;		// sequence indexed dictionary for char probabilities
		int pointer;																			// file stream pointer	
		float accuracy;																			// current model prediction accuracy
		float n_bits;																			// average num bits per symbol
		float expected_total_bits;																// expected total bits of compressed file
		time_t exec_time;																		// execution time

	public:
		/*
		*	Instantiation
		*	
		*/
		CopyModel(string filename, int k, float alpha, float threshold, string output_file)
		{
			
			this->filename = filename;				
			this->k = k;						
			this->alpha = alpha;					
			this->threshold = threshold;			
			this->output_file = output_file;	
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
			float maxProb = 0;
			char next;

			for ( auto const&p : this->sequences_lookahead[seq]) {
				if (p.second.prob > maxProb) {
					maxProb = p.second.prob;
					next = p.first;
				}
			}

			return next;

		}

		float calculate_probability(string seq, char ch)
		{
			int hits = this->sequences_lookahead[seq][ch].numHits;
			int fails = this->sequences_lookahead[seq][ch].numFails;

			this->sequences_lookahead[seq][ch].prob = ((hits + this->alpha) / (hits + fails + 2 * this->alpha));
			return float(this->sequences_lookahead[seq][ch].prob);
		}

		void reset_map() {
			this->sequences_lookahead.clear();
		}

		void start()
		{

			char ch;		
			vector<char> sliding_window;
			string seq;
			char next_character;
			char next_character_prevision;
			int Nh = 0, Nf = 0;
			int cur_Nh = 0, cur_Nf = 0;


			this->exec_time = clock();

			while (!this->file.eof()) {

				this->file.get(ch);
				sliding_window.push_back(ch);

				if (sliding_window.size() == k+1)
				{
					sliding_window.erase(sliding_window.begin(), sliding_window.begin() + 1);

					string seq(sliding_window.begin(), sliding_window.end());

					if (!this->file.get(next_character)) {
						// reached end of file before k characters
						//cerr << "Reached EOF" << endl;
						break;
					}

					if (this->sequences_lookahead.count(seq) < 1) { //sliding windows not in dictionary.keys
						this->sequences_lookahead.insert({seq, {}});	
					} else {
					
						next_character_prevision = get_next_character_prediction(seq);

						// cout << "Sequence: " << seq << endl;

						// cout << "Current table: " << endl;

						// for ( auto const&p : this->sequences_lookahead[seq] ) {
						// 	cout << p.first << ": " << "Nh = " << p.second.numHits << " Nf = " << p.second.numFails << " P = " << p.second.prob << endl;
						// }

						//cout << seq << " Predicted: " << next_character_prevision << " GOt: " << next_character << endl;

						if (next_character == next_character_prevision) {
							this->sequences_lookahead[seq][next_character_prevision].numHits += 1;
							cur_Nh++;
						} else {
							this->sequences_lookahead[seq][next_character_prevision].numFails += 1;
							cur_Nf++;
						}

						calculate_probability(seq, next_character_prevision);

						this->accuracy = ((float)cur_Nh) / (cur_Nh + cur_Nf);

						//cout << "Cur model prob: " << probability << endl;

						//cout << endl;

						if (this->accuracy < this->threshold) {
							Nh += cur_Nh;
							Nf += cur_Nf;
							cur_Nh = 0;
							cur_Nf = 0;
							reset_map();
						}
					}


					if (this->sequences_lookahead[seq].count(next_character) < 1) {
						char_data_t charInit = { 0, 0, 0 };
						this->sequences_lookahead[seq][next_character] = charInit;
						calculate_probability(seq, next_character);
					}
				} 
				
				this->file.seekg(++this->pointer, ios::beg);	// Increments pointer for next iteration (sliding-window)	
			}

			this->accuracy = ((float)Nh) / (Nh + Nf);
			this->n_bits = -log(this->accuracy)/log(2);
			this->expected_total_bits = this->n_bits * this->file_length;

			this->exec_time =  float (clock() - this->exec_time);
			
			cout << "Nh = " << Nh << endl;
			cout << "Nf = " << Nf << endl;
			cout << "Probability " << this->accuracy << endl;
			cout << "Bits = " << this->n_bits << endl;
			cout << "Expected total number of bits = " << this->expected_total_bits << endl;
			cout << this->exec_time << " ms" << endl;
			


			ofstream out;

			out.open("model");

			for ( auto const&p : this->sequences_lookahead) {
				string seq = p.first;
				out << seq << endl;
				for ( auto const &m : this->sequences_lookahead[seq] ) {
					out << m.first << " " << m.second.prob << endl;
				}
				out << endl;
			}
			out.close();

		}	

		void export_run(string filename) {
			ofstream out;
			out.open (filename);
			out << "file : " << this->filename << endl;
			out << "k : " << this->k << endl;
			out << "alpha : " << this->alpha << endl;
			out << "prob : " << this->accuracy << endl;
			out << "bits : " << this->n_bits << endl;
			out << "total_bits : " << this->expected_total_bits << endl;
			out << "time : " << this->exec_time << endl;
			out.close();
		}
};





int main(int argc, char **argv) {
	
	// Command line arguments
	string filename; 									// file to predict and compare
	string out_file;
	int k = 5;											// size of the sliding window
	float alpha = 0.1;									// alpha value for probability
	float threshold = 0.5;								// probability threshold
	string output_file = "output.txt";


	int opt;
    while ((opt = getopt(argc, argv, "f:k:a:t:o:")) != -1) {
        switch (opt) {
			case 'o':
				out_file = string(optarg);
				break;
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
                cerr << "Usage: " << argv[0] << " -f <filename> -k <window_size> -a <alpha> -t <threshold> -o <previsions_output_file>\n";
                return 1;
        }
    }

	CopyModel cp(filename, k, alpha, threshold, output_file);

	cp.start();
	cp.export_run(out_file);

	return 0;
}