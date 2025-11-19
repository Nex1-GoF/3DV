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
	if (initfire) {
		inittime += DeltaTime;
		if (inittime >= 5.0f) {

			initfire = false;
			if(MissileID==4)
			LaunchMissile(-60.f);
		}
	}
	//

	//const float SmoothSpeed = 5.0f;

	//// 보간 필수
	//float TargetPitchFixed = ShortestAngle(SmoothPitch, TargetPitch);
	//float TargetYawFixed = ShortestAngle(SmoothYaw, TargetYaw);

	//SmoothPitch = FMath::FInterpTo(SmoothPitch, TargetPitchFixed, DeltaTime, SmoothSpeed);
	//SmoothYaw = FMath::FInterpTo(SmoothYaw, TargetYawFixed, DeltaTime, SmoothSpeed);

	//// 최종 회전
	//FQuat QYaw = FQuat(FVector::UpVector, FMath::DegreesToRadians(SmoothYaw));
	//FQuat QPitch = FQuat(FVector::RightVector, FMath::DegreesToRadians(SmoothPitch));
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

			float NewYaw = FMath::Lerp(InitialYaw, LaunchTargetYaw, YawAlpha);
			TargetYaw = NewYaw;
		}

		// ---- (3) 4초 후 종료 ----
		if (LaunchTime >= 4.f)
		{
			bIsLaunching = false;
			TargetRoll = LaunchTargetRoll;
			TerminalChange(-90.f);
		}
	}
	if (bIsTerminal)
	{
		TerminalStartTime += DeltaTime;
		if (TerminalStartTime <= 2.f)
		{
			float YawAlpha = (TerminalStartTime) / 2.f; // 0 → 1 (2~4초)
			YawAlpha = FMath::Clamp(YawAlpha, 0.f, 1.f);

			float NewYaw = FMath::Lerp(InitialYaw, TerminalTargetYaw, YawAlpha);
			TargetYaw = NewYaw;
		}

		// ---- (3) 4초 후 종료 ----
		if (TerminalStartTime >= 2.f)
		{
			bIsTerminal = false;
			TargetYaw = TerminalTargetYaw;
			testtargetflag = true;
		}
	}



	//회전 변경하기
	FRotator newrot = FRotator(TargetPitch, TargetYaw, TargetRoll);
	MisslieMesh->SetRelativeRotation(newrot);

	if (testtargetflag)
	{
		TargetStartTime += DeltaTime;
		float Alpha = FMath::Clamp(TargetStartTime / TargetSpawnTime, 0.f, 1.f);
		// 거리 10000 → 0 으로 줄어듦
		float CurDist = FMath::Lerp(TestTargetDistance, 0.f, Alpha);
		UpdateTarget(CurDist);
		//기폭 테스트용
		if (CurDist <= 500) {
			if (!nosiganlcall) {
				AbortChange();
				nosiganlcall = true;
			}
			
		}
		//
	}

}

void AMissileActor::ApplyAttitude(float InRoll, float InYaw)
{
	TargetRoll = InRoll;
	TargetYaw = InYaw;

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
	LaunchTargetYaw = inYaw;
	mslstate = 1;
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