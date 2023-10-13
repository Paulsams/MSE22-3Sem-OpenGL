#pragma once

#include <QHBoxLayout>
#include <QWidget>
#include <QSlider>

class DebugWindow : private QWidget {
public:
    DebugWindow(QWidget* parent = nullptr);

    QSlider *addSlider(const std::string &title, int startValue, int min, int max);

private:
    QHBoxLayout* container_;
    const QSize size_;
};
