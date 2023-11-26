#include "Views/InspectorView.h"
#include "Views//FieldsDrawer.h"
#include "SceneLogic/SceneNode.h"

#include <QSizeGrip>

InspectorView::InspectorView(QWidget* parent)
    : QWidget(parent), container_(new QGridLayout(this)) {
    setStyleSheet("background-color: rgba(155, 155, 155, 200);");

    tabs_.setTabShape(QTabWidget::Triangular);
    tabs_.setTabPosition(QTabWidget::West);

    // TODO: придумать как перенести это без дубляжа в SceneHierarchy
    container_->setSizeConstraint(QLayout::SetMaximumSize);
    container_->setContentsMargins(0, 0, 5, 5);
    container_->setSpacing(0);

    container_->addWidget(&tabs_, 1, 1);
    tabs_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tabs_.setMinimumSize(250, 200);
    tabs_.setContentsMargins(0, 0, 0, 0);

    constexpr int sizeGrip = 20;
    this->setWindowFlags(Qt::SubWindow);
    auto * sizeGripLeftTop = new QSizeGrip(this);
    sizeGripLeftTop->setFixedSize(sizeGrip, sizeGrip);
    container_->addWidget(sizeGripLeftTop, 0, 0, Qt::AlignLeft | Qt::AlignTop);

    auto* sizeGripLeft = new QSizeGrip(this);
    sizeGripLeft->setFixedWidth(sizeGrip);

    container_->addWidget(sizeGripLeft, 1, 0);
    sizeGripLeft->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    auto* sizeGripTop = new QSizeGrip(this);
    sizeGripTop->setFixedHeight(sizeGrip);

    container_->addWidget(sizeGripTop, 0, 1);
    sizeGripTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
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
