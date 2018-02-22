#include "Debug.h"
#include "DxLib.h"
#include "Color.h"
#include "GlobalData.h"
#include "Command.h"
#include "Server.h"
#include <time.h>

const int ACTIVE_CLASS_X = 20;
const int LOG_X = WIDTH - 250;
const int SCENE_X = 20;
const int Y_POS = 20;
const int ACTIVE_CLASS_INIT_Y = 40;

Debug::Debug( GlobalDataPtr data ) :
_data( data ){
	setFlag( 0 );
	_command = CommandPtr( new Command( _data ) );
	_color = ColorPtr( new Color( ) );
	_drawer = _data->getDrawerPtr( );
}

Debug::Debug( ) {
}

Debug::~Debug( ) {
}

void Debug::initialize( ) {
	_log_y = Y_POS;
	_active_y = ACTIVE_CLASS_INIT_Y;
	initLog( );
	initActiveClass( );

	switch ( _data->getMachineType( ) ) {
	case SERVER: _my_ip = _data->getServerPtr( )->getServerIpStr( );
	case CLIENT: _my_ip = _data->getClientPtr( )->getClientIpStr( );
	}
}

std::string Debug::getTag( ) {
	return "DEBUG";
}

void Debug::error( std::string err ) {
	if ( fopen_s( &_fp, "error.txt", "w" ) != 0 ) {
		return;
	}
	time_t now = time( NULL );
	const int SIZE = 128;
	char buf[ SIZE ] = { };
	ctime_s( buf, SIZE, &now );
	//���s������
	int len = ( int )strlen( buf );
	buf[ len - 1 ] = '\0';

	fprintf_s( _fp, "[ %s ] %s", buf, err.c_str( ) );
	fclose( _fp );
}

void Debug::update( ) {
	printLog( );
	printActiveClass( );
	printScene( );
	drawMyIp( );
	drawMousePoint( );
	//DrawCircle( WIDTH / 2, HEIGHT /2, 5, _color->getColor( RED ), TRUE );

	if ( _data->getKeyState( KEY_INPUT_RETURN ) == 1 && !_data->getCommandFlag( ) ) {
		_command->setFlag( 1 );
	}

	_data->setCommandFlag( false );
	if ( _command->getFlag( ) && _data->getMachineType( ) == CLIENT ) {
		_command->update( );
		commandExecution( );
		_data->setCommandFlag( true );
	}

	initLog( );
	initActiveClass( );

	_log_y = Y_POS;
	_active_y = ACTIVE_CLASS_INIT_Y;
}

void Debug::commandExecution( ) {
	if ( _command->getWordNum( ) < 2 ) {
		return;
	}

	if ( _command->getWord( 0 ) == "SEND" ) {
		if ( _command->getWord( 1 ) == "UDP" ) {
			if ( _data->getMachineType( ) == SERVER ) {
				_data->getServerPtr( )->sendDataUdp( );
			}
		}
	}

	if ( _command->getWord( 0 ) == "SET" ) {
		if ( _command->getWord( 1 ) == "SCENE" ) {
			std::string str = _command->getWord( 2 );
			SCENE scene = NONE;
			if ( str == "TITLE" ) {
				scene = TITLE;
			}
			if ( str == "CONNECT" ) {
				scene = CONNECT;
			}
			if ( str == "BATTLE" ) {
				scene = BATTLE;
			}
			if ( str == "RESULT" ) {
				scene = RESULT;
			}
			if ( str == "SELECTITEM" ) {
				scene = SELECT_ITEM;
			}

			if ( scene != NONE ) {
				_data->setScene( scene );
			}
		}
	}
}

void Debug::drawMousePoint( ) {
	const int DRAW_X = 20;
	int x = _data->getMouseX( );
	int y = _data->getMouseY( );
	_drawer->setFrontString( false, DRAW_X, _active_y, WATER, "MOUSE_POINT" );
	_active_y += Y_POS;
	_drawer->setFrontString( false, DRAW_X, _active_y, WATER, "X : " + std::to_string( x ) );
	_active_y += Y_POS;
	_drawer->setFrontString( false, DRAW_X, _active_y, WATER, "Y : " + std::to_string( y ) );
	_active_y += Y_POS;
}

void Debug::printLog( ) {
	int size = ( int )_log.size( );

	for ( int i = 0; i < size; i++ ) {
		_drawer->setFrontString( false, LOG_X, _log_y, WHITE, "Log : " + _log[ i ] );
		_log_y += Y_POS;
	}
}

void Debug::printActiveClass( ) {
	int size = ( int )_active_class.size( );

	if ( size < 1 ) {
		return;
	}

	for ( int i = 0; i < size; i++ ) {
		_drawer->setFrontString( false, ACTIVE_CLASS_X, _active_y, RED, "Active : " + _active_class[ i ] );
		_active_y += Y_POS;
	}
}

void Debug::printScene( ) {
	std::string str = "";
	SCENE scene = _data->getScene( );

	switch ( scene ) {
	case NONE        : str = "NONE   "     ; break;
	case TITLE       : str = "TITLE  "     ; break;
	case CONNECT     : str = "CONNECT"     ; break;
	case BATTLE      : str = "BATTLE "     ; break;
	case RESULT      : str = "RESULT "     ; break;
	case SELECT_ITEM : str = "SELECT_ITEM" ; break;
	default          : str = "NONE"        ; break;
	}

	_drawer->setFrontString( false, SCENE_X, Y_POS, WHITE, "SCENE  : " + str );
}

void Debug::drawMyIp( ) {
	_drawer->setFrontString( false, ACTIVE_CLASS_X, _active_y, YELLOW, "MyIP : " + _my_ip );
	_active_y += Y_POS;
}

void Debug::initLog( ) {
	if ( _log.size( ) < 1 ) {
		return;
	}
	std::vector< std::string >( ).swap( _log );
}

void Debug::addLog( std::string add ) {
	if ( getFlag( ) < 1 ) {
		return;
	}
	_log.push_back( add );
}

void Debug::initActiveClass( ) {
	if ( _active_class.size( ) < 1 ) {
		return;
	}
	std::vector< std::string >( ).swap( _active_class );
}

void Debug::setActiveClass( std::string tag ) {
	int size = ( int )_active_class.size( );
	for ( int i = 0; i < size; i++ ) {
		if ( _active_class[ i ] == tag ) {
			return;
		}
	}
	_active_class.push_back( tag );
}

void Debug::setLine( double sx, double sy, double ex, double ey, COLOR col ) {
	_drawer->setLine( sx, sy, ex, ey, col );
}