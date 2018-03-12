#include "Client.h"

Client::Client( ) {
	setFlag( 1 );
}

Client::~Client( ) {
}

std::string Client::getTag( ) {
	return "CLIENT";
}

void Client::initialize( ) {
	_phase = READY;
	_connecting_tcp = false;
	_recving_tcp = false;
	_recving_udp = false;
	_matching = false;
	_interval = 0;
	_handle_tcp = -1;
	_handle_udp = -1;
	_player_num = 0;
	_recv_data_udp = NetWorkData( );
	_send_data_tcp = NetWorkData( );

	readIP( );
}

void Client::finalize( ) {
	disConnect( );
}

void Client::update( ) {
	switch ( _phase ) {
	case READY		: connect( ); break;
	case CONNECTING	: recving( ); break;
	}
}

void Client::readIP( ) {
	int handle = 0;
	_ip = IPDATA( );
	handle = FileRead_open( "IP.ini" );
	if ( handle == 0 ) {
		handle = FileRead_open( "../IP.ini" );
	}
	FileRead_read( &_ip, sizeof( IPDATA ), handle );

	FileRead_close( handle );
}

void Client::connect( ) {
	_interval = ( _interval + 1 ) % CONNECT_INTERVAL;

	if ( _interval != 1 ) {
		return;
	}

	if ( _handle_tcp < 0 ) {
		int handle = ConnectNetWork( _ip, TCP_PORT );
		if ( handle < 0 ) {
			return;
		}
		_handle_tcp = handle;
		_connecting_tcp = true;
		_phase = CONNECTING;
	}

}

void Client::recving( ) {
	recvTcp( );
	recvUdp( );
	lost( );
	clearBuffer( );
}

void Client::recvTcp( ) {
	int data = GetNetWorkDataLength( _handle_tcp );
	if ( data < 1 ) {
		_recving_tcp = false;
		return;
	}

	int size = sizeof( bool ) + sizeof( int );
	char *buf;
	buf = ( char* )malloc( size );
	int recv = NetWorkRecv( _handle_tcp, buf, size );
	_matching = *( bool* )buf;

	_player_num = 0;
	if ( _matching ) {
		_player_num = *( int* )( buf + sizeof( bool ) );
	}

	free( buf );

	if ( recv < 0 ) {
		_recving_tcp = false;
		return;
	}
	_recving_tcp = true;
}

void Client::recvUdp( ) {
	_recving_udp = false;
	if ( _handle_udp < 0 ) {
		_handle_udp = MakeUDPSocket( UDP_PORT );
	}
	if ( CheckNetWorkRecvUDP( _handle_udp ) == TRUE ) {
		NetWorkRecvUDP( _handle_udp, NULL, NULL, &_recv_data_udp, sizeof( NetWorkData ), FALSE );
		_recving_udp = true;
	}
}

void Client::lost( ) {
	int lost = GetLostNetWork( );
	if ( lost < 0 ) {
		return;
	}
	if ( lost == _handle_tcp ) {
		NetWorkRecvBufferClear( _handle_tcp );
		CloseNetWork( _handle_tcp );
		_handle_tcp = -1;
		_phase = READY;
	}
	if ( lost == _handle_udp ) {
		NetWorkRecvBufferClear( _handle_udp );
		CloseNetWork( _handle_udp );
		_handle_udp = -1;
	}
}

void Client::clearBuffer( ) {
	NetWorkRecvBufferClear( _handle_udp );
}

bool Client::isConnectingTcp( ) const {
	return _connecting_tcp;
}

bool Client::isRecvingTcp( ) const {
	return _recving_tcp;
}

bool Client::isRecvingUdp( ) const {
	return _recving_udp;
}

void Client::sendTcp( ) {
	NetWorkSend( _handle_tcp, &_send_data_tcp, sizeof( NetWorkData ) );
}

std::string Client::getSeverIP( ) const {
	std::string ip = "";
	ip += std::to_string( _ip.d1 );
	ip += ".";
	ip += std::to_string( _ip.d2 );
	ip += ".";
	ip += std::to_string( _ip.d3 );
	ip += ".";
	ip += std::to_string( _ip.d4 );

	return ip;
}

std::string Client::getPhase( ) const {
	std::string phase = "";
	switch ( _phase ) {
	case READY		: phase = "READY"		; break;
	case CONNECTING	: phase = "CONNECTING"	; break;
	}

	return phase;
}

bool Client::isMatching( ) const {
	return _matching;
}

