#include "LoadCSV.h"
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <errno.h>

const std::string DIR = "Resources/csv/";

LoadCSV::LoadCSV( ) {
}

LoadCSV::~LoadCSV( ) {
}

void LoadCSV::read( std::vector< CsvData > &list, std::string path ) {
	FILE *fp;
	errno_t read_csv;
	read_csv = fopen_s( &fp, ( DIR + path + ".csv" ).c_str( ), "rb" );

	assert( !read_csv );

	{//�f�[�^�����
		char c = '\0';
		int knd = 0;
		CsvData data;
		memset( &data, 0, sizeof( CsvData ) );
		while ( true ) {
			c = fgetc( fp );

			//�I�[�A�܂��͍s��
			if ( c == '\r' ) {
				continue;
			}

			if ( c == EOF || c == '\n' ) {
				data.value += '\0';
				list.push_back( data );
				memset( &data, 0, sizeof( CsvData ) );

				if ( c == EOF ) {
					break;
				} 
				if ( c == '\n' ) {
					knd = 0;
					continue;
				}
				if ( c == '\r' ) {
					knd = 0;
					continue;
				}
			}

			//�J���}�Ŏ��̃����o��
			if ( c == ',' ) {
				data.tag += '\0';
				knd += 1;
				continue;
			}

			//���������
			switch ( knd ) {
			case 0: data.tag += c; break;
			case 1: data.value += c; break;
			default : continue;
			}
		}
	}
	fclose( fp );
}