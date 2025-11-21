#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class ATarget;
class UParticleSystem;

UCLASS()
class VIEW3D_API AMissileActor : public AActor
{
	GENERATED_BODY()

public:
	AMissileActor();

protected:
	virtual void BeginPlay() override;

public:
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

	// =======================
	//  Guidance / Attitude
	// =======================

	/** 네트워크(UDP)로 받은 목표값 (Raw) */
	float RawYaw = 0.f;
	float RawRoll = 0.f;

	/** 실제 적용되는 자세값 */
	float TargetPitch = 0.f;
	float TargetYaw = 0.f;
	float TargetRoll = 0.f;

	/** UDP 보간 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guidance")
	float UdpInterpSpeed = 6.f;   // 5~12 추천

	bool canspawn = false;

	/** 네트워크 수신 적용 (중기유도 구간에서만 Raw 업데이트) */
	void ApplyAttitude(float InRoll, float InYaw);

	void SetMissileID(uint8 InID);
	void ApplyRenderTargetByID();

	// =======================
	//  Launch Phase
	// =======================
	void LaunchMissile(float inYaw);

	bool bIsLaunching = false;
	float LaunchTime = 0.f;
	float fireTime = 4.f;

	float InitialZ = 0.f;
	float LaunchIncrease = 300000.f;

	float InitialRoll = -90.f;
	float LaunchTargetRoll = 0.f;

	float InitialYaw = 0.f;
	float LaunchTargetYaw = 0.f;

	// =======================
	// Target spawn & move
	// =======================
	UPROPERTY(EditAnywhere, Category = "Target")
	TSubclassOf<ATarget> TargetClass;

	UPROPERTY()
	ATarget* TargetActor = nullptr;

	double TargetSpawnTime = 3.0;
	double TargetStartTime = 0.0;
	float CurrentTargetDistance = 0.0f;
	float TestTargetDistance = 10000.0f;

	void UpdateTarget(float Distance, float yaw);
	bool testtargetflag = false;

	// 거리 보간 관련(네가 미리 넣은 변수 유지)
	float PrevDistance = 0.f;
	float CurrentDistance = 0.f;
	float DistanceLerpTime = 0.f;
	float DistanceLerpDuration = 0.f;
	float LastDistancePacketTime = 0.f;

	// =======================
	// Terminal Phase
	// =======================
	void TerminalChange(float inYaw);

	bool bIsTerminal = false;
	float TerminalStartTime = 0.f;
	float TerminalTime = 2.f;
	float TerminalTargetYaw = 0.f;

	// =======================
	// State
	// 0: idle / 1: midcourse(UDP) / 2: terminal / 3: abort
	// =======================
	int mslstate = 0;

	void NoSignalChange();
	bool nosiganlcall = false;

	void AbortChange();


	// Distance smoothing
	float RawDistance = 0.f;      // UDP에서 받은 원본 거리
	float SmoothDistance = 0.f;   // Tick에서 보간된 거리
	float DistanceInterpSpeed = 1.f; // 보간 속도 (2~5 추천)

	// Yaw smoothing option (원하면 사용)
	float RawTargetYaw = 0.f;



	//abort
	bool bAbortMotion = false;
	float AbortStartTime = 0.f;
	float AbortInitialDistance = 0.f;
	float AbortDuration = 1.0f;   // 1초 안에 0까지 좁힘
};	