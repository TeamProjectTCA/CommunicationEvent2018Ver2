#include "Title.h"
#include "GlobalData.h"
#include "Image.h"
#include "Sound.h"
#include "Drawer.h"
#include "Soundplayer.h"
#include "Client.h"
#include "const.h"

const float TITLE_LOGO_X = WIDTH / 2;
const float TITLE_LOGO_Y = 150;
const float START_BUTTON_X = WIDTH_F / 2.0f;
const float START_BUTTON_Y = HEIGHT / 5 * 2.5;
const float ITEM_SET_BUTTON_X = START_BUTTON_X;
const float ITEM_SET_BUTTON_Y = HEIGHT / 5 * 3.5;
const float IP_SET_BUTTON_X = WIDTH - 100;
const float IP_SET_BUTTON_Y = HEIGHT - 100;

const short int BGM_NO = 2;

Title::Title( GlobalDataPtr data ) :
_data( data ) {
	setFlag( 1 );
	_drawer = _data->getDrawerPtr( );
	_soundplayer = _data->getSoundplayerPtr( );

	_cur_hand = LoadCursor( NULL, IDC_HAND );

	std::array< int, BUTTON_STATE_MAX >( ).swap( _startbutton_handles );
	std::array< int, BUTTON_STATE_MAX >( ).swap( _item_setbutton_handles );
	std::array< int, BUTTON_STATE_MAX >( ).swap( _ip_setbutton_handles );
	_startbutton    = BoxObject( );
	_item_setbutton = BoxObject( );
	_ip_setbutton   = BoxObject( );

	ImagePtr image_ptr = _data->getImagePtr( );
	SoundPtr sound_ptr = _data->getSoundPtr( );

	{//背景
		_back = LightImageProperty( );
		_back.cx = WIDTH / 2;
		_back.cy = HEIGHT / 2;
		_back.png = image_ptr->getPng( BACKGROUND_IMAGE, BACKGROUND_WHITE ).png;
	}

	{//タイトルロゴ
		_title_logo = LightImageProperty( );
		_title_logo.cx = TITLE_LOGO_X;
		_title_logo.cy = TITLE_LOGO_Y;
		_title_logo.png = image_ptr->getPng( TITLE_IMAGE, 0 ).png;
	}

	{//bgm
		_bgm = SoundProperty( );
		_bgm.loop = true;
		_bgm.top = true;
		_bgm.wav = sound_ptr->getWav( TITLE_BGM, BGM_NO ).wav;
	}

	{//startボタン
		float half_width  = image_ptr->getPng( BUTTON_IMAGE, GAME_START_BUTTON_IDX ).width * 0.5f;
		float half_height = image_ptr->getPng( BUTTON_IMAGE, GAME_START_BUTTON_IDX ).height * 0.5f;

		for ( int i = 0; i < BUTTON_STATE_MAX; i++ ) {
			_startbutton_handles[ i ] = image_ptr->getPng( BUTTON_IMAGE, GAME_START_BUTTON_IDX + i ).png;
		}
		_startbutton.image.cx = START_BUTTON_X;
		_startbutton.image.cy = START_BUTTON_Y;
		_startbutton.image.png = image_ptr->getPng( BUTTON_IMAGE, GAME_START_BUTTON_IDX ).png;

		_startbutton.collider.lx = ( float )_startbutton.image.cx - half_width;
		_startbutton.collider.rx = ( float )_startbutton.image.cx + half_width;
		_startbutton.collider.ly = ( float )_startbutton.image.cy - half_height;
		_startbutton.collider.ry = ( float )_startbutton.image.cy + half_height;
	
	//item_settingボタン
		for ( int i = 0; i < BUTTON_STATE_MAX; i++ ) {
			_item_setbutton_handles[ i ] = image_ptr->getPng( BUTTON_IMAGE, ITEM_SELECT_BUTTON_IDX + i ).png;
		}
		_item_setbutton.image.cx = ITEM_SET_BUTTON_X;
		_item_setbutton.image.cy = ITEM_SET_BUTTON_Y;
		_item_setbutton.image.png = image_ptr->getPng( BUTTON_IMAGE, ITEM_SELECT_BUTTON_IDX ).png;

		_item_setbutton.collider.lx = ( float )_item_setbutton.image.cx - half_width;
		_item_setbutton.collider.rx = ( float )_item_setbutton.image.cx + half_width;
		_item_setbutton.collider.ly = ( float )_item_setbutton.image.cy - half_height;
		_item_setbutton.collider.ry = ( float )_item_setbutton.image.cy + half_height;

	//ip_settingボタン
		for ( int i = 0; i < BUTTON_STATE_MAX; i++ ) {
			_ip_setbutton_handles[ i ] = image_ptr->getPng( BUTTON_IMAGE, IP_SETTING_BUTTON_IDX + i ).png;
		}
		_ip_setbutton.image.cx = IP_SET_BUTTON_X;
		_ip_setbutton.image.cy = IP_SET_BUTTON_Y;
		_ip_setbutton.image.png = image_ptr->getPng( BUTTON_IMAGE, IP_SETTING_BUTTON_IDX ).png;

		_ip_setbutton.collider.lx = ( float )_ip_setbutton.image.cx - ( float )image_ptr->getPng( BUTTON_IMAGE, IP_SETTING_BUTTON_IDX ).width / 2;
		_ip_setbutton.collider.rx = ( float )_ip_setbutton.image.cx + ( float )image_ptr->getPng( BUTTON_IMAGE, IP_SETTING_BUTTON_IDX ).width / 2;
		_ip_setbutton.collider.ly = ( float )_ip_setbutton.image.cy - ( float )image_ptr->getPng( BUTTON_IMAGE, IP_SETTING_BUTTON_IDX ).height / 2;
		_ip_setbutton.collider.ry = ( float )_ip_setbutton.image.cy + ( float )image_ptr->getPng( BUTTON_IMAGE, IP_SETTING_BUTTON_IDX ).height / 2;
	}

	{//ボタンSE
		_button_se = SoundProperty( );
		_button_se.loop = false;
		_button_se.top = true;
		_button_se.wav = sound_ptr->getWav( EFFECT_SOUND, BUTTON_SE ).wav;
	}
}

