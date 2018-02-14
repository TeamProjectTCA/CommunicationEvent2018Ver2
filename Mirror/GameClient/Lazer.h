#pragma once
#include "Base.h"
#include "smart_ptr.h"
#include "Field.h"
#include <string>
#include <vector>
#include <list>

PTR( Lazer );
PTR( GlobalData );
PTR( Drawer );
PTR( Field );

const int BOM_EFFECT_MAX = 8;
const int DEAD_EFFECT_MAX = 8;

class Lazer : public Base {
private:
	struct Coordinate {
		short int cnt;
		short int x;
		short int y;
	};
public:
	Lazer( GlobalDataPtr data );
	virtual ~Lazer( );

public:
	std::string getTag( );
	void initialize( );
	void update( );
public:
	bool isFinish( ) const;
	void clearLazerImage( );

private:
	void updateUnitVector( );
	void clearBomEffect( );
	double getLazerImageAngle( );

private:
	void drawRefrecEffect( );
	void drawDeadEffect( );

private:
	bool _lazer_update;
	bool _fin;
	bool _dead_flag;
	double _distance;
	short int _wait;
	Field::Vector _start;
	Field::Vector _dir_vec;
	Field::Vector _unit;
	std::list< Coordinate > _reflec_pnt;
	Coordinate _dead_pnt;

	//�摜
	int _lazer_image;
	std::array< int, BOM_EFFECT_MAX > _bom_images;
	std::array< int, DEAD_EFFECT_MAX > _dead_effect_images;
	std::vector< ImageProperty > _lazer;

	GlobalDataPtr _data;
	DrawerPtr _drawer;
	FieldPtr _field;
};