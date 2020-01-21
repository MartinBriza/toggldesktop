#include "settings.h"

Settings::Settings()
{

}

int64_t Settings::WindowX() const {
    return window_x_;
}

void Settings::SetWindowX(int64_t value) {
    window_x_ = value;
}

int64_t Settings::WindowY() const {
    return window_y_;
}

void Settings::SetWindowY(int64_t value) {
    window_y_ = value;
}

int64_t Settings::WindowHeight() const {
    return window_height_;
}

void Settings::SetWindowHeight(int64_t value) {
    window_height_ = value;
}

int64_t Settings::WindowWidth() const {
    return window_width_;
}

void Settings::SetWindowWidth(int64_t value) {
    window_width_ = value;
}
