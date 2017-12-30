#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "const.h"

struct Png {
	int width;
	int height;
	int png;
};

class Image {
public:
	Image( );
	virtual ~Image( );

public:
	void initialize( );
	Png getPng( IMAGE item, int num ) const;

private:
	void check( int png ) const;
	void inputImage( );
	void inputFileName( std::string path );

private:
	struct Directory {
		std::string name;		//�f�B���N�g����
		std::vector< Png > png;	//�ǂݍ��񂾉摜
	};

	std::vector< Directory > _data;
	std::vector< WIN32_FIND_DATA > _file;
	int _dir_num;
};