#pragma once

template <typename X, typename Y>
Y cubicInterpolate(X xValues[], Y yValues[], int numValues, X pointX, bool trim = true)
{
    if (trim)
    {
        if (pointX <= xValues[0]) return yValues[0];
        if (pointX >= xValues[numValues - 1]) return yValues[numValues - 1];
    }

    auto i = 0;
    if (pointX <= xValues[0])
        i = 0;
    else if (pointX >= xValues[numValues - 1])
        i = numValues - 1;
    else
        while (pointX >= xValues[i + 1]) i++;
    if (pointX == xValues[i + 1]) return yValues[i + 1];

    auto t = (pointX - xValues[i]) / (xValues[i + 1] - xValues[i]);
    t = t * t * (3 - 2 * t);
    return yValues[i] * (1 - t) + yValues[i + 1] * t;
}
