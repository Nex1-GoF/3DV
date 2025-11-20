// Fill out your copyright notice in the Description page of Project Settings.


#include "MissileActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Target.h"
#include "MonitorPlayerController.h"
#include "WBP_MissileMonitorBase.h"
// Sets default values
AMissileActor::AMissileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComp=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	MisslieMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MisslieMesh->SetupAttachment(RootComp);
	MisslieMesh->SetRelativeRotation(FRotator(0, -90, -90));

	//앞방향
	ForwardComp = CreateDefaultSubobject<USceneComponent>(TEXT("Forward"));
	ForwardComp->SetupAttachment(MisslieMesh);
	ForwardComp->SetRelativeLocation(FVector(0, 370, 0));

	//앞방향
	BackComp = CreateDefaultSubobject<USceneComponent>(TEXT("Back"));
	BackComp->SetupAttachment(MisslieMesh);
	BackComp->SetRelativeLocation(FVector(0, -300, 0));


	CameraComp = CreateDefaultSubobject<USceneComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(RootComp);


	MisslieCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
	if (MisslieCaptureComponent != nullptr) {
		MisslieCaptureComponent->SetupAttachment(CameraComp);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("[Missile] MisslieCaptureComponent NewObject FAILED"));
	}
	MisslieCaptureComponent->SetupAttachment(CameraComp);

	CameraComp->SetRelativeLocation(FVector(-800.f, 0.f, 300.f));
	CameraComp->SetRelativeRotation(FRotator(-20.f, 0, 0));

	//매 프레임 캡처 대신 필요 시 호출 (성능 ↑)
	/*MisslieCaptureComponent->bCaptureEveryFrame = false;
	MisslieCaptureComponent->bCaptureOnMovement = false;*/

}

// Called when the game starts or when spawned
void AMissileActor::BeginPlay()
{
	Super::BeginPlay();
	//RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	//RenderTarget->InitAutoFormat(960, 540);  // 원하는 해상도로 설정
	//RenderTarget->ClearColor = FLinearColor::Black;
	//RenderTarget->UpdateResource();

	//// SceneCapture에 연결
	//if (MisslieCaptureComponent != nullptr) {
	//	MisslieCaptureComponent->TextureTarget = RenderTarget; 
	//	UE_LOG(LogTemp, Warning,
	//		TEXT("[Missile] RenderTargets 호출됨 RT:%p"),
	//		RenderTarget); 
	//	UE_LOG(LogTemp, Warning,
	//		TEXT("[Missile] TextureTarget 호출됨 TT:%p"),
	//		*(MisslieCaptureComponent->TextureTarget));
	//}
	//else {
	//	UE_LOG(LogTemp, Error, TEXT("[Missile] MisslieCaptureComponent is nullptr"));
	//}
	TargetYaw = -90;
	TargetRoll = -90;
	TargetPitch = 0;
	FRotator newrot = FRotator(TargetPitch, TargetYaw, TargetRoll);

	MisslieMesh->SetRelativeRotation(newrot);
	MisslieCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;




}


