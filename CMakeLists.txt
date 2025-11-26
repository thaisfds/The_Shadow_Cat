cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "the_shadow_cat")
project(${PROJECT_NAME})

# Include local configuration (each person creates their own)
if(EXISTS "${CMAKE_SOURCE_DIR}/local_config.cmake")
    include(local_config.cmake)
else()
    message(FATAL_ERROR "Please copy local_config.cmake.example to local_config.cmake and configure your library paths")    
endif()

add_executable(${PROJECT_NAME}
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.h
        Source/Main.cpp
        Source/Math.cpp
        Source/Math.h
        Source/Random.cpp
        Source/Random.h
        Source/Game.cpp
        Source/Game.h
        Source/Actors/Actor.cpp
        Source/Actors/Actor.h
        Source/Components/Component.cpp
        Source/Components/Component.h
        Source/Components/Drawing/DrawComponent.cpp
        Source/Components/Drawing/DrawComponent.h
        Source/Components/Physics/RigidBodyComponent.cpp
        Source/Components/Physics/RigidBodyComponent.h
        Source/Components/Physics/AABBColliderComponent.cpp
        Source/Components/Physics/AABBColliderComponent.h
        Source/Components/ParticleSystemComponent.cpp
        Source/Components/ParticleSystemComponent.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Renderer/VertexArray.h
        Source/Renderer/VertexArray.cpp
        Source/Renderer/Texture.cpp
        Source/Renderer/Texture.h
        Source/Actors/Block.cpp
        Source/Actors/Block.h
        Source/Actors/Goomba.cpp
        Source/Actors/Goomba.h
        Source/Actors/Spawner.cpp
        Source/Actors/Spawner.h
        Source/Actors/Mario.cpp
        Source/Actors/Mario.h
        Source/Components/Drawing/AnimatorComponent.cpp
        Source/Components/Drawing/AnimatorComponent.h
        Source/Components/Drawing/RectComponent.cpp
        Source/Components/Drawing/RectComponent.h
        Source/AudioSystem.cpp
        Source/AudioSystem.h
        Source/Renderer/Font.h
        Source/Renderer/Font.cpp
        Source/UI/UIButton.cpp
        Source/UI/UIButton.h
        Source/UI/UIElement.cpp
        Source/UI/UIElement.h
        Source/UI/UIImage.cpp
        Source/UI/UIImage.h
        Source/UI/Screens/UIScreen.cpp
        Source/UI/Screens/UIScreen.h
        Source/UI/UIText.cpp
        Source/UI/UIText.h
        Source/UI/Screens/HUD.cpp
        Source/UI/Screens/HUD.h
        Source/UI/UIRect.cpp
        Source/UI/UIRect.h
        Source/UI/Screens/MainMenu.cpp
        Source/UI/Screens/MainMenu.h
        Source/Actors/Characters/ShadowCat.cpp
        Source/Actors/Characters/ShadowCat.h
        Source/Components/Skills/SkillBase.cpp
        Source/Components/Skills/SkillBase.h
        Source/Components/Skills/BasicAttack.cpp
        Source/Components/Skills/BasicAttack.h
        Source/Components/Skills/SkillInputHandler.cpp
        Source/Components/Skills/SkillInputHandler.h
        Source/Actors/Characters/Character.cpp
        Source/Actors/Characters/Character.h
        Source/Actors/Characters/Dummy.cpp
        Source/Actors/Characters/Dummy.h
)

configure_local_linking(${PROJECT_NAME})