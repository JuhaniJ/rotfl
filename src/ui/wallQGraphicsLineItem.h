#ifndef WALLQGRAPHICSLINEITEM_H
#define WALLQGRAPHICSLINEITEM_H

#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

class FleetManager;

class WallQGraphicsLineItem:
        public QGraphicsLineItem
{
public:
    explicit WallQGraphicsLineItem
    (FleetManager *fleetManager, qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = 0);

    virtual ~WallQGraphicsLineItem();
    int type() const;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    FleetManager* fleetManager_;
};

#endif // WALLQGRAPHICSLINEITEM_H
