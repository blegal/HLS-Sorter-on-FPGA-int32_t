#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <paths.h>
#include <termios.h>
#include <sysexits.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <cassert>
#include <iostream>


using namespace std;

void write_int(const int fileID, const int value)
{
	int wBytes = write( fileID, &value, sizeof(int) );
	assert( wBytes == (sizeof(int)) );
}

int read_int(const int fileID)
{
	int value;
    int rBytes = read( fileID, &value, sizeof(int) );
    assert( rBytes == (sizeof(int)) );
	return value;
}

int main (int argc, char * argv []){

	cout << "(II) Initialisation des composants..." << endl;

    int fileID = -1;
    
    fileID = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY );
    if(fileID == -1)
    {
        printf("Impossible d'ouvrir ttyUSB1 !\n");
        return -1;
    }

    struct termios t;
    tcgetattr(fileID, &t); // recupère les attributs
    cfmakeraw(&t); // Reset les attributs
    t.c_cflag     = CREAD | CLOCAL;     // turn on READ
    t.c_cflag    |= CS8;
    t.c_cc[VMIN]  = 0;
    t.c_cc[VTIME] = 255;     // 5 sec timeout

    cfsetispeed(&t, B921600);
    cfsetospeed(&t, B921600);
    tcsetattr(fileID, TCSAFLUSH, &t); // envoie le tout au driver    

	int32_t table[128];
	for(uint8_t x = 0; x < 128; x += 1)
	{  
        table[x] = rand()%65536 - 32768;
    }
	for(uint8_t x = 0; x < 128; x += 1)
	{  
        if( x%16 == 0 ) printf("\n");
        printf("%6d ", table[x]);
    }
    printf("\n");
    printf("\n");

#if 0
	for(uint8_t x = 0; x < 128; x += 1)
	{  
        int wBytes = write( fileID, table + x, sizeof(int32_t) );
        assert( wBytes == (sizeof(int32_t)) );
    }
#else
    write( fileID, table, 128 * sizeof(int32_t) );
#endif

   	int32_t results[128];
#if 0
	for(uint8_t x = 0; x < 128; x += 1)
	{  
        int rBytes = read( fileID, results + x, sizeof(int32_t) );
        printf("R%3d : %d bytes\n", x, rBytes);
//        assert( rBytes == (sizeof(int32_t)) );
    }
#else
    int rdata  = 0;
    int length = 128 * sizeof(int32_t);
    while( rdata != length )
    {
        int rBytes = read( fileID, ((int8_t*)results) + rdata, length - rdata );
        rdata += rBytes;
    }
#endif

    
    for(uint8_t x = 0; x < 128; x += 1)
	{  
        if( x%16 == 0 ) printf("\n");
        printf("%6d ", results[x]);
    }
    printf("\n");
    printf("\n");

    close( fileID );
    
    return 0;
}
