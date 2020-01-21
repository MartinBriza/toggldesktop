#ifndef SETTINGS_H
#define SETTINGS_H

#include <cstdint>

class Settings
{
public:
    Settings();

    int64_t WindowX() const;
    void SetWindowX(int64_t value);
    int64_t WindowY() const;
    void SetWindowY(int64_t value);
    int64_t WindowHeight() const;
    void SetWindowHeight(int64_t value);
    int64_t WindowWidth() const;
    void SetWindowWidth(int64_t value);

private:
    int64_t window_x_ { 100 };
    int64_t window_y_ { 100 };
    int64_t window_height_ { 300 };
    int64_t window_width_ { 100 };
};

#endif // SETTINGS_H
