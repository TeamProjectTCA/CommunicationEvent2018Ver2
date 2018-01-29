#include "GameMaster.h"
#include "GlobalData.h"
#include "Connector.h"
#include "Server.h"
#include "Log.h"
#include "Field.h"
#include "Command.h"
#include <random>
#include <map>

GameMaster::GameMaster( GlobalDataPtr data, ConnectorPtr connector, LogPtr log, CommandPtr command ) :
_data( data ),
_connector( connector ),
_log( log ),
_command( command ) {
	_field = FieldPtr( new Field( _data ) );
	_server = _data->getServerPtr( );
	setFlag( 1 );
}

GameMaster::~GameMaster( ) {
}

std::string GameMaster::getTag( ) {
	return "GAMEMASTER";
}

void GameMaster::initialize( ) {
	_matching = false;
	_dice = false;
	_winner = -1;
	std::array< Data, PLAYER_NUM >( ).swap( _client_data );
	_phase = SET_PLAYER_PHASE;
	_server->setBattlePhase( _phase );
}

void GameMaster::update( ) {
	bool matching = _connector->isMatching( );
	if ( _matching != matching ) {
		_matching = matching;
		if ( !_matching ) {
			initialize( );
		}
	}

	if ( !_matching ) {
		return;
	}
	commandExecution( );

	if ( !_dice ) {
		orderPlayer( );
	}

	switch ( _phase ) {
	case SET_PLAYER_PHASE	: updatePlayerPhase( ); break;
	case SET_MIRROR_PHASE	: updateMirrorPhase( ); break;
	case ATTACK_PHASE		: updateAttackPhase( ); break;
	case JUDGE_PHASE		: updateJudgePhase ( ); break;
	}

	_server->sendDataUdp( );

	switch ( _phase ) {
	case SET_PLAYER_PHASE	: inputPlayerPhase( ); break;
	case SET_MIRROR_PHASE	: inputMirrorPhase( ); break;
	case ATTACK_PHASE		: inputAttackPhase( ); break;
	case JUDGE_PHASE		: inputJudgePhase ( ); break;
	}
}

void GameMaster::orderPlayer( ) {
	std::random_device rd;
	int idx = rd( ) % 2;
	_client_data[ idx ].order = 1;
	idx = ( idx + 1 ) % 2;
	_client_data[ idx ].order = 2;
	_dice = true;
}

int GameMaster::getWaitingIdx( ) const {
	int idx = -1;
	for ( int i = 0; i < PLAYER_NUM; i++ ) {
		if ( _client_data[ i ].fin ) {
			continue;
		}
		if ( idx != -1 && _client_data[ i ].order - 1 > idx ) {
			continue;
		}
		idx = i;
	}
	return idx;
}

int GameMaster::getOrderIdx( int order ) const {
	int idx = -1;
	for ( int i = 0; i < PLAYER_NUM; i++ ) {
		if ( _client_data[ i ].order != order ) {
			continue;
		}
		idx = i;
		break;
	}
	return idx;
}

void GameMaster::updatePlayerPhase( ) {
	int idx = getWaitingIdx( );
	_server->setOrder( -1 );

	if ( idx != -1 ) {
		return;
	}

	_phase = SET_MIRROR_PHASE;
	_server->setBattlePhase( _phase );

	for ( int i = 0; i < MACHINE_MAX; i++ ) {
		_field->setPlayerPoint( i, _client_data[ i ].player_pos );
		_server->setPlayerPos( i, _client_data[ i ].player_pos );
		_client_data[ i ].fin = false;
	}

	int dif = 0;
	int player_one = _field->getPlayerPoint( 0 );
	int player_two = _field->getPlayerPoint( 1 );
	std::map< int, int > point;
	for ( int i = 0; i < PLAYER_POSITION * 2; i++ ) {
		point[ i ] = i;
	}

	point.erase( player_one );
	point.erase( player_two );

	int rand = _data->getRandom( 0, PLAYER_POSITION * 2 - 2 );
	std::map< int, int >::iterator ite;
	ite = point.begin( );
	for ( int i = 0; i < rand - 1; i++ ) {
		ite++;
	}

	_server->setLazerPos( ite->second );
}

void GameMaster::updateMirrorPhase( ) {
	int idx = getWaitingIdx( );

	_server->setOrder( idx );

	if ( idx != -1 ) {
		return;
	}

	_phase = ATTACK_PHASE;
	_server->setBattlePhase( _phase );

	idx = getOrderIdx( 1 );
	for ( int i = 0; i < PLAYER_NUM; i++ ) {
		Data data = _client_data[ idx ];
		_field->setMirrorPoint( idx, data.x, data.y, data.angle );
		_server->setStcPlayerNum( i, data.player_num );
		_server->setStcX( i, data.x );
		_server->setStcY( i, data.y );
		_server->setStcAngle( i, data.angle );
		_server->setStcFlag( i, true );

		idx = ( idx + 1 ) % PLAYER_NUM;
	}

	for ( int i = 0; i < PLAYER_NUM; i++ ) {
		_client_data[ i ].fin = false;
	}
}

