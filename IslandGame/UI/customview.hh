#ifndef CUSTOMVIEW_HH
#define CUSTOMVIEW_HH
#include <QGraphicsView>
#include <QWidget>
#include <QWheelEvent>

namespace Student {


class CustomView : public QGraphicsView
{
public:
    explicit CustomView(QWidget *parent);

protected:
    virtual void wheelEvent(QWheelEvent *event);

private:
    int totalZoom_;
};
}
#endif // CUSTOMVIEW_HH
