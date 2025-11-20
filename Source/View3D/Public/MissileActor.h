// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class ATarget;

UCLASS()
class VIEW3D_API AMissileActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMissileActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* RootComp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* CameraComp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* MisslieMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneCaptureComponent2D* MisslieCaptureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UTextureRenderTarget2D* ExternalRenderTarget = nullptr;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* ForwardComp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* BackComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	UParticleSystem* ExplosionFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	UParticleSystem* FireFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	uint8 MissileID = 1;

	/** 네트워크로 받은 순수 Raw Pitch/Yaw */
	float TargetPitch = 0.f;
	float TargetYaw = 0.f;
	float TargetRoll = 0.f;
	/** 실제 렌더링할 보간된 Pitch/Yaw */
	float SmoothPitch = 0.f;
	float SmoothYaw = 0.f;
	float inittime = 0.f;
	bool initfire = true;

	bool canspawn = false;
	/** 네트워크 수신 적용 */
	void ApplyAttitude(float InPitch, float InYaw);

	void SetMissileID(uint8 InID);

	void ApplyRenderTargetByID();

	void LaunchMissile(float inYaw);

	bool bIsLaunching = false;
	float LaunchTime = 0.f;
	float fireTime = 4.f;
	float InitialZ = 0.f;       // 발사 시작시 Z 기록
	float LaunchIncrease = 300000.f; // 4초 동안 증가할 Z 높이
	float InitialRoll = -90.f;  // 시작 Roll 값
	float LaunchTargetRoll = 0.f;     // 최종 Roll 값
	float InitialYaw = 0;
	float LaunchTargetYaw = 0.f;


	//타겟생성부분
	UPROPERTY(EditAnywhere, Category = "Target")
	TSubclassOf<ATarget> TargetClass;

	UPROPERTY()
	ATarget* TargetActor = nullptr;
	double TargetSpawnTime = 3.0;
	double TargetStartTime = 0.0;
	float CurrentTargetDistance = 0.0f;
	float TestTargetDistance = 10000.0f;
	void UpdateTarget(float Distance);
	bool testtargetflag = false;
	bool bFirstDistanceReceived = false;

	void ApplyTargetDistance(float NewDistance);
	float PrevDistance = 0.f;
	float CurrentDistance = 0.f;
	float DistanceLerpTime = 0.f;     // 현재 보간 진행 시간
	float DistanceLerpDuration = 0.f; // 이전 패킷 → 이번 패킷 도착 시간차
	float LastDistancePacketTime = 0.f;



	//종말부분
	void TerminalChange(float inYaw);
	bool bIsTerminal = false;
	float TerminalStartTime = 0.f;
	float TerminalTime = 2.f;
	float TerminalTargetYaw = 0.f;

	int mslstate = 0;

	void NoSignalChange();
	bool nosiganlcall = false;


	void AbortChange();

};
