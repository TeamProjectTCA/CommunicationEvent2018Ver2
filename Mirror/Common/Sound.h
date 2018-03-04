#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "const.h"

struct Wav {
	int wav;
};

class Sound {
public:
	Sound( );
	virtual ~Sound( );

public:
	void initialize( );
	Wav getWav( SOUND item, int num ) const;

private:
	void check( int wav ) const;
	void inputSound( );
	void inputFileName( std::string path );

private:
	struct Directory {
		std::string name;		//�f�B���N�g����
		std::vector< Wav > wav;	//�ǂݍ��񂾃T�E���h
	};

	std::vector< Directory > _data;
	std::vector< WIN32_FIND_DATA > _file;
	int _dir_num;
};