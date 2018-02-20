#pragma once
#include <string>

class Base {
public:
	Base( );
	virtual ~Base( );

	virtual void setFlag( int flag );
	virtual int getFlag( ) const;
	virtual void setSoundFlag( int flag );
	virtual int getSoundFlag( ) const;
	virtual void update( ) = 0;
	virtual void initialize( );
	virtual std::string getTag( ) = 0;

private:
	int _flag;
	int _soundflag;

public:
	struct ImageProperty {
		int flag;
		double cx;	//���S
		double cy;
		double lx;	//����
		double ly;
		double rx;	//�E��
		double ry;
		double r;	//�����蔻��
		double size = 1;//�摜�g�嗦
		double angle;
		int knd;
		int brt = 255;
		int cnt;
		int png;
	};
	struct LightImageProperty {
		double cx;
		double cy;
		int png;
	};
	struct SoundProperty {
		int soundflag;
		bool isLoop;
		bool top;
		int volume = -1;
		int wav;
	};
};