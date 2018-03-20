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

class Lazer {
private:
	struct Coordinate {
		short int cnt;
		short int x;
		short int y;
		float angle;
	};
	struct ImageSize {
		float width;
		float height;
	};

	enum DIRECT {
		DIR_UP,
		DIR_DOWN,
		DIR_LEFT,
		DIR_RIGHT
	};
public:
	Lazer( GlobalDataPtr data );
	virtual ~Lazer( );

public:
	std::string getTag( );
	void update( );

public:
	bool isFinish( ) const;
	void clearLazerImage( );

private:
	void updateLazerVector( );
	void updateUnitVector( );
	void checkPlayerHit( );
	DIRECT convVectorToDir( Vector vec );

private:
	void setDirect( MIRROR_ANGLE angle );

private:
	double getLazerImageAngle( );
	double getReflectEffectAngle( Vector old_vec, Vector new_vec );
	Vector getDirectVector( ) const;

private:
	void drawLazerLine( ) const;
	void drawRefrecEffect( );
	void drawDeadEffect( );

private:
	bool _play_lazer_se;
	bool _lazer_update;
	bool _fin;
	bool _dead_flag;
	double _distance;
	double _lazer_speed;
	short int _wait;
	DIRECT _direct;
	Vector _start;     //�n�_
	Vector _dir_vec;   //����(������W�n)
	Vector _unit;      //������W�n
	Vector _hit_point; //�����蔻��̂��������̏ꏊ
	std::list< Coordinate > _reflec_point;
	Coordinate _dead_pnt;

	//�摜
	int _lazer_image;
	int _lazer_reflect_image;
	std::array< int, DEAD_EFFECT_MAX > _dead_effect_images;
	std::vector< Base::ImageProperty > _lazer;
	ImageSize _lazer_size;

	GlobalDataPtr _data;
	DrawerPtr _drawer;
	FieldPtr _field;
};