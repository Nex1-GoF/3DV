// Fill out your copyright notice in the Description page of Project Settings.


#include "WBP_MissileMonitorBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Styling/SlateBrush.h"

void UWBP_MissileMonitorBase::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("[UMG] NativeConstruct 실행됨"));

	
}

void UWBP_MissileMonitorBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (signal1) {
		signal1time += InDeltaTime;
		float Alpha = FMath::Clamp(signal1time / signalRunTime, 0.f, 1.0f);
		FLinearColor NewColor = NoSignal_1->ColorAndOpacity;
		NewColor.A = Alpha; 
		NoSignal_1->SetColorAndOpacity(NewColor);
		if (signal1time >= signalRunTime)
		{
			signal1 = false;
			signal1time = 0;
		}
	}
	if (signal2) {
		signal2time += InDeltaTime;
		float Alpha = FMath::Clamp(signal2time / signalRunTime, 0.f, 1.0f);
		FLinearColor NewColor = NoSignal_2->ColorAndOpacity;
		NewColor.A = Alpha;
		NoSignal_2->SetColorAndOpacity(NewColor);
		if (signal2time >= signalRunTime)
		{
			signal2 = false;
			signal2time = 0;
		}
	}
	if (signal3) {
		signal3time += InDeltaTime;
		float Alpha = FMath::Clamp(signal3time / signalRunTime, 0.f, 1.0f);
		FLinearColor NewColor = NoSignal_3->ColorAndOpacity;
		NewColor.A = Alpha;
		NoSignal_3->SetColorAndOpacity(NewColor);
		if (signal3time >= signalRunTime)
		{
			signal3 = false;
			signal3time = 0;
		}
	}
	if (signal4) {
		signal4time += InDeltaTime;
		float Alpha = FMath::Clamp(signal4time / signalRunTime, 0.f, 1.0f);
		FLinearColor NewColor = NoSignal_4->ColorAndOpacity;
		NewColor.A = Alpha;
		NoSignal_4->SetColorAndOpacity(NewColor);
		if (signal4time >= signalRunTime)
		{
			signal4 = false;
			signal4time = 0;
		}
	}

	// =========================
// 6) Time Clock Update (1초마다 갱신)
// =========================
	TimeUpdateAccumulator += InDeltaTime;
	if (TimeUpdateAccumulator >= 1.0f)
	{
		TimeUpdateAccumulator = 0.f;

		FDateTime Now = FDateTime::Now();

		FString NowStr = FString::Printf(
			TEXT("%04d %02d %02d %02d:%02d:%02d"),
			Now.GetYear(),
			Now.GetMonth(),
			Now.GetDay(),
			Now.GetHour(),
			Now.GetMinute(),
			Now.GetSecond()
		);

		if (Time_Block_1) Time_Block_1->SetText(FText::FromString(NowStr));
		if (Time_Block_2) Time_Block_2->SetText(FText::FromString(NowStr));
		if (Time_Block_3) Time_Block_3->SetText(FText::FromString(NowStr));
		if (Time_Block_4) Time_Block_4->SetText(FText::FromString(NowStr));
	}
}


void UWBP_MissileMonitorBase::NosignalRun(uint8 id)
{
	if (id == 1) {
		signal1 = true;
		signal1time = 0;
		NoSignal_1->ColorAndOpacity.A=0;
	}
	else if(id == 2) {
		signal2 = true;
		signal2time = 0;
		NoSignal_2->ColorAndOpacity.A =0;
	}
	else if (id == 3) {

		signal3 = true;
		signal3time = 0;

		NoSignal_3->ColorAndOpacity.A = 0;
	}
	else if (id == 4) {
		signal4 = true;
		signal4time = 0;
		NoSignal_4->ColorAndOpacity.A = 0;
	}
}

FORCEINLINE bool GetBit(uint8 Byte, int32 BitIndex)
{
	return (Byte & (1 << BitIndex)) != 0;
}

void UWBP_MissileMonitorBase::UpdateTelemetry(uint8 Id, uint8 TelemetryByte)
{
	bool bSeeker = GetBit(TelemetryByte, 0); // 1번 비트
	bool bTDD = GetBit(TelemetryByte, 1); // 2번 비트
	bool bDL = GetBit(TelemetryByte, 2); // 3번 비트

	UE_LOG(LogTemp, Error, TEXT("TelemetryByte %d"), TelemetryByte);
	if (bSeeker)UE_LOG(LogTemp, Error, TEXT("Seekeron"));
	if (bTDD)UE_LOG(LogTemp, Error, TEXT("bTDD"));
	if (bDL)UE_LOG(LogTemp, Error, TEXT("bDL"));
	UImage* Target1 = nullptr;
	UImage* Target2 = nullptr;
	UImage* Target3 = nullptr;

	switch (Id)
	{
	case 1:
		Target1 = Telemetry_1_1;
		Target2 = Telemetry_1_2;
		Target3 = Telemetry_1_3;
		break;

	case 2:
		Target1 = Telemetry_2_1;
		Target2 = Telemetry_2_2;
		Target3 = Telemetry_2_3;
		break;

	case 3:
		Target1 = Telemetry_3_1;
		Target2 = Telemetry_3_2;
		Target3 = Telemetry_3_3;
		break;

	case 4:
		Target1 = Telemetry_4_1;
		Target2 = Telemetry_4_2;
		Target3 = Telemetry_4_3;
		break;

	default:
		return;
	}

	if (Target1) Target1->SetBrushFromTexture(bSeeker ? GreenTexture : RedTexture);
	if (Target2) Target2->SetBrushFromTexture(bTDD ? GreenTexture : RedTexture);
	if (Target3) Target3->SetBrushFromTexture(bDL ? GreenTexture : RedTexture);
}