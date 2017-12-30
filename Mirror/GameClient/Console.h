#include "Base.h"
#include "smart_ptr.h"
#include "Client.h"

PTR( Console );
PTR( GlobalData );
PTR( Client );
PTR( Drawer );

class Console : public Base {
public:
	Console( GlobalDataPtr data );
	virtual ~Console( );

public:
	std::string getTag( );
	void initialize( );
	void update( );

private:
	void ready( );
	void connecting( );

private:
	Client::NetWorkData _recv_data_tcp;
	Client::NetWorkData _recv_data_udp;
	Client::NetWorkData _send_data;

	GlobalDataPtr _data;
	ClientPtr _client;
	DrawerPtr _drawer;
};