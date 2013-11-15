#include "mainwindow.h"
#include "fleetManager.h"
#include "croi/roombaRoowifi.h"
#include <QDebug>
#include <QObject>
#include <QMessageBox>

FleetManager::FleetManager(MainWindow* mainWindow, QObject *parent):
    QObject(parent), mainWindow_(mainWindow), selectedRoomba_(NULL), map_(NULL)
{
    updateTimer_ = new QTimer(this);
    QObject::connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateTimerTimeout()));
    updateTimer_->setSingleShot(false);
    updateTimer_->start(500);
}


//map_ cannot be set in constructor because map_ is constructed after
//FleetManager
void FleetManager::setMap(MapQGraphicsView* map)
{
    map_ = map;
}

void FleetManager::updateTimerTimeout()
{
    //qDebug() << "updateTimerTimeout";
    if(roombas_.empty()) //in case timer is on and there is no roomba
    {
        return;
    }

    mainWindow_->setRoombaStatusData(selectedRoomba_);

    //rest of the function is selection logic upon cursor select.
    //Always makes selectedRoomba_'s starting point and roomba-icon selected
    //if either one is selected by cursor but does nothing if nothing is
    //selected. changes selectedRoomba_ if it isn't selected and another roomba
    //is selected (either start point or roomba).
    if(selectedRoomba_->getPolygon()->isSelected())
    {
       selectedRoomba_->getStartPoint()->setSelected(true);
    }
    else if(selectedRoomba_->getStartPoint()->isSelected())
    {
        selectedRoomba_->getPolygon()->setSelected(true);
    }
    else //neither is selected by cursor
    {
        for (int i = 0; i < roombas_.size(); ++i)
        {
            //selected roomba found
            if(roombas_.at(i)->getPolygon()->isSelected() ||
               roombas_.at(i)->getStartPoint()->isSelected())
            {
                selectedRoomba_ = roombas_.at(i);
                //both the roomba and it's startPoint are selected for clarity
                selectedRoomba_->getPolygon()->setSelected(true);
                selectedRoomba_->getStartPoint()->setSelected(true);
                //
                break;
            }
        }
    }

    //unselects other roombas that are selected by cursor
    for (int i = 0; i < roombas_.size(); ++i)
    {
        if(roombas_.at(i) != selectedRoomba_)
        {
            roombas_.at(i)->getPolygon()->setSelected(false);
            roombas_.at(i)->getStartPoint()->setSelected(false);
        }
    }
}

void FleetManager::createRoomba(PoiQGraphicsEllipseItem *startPoint)
{
    selectedRoomba_ = new Croi::RoombaRoowifi(startPoint, map_, this);
    //selectedRoomba_ = new Croi::RoombaSerial();
    //TODO: selectedRoomba_ = new Croi::RoombaVirtual();
    roombas_.append(selectedRoomba_);
}

void FleetManager::addPoi(PoiQGraphicsEllipseItem* poi)
{
    pois_.insert(poi);
}

void FleetManager::addWall(WallQGraphicsLineItem* wall)
{
    walls_.insert(wall);
}

void FleetManager::removeRedObjects()
{
    for (std::set<PoiQGraphicsEllipseItem*>::iterator i = pois_.begin();
         i != pois_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            map_->scene()->removeItem(*i);
            delete *i;
            pois_.erase(i);
        }
    }
    for (std::set<WallQGraphicsLineItem*>::iterator i = walls_.begin();
         i != walls_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            map_->scene()->removeItem(*i);
            delete *i;
            walls_.erase(i);
        }
    }

    //THE STARTING POINT CAN'T BE REMOVED NOW (TEMPORARY CHANGE)
    //all tracking and tracing is taken away if startPoint_ is removed
    //    if (startPoint_ != NULL && startPoint_->pen().color() == Qt::GlobalColor::red)
    //    {
    //        delete startPoint_;
    //        startPoint_ = NULL;
    //        removeTraces();
    //        scene()->removeItem(curPoint_);
    //        delete curPoint_;
    //        curPoint_ = NULL;
    //        scene()->removeItem(curSpeed_);
    //        delete curSpeed_;
    //        curSpeed_ = NULL;
    //        initX_= 0.0;
    //        initY_= 0.0;

    //    }
}

