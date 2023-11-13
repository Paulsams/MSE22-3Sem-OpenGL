#include "Views/ItemInHierarchy.h"
#include "SceneLogic/SceneNode.h"

ItemInHierarchy::ItemInHierarchy(SceneNode& node)
    : node_(node), QStandardItem(node.getName().c_str()) {
}
