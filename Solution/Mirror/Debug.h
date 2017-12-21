#pragma once
#include "Base.h"
#include "smart_ptr.h"
#include "const.h"
#include <vector>
#include <stdio.h>

PTR( Debug );
PTR( GlobalData );
PTR( Color );

class Debug : public Base {
public:
	Debug( GlobalDataPtr data );
	virtual ~Debug( );

public:
	void update( );
	void initialize( );

public:
	std::string getTag( );
	void error( std::string err );
	void addLog( std::string add );
	void setActiveClass( std::string tag );

private:
	int calcLogYpos( int num );
	void printLog( );
	void printActiveClass( );
	void initLog( );
	void initActiveClass( );

private:
	FILE *_fp;
	std::vector< std::string > _log;
	std::vector< std::string > _active_class;

	GlobalDataPtr _data;
	ColorPtr _color;
};

