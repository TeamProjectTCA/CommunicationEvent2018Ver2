#pragma once
#include "smart_ptr.h"
#include "Base.h"
#include "const.h"
#include "Drawer.h"
#include <math.h>
#include <string>
#include <array>
#include <map>
#include <cmath>
#include <Windows.h>

PTR( Field );
PTR( GlobalData );
PTR( Drawer );
PTR( Image );
PTR( Debug );

const short int PLAYER_POSITION = 4;
const short int SQUARE_SIZE = ( short int )( 96 * FIELD_SIZE_RATE );
const short int FIELD_ROW = 4;
const short int FIELD_COL = 4;
const int START_POS_X = WIDTH / 3 * 2 - SQUARE_SIZE * FIELD_COL / 2;
const int START_POS_Y = HEIGHT / 2 - SQUARE_SIZE * FIELD_ROW / 2 + SQUARE_SIZE + SQUARE_SIZE / 10;
const int INFO_TEXT_MAX = 10;
const int BATTLE_BUTTON_IMAGE_NUM = 4;

struct Vector { 
	double x;
	double y;
	Vector normalize( ) {
		Vector vec = Vector( );
		vec.x = x;
		vec.y = y;

		double length = std::sqrt( vec.x * vec.x + vec.y * vec.y );

		Vector normal = Vector( );
		normal.x /= length;
		normal.y /= length;

		return normal;
	}
};

class Field : public Base {
#pragma pack( 1 )
private:
	static const short int LAZER_TYPE_MAX = 2;

public:
	struct Mirror {
		bool flag;
		int player_num;
		int x;
		int y;
		MIRROR_ANGLE angle = ANGLE_NONE;
	};

	struct PlayerPos {
		float x;
		float y;
	};

	struct Item {
		bool flag;
		short int type;
		float x;
		float y;
		short int used_turn;
	};

	enum COMMAND {
		COMMAND_LEFT,
		COMMAND_RIGHT,
		COMMAND_DELETE,
		COMMAND_TYPE_MAX,
		COMMAND_NONE
	};
#pragma pack( )

public:
	Field( GlobalDataPtr data );
	virtual ~Field( );

public:
	std::string getTag( );
	void initialize( );
	void finalize( );
	void nextTurn( );
	void update( );

public:
	void setInfoText( std::string str, COLOR col = YELLOW, Drawer::FONTSIZE_TYPE size = Drawer::NORMAL );
	void hitPlayerPos( );
	void hitFieldPos( );
	void setTurn( int turn );
	void setPlayerNum( int num );
	void setPhase( BATTLE_PHASE phase );
	void setPlayerPoint( int idx, int pos );
	void setLazerPoint( int pos );
	void setTmpPlayerPoint( );
	void resetTmpMirror( );
	void setTmpMirrorPoint( int player_num, int x, int y, MIRROR_ANGLE angle );
	void setMirrorPoint( int player_num, int x, int y, MIRROR_ANGLE angle );
	void deleteMirrorPoint( int idx );
	void setOrder( int order );
	void playerPosSelected( );
	void mirrorPosSelected( );
	void mirrorPosNotSelected( );
	void selectItem( int idx );
	void useItem( );
	void reverseMirror( );
	void changeClickButton( );
	void activeButtonLighting( );
	void deadCount( );
	void setDeadPlayer( int player );
	void setReverseFlag( );
	void setFirstOrSecond( bool first );
	void blindField( );
	void clearBlindField( );
	void setDeadCount( float time );

public:
	std::map< int, Mirror > &getAllMirror( );
	Vector getLazerPoint( ) const;
	PlayerPos getPlayerPos( int idx ) const;
	BATTLE_PHASE getPhase( ) const;
	int getTurn( ) const;
	int getTmpPlayerPoint( ) const;
	int getPlayerPosIdx( int idx ) const;
	int getHitMirrorIdx( ) const;
	int getPlayerPosHitNum( ) const;
	int getFieldPosHitNum( ) const;
	int getDeadPlayer( ) const;
	int getLazerPointIdx( ) const;
	int getSelectItem( ) const;
	int getHitItemIdx( ) const;
	int getHitMirrorCommandIdx( ) const;
	int getDeadCount( ) const;
	COMMAND getMirrorCommand( ) const;
	MIRROR_ANGLE getMirrorAngle( int idx ) const;
	bool isSelectedPlayer( ) const;
	bool isHitDecisionButton( ) const;
	bool isSelectedMirror( ) const;
	bool isHitItemCancelButton( ) const;
	bool isReverseFlag( ) const;

private:
	void resetInfo( );
	void checkHitMirrorCommand( );
	void checkItemRecovery( );

//Draw系
protected:
	void drawField( );
	void drawArmament( ) const;
	void drawTmpMirror( ) const;
	void drawDecisionButton( ) const;
	void drawMirror( ) const;
	void drawPlayerPos( ) const;
	void drawPlayer( ) const;
	void drawInfo( ) const;
	void drawRound( ) const;
	void drawSettingPlayer( );
	void drawItem( ) const;
	void drawBackGround( ) const;
	void drawMirrorCommand( ) const;
	void drawDeathCount( ) const;
	void drawItemCancelButton( ) const;
	void drawFirstOrSecond( ) const;

protected:
#pragma pack( 1 )
	struct BoxObject {
		LightImageProperty image;
		BoxCollider collider;
		short int half_width;
		short int half_height;
	};
	
	struct Info {
		std::string str;
		COLOR col;
		Drawer::FONTSIZE_TYPE size;
	};

	std::map< int, Mirror > _mirrors;
	std::array< Info, INFO_TEXT_MAX > _info;
	std::array< int, PLAYER_NUM > _player_pos_idx;
	std::array< PlayerPos, PLAYER_POSITION * 2 > _select_player_pos;
	std::array< COLOR, PLAYER_NUM >_player_color;
	std::array< Item, ITEM_POSSESSION_MAX > _item;
	bool _player_selected;
	bool _mirror_selected;
	bool _button_lighting;
	bool _reverse_mirror;
	bool _first;//先攻かどうか
	bool _blind_field;
	short int _turn;
	short int _order;
	short int _info_idx;
	short int _tmp_player_pos;
	short int _dead_player;
	short int _player_num;
	short int _player_pos_hit_num;
	short int _field_pos_hit_num;
	short int _hit_mirror_num;
	short int _lazer_point_idx;
	short int _select_item;
	int _dead_count;
	HCURSOR _cur_hand;
	BATTLE_PHASE _phase;
	Mirror _tmp_mirror;
	COMMAND _command;//鏡の処理ボタン

	//画像関連
	int _table_handle;
	int _time_string_handle;
	int _timeboard_handle;
	int _item_not_handle;
	std::array< int, PLAYER_NUM > _player_handle;
	std::array< int, NUMBER_HANDLE_MAX > _number_handle;
	std::array< int, PLAYER_NUM > _mirror_handle;
	std::array< int, BATTLE_BUTTON_IMAGE_NUM > _button_handle;
	std::array< int, ITEM_MAX > _item_handle;
	std::array< int, LAZER_TYPE_MAX > _lazer_handle;
	std::array< int, PLAYER_NUM > _first_or_second_handle;
	std::array< BoxObject, COMMAND_TYPE_MAX > _mirror_cmd;
	LightImageProperty _button_image;
	LightImageProperty _background;
	LightImageProperty _lazer_image;
	BoxObject _board;
	BoxObject _item_cancel;
#pragma pack( )

	GlobalDataPtr _data;
	DrawerPtr _drawer;
	ImagePtr _image;
	DebugPtr _debug;
};