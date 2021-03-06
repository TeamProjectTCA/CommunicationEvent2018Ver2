#pragma once
#include "smart_ptr.h"
#include <string>
#include <vector>

PTR( LoadCSV );

struct CsvData {
	std::string tag;
	std::vector< std::string > values;
};

class LoadCSV {
public:
	LoadCSV( );
	virtual ~LoadCSV( );

public:
	//参照で値を直接入れるため引数必須
	void read( std::vector< CsvData > &list, std::string file_name );

private:
	void findComma( std::vector< std::string > &value, std::string str );
};