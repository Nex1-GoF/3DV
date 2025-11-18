// Fill out your copyright notice in the Description page of Project Settings.


#include "MissileMonitorGameModeBase.h"
#include "MonitorPlayerController.h"

AMissileMonitorGameModeBase::AMissileMonitorGameModeBase()
{
    // 시작할 때 사용할 PlayerController 설정
    PlayerControllerClass = AMonitorPlayerController::StaticClass();

    // Pawn은 필요 없음 → No Pawn
    DefaultPawnClass = nullptr;
}

