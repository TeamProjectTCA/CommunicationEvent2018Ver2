#pragma once
#include "Base.h"
#include "smart_ptr.h"
#include <array>

PTR( GlobalData );
PTR( Drawer );

const int SERVER_CONSOLE_LOG_MAX = 8;

class Table {
public:
	Table( GlobalDataPtr data );
	virtual ~Table( );

public:
	void initialize( );
	void update( );

public:
	void add( std::string log );

private:
	void isLogPring( );

private:
	int _add;
	std::array< std::string, SERVER_CONSOLE_LOG_MAX > _log;

	GlobalDataPtr _data;
	DrawerPtr _drawer;
};
