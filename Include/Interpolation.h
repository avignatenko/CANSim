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

template <typename X, typename Y>
Y catmullSlope(X x[], Y y[], int n, int i)
{
    if (x[i + 1] == x[i - 1]) return 0;
    return (y[i + 1] - y[i - 1]) / (x[i + 1] - x[i - 1]);
}
template <typename X, typename Y>
Y catmullSplineInterpolate(X xValues[], Y yValues[], int numValues, double pointX, bool trim = true)
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
    auto t_2 = t * t;
    auto t_3 = t_2 * t;

    auto h00 = 2 * t_3 - 3 * t_2 + 1;
    auto h10 = t_3 - 2 * t_2 + t;
    auto h01 = 3 * t_2 - 2 * t_3;
    auto h11 = t_3 - t_2;

    auto x0 = xValues[i];
    auto x1 = xValues[i + 1];
    auto y0 = yValues[i];
    auto y1 = yValues[i + 1];

    Y m0;
    Y m1;
    if (i == 0)
    {
        m0 = (yValues[1] - yValues[0]) / (xValues[1] - xValues[0]);
        m1 = (yValues[2] - yValues[0]) / (xValues[2] - xValues[0]);
    }
    else if (i == numValues - 2)
    {
        m0 = (yValues[numValues - 1] - yValues[numValues - 3]) / (xValues[numValues - 1] - xValues[numValues - 3]);
        m1 = (yValues[numValues - 1] - yValues[numValues - 2]) / (xValues[numValues - 1] - xValues[numValues - 2]);
    }
    else
    {
        m0 = catmullSlope(xValues, yValues, numValues, i);
        m1 = catmullSlope(xValues, yValues, numValues, i + 1);
    }

    auto rst = h00 * y0 + h01 * y1 + h10 * (x1 - x0) * m0 + h11 * (x1 - x0) * m1;
    return rst;
}
