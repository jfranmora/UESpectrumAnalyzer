// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpectrumVisualizer.generated.h"

UCLASS()
class UESPECTRUMANALYZER_API ASpectrumVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpectrumVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	/* Refresh the actor */
	void Refresh();
	
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UTimeSynthComponent* TimeSynthComponent;

	UPROPERTY(EditAnywhere, Category = "Audio")
	class UTimeSynthClip* TimeSynthClip;

	UPROPERTY(EditAnywhere, Category = "Audio")
	UStaticMesh* SpectrumBarMesh;

	UPROPERTY(EditAnywhere, Category = "Audio")
	UMaterial* SpectrumBarMaterial;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<UStaticMeshComponent*> SpectrumBars;

	/* Amount of spacing between bars */
	UPROPERTY(EditAnywhere, Category = "Audio")
	float SpectrumBarSpacing;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// y = 5.220585 + 146.3705*e^(-0.1226524*x)
	static float GetNormalizedValue(float Index);

};
