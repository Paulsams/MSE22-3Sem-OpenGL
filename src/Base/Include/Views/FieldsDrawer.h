#pragma once

#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QVector3D>

#include <functional>
#include <optional>
#include <string>

struct SliderInfo {
    QSlider* slider;
};

struct ColorPickerFieldInfo {
    QPushButton* button;
};

struct Vector3FieldInfo {
    QDoubleSpinBox* xField;
    QDoubleSpinBox* yField;
    QDoubleSpinBox* zField;
};

struct SpinxBoxInFloatInfo {
    QDoubleSpinBox* field;
};

class ToggleSwitch;

class FieldsDrawer final : public QWidget {
    Q_OBJECT

public:
    explicit FieldsDrawer(int widthLabel, QWidget* parent = nullptr);

    void setBackground(const QColor&color);
    void setFontSize(int fontSize);

    QPushButton* addButton(const std::string& title, const std::string& textOnButton, const std::function<void()>& changedValueCallback);

    SliderInfo addSlider(
        const std::string& title, int startValue, int min, int max,
        const std::optional<const std::function<void(int)>>& changedValueCallback = std::nullopt
    );

    SpinxBoxInFloatInfo addSpinBox(
        const std::string&title, float startValue, float min, float max, float step,
        const std::optional<const std::function<void(float)>>&changedValueCallback = std::nullopt
    );

    SliderInfo addSlider(
        const std::string&title, int* valuePtr, int min, int max
    );

    SliderInfo addSliderIn01Range(
        const std::string& title, float* valuePtr
    );

    SliderInfo addSliderIn01Range(
        const std::string& title, float startValue,
        const std::optional<const std::function<void(float)>>&changedValueCallback = std::nullopt
    );

    QLabel* addReadonlyLabel(const std::string& title);

    ToggleSwitch* addToggle(const std::string& title, bool* valuePtr);

    ColorPickerFieldInfo addColorPicker(
        const std::string&title, const std::function<QColor()>& getterColor,
        const std::optional<const std::function<void(QColor)>>&changedValueCallback = std::nullopt
    );

    ColorPickerFieldInfo addColorPicker(
        const std::string&title, const QVector3D* updatedColor,
        const std::optional<const std::function<void(QVector3D)>>&changedValueCallback = std::nullopt
    );

    ColorPickerFieldInfo addColorPicker(
        const std::string&title, QColor* colorPtr
    );

    Vector3FieldInfo addVector3Field(
        const std::string&title, const QVector3D&startValue, float step,
        const std::optional<const std::function<void(QVector3D)>>&changedValueCallback = std::nullopt
    );

private:
    void updateStyleSheet();

    QLabel* createLabel(const std::string&title = "") const;

    static constexpr int heightField = 20;
    static constexpr int defaultFontSize = 12;
    static constexpr QColor defaultBackgroundColor = QColor(155, 155, 155, 200);

    int fontSize_ = defaultFontSize;
    QColor backgroundColor_ = defaultBackgroundColor;

    const int widthLabel_;
    QVBoxLayout* container_;
};
