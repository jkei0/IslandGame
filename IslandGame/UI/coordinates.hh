#ifndef COORDINATES_HH
#define COORDINATES_HH
#include "cubecoordinate.hh"
#include <vector>
#include <QPointF>
#include <QPolygonF>
#include <QPoint>

namespace Student {


struct Orientation {
    const double f0, f1, f2 ,f3;
    const double b0, b1, b2, b3;
    const double start_angle;
    Orientation(double f0_, double f1_, double f2_, double f3_,
                double b0_, double b1_, double b2_, double b3_,
                double start_angle_)
        : f0(f0_), f1(f1_), f2(f2_), f3(f3_),
          b0(b0_), b1(b1_), b2(b2_), b3(b3_),
          start_angle(start_angle_) {}
};

struct Layout
{
    const Orientation orientation;
    const QPointF size;
    const QPointF origin;
    Layout(Orientation orientation_, QPointF size_, QPointF origin_)
        : orientation(orientation_), size(size_), origin(origin_) {}
};

struct FractionalHex {
    const double x, y, z;
    FractionalHex(double x_, double y_, double z_)
        : x(x_), y(y_), z(z_) {}
};

Layout getLayout(); // Funktiossa määritellään hexojen asento, koko ja pelilaudan keskipiste
Orientation layout_pointy(); // tässä työssä käytetty layoutti
Orientation layout_flat();
QPointF hex_to_pixel(Layout layout, Common::CubeCoordinate h); // Cubekoordinaatit -> QKoordinaatit muunnos
QPointF hex_corner_offset(Layout layout, int corner);
QPolygonF polygon_corners(Layout layout, Common::CubeCoordinate h);
Common::CubeCoordinate round_hex(FractionalHex h);
Common::CubeCoordinate pixel_to_hex(Layout layout, QPointF p);

}

#endif // COORDINATES_HH
