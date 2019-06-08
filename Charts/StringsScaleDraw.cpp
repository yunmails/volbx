#include <cmath>

#include "StringsScaleDraw.h"

StringsScaleDraw::StringsScaleDraw(QVector<QString>* intervals) :
    intervals_(intervals)
{
    setLabelRotation(-50.0);
    setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
}

QwtText StringsScaleDraw::label(double v) const
{
    if (intervals_->isEmpty() || fmod(v, 1) || !v || v < 0 ||
        v > intervals_->count())
    {
        return QwtText(QStringLiteral("                     "));
    }

    int point = static_cast<int>(v);
    if (intervals_->count() >= point)
    {
        return QwtText(intervals_->at(point - 1));
    }

    return QwtText(QStringLiteral("                     "));
}
