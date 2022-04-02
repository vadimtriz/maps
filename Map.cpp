#include "Map.h"
#include <fstream>
#include <sstream>
#include <string>
Map::Map(const char *filename) {
std::ifstream file {filename};
if (not file.good())
	throw std::runtime_error(std::string("error ")+std::string(filename));
std::string line;
std::getline(file, line);
std::stringstream params {line};
params >>_width>>_height>>_start_x>> _start_y>>_start_a;
_data.resize(_width*_height);
for (int i=0; i<_height;++i){
	std::getline(file,line);
	for (int j=0; j<_width;++j){
		int cell=1;
		if (line[j]==' ') cell =0;
		if (line[j]=='@') cell =2;
		if (line[j]=='&') cell =3;
		if (line[j]=='_') cell =4;
		if (line[j]=='X') cell =5;

		_data[i*_width+j]=cell;
	}
}
file.close();
}