// Called every frame
void AMissileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 
	static float Accum = 0.f;
	Accum += DeltaTime;
	if (Accum > 0.01f) {
		MisslieCaptureComponent->CaptureSceneDeferred();
		Accum = 0.01f;
	}
	if (mslstate == 3)return;
	//발사TEST용
	/*if (initfire) {
		inittime += DeltaTime;
		if (inittime >= 5.0f) {

			initfire = false;
			if(MissileID==4)
			LaunchMissile(-60.f);
		}
	}*/

	//발사함수틱
	if (bIsLaunching)
	{
		LaunchTime += DeltaTime;
		// ---- (1) 4초간 Z 상승 ----
		float ZProgress = FMath::Clamp(LaunchTime / 4.f, 0.f, 1.f);
		float NewZ = InitialZ + (LaunchIncrease * ZProgress);

		FVector Loc = GetActorLocation();
		Loc.Z = NewZ;
		SetActorLocation(Loc);

		// ---- (2) 마지막 2초간 Roll 보정(-90 → 0) ----
		if (LaunchTime >= 2.f)
		{
			float RollAlpha = (LaunchTime - 2.f) / 2.f; // 0 → 1 (2~4초)
			RollAlpha = FMath::Clamp(RollAlpha, 0.f, 1.f);

			float NewRoll = FMath::Lerp(InitialRoll, LaunchTargetRoll, RollAlpha);
			TargetRoll = NewRoll;

			float YawAlpha = (LaunchTime - 2.f) / 2.f; // 0 → 1 (2~4초)
			YawAlpha = FMath::Clamp(YawAlpha, 0.f, 1.f);

			UE_LOG(LogTemp, Warning, TEXT("Invalid MissileID=%f"), TargetYaw);
			float NewYaw = FMath::Lerp(InitialYaw, LaunchTargetYaw, YawAlpha);
			TargetYaw = NewYaw;
		}

		// ---- (3) 4초 후 종료 ----
		if (LaunchTime >= 4.f)
		{
			bIsLaunching = false;
			TargetRoll = LaunchTargetRoll;
			//종말태스크 임시실행
			//TerminalChange(-90.f);
		}
	}
	//
	//종말태스크 진행
	if (bIsTerminal)
	{
		TerminalStartTime += DeltaTime;
		if (TerminalStartTime <= 1.5f)
		{
			float YawAlpha = (TerminalStartTime) / 2.f; // 0 → 1 (2~4초)
			YawAlpha = FMath::Clamp(YawAlpha, 0.f, 1.f);

			float NewYaw = FMath::Lerp(InitialYaw, TerminalTargetYaw, YawAlpha);
			TargetYaw = NewYaw;
		}

		// ---- (3) 4초 후 종료 ----
		if (TerminalStartTime >= 1.5f)
		{
			bIsTerminal = false;
			TargetYaw = TerminalTargetYaw;
			if (!canspawn)canspawn = true;
		}
	}
	//


	//회전 변경하기 이게 사실상 중기유도임!
	FRotator newrot = FRotator(TargetPitch, TargetYaw, TargetRoll);
	MisslieMesh->SetRelativeRotation(newrot);

	//타겟생성코드 종말부분
	if (bFirstDistanceReceived)
	{
		DistanceLerpTime += DeltaTime;

		float Alpha = 0.f;

		if (DistanceLerpDuration > 0.f)
			Alpha = FMath::Clamp(DistanceLerpTime / DistanceLerpDuration, 0.f, 1.f);

		float LerpDist = FMath::Lerp(PrevDistance, CurrentDistance, Alpha);

		UpdateTarget(LerpDist);
	}

}

void AMissileActor::ApplyTargetDistance(float NewDistance)
{
	float Now = GetWorld()->GetTimeSeconds();

	if (!bFirstDistanceReceived)
	{
		// 초기 패킷: 생성만 수행
		PrevDistance = NewDistance;
		CurrentDistance = NewDistance;
		LastDistancePacketTime = Now;
		bFirstDistanceReceived = true;

		// 타겟 생성만 하고 이동은 하지 않는다
		UpdateTarget(NewDistance);
		return;
	}

	// 두 번째 패킷부터는 보간 시작
	PrevDistance = CurrentDistance;
	CurrentDistance = NewDistance;

	float NewPacketTime = Now;
	DistanceLerpDuration = NewPacketTime - LastDistancePacketTime; // ex: 0.25초
	LastDistancePacketTime = NewPacketTime;

	DistanceLerpTime = 0.f; // 보간 시작
}

void AMissileActor::ApplyAttitude(float InRoll, float InYaw)
{
	if (mslstate != 1)return;
	TargetRoll = InRoll;
	TargetYaw = InYaw-90.0f;

	/*UE_LOG(LogTemp, Warning, TEXT("Target Pitch=%f, Yaw=%f"),
		TargetPitch, TargetYaw);*/
}

void AMissileActor::SetMissileID(uint8 InID)
{
	MissileID = InID; 
	ApplyRenderTargetByID();

}

void AMissileActor::ApplyRenderTargetByID()
{
	if (MissileID <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid MissileID=%d"), MissileID);
		return;
	}

	FString Path = FString::Printf(
		TEXT("/Game/RenderTargets/RT_MissileCam%d.RT_MissileCam%d"),
		MissileID, MissileID
	);

	UTextureRenderTarget2D* RT =
		LoadObject<UTextureRenderTarget2D>(nullptr, *Path);

	if (!RT)
	{
		UE_LOG(LogTemp, Error, TEXT("Missile %d : Load Fail %s"),
			MissileID, *Path);
		return;
	}

	if (MisslieCaptureComponent)
		MisslieCaptureComponent->TextureTarget = RT;

	UE_LOG(LogTemp, Warning,
		TEXT("Missile %d assigned RT=%p"), MissileID, RT);

}

