#pragma once

#include <QHBoxLayout>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QColorDialog>

#include <optional>
#include <unordered_map>

class DebugWindow : public QWidget {
public:
    explicit DebugWindow(QWidget *parent = nullptr);

    template<class T>
    [[maybe_unused]] T *getWidget(std::string &key);

    [[maybe_unused]] QSlider *addSlider(
            const std::string &key, const std::string &title, int startValue, int min, int max,
            const std::optional<const std::function<void(int)>> &changedValueCallback = std::nullopt
    );

    [[maybe_unused]] QLabel *addReadonlyLabel(const std::string &key, const std::string &title);

    void *addColorPicker(
            const std::string &key, const std::string &title, const QColor &startColor,
            const std::optional<const std::function<void(QColor)>> &changedValueCallback = std::nullopt
    );

private:
    static QLabel *createLabel(const std::string &title = "");

    QVBoxLayout *container_;
    std::unordered_map<std::string, QObject *> widgets_;
};