Title::~Title( ) {
}

std::string Title::getTag( ) {
	return "TITLE";
}

void Title::initialize( ) {
	_hit_button_idx = -1;
	_past_hit_button_idx = -1;
	_startbutton_clicking = false;
	_item_setbutton_clicking = false;
	_ip_setbutton_clicking = false;
}

void Title::finalize( ) {
}

void Title::update( ) {
	//bgm
	if ( !_soundplayer->isPlaying( _bgm ) ) {
		_soundplayer->play( _bgm );
	}

	if ( _data->getKeyState( KEY_INPUT_Z ) == 1 && !_data->isCommandFlag( ) ) {
		_soundplayer->stop( _bgm );
		_data->setScene( CONNECT );
	}
	ClientPtr client = _data->getClientPtr( );
	if ( client->getPhase( ) == "CONNECTING" ) {
		
		client->disConnect( );
		client->initialize( );
	}

	//クリックのチェック
	chengeNextScene( );
	calcButtonAction( );

	drawBackGround( );
	drawTitle( );
	drawStartButton( );
	drawItemSettingButton( );
	drawIpSettingButton( );
}

void Title::chengeNextScene( ) {
	if ( ( _startbutton_clicking || _item_setbutton_clicking || _ip_setbutton_clicking ) && !isDrag( ) ) {
		int hit = getHitButton( );
				
		switch ( hit ) {
		case START_BUTTON:
			_soundplayer->stop( _bgm );
			_soundplayer->play( _button_se );
			_data->setScene( CONNECT );
			_data->getClientPtr( )->readIP( );
			break;
		case ITEM_SETTING_BUTTON:
			_soundplayer->play( _button_se );
			_data->setScene( SELECT_ITEM );
			break;
		case IP_SETTING_BUTTON:
			_soundplayer->play( _button_se );
			_data->setScene( IP_SETTING );
			break;
		default:
			_startbutton_clicking = false;
			_item_setbutton_clicking = false;
			_ip_setbutton_clicking = false;
			break;		
		}
	}
}

void Title::calcButtonAction( ){
	_startbutton.image.png = _startbutton_handles[ NORMAL ];
	_item_setbutton.image.png = _item_setbutton_handles[ NORMAL ];
	_ip_setbutton.image.png = _ip_setbutton_handles[ NORMAL ];

	if ( getHitButton( ) == NONE_BUTTON ) {
		return;
	}

	if ( !isDrag( ) ) {
		return;
	}

	BUTTON hit = getHitButton( );
	switch ( hit ) {
	case START_BUTTON:
		_startbutton.image.png = _startbutton_handles[ CLICKING ];
		_startbutton_clicking = true;
		break;

	case ITEM_SETTING_BUTTON:
		_item_setbutton.image.png = _item_setbutton_handles[ CLICKING ];
		_item_setbutton_clicking = true;
		break;

	case IP_SETTING_BUTTON:
		_ip_setbutton.image.png = _ip_setbutton_handles[ CLICKING ];
		_ip_setbutton_clicking = true;
	}
}

Title::BUTTON Title::getHitButton( ) const {
	double mouse_x = _data->getMouseX( );
	double mouse_y = _data->getMouseY( );

	//start button
	if ( _startbutton.collider.lx <= mouse_x && mouse_x <= _startbutton.collider.rx &&
		 _startbutton.collider.ly <= mouse_y && mouse_y <= _startbutton.collider.ry ) {
		SetCursor( _cur_hand );
		return START_BUTTON;
	}

	//item setting button
	if ( _item_setbutton.collider.lx <= mouse_x && mouse_x <= _item_setbutton.collider.rx &&
		 _item_setbutton.collider.ly <= mouse_y && mouse_y <= _item_setbutton.collider.ry ) {
		SetCursor( _cur_hand );
		return ITEM_SETTING_BUTTON;
	}

	//ip setting buton
	if ( _ip_setbutton.collider.lx <= mouse_x && mouse_x <= _ip_setbutton.collider.rx &&
		 _ip_setbutton.collider.ly <= mouse_y && mouse_y <= _ip_setbutton.collider.ry ) {
		SetCursor( _cur_hand );
		return IP_SETTING_BUTTON;
	}

	return NONE_BUTTON;
}

bool Title::isDrag( ) const {
	if ( _data->getClickingLeft( ) > 0 ) {
		return true;
	}
	return false;
}

void Title::drawBackGround( ) const {
	_drawer->setBackGroundImage( _back );
}

void Title::drawTitle( ) const {
	_drawer->setFrontImage( _title_logo );
}

void Title::drawStartButton( ) const {
	_drawer->setFrontImage( _startbutton.image );
}

void Title::drawItemSettingButton( ) const {
	_drawer->setFrontImage( _item_setbutton.image );
}

void Title::drawIpSettingButton( ) const {
	_drawer->setFrontImage( _ip_setbutton.image );
}