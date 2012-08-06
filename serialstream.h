/*
 * serialstream.h
 *
 *  Created on: 2012-07-27
 *      Author: edmund
 */

#ifndef SERIALSTREAM_H_
#define SERIALSTREAM_H_

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>

using namespace std;


namespace ardroid {
class serial_nix;
struct termios config;

class serial_nix {
	int fd1;
	const char *path;
	char *buff;//,*buffer,*bufptr;
	int wr,rd;//,nbytes,tries;
	void initConfig();
public:
	serial_nix();
    serial_nix(const char *, bool);
    bool openport(const char *);
    bool writechar(char);
    bool readchar(char*);
    bool writeline(const char *);
    void closeport();
    const char *getline();
};

void serial_nix::initConfig() {
	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
	                    INLCR | PARMRK | INPCK | ISTRIP | IXON);
	config.c_oflag = 0;
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= CS8;
	config.c_cc[VMIN]  = 1;
	config.c_cc[VTIME] = 0;

	if(cfsetispeed(&config, B9600) < 0 | cfsetospeed(&config, B9600) < 0) {
	   //TODO: Baud rate and config error handling.
	}
	if(tcsetattr(fd1, TCSAFLUSH, &config) < 0) {
		//TODO: See above.
	}
}


serial_nix::serial_nix(const char * location, bool open) {
	path = location;
	fd1 = -1;
	wr = 0;
	rd = 0;
	buff = new char;
	if (open) openport(NULL);
}

serial_nix::serial_nix() {
	fd1 = 0;
	path = 0;
	wr = 0;
	buff = new char;
	rd = 0;
}

bool serial_nix::openport(const char * location) {
	if (!location && path) location = path;
	fd1 = open(location, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd1 < 0) {
	cerr << "Could not open port " << location << endl;
	return false;
	}
	fcntl(fd1, F_SETFL,0);

	cout << "Port " << string(location) << "has been successfully opened and " <<
			fd1 << "is the file descriptor." << endl;
	initConfig();

	return true;
}

const char *serial_nix::getline() {
char nextCh = 0;
string retStr;
while (nextCh != '\n') {
	readchar(&nextCh);
	retStr += nextCh;
}
return retStr.c_str();
}

bool serial_nix::writechar(char ch) {
	char ch1 = ch;
	wr = write(fd1,&ch1,1);
	return !!wr;
}

bool serial_nix::writeline(const char *str) {
	wr = 1;
	for (const char* ptr = str; ptr++;) {
	wr &= write(fd1, ptr, 1);
}
	char newline = '\n';
	wr &= write(fd1, &newline, 1);
	return !!wr;

}

bool serial_nix::readchar(char* ch) {
	rd = read(fd1, ch, 1);
	return !!rd;
}

void serial_nix::closeport() {
	close(fd1);
	delete buff;
}

serial_nix& operator<<(serial_nix& serial, char& ch) {
	serial.writechar(ch);
	return serial;
}

serial_nix& operator<<(serial_nix& serial, const char* str) {
	serial.writeline(str);
	return serial;
}
serial_nix& operator>>(serial_nix& serial, const char* str) {
	str = serial.getline();
	return serial;
}
serial_nix& operator<<(serial_nix& serial, string str) {
	return serial << str.c_str();
}
serial_nix& operator>>(serial_nix& serial, string& str) {
	str = string(serial.getline());
	return serial;
}
serial_nix& operator>>(serial_nix& serial, char& ch) {
	serial.readchar(&ch);
	return serial;
}
}





#endif /* SERIALSTREAM_H_ */
