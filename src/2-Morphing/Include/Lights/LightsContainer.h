#pragma once

#include <QOpenGLShaderProgram>

#include "Base/Include/SceneLogic/BaseComponent.h"
#include "Base/Include/Utils/OpenGLUniformBuffer.h"
#include "Base/Include/SceneLogic/SceneNode.h"

template<typename TLight, typename TUniformData>
class BaseLight;

template<typename T>
class LightsContainer final : public BaseComponent {
public:
    explicit LightsContainer(QOpenGLExtraFunctions& funcs, const int location, int countLightsInShader,
                             const QOpenGLShaderProgram& program, const std::string& blockName)
        : buffer_(funcs), countLightsInShader_(countLightsInShader) {
        buffer_.create();
        buffer_.bind();

        const GLuint blockIndex = funcs.glGetUniformBlockIndex(program.programId(), blockName.c_str());
        funcs.glUniformBlockBinding(program.programId(), blockIndex, location);
        GLint blockSize;
        funcs.glGetActiveUniformBlockiv(program.programId(), blockIndex,
                                        GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

        buffer_.allocate(blockSize, location);

        buffer_.release();
    }

    T& addLight() {
        const int index = static_cast<int>(getNode().getChildren().size());
        const auto lightNode = SceneNode::create(
            std::string(T::defaultNameInHiearchy) + " " + std::to_string(index));
        std::unique_ptr<T> lightComponent = std::unique_ptr<T>(
            new T(*this, index));
        T* ptrLightComponent = lightComponent.get();

        lightNode->addComponent(std::move(lightComponent));
        getNode().addChild(lightNode);

        if (countLightsInShader_ != 1) {
            const int countLights = index + 1;

            buffer_.bind();
            buffer_.setSubData(countLightsInShader_ * sizeData, sizeof(int), &countLights);
            buffer_.release();
        }

        return *ptrLightComponent;
    }

    const char* getComponentName() override { return "Lights Container"; }

private:
    static constexpr int sizeData = sizeof(typename T::UniformData);

    friend BaseLight;

    void updatePoint(const int index, const typename T::UniformData& data) {
        buffer_.bind();
        buffer_.setSubData(index * sizeData, sizeData, &data);
        buffer_.release();
    }

    OpenGLUniformBuffer buffer_;
    int countLightsInShader_;
};
