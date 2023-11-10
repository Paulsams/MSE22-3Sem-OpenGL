#include "DebugWindow.h"

#include <QLineEdit>
#include <QRegExpValidator>
#include <QPushButton>

DebugWindow::DebugWindow(QWidget *parent)
        : QWidget(parent), container_(new QVBoxLayout(this)), widgets_() {
    setGeometry(10, 10, 400, 0);
    setStyleSheet("background-color: rgba(155, 155, 155, 200); font: 18px;");
    container_->setSizeConstraint(QLayout::SetMinimumSize);
    container_->setAlignment(Qt::AlignTop);
}

QSlider *DebugWindow::addSlider(const std::string &key, const std::string &title, int startValue, int min, int max,
                                const std::optional<const std::function<void(int)>> &changedValueCallback) {
    auto layout = new QHBoxLayout();

    auto label = createLabel(title);

    auto slider = new QSlider{Qt::Orientation::Horizontal};
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(startValue);

    auto numberEdit = new QLineEdit(std::to_string(startValue).c_str());
    numberEdit->setMaximumWidth(40);
    auto validator = new QIntValidator(min, max, this);
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

    widgets_.insert({key, slider});

    if (changedValueCallback.has_value())
        QWidget::connect(slider, &QSlider::valueChanged, changedValueCallback.value());

    return slider;
}

[[maybe_unused]] QLabel *DebugWindow::addReadonlyLabel(const std::string &key, const std::string &title) {
    auto layout = new QHBoxLayout();
    layout->setGeometry({0, 0, 150, 40});

    auto label = createLabel(title);
    auto valueField = new QLabel();

    layout->addWidget(label);
    layout->addWidget(valueField);
    container_->addLayout(layout, 0);

    widgets_.insert({key, label});
    return valueField;
}

void *DebugWindow::addColorPicker(
        const std::string &key, const std::string &title, const QColor &startColor,
        const std::optional<const std::function<void(QColor)>> &changedValueCallback) {
    auto layout = new QHBoxLayout();

    auto label = createLabel(title);
    auto colorPickerButton = new QPushButton();

    const auto updateBackgroundColorForButton = [colorPickerButton](const QColor& color) {
        QPalette palette = colorPickerButton->palette();
        palette.setColor(QPalette::Button, color);
        colorPickerButton->setAutoFillBackground(true);
        colorPickerButton->setPalette(palette);
        colorPickerButton->setText(color.name());
    };

    updateBackgroundColorForButton(startColor);

    connect(colorPickerButton, &QPushButton::clicked, [this, startColor, changedValueCallback, updateBackgroundColorForButton]() {
        auto colorDialog = new QColorDialog(startColor);

        DebugWindow::connect(colorDialog, &QColorDialog::currentColorChanged, [&changedValueCallback, updateBackgroundColorForButton](const QColor& color) {
            updateBackgroundColorForButton(color);

            if (changedValueCallback.has_value())
                changedValueCallback.value()(color);
        });

        colorDialog->setWindowFlags(colorDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        colorDialog->show();
    });

    layout->addWidget(label);
    layout->addWidget(colorPickerButton);
    container_->addLayout(layout, 0);

    widgets_.insert({key, colorPickerButton});

    return colorPickerButton;
}

QLabel *DebugWindow::createLabel(const std::string &title) {
    auto label = new QLabel(title.c_str());
    label->setFixedWidth(160);
    return label;
}

template<class T>
T *DebugWindow::getWidget(std::string &key) {
    if (auto it = widgets_.find(key); it != widgets_.end())
        return it->second;
    return nullptr;
}
