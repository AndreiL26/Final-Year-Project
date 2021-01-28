// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsLogger.h"

// Sets default values
APhysicsLogger::APhysicsLogger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APhysicsLogger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhysicsLogger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

