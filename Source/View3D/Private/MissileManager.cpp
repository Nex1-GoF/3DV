// Fill out your copyright notice in the Description page of Project Settings.


#include "MissileManager.h"
#include "Engine/World.h"
#include "MissileActor.h"
#include "MonitorPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "WBP_MissileMonitorBase.h"

// Sets default values
AMissileManager::AMissileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMissileManager::BeginPlay()
{
	Super::BeginPlay();

    for (uint8 i = 1; i <= 4; i++)
    {
        FActorSpawnParameters Params;
        Params.Owner = this;

        FVector SpawnLoc(InitX, i * OffsetY, InitZ);

        AMissileActor* NewMissile = GetWorld()->SpawnActor<AMissileActor>(
            MissileClass,
            SpawnLoc,
            FRotator::ZeroRotator,
            Params
        );

        if (NewMissile)
        {
            NewMissile->SetMissileID(i);
            Missiles.Add(i, NewMissile);
        }

        

    }
}

AMissileActor* AMissileManager::GetMissileByID(uint8 ID) const
{
    if (Missiles.Contains(ID))
        return Missiles[ID];
    return nullptr;
}

void AMissileManager::ApplyAttitude(uint8 ID, float Pitch, float Yaw)
{
    if (!Missiles.Contains(ID))
        return;

    AMissileActor* Missile = Missiles[ID];
    if (!Missile) return;

   
    Missile->ApplyAttitude(Pitch, Yaw);
}

void AMissileManager::LaunchMissile(int ID, float Yaw)
{

    UE_LOG(LogTemp, Warning, TEXT("LaunchMissile! id: %d yaw:%f"), ID,Yaw);
    AMissileActor* Missile = GetMissileByID(ID);
    if (Missile == nullptr)return;
    Missile->LaunchMissile(Yaw);
}

void AMissileManager::ApplyAttitude(int ID, float Pitch, float Yaw)
{
    AMissileActor* Missile = GetMissileByID(ID);
    if (Missile == nullptr)return;
    Missile->ApplyAttitude(Pitch, Yaw);
    
}

void AMissileManager::UpdateTargetDistance(int ID, float Distance)
{
    AMissileActor* Missile = GetMissileByID(ID);
    if (Missile == nullptr)return;
    Missile->UpdateTarget(Distance);
    
}

void AMissileManager::Explode(int ID)
{
    AMissileActor* Missile = GetMissileByID(ID);
    if (Missile == nullptr)return;
    Missile->AbortChange();
}

//void AMissileManager::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//    /*if (bFirstTick)
//    {
//        bFirstTick = false;
//        AMonitorPlayerController* PC = Cast<AMonitorPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
//        if (!PC) return;
//
//        auto UI = PC->GetMonitorWidget();
//        if (!UI) return;
//        UI->SetRenderTargets(
//            GetMissileByID(1)->GetRenderTarget(),
//            GetMissileByID(2)->GetRenderTarget(),
//            GetMissileByID(3)->GetRenderTarget(),
//            GetMissileByID(4)->GetRenderTarget()
//        );
//
//        if (GetMissileByID(1)->GetRenderTarget() == nullptr) {
//            UE_LOG(LogTemp, Error, TEXT("[Missile] GetRenderTarget is nullptr"));
//        }
//    }*/
//}