#include "customview.hh"

namespace Student {

CustomView::CustomView(QWidget *parent)
{
    totalZoom_ = 0;
}

void CustomView::wheelEvent(QWheelEvent *event)
{

    int max_zoom = 25;
    int min_zoom = -10;
    QPointF suunta = mapToScene(viewport()->rect().center()) -
            event->pos();
    if (event->angleDelta().y() > 0 && totalZoom_ <= max_zoom) {
        scale(1.05,1.05);
        centerOn(this->mapToScene(viewport()->rect().center()) - 0.05 * suunta);
        ++totalZoom_;

    }

    else if(event->angleDelta().y() < 0 && totalZoom_ >= min_zoom) {
        scale(1/1.05,1/1.05);
        --totalZoom_;
    }
    else{
    }
}
}
