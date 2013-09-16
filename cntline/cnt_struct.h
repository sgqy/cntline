
#pragma once

struct cnt_struct {
    cnt_struct()
        : lines(0)
        , fsiz(0)
    {}
    long long lines;
    long long fsiz;
    cnt_struct& operator+= (const cnt_struct& lhs)
    {
        lines += lhs.lines;
        fsiz += lhs.fsiz;
        return *this;
    }
};
