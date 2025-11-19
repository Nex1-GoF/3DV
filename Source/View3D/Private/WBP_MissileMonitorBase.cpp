// Fill out your copyright notice in the Description page of Project Settings.


#include "WBP_MissileMonitorBase.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Styling/SlateBrush.h"

void UWBP_MissileMonitorBase::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("[UMG] NativeConstruct ½ÇÇàµÊ"));

	
}

void UWBP_MissileMonitorBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (signal1) {
		signal1time += InDeltaTime;
		float Alpha = FMath::Clamp(signal1time / signalRunTime, 0.f, 1.f);
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
		float Alpha = FMath::Clamp(signal2time / signalRunTime, 0.f, 1.f);
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
		float Alpha = FMath::Clamp(signal3time / signalRunTime, 0.f, 1.f);
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
		float Alpha = FMath::Clamp(signal4time / signalRunTime, 0.f, 1.f);
		FLinearColor NewColor = NoSignal_4->ColorAndOpacity;
		NewColor.A = Alpha;
		NoSignal_4->SetColorAndOpacity(NewColor);
		if (signal4time >= signalRunTime)
		{
			signal4 = false;
			signal4time = 0;
		}
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