void GameMaster::updateAttackPhase( ) {
	int idx = getWaitingIdx( );
	_server->setOrder( -1 );

	if ( idx != -1 ) {
		return;
	}

	for ( int i = 0; i < PLAYER_NUM; i++ ) {
		if ( !_client_data[ i ].alive ) {
			_winner = ( i + 1 ) % PLAYER_NUM;
			break;
		}
	}
	
	_phase = JUDGE_PHASE;
	_server->setBattlePhase( _phase );
	_server->setStcWinner( _winner );

	for ( int i = 0; i < PLAYER_NUM; i++ ) {
		_client_data[ i ].fin = false;
	}
}

void GameMaster::updateJudgePhase( ) {
	int idx = getWaitingIdx( );
	_server->setOrder( -1 );

	if ( idx != -1 ) {
		return;
	}

	if ( _winner < 0 ) {
		//���s�Ȃ�
		_phase = SET_MIRROR_PHASE;
		_server->setBattlePhase( _phase );
		int one = _client_data[ 0 ].player_pos;
		int two = _client_data[ 1 ].player_pos;
		std::array< Data, PLAYER_NUM >( ).swap( _client_data );
		_client_data[ 0 ].player_pos = one;
		_client_data[ 1 ].player_pos = two;
		_dice = false;
	}
}

void GameMaster::inputPlayerPhase( ) {
	for ( int i = 0; i < MACHINE_MAX; i++ ) {
		if ( _server->isRecving( i ) ) {
			_client_data[ i ].player_pos = _server->getPlayerPos( i );
			_client_data[ i ].fin = true;
		}
	}
}

void GameMaster::inputMirrorPhase( ) {
	for ( int i = 0; i < MACHINE_MAX; i++ ) {
		if ( _server->isRecving( i ) ) {
			_client_data[ i ].player_num = i;
			_client_data[ i ].x = _server->getCtsX( i );
			_client_data[ i ].y = _server->getCtsY( i );
			_client_data[ i ].angle = _server->getCtsAngle( i );
			_client_data[ i ].fin = true;
		}
	}
}

void GameMaster::inputAttackPhase( ) {
	for ( int i = 0; i < MACHINE_MAX; i++ ) {
		if ( _server->isRecving( i ) ) {
			_client_data[ i ].alive = _server->getAlive( i );
			_client_data[ i ].fin = true;
		}
	}
}

void GameMaster::inputJudgePhase( ) {
	for ( int i = 0; i < MACHINE_MAX; i++ ) {
		if ( _server->isRecving( i ) ) {
			_client_data[ i ].fin = true;
		}
	}
}

void GameMaster::commandExecution( ) {
	if ( _command->getWordNum( ) < 1 ) {
		return;
	}

	if ( _command->getWordNum( ) < 2 ) {
		return;
	}

	if ( _command->getWord( 0 ) != "SET" ) {
		return;
	}
	if ( _command->getWord( 1 ) == "PLAYER" ) {
		std::string pos = _command->getWord( 2 );
		if ( ( int )pos.size( ) > 1 ) {
			return;
		}
		int val = atoi( pos.c_str( ) );
		if ( 0 <= val && val <= 4 ) {
			_client_data[ 1 ].player_pos = val + PLAYER_POSITION;
			_client_data[ 1 ].fin = true;
			_log->add( "player 1 pos " + std::to_string( val ) );
		}
	}

	if ( _command->getWordNum( ) < 5 ) {
		return;
	}

	if ( _command->getWord( 1 ) == "MIRROR" ) {
		std::string x = _command->getWord( 2 );
		std::string y = _command->getWord( 3 );
		std::string angle = _command->getWord( 4 );
		if ( ( int )x.size( ) > 1 || ( int )y.size( ) > 1 ) {
			return;
		}
		if ( angle != "RIGHT" && angle != "LEFT" ) {
			return;
		}
		const int ROW = 5;
		const int COL = 5;
		int val_x = atoi( x.c_str( ) );
		int val_y = atoi( y.c_str( ) );
		if ( 0 <= val_x && val_x < COL &&
			 0 <= val_y && val_y < ROW ) {
			_client_data[ 1 ].player_num = 1;
			_client_data[ 1 ].x = val_x;
			_client_data[ 1 ].y = val_y;
			_client_data[ 1 ].angle = ( angle == "RIGHT" ? RIGHT : LEFT );
			_client_data[ 1 ].fin = true;
			_log->add( "mirror pos ( " + x + ", " + y + " )" );
			_log->add( "       angle " + angle );
		}
	}

	if ( _command->getWord( 0 ) == "LIVE" ) {
		_client_data[ 1 ].alive = true;
		_client_data[ 1 ].fin = true;
	}
}