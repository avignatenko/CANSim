#pragma once
#include "Common.h"

#include <EEPROM.h>

#include <ArduinoSTL.h>

#include <vector>

class StoredLUT : public Printable
{
public:
    StoredLUT(byte maxSize)
    {
        xValues_.reserve(maxSize);
        yValues_.reserve(maxSize);
    }

    ~StoredLUT() {}

    double* x() { return &xValues_[0]; }
    double* y() { return &yValues_[0]; }

    size_t size() { return xValues_.size(); }
    size_t maxSize() const { return xValues_.capacity(); }

    void clear()
    {
        xValues_.clear();
        yValues_.clear();
    }

    bool removeValue(double x)
    {
        // find spot
        int idx = xValues_.size();
        for (int i = 0; i < (int)xValues_.size(); ++i)
        {
            if (xValues_[i] == x)
            {
                idx = i;
                break;
            }
        }
        if (idx == (int)xValues_.size()) return false;

        xValues_.erase(xValues_.begin() + idx);
        return true;
    }

    void addValue(double x, int y)
    {
        // find spot
        int idx = xValues_.size();
        for (int i = 0; i < (int)xValues_.size(); ++i)
        {
            if (xValues_[i] == x)
            {
                yValues_[i] = y;
                return;
            }
            if (xValues_[i] > x)
            {
                idx = i;
                break;
            }
        }

        xValues_.push_back(xValues_.back());
        yValues_.push_back(yValues_.back());

        for (int j = xValues_.size() - 2; j >= idx + 1; --j)
        {
            xValues_[j] = xValues_[j - 1];
            yValues_[j] = yValues_[j - 1];
        }

        xValues_[idx] = x;
        yValues_[idx] = y;
    }

    size_t printTo(Print& p) const override
    {
        size_t r = 0;

        for (int i = 0; i < (int)xValues_.size(); ++i)
        {
            if (i > 0) r += p.println();
            r += p.print(xValues_[i]);
            r += p.print(' ');
            r += p.print(yValues_[i]);
        }
        return r;
    }

    bool load(int eepromIdx)
    {
        int idx = eepromIdx;

        byte size = 255;
        EEPROM.get(idx, size);
        if (size == 255 || size == 0) return false;

        xValues_.clear();
        yValues_.clear();

        idx += sizeof(size);
        double value = 0;
        for (int i = 0; i < size; ++i)
        {
            EEPROM.get(idx, value);
            idx += sizeof(value);
            xValues_.push_back(value);

            EEPROM.get(idx, value);
            idx += sizeof(value);
            yValues_.push_back(value);
        }

        return true;
    }

    void save(int eepromIdx)
    {
        int idx = eepromIdx;
        byte size = xValues_.size();
        EEPROM.put(idx, size);
        idx += sizeof(size);
        for (int i = 0; i < size; ++i)
        {
            EEPROM.put(idx, xValues_[i]);
            idx += sizeof(xValues_[i]);

            EEPROM.put(idx, yValues_[i]);
            idx += sizeof(yValues_[i]);
        }
    }

private:
    std::vector<double> xValues_;
    std::vector<double> yValues_;
};
