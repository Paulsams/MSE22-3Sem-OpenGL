#include "Views/InspectorView.h"
#include "Views//FieldsDrawer.h"
#include "SceneLogic/SceneNode.h"

InspectorView::InspectorView(QWidget* parent)
    : QWidget(parent), container_(new QVBoxLayout(this)), tabs_(this) {


    setStyleSheet("background-color: rgba(155, 155, 155, 200);");

    tabs_.setTabShape(QTabWidget::Triangular);
    tabs_.setTabPosition(QTabWidget::West);

    container_->setSizeConstraint(QLayout::SetMaximumSize);
    container_->setAlignment(Qt::AlignTop);

    container_->addWidget(&tabs_);
    tabs_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void InspectorView::clear() {
    tabs_.clear();
}

void InspectorView::fillComponents(SceneNode& sceneNode) {
    auto* transformDrawer = new FieldsDrawer(50);
    transformDrawer->setFontSize(12);
    tabs_.addTab(transformDrawer, "Transform");

    transformDrawer->addVector3Field(
        "Position", sceneNode.getTransform().getPosition(), 0.2f,
        [&sceneNode](const QVector3D newValue) {
            sceneNode.getTransform().setPosition(newValue);
        }
    );

    transformDrawer->addVector3Field(
        "Rotation", sceneNode.getTransform().getRotation().toEulerAngles(), 5.0f,
        [&sceneNode](const QVector3D newValue) {
            sceneNode.getTransform().setRotation(QQuaternion::fromEulerAngles(newValue));
        }
    );

    transformDrawer->addVector3Field(
        "Scale", sceneNode.getTransform().getScale(), 0.1f,
        [&sceneNode](const QVector3D newValue) {
            sceneNode.getTransform().setScale(newValue);
        }
    );

    sceneNode.iterateComponents([this](BaseComponent& component) {
        // TODO: надо бы не делать так в идеале. Можно делать два метода, где один возвращает бул, а другой нет -- но это несоглассование состояние
        auto* drawer = new FieldsDrawer(50);
        drawer->setFontSize(12);
        if (component.drawValueInInspector(*drawer)) {
            tabs_.addTab(drawer, component.getComponentName());
        } else {
            delete drawer;
        }
    });
}
