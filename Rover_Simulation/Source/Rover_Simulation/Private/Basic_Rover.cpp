/*
	Copyright [2021] [Andrei Lazar]

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
 */

#include "Basic_Rover.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Math/Quat.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

ABasic_Rover::ABasic_Rover() 
{
	// Allows the individual scaling of the wheels and the chasis.
	DummnyRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DummnyRootComponent"));
	DummnyRootComponent->SetWorldRotation(FRotator(0, 0, 0));
	SetRootComponent(DummnyRootComponent);
	// Create and attach the Static Mesh represting the 'chassis' of the rover.
	RoverChassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoverChassis"));
	RoverChassis->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Shapes/Shape_Cube.Shape_Cube")).Object);
	RoverChassis->SetWorldScale3D(FVector(2, 1, 0.2));
	// This is the case because in UE4  when you set a cube at location 0, 0, 0, it sits above the ground plane(basically taking the measurement from the 'bottom of the object) whilst in Gazebo in takes the measurement from the center of the object. Therefore 0.5 in gazebo = 50 - z_scale*100/2 in UE4.
	RoverChassis->SetRelativeLocation(FVector(0, 0, 45));
	// Initialize basic Physics properties for the Mesh.
	RoverChassis->SetSimulatePhysics(true);
	RoverChassis->SetMassOverrideInKg("RoverChassis", 10.0f, true);
	RoverChassis->SetLinearDamping(0.0f);
	RoverChassis->SetAngularDamping(0.0f);
	RoverChassis->SetupAttachment(RootComponent);

	// Create and attach the front wheels.
	FrontLeftWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrontLeftWheel"));
	FrontLeftWheel_Constraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("FrontLeftWheel_Constraint"));
	AttachWheel(FrontLeftWheel, FVector(65, 35, 15));

	FrontRightWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrontRightWheel"));
	FrontRightWheel_Constraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("FrontRightWheel_Constraint"));
	AttachWheel(FrontRightWheel, FVector(65, -35, 15));

	// Create and attach the back wheels.
	BackLeftWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackLeftWheel"));
	BackLeftWheel_Constraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("BackLeftWheel_Constraint"));
	AttachWheel(BackLeftWheel, FVector(-65, 35, 15));

	BackRightWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackRightWheel"));
	BackRightWheel_Constraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("BackRightWheel_Constraint"));
	AttachWheel(BackRightWheel, FVector(-65, -35, 15));

	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
}

void ABasic_Rover::AttachWheel(UStaticMeshComponent* Wheel, FVector RelativeLocation)
{
	Wheel->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Shapes/Shape_Sphere.Shape_Sphere")).Object);
	Wheel->SetupAttachment(RootComponent);
	Wheel->SetWorldScale3D(FVector(0.3, 0.3, 0.3));
	Wheel->SetSimulatePhysics(true);
	Wheel->SetLinearDamping(0.0f);
	Wheel->SetAngularDamping(0.0f);
	Wheel->SetRelativeLocation(RelativeLocation);
	// Set the mass of the wheel to 1kg.
	Wheel->SetMassOverrideInKg();
}

void ABasic_Rover::SetupConstraint(UStaticMeshComponent* Wheel, UPhysicsConstraintComponent* Constraint, FVector RelativeLocation) {
	// Note that by default all linear limits of the constraint are set to Locked.
	UE_LOG(LogTemp, Warning, TEXT("Okay:%f,%f,%f"), (RelativeLocation + FVector(0, 0, 30)).X, (RelativeLocation + FVector(0, 0, 30)).Y, (RelativeLocation + FVector(0, 0, 30)).Z);
	Constraint->SetRelativeLocation(RelativeLocation + FVector(0, 0, 30));
	Constraint->ConstraintActor1 = this;
	Constraint->ConstraintActor2 = this;
	Constraint->SetConstrainedComponents(Wheel, Wheel->GetMemberNameChecked_StaticMesh(), RoverChassis, RoverChassis->GetMemberNameChecked_StaticMesh());

	Constraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Free, 0);
	Constraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0);
	Constraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
}



void ABasic_Rover::BeginPlay()
{
	Super::BeginPlay();

	SetupConstraint(FrontLeftWheel, FrontLeftWheel_Constraint, FVector(65, 35, 10));
	SetupConstraint(FrontRightWheel, FrontRightWheel_Constraint, FVector(65, -35, 10));
	SetupConstraint(BackLeftWheel, BackLeftWheel_Constraint, FVector(-65, 35, 10));
	SetupConstraint(BackRightWheel, BackRightWheel_Constraint, FVector(-65, -35, 10));
}

void ABasic_Rover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bHasAppliedForce && TotalTime > 1.0f) {
		RoverChassis->AddForce(ForceToApply);
		bHasAppliedForce = false;

	}
	TotalTime += DeltaTime;
	UE_LOG(LogTemp, Warning, TEXT("Chassis Velocity:%f,%f,%f"), RoverChassis->GetPhysicsLinearVelocity().X, RoverChassis->GetPhysicsLinearVelocity().Y, RoverChassis->GetPhysicsLinearVelocity().Z);
	
	/*
	FrontLeftWheel->AddForce(ForceToApply);
	FrontRightWheel->AddForce(ForceToApply);

	BackLeftWheel->AddForce(ForceToApply);
	BackRightWheel->AddForce(ForceToApply);
	*/
}
