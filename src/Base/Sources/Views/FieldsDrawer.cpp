#include "Views/FieldsDrawer.h"
#include "Utils/ToggleSwitch.h"

#include <QColorDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QValidator>

FieldsDrawer::FieldsDrawer(const int widthLabel, QWidget* parent)
        : QWidget(parent), widthLabel_(widthLabel), container_(new QVBoxLayout(this)) {
    container_->setSizeConstraint(QLayout::SetMinimumSize);
    container_->setAlignment(Qt::AlignTop);
}

void FieldsDrawer::setBackground(const QColor&color) {
    backgroundColor_ = color;
    updateStyleSheet();
}

void FieldsDrawer::setFontSize(const int fontSize) {
    fontSize_ = fontSize;
    updateStyleSheet();
}

SliderInfo FieldsDrawer::addSlider(const std::string&title, const int startValue, const int min, const int max,
                                   const std::optional<const std::function<void(int)>>&changedValueCallback) {
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

    if (changedValueCallback.has_value())
        QWidget::connect(slider, &QSlider::valueChanged, changedValueCallback.value());

    return {slider};
}

SpinxBoxInFloatInfo FieldsDrawer::addSpinBox(const std::string& title, float startValue, float min, float max, float step,
        const std::optional<const std::function<void(float)>>& changedValueCallback) {
    auto layout = new QHBoxLayout();
    layout->setGeometry({0, 0, 150, heightField});

    auto label = createLabel(title);
    auto valueField = new QDoubleSpinBox();
    valueField->setValue(startValue);
    valueField->setRange(min, max);
    valueField->setSingleStep(step);

    if (changedValueCallback.has_value()) {
        connect(valueField, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [changedValueCallback](const double newValue) {
            changedValueCallback.value()(static_cast<float>(newValue));
        });
    }

    layout->addWidget(label);
    layout->addWidget(valueField);
    container_->addLayout(layout, 0);

    return {valueField};
}

SliderInfo FieldsDrawer::addSlider(const std::string& title, int* valuePtr, int min, int max) {
    return addSlider(title, *valuePtr, min, max, [valuePtr](const int newValue) {
        *valuePtr = newValue;
    });
}

SliderInfo FieldsDrawer::addSliderIn01Range(const std::string& title, float* valuePtr) {
    return addSlider(title, *valuePtr, 0, 100, [valuePtr](const int newValue) {
        *valuePtr = static_cast<float>(newValue) / 100.0f;
    });
}

SliderInfo FieldsDrawer::addSliderIn01Range(const std::string& title, const float startValue,
    const std::optional<const std::function<void(float)>>& changedValueCallback) {
    if (changedValueCallback.has_value()) {
        return addSlider(title, startValue * 100, 0, 100, [changedValueCallback](const int newValue) {
            changedValueCallback.value()(newValue / 100.0f);
        });
    } else {
        return addSlider(title, startValue * 100, 0, 100);
    }
}

[[maybe_unused]] QLabel* FieldsDrawer::addReadonlyLabel(const std::string&title) {
    auto layout = new QHBoxLayout();
    layout->setGeometry({0, 0, 150, heightField});

    auto label = createLabel(title);
    auto valueField = new QLabel();

    layout->addWidget(label);
    layout->addWidget(valueField);
    container_->addLayout(layout, 0);

    return valueField;
}

ToggleSwitch* FieldsDrawer::addToggle(const std::string& title, bool* valuePtr) {
    auto layout = new QHBoxLayout();
    layout->setGeometry({0, 0, 150, heightField});

    auto label = createLabel(title);
    auto valueField = new ToggleSwitch();
    valueField->setStatus(*valuePtr);
    valueField->setGeometry({0, 0, 150, heightField});

    connect(valueField, &ToggleSwitch::statusChanged, this, [valuePtr](bool newValue) {
       *valuePtr = newValue;
    });

    layout->addWidget(label);
    layout->addWidget(valueField);
    container_->addLayout(layout, 0);

    return valueField;
}

