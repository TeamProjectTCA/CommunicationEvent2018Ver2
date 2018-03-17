#pragma once
#include "const.h"
#include "smart_ptr.h"
#include <Windows.h>
#include <string>
#include <vector>

PTR( Sound );
PTR( MultiThreadLoad );

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

	bool _sound_load; //���y�ǂݍ��݂����邩�ǂ���

	std::vector< Directory > _data;
	std::vector< WIN32_FIND_DATA > _file;
	int _dir_num;

	MultiThreadLoadPtr _load;
};