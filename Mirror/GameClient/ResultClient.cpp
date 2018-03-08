#include "ResultClient.h"
#include "GlobalData.h"
#include "Game.h"
#include "Drawer.h"

ResultClient::ResultClient( GlobalDataPtr data, GamePtr game ) :
_data( data ),
_game( game ) {
	setFlag( 1 );
	_drawer = _data->getDrawerPtr( );
}

ResultClient::~ResultClient( ) {
}

std::string ResultClient::getTag( ) {
	return "RESULTCLIENT";
}

void ResultClient::initialize( ) {
	_win = _game->isWin( );
	_cnt = FRAME * 5;
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

	_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 5 * 3, YELLOW, "Return Title ....", Drawer::BIG );
	_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 5 * 3, RED,    "                  " + std::to_string( _cnt / FRAME ), Drawer::BIG );

	_cnt--;

	if ( _cnt <= 0 ) {
		_data->setInitFlag( );
	}
}
