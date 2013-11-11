#ifndef MAPQGRAPHICSVIEW_H
#define MAPQGRAPHICSVIEW_H

#include <QGraphicsView>
#include "wallQGraphicsLineItem.h"
#include "uiUtils.h"
#include <QMouseEvent>
#include "croi/iRoomba.h"

//SOME OF THESE CONSTANTS ARE TO BE MOVED
//AND SOME CHANGED TO VARIABLES
const double POIWIDTH = 10.0;
const double TRACEWIDTH = 34.0;
const double ARROWWIDTH = 27.0;
const double PI = 3.14159265;
const double ANGLECORRECTION = 3.05;
const double DISTANCECORRECTION = 6.1;

class FleetManager;

class MapQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapQGraphicsView(FleetManager* fleetManager, QWidget* parent = NULL);
    //updates roomba's location and heading in map. Keeps a trace
    void updateLoc(QVector<Croi::IRoomba *>* roombas);
    //gives map's width in cm
    int giveMapWidth();
    //give new width in cm
    void setMapWidth(int width);
    void setSelectedPaintTool(Util::SelectedPaintTool tool);

    virtual ~MapQGraphicsView();

public slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    WallQGraphicsLineItem* wallToBeAdded_;
    QPointF* wallToBeAddedStartPoint_;
    int mapWidth_;
    bool traceShown_;
    Util::SelectedPaintTool selectedPaintTool_;
    FleetManager* fleetManager_;
};

#endif // MAPQGRAPHICSVIEW_H