ColorPickerFieldInfo FieldsDrawer::addColorPicker(
    const std::string&title, const std::function<QColor()>& getterColor,
    const std::optional<const std::function<void(QColor)>>&changedValueCallback) {
    auto layout = new QHBoxLayout();

    auto label = createLabel(title);
    auto colorPickerButton = new QPushButton();

    const auto updateBackgroundColorForButton = [colorPickerButton](const QColor&color) {
        colorPickerButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        colorPickerButton->setText(color.name());
    };

    connect(colorPickerButton, &QPushButton::clicked,
            [this, getterColor, changedValueCallback, updateBackgroundColorForButton, colorPickerButton]() {
                auto colorDialog = new QColorDialog(getterColor());

                QWidget::connect(colorDialog, &QColorDialog::currentColorChanged, this,
                                 [changedValueCallback, updateBackgroundColorForButton](const QColor&color) {
                                     updateBackgroundColorForButton(color);

                                     if (changedValueCallback.has_value())
                                         changedValueCallback.value()(color);
                                 });

                colorDialog->setWindowFlags(Qt::Popup);
                colorDialog->move(colorPickerButton->mapToGlobal(QPoint(0,0)));
                colorDialog->show();
            });

    layout->addWidget(label);
    layout->addWidget(colorPickerButton);
    container_->addLayout(layout, 0);

    updateBackgroundColorForButton(getterColor());

    return {colorPickerButton};
}

ColorPickerFieldInfo FieldsDrawer::addColorPicker(const std::string& title, const QVector3D* updatedColor,
    const std::optional<const std::function<void(QVector3D)>>& changedValueCallback) {

    auto getterColor = [updatedColor] {
        return QColor(
            updatedColor->x() * 255,
            updatedColor->y() * 255,
            updatedColor->z() * 255
        );
    };

    if (changedValueCallback.has_value()) {
        return addColorPicker(title,
            getterColor,
            [changedValueCallback](const QColor& color) {
            changedValueCallback.value()({
                static_cast<float>(color.redF()),
                static_cast<float>(color.greenF()),
                static_cast<float>(color.blueF())
            });
        });
    } else {
        return addColorPicker(title, getterColor);
    }
}

ColorPickerFieldInfo FieldsDrawer::addColorPicker(const std::string& title, QColor* colorPtr) {
    return addColorPicker(title,
        [colorPtr]() { return *colorPtr; },
        [colorPtr](const QColor&color) {
            *colorPtr = color;
        }
    );
}

// ColorPickerFieldInfo FieldsDrawer::addColorPicker(const std::string& title, QVector3D* colorPtr) {
//     return addColorPicker(title, QColor(
//         colorPtr->x() * 255,
//         colorPtr->y() * 255,
//         colorPtr->z() * 255
//         ), [colorPtr](const QColor&color) {
//             *colorPtr ={
//                 static_cast<float>(color.redF()),
//                 static_cast<float>(color.greenF()),
//                 static_cast<float>(color.blueF())
//             };
//     });
// }

Vector3FieldInfo FieldsDrawer::addVector3Field(
    const std::string&title, const QVector3D&startValue, float step,
    const std::optional<const std::function<void(QVector3D)>>&changedValueCallback) {

    auto layout = new QHBoxLayout();
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    auto label = createLabel(title);

    auto* xField = new QDoubleSpinBox();
    xField->setPrefix("x: ");
    xField->setRange(-1000.0f, 1000.0f);
    xField->setSingleStep(step);
    xField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    xField->setValue(startValue.x());
    xField->setFixedHeight(heightField);

    auto* yField = new QDoubleSpinBox();
    yField->setPrefix("y: ");
    yField->setRange(-1000.0f, 1000.0f);
    xField->setSingleStep(step);
    yField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    yField->setValue(startValue.y());
    yField->setFixedHeight(heightField);

    auto* zField = new QDoubleSpinBox();
    zField->setPrefix("z: ");
    zField->setRange(-1000.0f, 1000.0f);
    xField->setSingleStep(step);
    zField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    zField->setValue(startValue.z());
    zField->setFixedHeight(heightField);

    if (changedValueCallback.has_value()) {
        const auto valueChangedForVector = changedValueCallback.value();
        std::function valueChanged = [=](double) {
            valueChangedForVector(QVector3D(xField->value(), yField->value(), zField->value()));
        };

        QWidget::connect(xField, QOverload<double>::of(&QDoubleSpinBox::valueChanged), valueChanged);
        QWidget::connect(yField, QOverload<double>::of(&QDoubleSpinBox::valueChanged), valueChanged);
        QWidget::connect(zField, QOverload<double>::of(&QDoubleSpinBox::valueChanged), valueChanged);
    }

    layout->addWidget(label);
    layout->addWidget(xField);
    layout->addWidget(yField);
    layout->addWidget(zField);
    container_->addLayout(layout, 0);

    return {xField, yField, zField};
}

void FieldsDrawer::updateStyleSheet() {
    setStyleSheet(QString("background-color: %1; font: %2px;")
        .arg(backgroundColor_.name())
        .arg(fontSize_));
}

QLabel* FieldsDrawer::createLabel(const std::string&title) const {
    const auto label = new QLabel(title.c_str());
    label->setFixedSize(widthLabel_, heightField);
    return label;
}
