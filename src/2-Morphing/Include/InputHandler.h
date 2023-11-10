#pragma once

#include <QWidget>

class InputHandler : public QObject {
Q_OBJECT;
public:
    static InputHandler &getInstance() { return *_instance; }

    [[nodiscard]] static std::unique_ptr<InputHandler> createInstance() {
        auto smartInstance = std::unique_ptr<InputHandler>(new InputHandler());
        _instance = smartInstance.get();
        return smartInstance;
    }

    ~InputHandler() override {
        _instance = nullptr;
    }

signals:
    void keyPressed(int keyCode);

    void keyReleased(int keyCode);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    explicit InputHandler();

    static InputHandler *_instance;
};