void AMissileActor::LaunchMissile(float inYaw)
{
	bIsLaunching = true;
	LaunchTime = 0.f;
	InitialZ = GetActorLocation().Z;
	FRotator initialRot = MisslieMesh->GetRelativeRotation();
	InitialRoll = initialRot.Roll;
	InitialYaw = initialRot.Yaw;
	LaunchTargetYaw = inYaw-90;
	mslstate = 1;
	UE_LOG(LogTemp, Warning, TEXT("[Missile %d] FireFX Spwan"), MissileID);
	if (FireFX && BackComp)
	{
		UParticleSystemComponent* FirePSC =
			UGameplayStatics::SpawnEmitterAttached(
				FireFX,
				BackComp,
				NAME_None,
				FVector::ZeroVector,
				FRotator(0,0,-90),
				FVector(2.f, 2.f, 2.f),
				EAttachLocation::KeepRelativeOffset,
				true
			);
		UE_LOG(LogTemp, Warning, TEXT("[Missile %d] FireFX SpwanComplete"), MissileID);
		if (FirePSC)
		{
			FirePSC->SetRelativeLocation(FVector(0, 0, 0));  // 필요하면 미세 위치 조정
			UE_LOG(LogTemp, Warning, TEXT("[Missile %d] FireFX Attached"), MissileID);
			FirePSC->CustomTimeDilation = 1.0f;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Missile %d] Failed to spawn FireFX"), MissileID);
		}
	}

}


void AMissileActor::TerminalChange(float inYaw)
{
	bIsTerminal = true;
	TerminalStartTime = 0.f;
	FRotator initialRot = MisslieMesh->GetRelativeRotation();
	InitialYaw = initialRot.Yaw;
	TerminalTargetYaw = inYaw;
	mslstate = 2;
}

void AMissileActor::UpdateTarget(float Distance)
{
	if (!ForwardComp || !MisslieMesh) return;
	if (!canspawn)return;
	// 1) 방향벡터
	FVector ForwardVector =
		(ForwardComp->GetComponentLocation() - MisslieMesh->GetComponentLocation()).GetSafeNormal();

	// 2) 목표 위치
	FVector TargetLoc =
		ForwardComp->GetComponentLocation() + ForwardVector * Distance;

	// 3) 타겟 최초 생성
	if (TargetActor == nullptr)
	{
		if (!TargetClass)
		{
			UE_LOG(LogTemp, Error, TEXT("TargetClass is NOT set!"));
			return;
		}

		FActorSpawnParameters Params;
		Params.Owner = this;

		TargetActor = GetWorld()->SpawnActor<ATarget>(
			TargetClass,
			TargetLoc,
			FRotator::ZeroRotator,
			Params
		);

		if (!TargetActor)
		{
			UE_LOG(LogTemp, Error, TEXT("Target spawn failed!"));
			return;
		}
		//TargetActor->TargetMesh->SetOnlyOwnerSee(true);
		UE_LOG(LogTemp, Warning, TEXT("★ Target created at %d"),
			MissileID);

	}

	// 4) 이후 계속 이동
	TargetActor->SetActorLocation(TargetLoc);
}

void AMissileActor::NoSignalChange() {
	//GetPlayerController Cast to AMonitorPlayerController
	//and AMonitorPlayerController.MissileManagerClass.NosignalRun(MisslieID)
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("[Missile %d] NoSignalChange: PlayerController not found"), MissileID);
		return;
	}
	// 2) AMonitorPlayerController로 캐스팅
	AMonitorPlayerController* MPC = Cast<AMonitorPlayerController>(PC);
	if (!MPC)
	{
		UE_LOG(LogTemp, Error, TEXT("[Missile %d] NoSignalChange: Cast to AMonitorPlayerController failed"), MissileID);
		return;
	}
	UWBP_MissileMonitorBase* WBP = MPC->GetMonitorWidget();
	// 3) WBP 가져오기
	if (!WBP)
	{
		UE_LOG(LogTemp, Error, TEXT("[Missile %d] NoSignalChange: WBP is nullptr"), MissileID);
		return;
	}
	WBP->NosignalRun(MissileID);
	UE_LOG(LogTemp, Error, TEXT("[Missile %d] NoSignalCall"), MissileID);




}

void AMissileActor::AbortChange()
{
	NoSignalChange();
	mslstate = 3; if (ExplosionFX && ForwardComp)
	{
		FVector SpawnLoc = ForwardComp->GetComponentLocation();
		FRotator SpawnRot = ForwardComp->GetComponentRotation(); 
		FVector SpawnScale = FVector(5.f, 5.f, 5.f);
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionFX,
			SpawnLoc,
			SpawnRot,
			SpawnScale,
			true
		);

		UE_LOG(LogTemp, Warning, TEXT("[Missile %d] Explosion FX spawned"), MissileID);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Missile %d] ExplosionFX or ForwardComp is NULL"), MissileID);
	}
}