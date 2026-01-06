//========================================================================
// SceneCore.hpp
//========================================================================

#pragma once

// 공통되는 헤더를 모아뒀슴!

// 기본 시스템
#include "Application.hpp"
#include "MyGameApp.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"

// 컴포넌트
#include "TransformComponent.hpp"
#include "SpriteRendererComponent.hpp"
#include "TextRendererComponent.hpp"
#include "AnimationComponent.hpp"
#include "FSMComponent.hpp"

// 시스템
#include "InputSystem.hpp"
#include "InputEventSystem.hpp"
#include "ResourceManager.hpp"
#include "RenderSystem.hpp"
#include "CameraSystem.hpp"
#include "TransformSystem.hpp"
#include "AnimationSystem.hpp"
#include "PhysicsSystemManager.hpp"

// 유틸
#include "Direct2DWrapper.hpp"
#include "SceneManager.hpp"