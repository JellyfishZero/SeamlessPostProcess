// Copyright Epic Games, Inc. All Rights Reserved.

#include "SeamlessPostProcessCharacter.h"
#include "SeamlessPostProcessProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"


//////////////////////////////////////////////////////////////////////////
// ASeamlessPostProcessCharacter

ASeamlessPostProcessCharacter::ASeamlessPostProcessCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetCapsuleComponent());

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void ASeamlessPostProcessCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	for (FSoftObjectPath& Asset : TestAssets)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, Asset.ToString());
		UKismetSystemLibrary::PrintString(this, *FString::Printf(TEXT("%s"), *Asset.ToString()), true, true, FLinearColor(0.0, 0.66, 1.0), 20.f);
	}

	FStreamableManager& AssetLoader = UAssetManager::GetStreamableManager();
	AssetLoader.RequestAsyncLoad(TestAssets, FStreamableDelegate::CreateUObject(this, &ASeamlessPostProcessCharacter::AnimAssetsDeferred));
	//AnimAssetsDeferred();
}

void ASeamlessPostProcessCharacter::AnimAssetsDeferred()
{
	for (FSoftObjectPath SoftObj : TestAssets)
	{
		TSoftObjectPtr<UStaticMesh> AnimAsset(SoftObj);

		UStaticMesh* StaticMeshObj = AnimAsset.Get();
		if (StaticMeshObj)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, AnimObj->GetName());
			UKismetSystemLibrary::PrintString(this, *FString::Printf(TEXT("%s"), *StaticMeshObj->GetName()), true, true, FLinearColor(0.0, 0.66, 1.0), 20.f);
			StaticMesh->SetStaticMesh(StaticMeshObj);
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void ASeamlessPostProcessCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASeamlessPostProcessCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASeamlessPostProcessCharacter::Look);
	}
}


void ASeamlessPostProcessCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ASeamlessPostProcessCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASeamlessPostProcessCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ASeamlessPostProcessCharacter::GetHasRifle()
{
	return bHasRifle;
}