//========================================================================
// Singleton.hpp
//========================================================================
#pragma once

#include "pch.h"

#include <cassert>

//friend class Singleton<클래스>; << 꼭 박아줘야함

template <typename T>
class Singleton
{
public:
	// 명시적인 인스턴스 생성
	static void Create()
	{
		if (!instance_) {
			instance_ = new T();
			Destroyer::Register();
		}
			
	}

	// 인스턴스 반환
	static T& GetInstance()
	{
		assert(instance_ && "Singleton not created! Call Create() first.");
		return *instance_;
	}

	// 명시적인 인스턴스 파괴 << 자동화시킴
	//static void Destroy()	{		delete instance_;		instance_ = nullptr;	}

protected:
	Singleton() = default;
	~Singleton() = default;

private:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	static T* instance_;

	struct Destroyer {
		~Destroyer() {
			delete instance_;
			instance_ = nullptr;
		}

		static void Register() {
			static Destroyer d; // 전역 정적 객체 > 프로그램 종료 시 자동 파괴됨
		}

	};
};

template <typename T>
T* Singleton<T>::instance_ = nullptr;