void FleetManager::removePoi(PoiQGraphicsEllipseItem* poi)
{
    map_->scene()->removeItem(poi);
    pois_.erase(poi);
    delete poi;
}

//shows or hides all traces
void FleetManager::ifShowTraces()
{
    for (int i = 0; i < roombas_.size(); ++i)
    {
        roombas_.at(i)->ifShowTraces();
    }
}

//removes selectedRoomba's traces
void FleetManager::removeTraces()
{
    selectedRoomba_->removeTraces();
}

void FleetManager::go2Poi()
{
    QPointF poiCoordinate = (*pois_.begin())->pos();
    QPointF roombaCoordinate = selectedRoomba_->getLoc();
    qDebug() << "POI coordinate x: " << poiCoordinate.x()
             << " , y: " << poiCoordinate.y();
    qDebug() << "Roomba coordinate x: " << roombaCoordinate.x()
             << " , y: " << roombaCoordinate.y();

    //stop
//    selectedRoomba_->drive(0,32767);
//    radius_ = 32767;
//    moving_ = false;

    //calculate the angle

    float deltaX = poiCoordinate.x()-roombaCoordinate.x();
    float deltaY = roombaCoordinate.y() - poiCoordinate.y();
    //float angleRadian = atan2(deltaY, deltaX);
    //float anglePi = angleRadian*180 / PI;

    qDebug() << "Delta X: " << deltaX;
    qDebug() << "Delta Y: " << deltaY;

    float calculatedAngle = 0;

    if (deltaX == 0 && deltaY == 0)
    {
        qDebug() << "Roomba is at POI";
        return;
    }
    else
    {
        calculatedAngle = -atan2(deltaY, deltaX);
    }

    qDebug() << "Calculated angle in degrees: " << calculatedAngle*(180/PI);

    float turningAngle = 0.0;
    float roombasAngle = selectedRoomba_->getCurrentAngle(); //0 - 2PI

    if (roombasAngle > PI)
    {
        roombasAngle -= 2*PI;
    }
    qDebug() << "Roombas angle in degrees: " << roombasAngle*(180/PI);

    //both roombasAngle and calculatedAngle are between -PI and PI
    if (roombasAngle >= 0)
    {
        if (calculatedAngle >= 0)
        {
            if (calculatedAngle > roombasAngle)
            {
                turningAngle = calculatedAngle - roombasAngle;
            }
            else
            {
                turningAngle = calculatedAngle - roombasAngle;
            }

        }
        else //calculatedAngle < 0
        {
            if (calculatedAngle -roombasAngle < (-1*PI) )
            {
                turningAngle = calculatedAngle - roombasAngle + 2*PI;
            }
            else
            {
                turningAngle = calculatedAngle - roombasAngle;
            }
        }
    }
    else //roombasAngle < 0
    {
        if (calculatedAngle >= 0)
        {
            if ( calculatedAngle - roombasAngle > PI )
            {
                turningAngle = calculatedAngle - roombasAngle - 2*PI;
            }
            else
            {
                turningAngle = calculatedAngle - roombasAngle;
            }

        }
        else //calculatedAngle <0
        {
            if ( calculatedAngle > roombasAngle )
            {
                turningAngle = calculatedAngle - roombasAngle;
            }
            else
            {
                turningAngle = calculatedAngle - roombasAngle;
            }

        }
    }

    int turningSpeed = 50;
    float turningTime = 0;


    qDebug() << "Turning angle in degrees: " << turningAngle*(180/PI);

    if (turningAngle < 0) //Turn counter-clockwise
    {
        turningTime = -(((PI*TRACEWIDTH*10)/turningSpeed)/(2*PI))*turningAngle;
    }
    else //Turn clockwise
    {
        turningTime = (((PI*TRACEWIDTH*10)/turningSpeed)/(2*PI))*turningAngle;
    }



/*    qDebug() << "Turning time: " << turningTime;



//    selectedRoomba_->drive(-100,65535);
//    radius_ = 65535;
//    moving_ = true;

    short dRoombaAngle = 0;

    QTime dieTime = QTime::currentTime().addSecs(5);
//    while( true/*QTime::currentTime() < dieTime ) {

//        short ang = iRoomba_->getAngle();
//        if (ang != 0)
//            qDebug() << "angle: " << ang << "Rimpsu: " << (-3.05)*turningAngle*(180/PI) <<"\n";
//        dRoombaAngle += ang;

//        qDebug() << "Roomba sensor read angle " << dRoombaAngle << "Rimpsu: " << (-3.05)*turningAngle*(180/PI) <<"\n";

//        if(dRoombaAngle > (-3.05)*turningAngle*(180/PI)) break;

//        QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
//    }*/



    if (turningAngle > 0)
    {
        selectedRoomba_->drive(100,RADTURNCW);
    }
    else
    {
        selectedRoomba_->drive(100,RADTURNCCW);
    }

    QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
    float tabs = abs(turningAngle*(180/PI));
    useconds_t ttime = tabs * 18055;
    // Roomba turns 1 degree in 18055 microseconds, when speed is 100
    qDebug() << "turningAngle: " << turningAngle << "tabs: " << tabs << "ttime: " << ttime  << "\ns";
    usleep(ttime);
    selectedRoomba_->drive(0,32767);
//    selectedRoomba_->Drive(0,32767);
//    radius_ = 32767;
//    moving_ = false;



}