void Client::disConnect( ) {
	if ( _handle_tcp > 0 ) {
		if ( GetNetWorkDataLength( _handle_tcp ) > 0 ) {
			NetWorkRecvBufferClear( _handle_tcp );
		}
	}
	CloseNetWork( _handle_tcp );
	NetWorkRecvBufferClear( _handle_udp );
	DeleteUDPSocket( _handle_udp );
}

void Client::setOrder( int order ) {
	_send_data_tcp.order = ( unsigned char )order;
}

void Client::setPlayerPos( int pos ) {
	_send_data_tcp.player_pos[ _player_num ] = ( unsigned char )pos;
}

void Client::setCauseOfDeath( CAUSE_OF_DEATH cause ) {
	_send_data_tcp.cause_of_death = ( unsigned char )cause;
}

void Client::setItemFlag( bool flag ) {
	_send_data_tcp.item_flag = flag;
}

void Client::setItem( int item ) {
	_send_data_tcp.item = ( unsigned char )item;
}

void Client::setItemUser( ) {
	_send_data_tcp.item_user = ( unsigned char )_player_num;
}

void Client::setCtsPlayerNum( ) {
	_send_data_tcp.cts.player_num = ( unsigned char )_player_num;
}

void Client::setCtsFlag( bool flag ) {
	_send_data_tcp.cts.flag = flag;
}

void Client::setCtsX( int x ) {
	_send_data_tcp.cts.x = ( unsigned char )x;
}

void Client::setCtsY( int y ) {
	_send_data_tcp.cts.y = ( unsigned char )y;
}

void Client::setCtsAngle( MIRROR_ANGLE angle ) {
	_send_data_tcp.cts.angle = ( unsigned char )angle;
}

void Client::setFinish( bool fin ) {
	_send_data_tcp.fin = fin;
}

void Client::setAlive( bool alive ) {
	_send_data_tcp.alive = alive;
}

std::string Client::getClientIpStr( ) {
	IPDATA ip;
	GetMyIPAddress( &ip );

	std::string str = "";
	str += std::to_string( ip.d1 );
	str += ".";
	str += std::to_string( ip.d2 );
	str += ".";
	str += std::to_string( ip.d3 );
	str += ".";
	str += std::to_string( ip.d4 );

	return str;
}

int Client::getPlayerNum( ) const {
	if ( _player_num == ( unsigned char )-1 ) {
		return -1;
	}
	return _player_num;
}

int Client::getLazerPoint( ) const {
	if ( _recv_data_udp.lazer_pos == ( unsigned char )-1 ) {
		return -1;
	}
	return ( int )_recv_data_udp.lazer_pos;
}

int Client::getOrder( ) const {
	if ( _recv_data_udp.order == ( unsigned char )-1 ) {
		return -1;
	}
	return ( int )_recv_data_udp.order;
}

int Client::getPlayerPos( int idx ) const {
	if ( _recv_data_udp.player_pos[ idx ] == ( unsigned char )-1 ) {
		return -1;
	}
	return ( int )_recv_data_udp.player_pos[ idx ];
}

CAUSE_OF_DEATH Client::getEnemyCauseOfDeath( ) const {
	return ( CAUSE_OF_DEATH )_recv_data_udp.cause_of_death;
}

bool Client::isItemFlag( ) const {
	return _recv_data_udp.item_flag;
}

int Client::getItem( ) const {
	return ( int )_recv_data_udp.item;
}

int Client::getItemUser( ) const {
	return ( int )_recv_data_udp.item_user;
}

bool Client::getStcFlag( int idx ) const {
	return _recv_data_udp.stc[ idx ].flag;
}

int Client::getStcPlayerNum( int idx ) const {
	return ( int )_recv_data_udp.stc[ idx ].player_num;
}

int Client::getStcX( int idx ) const {
	return ( int )_recv_data_udp.stc[ idx ].x;
}

int Client::getStcY( int idx ) const {
	return ( int )_recv_data_udp.stc[ idx ].y;
}

MIRROR_ANGLE Client::getStcAngle( int idx ) const {
	return ( MIRROR_ANGLE )_recv_data_udp.stc[ idx ].angle;
}

BATTLE_PHASE Client::getBattlePhase( ) const {
	return ( BATTLE_PHASE )_recv_data_udp.phase;
}

int Client::getWinner( ) const {
	if ( _recv_data_udp.winner == ( unsigned char )-1 ) {
		return -1;
	}
	return ( int )_recv_data_udp.winner;
}