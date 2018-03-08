#pragma once
#include "smart_ptr.h"
#include <string>
#include <vector>

PTR( Message );
PTR( Drawer );

/*

��ʂ����ς��ɒ��ӎ�����A�G���[���e��\������N���X�ł��B
��F
�@�@�T�[�o�[�Ƃ̐ڑ��ɂ����ς����܂���

*/


class Message {
public:
	Message( DrawerPtr drawer );
	virtual ~Message( );

public:
	void initialize( );
	void update( );

public:
	void add( std::string str );

private:
	std::vector< std::string > _message;

	DrawerPtr _drawer;
};

