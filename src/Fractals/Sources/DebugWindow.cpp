//
// Created by pasha on 13.10.2023.
//

#include <QLabel>
#include <QLineEdit>
#include <QRegExpValidator>
#include "Fractals/Include/DebugWindow.h"

DebugWindow::DebugWindow(QWidget* parent)
    : QWidget(parent)
    , container_(new QHBoxLayout(this)) {
    setGeometry(10, 10, 0, 0);
    setStyleSheet("background-color: rgba(155, 155, 155, 200);");
    container_->setSizeConstraint(QLayout::SetFixedSize);
    container_->setAlignment(Qt::AlignTop);
}

QSlider * DebugWindow::addSlider(const std::string& title, int startValue, int min, int max) {
    auto layout = new QHBoxLayout();
    layout->setGeometry({0, 0, 150, 40});

    auto label = new QLabel(title.c_str());

    auto slider = new QSlider{Qt::Orientation::Horizontal};
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(startValue);

    auto numberEdit = new QLineEdit(std::to_string(startValue).c_str());
    numberEdit->setMaximumWidth(30);
    QIntValidator* validator = new QIntValidator(min, max, this);
    numberEdit->setValidator(validator);

    QWidget::connect(numberEdit, &QLineEdit::textChanged, this, [slider](QString newValue) {
        slider->setValue(newValue.toInt());
    });

    QWidget::connect(slider, &QSlider::valueChanged, this, [numberEdit](int newValue) {
        numberEdit->setText(std::to_string(newValue).c_str());
    });

    layout->addWidget(label);
    layout->addWidget(slider);
    layout->addWidget(numberEdit);
    container_->addLayout(layout, 0);
    return slider;
}
