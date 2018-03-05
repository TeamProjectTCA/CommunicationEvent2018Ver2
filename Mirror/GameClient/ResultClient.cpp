#include "ResultClient.h"
#include "GlobalData.h"
#include "Game.h"
#include "Drawer.h"
#include "KeyBoard.h"

ResultClient::ResultClient( GlobalDataPtr data, GamePtr game ) :
_data( data ),
_game( game ) {
	_drawer = _data->getDrawerPtr( );
	_keyboard = KeyBoardPtr( new KeyBoard( ) );
	setFlag( 1 );
}

ResultClient::~ResultClient( ) {
}

std::string ResultClient::getTag( ) {
	return "RESULTCLIENT";
}

void ResultClient::initialize( ) {
	_win = _game->isWin( );
}

void ResultClient::finalize( ) {
}

void ResultClient::update( ) {
	if ( _win ) {
		//勝利
		_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 3, RED, "あなたの勝ち", Drawer::SUPER_BIG );
	} else {
		//敗北
		_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 3, RED, "あなたの負け", Drawer::SUPER_BIG );
	}

	_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 5 * 3, YELLOW, "Press \"ENTER\" Key", Drawer::BIG );

	if ( _data->getKeyState( _keyboard->getKeyCode( ENTER_KEY ) ) == 1 ) {
		_data->setInitFlag( );
	}
}
