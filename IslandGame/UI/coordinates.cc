#include "coordinates.hh"
#include <cmath>

namespace Student {

QPointF const HEX_SIZE = QPointF(25, 25);
QPointF const ORIGIN = QPointF(250, 200);

Orientation layout_pointy()
{
    return Orientation(sqrt(3.0), sqrt(3.0)/2.0, 0.0, 3.0/2.0,
                sqrt(3.0)/3.0, -1.0/3.0, 0.0, 2.0/3.0,
                0.5);
}



Orientation layout_flat()
{
    return Orientation(3.0/2.0, 0.0, sqrt(3.0)/2.0, sqrt(3.0),
                       2.0/3.0, 0.0, -1.0/3.0, sqrt(3.0)/3.0,
                       0.0);
}


// Funktio palauttaa hexagonin keskipisteen koordinaatit QKoordinaatteina
QPointF hex_to_pixel(Layout layout, Common::CubeCoordinate h)
{
    const Orientation& M = layout.orientation;
    double x = ((M.f0 * h.x) + (M.f1 * h.y)) * layout.size.x();
    double y = ((M.f2 * h.x) + (M.f3 * h.y)) * layout.size.y();
    return QPointF(x + layout.origin.x(), y + layout.origin.y());
}


QPointF hex_corner_offset(Layout layout, int corner)
{
    QPointF size = layout.size;
    double angle = 2.0 * M_PI*
            (layout.orientation.start_angle + corner) / 6;
    return QPointF(size.x() * cos(angle), size.y() * sin(angle));
}

// Funktio laskee hexagonin kärkien koordinaatit ja palauttaa ne
// QPolygonF vektorissa
QPolygonF polygon_corners(Layout layout, Common::CubeCoordinate h)
{
    QPolygonF corners;
    int kulmienMaara = 6;
    QPointF center = hex_to_pixel(layout, h);
    for (int i = 0; i<kulmienMaara; i++) {
        QPointF offset = hex_corner_offset(layout, i);
        corners.push_back(QPointF(center.x() + offset.x(),
                                center.y() + offset.y()));
    }
    return corners;
}


// Funktio määrittelee hexagonien koon ja asennon sekä pelilaudan sijainnin
Layout getLayout()
{
    // Hexagonien asento
    Orientation orientation = layout_pointy();

    // Hexan koko
    QPointF size = HEX_SIZE;

    // Pelilaudan keskipiste
    QPointF origin = ORIGIN;

    Layout layout = Layout(orientation, size, origin);
    return layout;
}

Common::CubeCoordinate round_hex(FractionalHex h)
{
    int x = int(round(h.x));
    int y = int(round(h.y));
    int z = int(round(h.z));
    double x_diff = abs(x - h.x);
    double y_diff = abs(y - h.y);
    double z_diff = abs(z - h.z);
    if (x_diff > y_diff && x_diff > z_diff) {
        x = -y-z;
    }
    else if ( y_diff > z_diff ) {
        y = -x-z;
    }
    else {
        z = -x-y;
    }
    return Common::CubeCoordinate(x,y,z);
}

Common::CubeCoordinate pixel_to_hex(Layout layout, QPointF p)
{
    const Orientation& M = layout.orientation;
    QPointF pt = QPointF((p.x() - layout.origin.x()) / layout.size.x(),
                        (p.y() - layout.origin.y()) / layout.size.y());
    double x = M.b0 * pt.x() + M.b1 * pt.y();
    double y = M.b2 * pt.x() + M.b3 * pt.y();
    Common::CubeCoordinate coords = round_hex(FractionalHex(x,y,-x-y));
    return coords;
}

}
