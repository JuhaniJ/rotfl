#include <QDebug>
#include <iostream>
#include <QString>
#include <sstream>
#include <string>
#include "roombaSerial.h"
#include "posixSerial.h"
#include "croiUtil.h"


namespace Croi {

RoombaSerial::RoombaSerial() :
    posixSerial_(new PosixSerial())
{
}

RoombaSerial::~RoombaSerial()
{
    if (posixSerial_  != nullptr) {
        delete posixSerial_;
    }
}

int RoombaSerial::rmb_connect(std::string ip)
{

    int rval = posixSerial_->initializePort();

    if(rval == Croi::CROI_ERROR) {
        return Croi::CROI_ERROR;
    }
    qDebug() << "rmb_connect";

    char buf = START;
    posixSerial_->writeSerial(&buf,1);

    return Croi::CROI_SUCCESS;

}

int RoombaSerial::disconnect()
{

}

void RoombaSerial::safeMode()
{
    char buf = SAFE;
    posixSerial_->writeSerial(&buf,1);
}

void RoombaSerial::fullMode()
{

}

void RoombaSerial::allMotorsOn()
{

}

void RoombaSerial::allMotorsOff()
{

}

void RoombaSerial::clean()
{
    char buf = CLEAN;
    posixSerial_->writeSerial(&buf,1);
    qDebug() << "clean";
//    posixSerial_->writeSerial(optCodeToStr(CLEAN));
}

char RoombaSerial::getTemperature()
{

}

unsigned short RoombaSerial::getChargeLevel()
{

}

short RoombaSerial::getDistance()
{

}

short RoombaSerial::getAngle()
{
    char command[2];

    command[0] = QUERY;
    command[1] = 2;

    posixSerial_->writeSerial(command,2);

    char recv[255];
    size_t retSize = 0;

    posixSerial_->readSerial(recv, retSize);


    int dist = ((short)recv[2] << 8 | (short)recv[3]);

    int angl = (recv[4] << 8 | recv[5]);

    std::cout << "paluuarvo " << retSize << "dist " << dist << "angl " << angl << std::endl;





}

int RoombaSerial::getRadius()
{

}

int RoombaSerial::getVelocity()
{

}

std::string RoombaSerial::optCodeToStr(int optCode)
{
    char cmd = static_cast<char>(optCode);
    std::stringstream stream;
    stream << std::hex << cmd;
    std::string command(stream.str());
    return command;

}

void RoombaSerial::Drive(int Velocity, int Radius)
{
    char command[5];

    command[0] = DRIVE;
    command[1] = ( Velocity >> 8 ) & 0x00FF;
    command[2] = Velocity & 0x00FF;

    command[3] = ( Radius >> 8 ) & 0x00FF;
    command[4] = Radius & 0x00FF;

    if(posixSerial_->writeSerial(command, 5) != CROI_SUCCESS){
    }
}

void RoombaSerial::playSong(int songNumber)
{

}

} //namespace Croi