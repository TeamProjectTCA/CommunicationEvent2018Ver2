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
	_cause = _game->getCauseOfDeath( );
	_cnt = FRAME * 10;
}

void ResultClient::finalize( ) {
}

void ResultClient::update( ) {
	if ( _win ) {
		//����
		_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 3, RED, "���Ȃ��̏���", Drawer::SUPER_BIG );
		_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 2, WATER, convResultMessage( _win, _cause ), Drawer::LITTLE_BIG );
	} else {
		//�s�k
		_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 3, RED, "���Ȃ��̕���", Drawer::SUPER_BIG );
		_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 2, WATER, convResultMessage( _win, _cause ), Drawer::LITTLE_BIG );
	}

	_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 5 * 3.5, YELLOW, "Return Title ....", Drawer::BIG );
	_drawer->setFrontString( true, WIDTH / 2, HEIGHT / 5 * 3.5, RED,    "                  " + std::to_string( _cnt / FRAME ), Drawer::BIG );

	_cnt--;

	if ( _data->getClickLeft( ) ) {
		_cnt -= FRAME;
	}
	if ( _cnt <= 0 ) {
		_data->setInitFlag( );
	}
}

std::string ResultClient::convResultMessage( bool win, CAUSE_OF_DEATH cause ) {
	std::string str;
	if ( win ) {
		str = convWinMessage( cause );
	} else {
		str = convLoseMessage( cause );
	}
	return str;
}

std::string ResultClient::convWinMessage( CAUSE_OF_DEATH cause ) {
	std::string str;
	switch ( cause ) {
	case CAUSE_HIT:
		str = "�������I�����|�����Ƃɐ������܂����I";
		break;

	case CAUSE_TIME:
		str = "����͎��Ԑ؂�݂����ł��B���Ȃ��̐헪�ɖ��΁I";
		break;

	default:
		break;
	}
	return str;
}

std::string ResultClient::convLoseMessage( CAUSE_OF_DEATH cause ) {
	std::string str;
	switch ( cause ) {
	case CAUSE_HIT:
		str = "������̓I���󂳂�Ă��܂��܂���......";
		break;

	case CAUSE_TIME:
		str = "���Ԑ؂�̂悤�ł��B�헪���Â������݂����ł�....";
		break;

	default:
		break;
	}
	return str;
}