void FleetManager::checkPoiCollision()
{
    for (std::set<PoiQGraphicsEllipseItem*>::iterator i = pois_.begin();
         i != pois_.end(); ++i)
    {
        QList<QGraphicsItem*> collidingItems = (*i)->collidingItems();
        if (!collidingItems.empty())
        {
            removePoi(*i);
        }
        //TODO: check issue #3 on Github, implementation is missing
        //bool isTrace = false;
        //goes through collidingItems and removes POI if it
        //finds an item that isn't a trace
        //while (!collidingItems.empty())
        //{
        //  collidingItems.pop_front();
        //}
    }
}

void FleetManager::connect(std::string stdip)
{
    if (isRoombaSelected())
    {
        //    Disabled until Roowifi AutoCapture is used instead
        //    updateSensorData_->start(500);
        selectedRoomba_->rmb_connect(stdip);
    }
}

void FleetManager::disconnect()
{
    if (isRoombaSelected())
    {
        //    Disabled until Roowifi AutoCapture is used instead
        //    updateSensorData_->stop();
        selectedRoomba_->disconnect();
    }
}

void FleetManager::clean()
{
    if (isRoombaSelected())
    {
        selectedRoomba_->clean();
    }
}

void FleetManager::allMotorsOn()
{
    if (isRoombaSelected())
    {
        selectedRoomba_->allMotorsOn();
    }
}

void FleetManager::allMotorsOff()
{
    if (isRoombaSelected())
    {
        selectedRoomba_->allMotorsOff();
    }
}

void FleetManager::safeMode()
{
    if (isRoombaSelected())
    {
        selectedRoomba_->safeMode();
    }
}

void FleetManager::fullMode()
{
    if (isRoombaSelected())
    {
        selectedRoomba_->fullMode();
    }
}

void FleetManager::resetAngle()
{
    if (isRoombaSelected())
    {
        selectedRoomba_->resetAngle();
    }
}

void FleetManager::playSong(unsigned int songNum)
{
    if (isRoombaSelected())
    {
        selectedRoomba_->playSong(songNum);
    }
}

void FleetManager::setVelocity(int velocity)
{
    if (isRoombaSelected())
    {
        if (selectedRoomba_->getVelocity() != 0)
        {
            selectedRoomba_->drive(velocity);
        }
    }
}

void FleetManager::drive( int velocity, int radius )
{
    if (isRoombaSelected())
    {
        selectedRoomba_->drive(velocity, radius);
    }
}

void FleetManager::drive( int velocity)
{
    if (isRoombaSelected())
    {
        selectedRoomba_->drive(velocity);
    }
}

bool FleetManager::isRoombaSelected()
{
    if ( selectedRoomba_ == NULL)
    {
        MainWindow* mainwindow = qobject_cast<MainWindow*>(parent());
        QMessageBox::warning(mainwindow, "", tr("Please select a Roomba!"));
        return false;
    }
    else
    {
        return true;
    }
}

FleetManager::~FleetManager()
{
    for (int i = 0; i < roombas_.size(); ++i)
    {
        roombas_[i]->disconnect();
    }
}
