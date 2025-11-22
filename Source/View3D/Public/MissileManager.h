// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileManager.generated.h"

class AMissileActor;

UCLASS()
class VIEW3D_API AMissileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissileManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// 미사일 블루프린트 클래스
	UPROPERTY(EditAnywhere, Category = "Missile")
	TSubclassOf<AMissileActor> MissileClass;

	// ID → MissileActor 매핑
	UPROPERTY()
	TMap<uint8, AMissileActor*> Missiles;

	AMissileActor* GetMissileByID(uint8 ID) const;

	void ApplyAttitude(uint8 ID, float Pitch, float Yaw);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InitMSL")
	float OffsetY = 12500.f;   // Y축을 넓게 벌림
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InitMSL")
	float InitX = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InitMSL")
	float InitZ = 1000.f;

	bool bFirstTick = true;

	UFUNCTION(BlueprintCallable)
	void LaunchMissile(int ID, float Yaw);

	UFUNCTION(BlueprintCallable)
	void TerminalMissile(int ID);

	UFUNCTION(BlueprintCallable)
	void ApplyAttitude(int ID, float Pitch, float Yaw);

	UFUNCTION(BlueprintCallable)
	void UpdateTargetDistance(int ID, float Distance,float yaw);

	UFUNCTION(BlueprintCallable)
	void UpdateTelemetry(int ID, uint8 TelemetryByte);

	UFUNCTION(BlueprintCallable)
	void Explode(int ID);
};
