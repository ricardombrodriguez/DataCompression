#include <iostream>

class DataStream 
{
	private:
		std::string filename;
		char c;
		fstream::ifstream stream;
	
	public:
		DataStream(std::string filename);
		char next();
		void close();
};
