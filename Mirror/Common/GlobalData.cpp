#include "GlobalData.h"
#include "UpdateKey.h"
#include "MouseEvent.h"
#include "Image.h"
#include "Sound.h"
#include "Debug.h"
#include "Server.h"
#include "Client.h"
#include "Drawer.h"
#include "Soundplayer.h"
#include "Message.h"
#include "LoadCSV.h"
#include <ctime>
#include <cstdlib>
#include <limits.h>

GlobalData::GlobalData( MACHINE_TYPE type ) :
_type( type ) {
	setFlag( 1 );
	_key =   UpdateKeyPtr( new UpdateKey( ) );
	_mouse = MouseEventPtr( new MouseEvent( ) );
	_image = ImagePtr( new Image( _type ) );
	_drawer = DrawerPtr( new Drawer( ) );
	_soundplayer = SoundplayerPtr( new Soundplayer( ) );
	_message = MessagePtr( new Message( _drawer ) );
	_sound = SoundPtr( new Sound( _type ) );
	_debug = NULL;

	switch ( _type ) {
	case SERVER	: 
		_server = ServerPtr( new Server( ) );
		break;

	case CLIENT	: 
		_client = ClientPtr( new Client( ) );
		break;

	case TEST	: 
		_debug  = DebugPtr ( new Debug( NULL ) );
		break;
	}

	initialize( );

	_init = true;
}

GlobalData::~GlobalData( ) {
}

std::string GlobalData::getTag( ) {
	return "GLOBALDATA";
}

void GlobalData::initialize( ) {
	_scene = TITLE;
	_command_flag = false;
	_clicking = false;
	_live = false;
	_count = 0;
	_soundplayer->initialize( );

	switch ( _type ) {
	case SERVER:
		_server->disConnect( );
		break;

	case CLIENT:
		_client->disConnect( );
		break;

	case TEST:
		_debug->initialize( );
		break;
	}

	LoadCSVPtr csv = LoadCSVPtr( new LoadCSV( ) );
	std::vector< CsvData > data;
	csv->read( data, "flag" );
	int size = ( int )data.size( );
	bool load_scene = false;
	for ( int i = 0; i < size; i++ ) {
		if ( _type != SERVER ) {
			continue;
		}
		//���C�u�ɂ��邩�ǂ���
		if ( "LIVE_DRAW" == data[ i ].tag ) {
			if ( atoi( data[ i ].values.begin( )->c_str( ) ) ) {
				_live = true;
			}
			continue;
		}
	}


	foldInitFlag( );
}

void GlobalData::finalize( ) {
	if ( _type == CLIENT ) {
		_sound->~Sound( );
	}
	_key->~UpdateKey( );
	_mouse->~MouseEvent( );
	_image->~Image( );
	_message->~Message( );
}

void GlobalData::update( ) {
	//�������R�}���h
	if ( _key->getKeyState( KEY_INPUT_Z ) == 1 &&
		 _key->getKeyState( KEY_INPUT_X ) == 1 &&
		 _key->getKeyState( KEY_INPUT_C ) == 1 ) {
		setInitFlag( );
	}

	_key->update( );
	_mouse->update( );
	_soundplayer->update( );

	if ( _message_flag ) {
		_message->update( );

		if ( getClickingLeft( ) ) {
			_clicking = true;
		}
		if ( !getClickingLeft( ) && _clicking ) {
			_message_flag = false;
			_clicking = false;
			_message->initialize( );
		}
		return;
	}

	_count++;
	if ( _count >= INT_MAX ) {
		_count = 0;
	}
}

void GlobalData::setScene( SCENE scene ) {
	_scene = scene;
}

int GlobalData::getKeyState( int key ) const {
	return _key->getKeyState( key );
}

int GlobalData::getMouseX( ) const {
	return _mouse->getPointX( );
}

int GlobalData::getMouseY( ) const {
	return _mouse->getPointY( );
}

bool GlobalData::getClickLeft( ) const {
	return _mouse->getClickLeft( );
}

bool GlobalData::getClickRight( ) const {
	return _mouse->getClickRight( );
}

int GlobalData::getClickingLeft( ) const {
	return _mouse->getClickingLeft( );
}

int GlobalData::getClickingRight( ) const {
	return _mouse->getClickingRight( );
}

int GlobalData::getRandom( int min, int max ) const {
	std::srand( ( unsigned int )time( NULL ) );
	int random = min + rand( ) % ( max - min + 1 );
	return random;
}

int GlobalData::getCount( ) const{
	return _count;
}

SCENE GlobalData::getScene( ) const {
	return _scene;
}

MACHINE_TYPE GlobalData::getMachineType( ) const {
	return _type;
}

bool GlobalData::isCommandFlag( ) const {
	return _command_flag;
}

bool GlobalData::isInitFlag( ) const {
	return _init;
}

bool GlobalData::isMessageFlag( ) const {
	return _message_flag;
}

bool GlobalData::isLiveFlag( ) const {
	return _live;
}

void GlobalData::setCommandFlag( bool flag ) {
	_command_flag = flag;
}

void GlobalData::setInitFlag( ) {
	_init = true;
}

void GlobalData::foldInitFlag( ) {
	_init = false;
}

void GlobalData::setMessage( std::string str ) {
	_message->add( str );
	_message_flag = true;
}

void GlobalData::setPtr( DebugPtr ptr ) {
	_debug = ptr;
}

void GlobalData::setPtr( ServerPtr ptr ) {
	_server = ptr;
}

void GlobalData::setPtr( ClientPtr ptr ) {
	_client = ptr;
}

void GlobalData::setPtr( FieldPtr ptr ) {
	_field = ptr;
}

ImagePtr GlobalData::getImagePtr( ) const {
	return _image;
}

SoundPtr GlobalData::getSoundPtr( ) const {
	return _sound;
}

DebugPtr GlobalData::getDebugPtr( ) const {
	return _debug;
}

ServerPtr GlobalData::getServerPtr( ) const {
	return _server;
}

ClientPtr GlobalData::getClientPtr( ) const {
	return _client;
}

DrawerPtr GlobalData::getDrawerPtr( ) const {
	return _drawer;
}

SoundplayerPtr GlobalData::getSoundplayerPtr( ) const {
	return _soundplayer;
}

FieldPtr GlobalData::getFieldPtr( ) const {
	return _field;
}