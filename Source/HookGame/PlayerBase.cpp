// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "DrawDebugHelpers.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIBehaviorBase.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SceneView.h"
#include "DirectXMath.h"
#include "HookGameGameModeBase.h"
#include "HookGameInstance.h"
#include "PowerUpActor.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "NavigationPath.h"
#include "GrappleTraceComponent.h"
#include "GrappleTracer.h"

typedef APlayerBase THIS;

constexpr auto CharacterTopBone = TEXT("Head");
constexpr auto CharacterBotBone = TEXT("Toes_R_end");
constexpr auto CharacterSpine2 = TEXT("Head");
constexpr auto CharacterHips = TEXT("Belly");
constexpr auto CharacterSwordSocket = TEXT("SwordSocket");
constexpr auto CharacterHookSpawnPoint = TEXT("HookSocket");

#define CreateObjectAndLog(Name, Type) \
Name = CreateDefaultSubobject<USkillTree>(TEXT(#Name)); \
if (!SkillTreeFuckUnreal) \
	UE_LOG(LogTemp, Error, TEXT("Failed to create; %s"), *FString(#Name));

#ifdef _DEBUG_ALL
#define _DEBUG_VAULTING
#define _DEBUG_ANIM_NOTIFY
#define _DEBUG_GRAPPLE
#define _DEBUG_ANIM_STATES
#define _DEBUG_UPDATE_CAMERA
#endif
//#define _DEBUG_UPDATE_CAMERA
//#define _DEBUG_GRAPPLE
//#define _DEBUG_GRAPPLE
//#define _DEBUG_ANIM_NOTIFY

APlayerBase::APlayerBase()
{
	LogWarning("Playerbase recreated");
	CreateComponents();
	CharacterLoad();
	InteractLoad();
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	const auto PlayerController = Cast<APlayerControllerBase>(Controller);
	assert(PlayerController);

	CharacterInit();
	WidgetInit(PlayerController);
	GrappleInit();
	SwordInit();
	CameraInit();
	ShadowProjectionInit();
	InteractInit(PlayerController);
	VaultInit();

	if (SkillTreeFCRI)
		SkillTreeFCRI->Init(this, PlayerController);

	if(bRestartGame)
	{
		bRestartGame = false;
		/*LoadCheckpoint();*/
	}
	
	LoadPlayerData();
}
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CameraUpdate(DeltaTime);
	HookUpdate(DeltaTime);
	VaultingUpdate(DeltaTime);
	CursorUpdate(DeltaTime);
	DebugUpdate(DeltaTime);
}

void APlayerBase::CameraUpdate(const float DeltaTime)
{
	CameraPositionUpdate(DeltaTime);
	CameraRotationUpdate(DeltaTime);
}

void APlayerBase::CameraPositionUpdate(const float DeltaTime) const
{
	const auto PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController)
		return;

	float x, y;
	if (!PlayerController->GetMousePosition(x, y))
		return;

#ifdef _DEBUG_UPDATE_CAMERA
	//UE_LOG(LogTemp, Warning, TEXT("Mouse X: %f\nMouse Y: %f"), x, y);
#endif
	int32 Width, Height;
	PlayerController->GetViewportSize(Width, Height);
	const float FWidth = static_cast<float>(Width);
	const float FHeight = static_cast<float>(Height);
	const float XScale = x / FWidth;
	const float YScale = y / FHeight;
	const float AspectRatio = FWidth / FHeight;
	const float ScreenRatio = EdgeScale;
	
	const auto XMin = ScreenRatio;
	const auto XMax = 1.f - XMin;
	const auto YMin = ScreenRatio * AspectRatio;
	const auto YMax = 1.f - YMin;

	struct Bounds
	{
		Bounds() :XMin(0.f), XMax(0.f), YMin(0.f), YMax(0.f) {};
		Bounds(const float XMin, const float XMax, const float YMin, const float YMax)
			:XMin(XMin), XMax(XMax), YMin(YMin), YMax(YMax) {};
		float XMin, XMax, YMin, YMax;
		bool Intersect(const float MouseX, const float MouseY) const
		{
			return MouseX >= XMin && MouseX <= XMax && MouseY >= YMin && MouseY <= YMax;
		}
	}

	const MidScreen(XMin, XMax, YMin, YMax);
	if (MidScreen.Intersect(XScale, YScale))
	{
		CameraMoveToPlayer(DeltaTime);
		return;
	}

	const float YInverted = 1.f - YScale;
	const float YH = YInverted * 2.f - 1.f;
	const float XH = (XScale * 2.f) - 1.f;
	const FVector2D MousePos(XH, YH);
	const FVector2D MouseDir = MousePos / MousePos.Size();
	CameraMoveTo2DDirection(MouseDir, DeltaTime);

#ifdef _DEBUG_UPDATE_CAMERA
	UE_LOG(LogTemp, Warning, TEXT("Mouse DIR: %s"), *MouseDir.ToString());
#endif
}

void APlayerBase::CameraMoveTo2DDirection(const FVector2D& Dir, const float DeltaTime) const
{
#ifdef _DEBUG_UPDATE_CAMERA
	UE_LOG(LogTemp, Warning, TEXT("target offset: %s"), *SpringArm->TargetOffset.ToString());
#endif
	const FQuat SpringArmYawQuaternion = FRotator{0.f, SpringArm->GetRelativeRotation().Yaw, 0.f}.Quaternion();
	const FVector Dir3D = FVector(Dir.Y, Dir.X, 0.f);
	const FVector Dir3DVelocity = (Dir3D * CameraLerpToSpeed * DeltaTime);
	const FVector Dir3DVelocityTransformed = SpringArmYawQuaternion.RotateVector(Dir3DVelocity);

	FVector NewLocation = SpringArm->TargetOffset + Dir3DVelocityTransformed;
	NewLocation = FMath::VInterpConstantTo(SpringArm->TargetOffset, NewLocation, DeltaTime, CameraLerpToSpeed);
	NewLocation.Z = 0.f;

	//** This feels better than just setting it back to springarm->targetoffset**//
	if (FMath::Abs(NewLocation.X) > MaxCameraExtent.X)
	{
		const float Direction = NewLocation.X < 0.f ? -1.f : 1.f;
		NewLocation.X = FMath::FInterpTo(NewLocation.X, MaxCameraExtent.X * Direction, DeltaTime, (CameraLerpToSpeed*0.05f));
	}
	if (FMath::Abs(NewLocation.Y) > MaxCameraExtent.Y)
	{
		const float Direction = NewLocation.Y < 0.f ? -1.f : 1.f;
		NewLocation.Y = FMath::FInterpTo(NewLocation.Y, MaxCameraExtent.Y * Direction, DeltaTime, (CameraLerpToSpeed * 0.05f));
	}
	
	SpringArm->TargetOffset = NewLocation;
}

void APlayerBase::CameraMoveToPlayer(const float DeltaTime) const
{
	SpringArm->TargetOffset = FMath::Lerp(SpringArm->TargetOffset, CameraInitialPosition, CameraLerpBackSpeed* DeltaTime);
}

void APlayerBase::CameraRotationUpdate(const float DeltaTime)
{
	if (!AssociatedTriggerBox)
		return;

	auto const RotateCamera = [&DeltaTime](const FRotator &Current, const FRotator &Target, const float Speed)
	{
		return FMath::RInterpConstantTo(Current, Target, DeltaTime, Speed);	
	};
	
	if (AssociatedTriggerBox->Reverse)
	{
		const auto CameraRotation = SpringArm->GetRelativeRotation();
		const auto NewRotation = RotateCamera(CameraRotation, AssociatedTriggerBox->EndOverlapRotation, AssociatedTriggerBox->RotationSpeed);
		SpringArm->SetRelativeRotation(NewRotation);
		
		if (NewRotation.Equals(AssociatedTriggerBox->EndOverlapRotation, 1.f))
		{
			AssociatedTriggerBox->Reverse = false;
			AssociatedTriggerBox = nullptr;		
		}
	}
	else if (AssociatedTriggerBox->bOnBeginOverlapRotation)
	{
		const auto CameraRotation = SpringArm->GetRelativeRotation();	
		const auto NewRotation = RotateCamera(
			CameraRotation, 
			AssociatedTriggerBox->BeginOverlapRotation, 
			AssociatedTriggerBox->RotationSpeed
		);		
		SpringArm->SetRelativeRotation(NewRotation);
		
		if (NewRotation.Equals(AssociatedTriggerBox->BeginOverlapRotation, 1.f) && !AssociatedTriggerBox->bOnEndOverlapRotation)
		{
			AssociatedTriggerBox = nullptr;
		}
	}
}


FVector2D APlayerBase::GetPlayerToMouseDirScreenSpace2D() const
{
	const auto PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController)
		return FVector2D::ZeroVector;

	float x, y;
	if (!PlayerController->GetMousePosition(x, y))
		return FVector2D::ZeroVector;

	int32 Width, Height;
	PlayerController->GetViewportSize(Width, Height);
	const float FWidth = static_cast<float>(Width);
	const float FHeight = static_cast<float>(Height);
	const float AspectRatio = FWidth / FHeight;
	float XScale = x / FWidth;
	float YScale = y / FHeight;
	YScale = 1.f - YScale;
	YScale = (2.f * YScale) - 1.f;
	XScale = (2.f * XScale) - 1.f;
	XScale *= AspectRatio;
	const FRotator SpringArmYawRotation = { 0.f, SpringArm->GetRelativeRotation().Yaw, 0.f };
	const FQuat SpringArmYawQuaternion = SpringArmYawRotation.Quaternion();
	const FVector2D MousePos2D = { XScale,  YScale };

	FVector2D PlayerPosScreen{ FVector2D::ZeroVector };
	PlayerController->ProjectWorldLocationToScreen(GetActorLocation(), PlayerPosScreen);
	PlayerPosScreen.X /= FWidth;
	PlayerPosScreen.Y /= FHeight;
	PlayerPosScreen.Y = 1.f - PlayerPosScreen.Y;
	PlayerPosScreen.X = (PlayerPosScreen.X * 2.f) - 1.f;
	PlayerPosScreen.X *= AspectRatio;
	PlayerPosScreen.Y = (PlayerPosScreen.Y * 2.f) - 1.f;

	FVector2D PlayerToMouse = (MousePos2D - PlayerPosScreen);
	PlayerToMouse = PlayerToMouse.GetSafeNormal();
	
	FVector MouseDir3D = { PlayerToMouse.Y, PlayerToMouse.X, 0.f };
	MouseDir3D = SpringArmYawQuaternion.RotateVector(MouseDir3D);
	const FVector2D Result = { MouseDir3D.X, MouseDir3D.Y };

	
#ifdef  _DEBUG_MOUSEDIR
	UE_LOG(LogTemp, Warning, TEXT("MousePos X: %f   -    MousePos Y: %f"), MousePos2D.X, MousePos2D.Y);
	UE_LOG(LogTemp, Warning, TEXT("Player X: %f   -    Player Y: %f"), PlayerPosScreen.X, PlayerPosScreen.Y);
	UE_LOG(LogTemp, Warning, TEXT("MouseToPlayer X: %f   -    MouseToPlayer Y: %f"), PlayerToMouse.X, PlayerToMouse.Y);
	UE_LOG(LogTemp, Warning, TEXT("Result X: %f   -    Result Y: %f"), Result.X, Result.Y);
#endif
	return Result;
}

FVector APlayerBase::GetPlayerToMouseDirScreenSpace3D() const
{
	const FVector2D Dir2D = GetPlayerToMouseDirScreenSpace2D();
	return { Dir2D.X, Dir2D.Y, 0.f };
}

FRotator APlayerBase::GetPlayerToMouseYawScreenSpace() const
{
	const auto Rot = GetPlayerToMouseDirScreenSpace3D().Rotation();
	return { 0.f, Rot.Yaw, 0.f };
}

FVector APlayerBase::CalculateGrappleFireDirection() const
{
	FVector DirToTarget{ FVector::ForwardVector };
	const auto PlayerController = Cast<APlayerController>(GetController());
	if (!Hook || !PlayerController)
		return DirToTarget;


	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectQuery;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectQueryComplex;
	ObjectQuery.Push(ObjectTypeQuery8);
	ObjectQueryComplex.Push(ObjectTypeQuery10);
	
	const auto HookLocation = Hook->GetActorLocation();
	
	const auto HookToTarget = [&DirToTarget, &HookLocation](const FVector& Target)
	{
		DirToTarget = Target - HookLocation;
		return DirToTarget /= DirToTarget.Size();
	};

	FHitResult FHitCursor; //Will try to search for complex collision first because not all grapple plants will have hit boxes.
	if (PlayerController->GetHitResultUnderCursorForObjects(ObjectQueryComplex, true, FHitCursor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Complex collision hit, name: %s"), *FHitCursor.Actor->GetName());
		//Get the grapple point if it has this component
		if (auto TraceComponent = FHitCursor.GetActor()->FindComponentByClass<UGrappleTraceComponent>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Complex collision UGrappleTraceComponent hit, name: %s"), *FHitCursor.Actor->GetName());
			return HookToTarget(TraceComponent->GrapplePoint->GetComponentLocation());
		}

		if (auto TraceComponent = FHitCursor.GetActor()->FindComponentByClass<UGrappleTracer>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Complex collision UGrappleTracer hit, name: %s"), *FHitCursor.Actor->GetName());
			return HookToTarget(TraceComponent->GrapplePoint->GetComponentLocation());
		}
		
		return HookToTarget(FHitCursor.ImpactPoint);
	}
	//Do a hit box check, will not wrap this around complex collision
	if (PlayerController->GetHitResultUnderCursorForObjects(ObjectQuery, false, FHitCursor))
	{
		if (auto TraceComponent = FHitCursor.GetActor()->FindComponentByClass<UGrappleTraceComponent>())
			return HookToTarget(TraceComponent->GrapplePoint->GetComponentLocation());
		if (auto TraceComponent = FHitCursor.GetActor()->FindComponentByClass<UGrappleTracer>())
			return HookToTarget(TraceComponent->GrapplePoint->GetComponentLocation());
	}

	FVector MouseWorldLocation{FVector::ZeroVector}, MouseWorldDirection{FVector::ZeroVector};
	PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);
	MouseWorldDirection.Z *= MouseWorldDirection.Z > 0.f ? -1.f : 1.f;//Handle parallel, and it should not be positive.
	MouseWorldDirection.Z -= MouseWorldDirection.Z == 0.f ? 0.1f : 0.f;
	const FPlane Plane(FVector::UpVector, GetActorLocation().Z);
	
#ifdef _DEBUG_CALCULATE_GRAPPLE_FIRE_DIRECTION
	DrawDebugLine(GetWorld(), MouseWorldLocation, MouseWorldLocation + MouseWorldDirection* 2000.f, FColor::Green, true, 5.f, 0, 2.f);
#endif	
	const auto PlaneImpactPoint = FMath::RayPlaneIntersection(MouseWorldLocation, MouseWorldDirection, Plane);
#ifdef _DEBUG_CALCULATE_GRAPPLE_FIRE_DIRECTION
	DrawDebugPoint(GetWorld(), PlaneImpactPoint, 10.f, FColor::Red, true, 2.f, 0);
#endif
	auto HookToMouseDir = HookToTarget(PlaneImpactPoint);
	const float StartPos = 300.f;
	const auto TraceStart = HookLocation + HookToMouseDir * StartPos;
	const auto TraceEnd = HookLocation + HookToMouseDir * (HookMaxRange - StartPos);
	
	//Aim assist
	const FVector& BoxHalfSize = AimAssistBoundingBox;
	const TArray<AActor*> IgnoreActors; TArray<FHitResult> HitActors;
	
	if (bShowAimAssistBoundingBox)
		UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), TraceStart, TraceEnd, BoxHalfSize, HookToMouseDir.Rotation(), ObjectQuery, false, IgnoreActors, EDrawDebugTrace::ForDuration, HitActors, false, FLinearColor::Red, FLinearColor::Green, 1.f);
	else
		UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), TraceStart, TraceEnd, BoxHalfSize, HookToMouseDir.Rotation(), ObjectQuery, false, IgnoreActors, EDrawDebugTrace::None, HitActors, false, FLinearColor::Red, FLinearColor::Green, 1.f);

	float MinDistance = FLT_MAX;
	AActor* BlockingActor{ nullptr };
	for (const auto Hit : HitActors)
	{
		const auto ImpactPoint = Hit.ImpactPoint;
		auto DistanceToLine = FMath::ClosestPointOnLine(TraceStart, TraceEnd, ImpactPoint).Size();
		if (DistanceToLine < MinDistance)
		{
			MinDistance = DistanceToLine;
			BlockingActor = Hit.GetActor();
		}
	}
	if (BlockingActor)
	{
		
		const UGrappleTraceComponent* GTC = BlockingActor->FindComponentByClass<UGrappleTraceComponent>();
		if (GTC)
			return HookToTarget(GTC->GrapplePoint->GetComponentLocation());
		
		const UGrappleTracer* GT = BlockingActor->FindComponentByClass<UGrappleTracer>();
		if (GT)
			return HookToTarget(GT->GrapplePoint->GetComponentLocation());
			
	}
	
	return HookToMouseDir;
	
}
void APlayerBase::VaultingUpdate(const float DeltaTime)
{
	if (CustomMovementModes::Vaulting == _CustomMovementMode)
	{
		const FVector CharacterTopPoint = GetMesh()->GetBoneLocation(CharacterTopBone);
		const FVector CharacterLowPoint = GetMesh()->GetBoneLocation(CharacterBotBone);
		const float CharacterHeight = (CharacterTopPoint - CharacterLowPoint).Size();
		const float CharacterHalfHeight = CharacterHeight * 0.5f;
		GetCapsuleComponent()->SetCapsuleHalfHeight(CharacterHalfHeight);
	}
}

void APlayerBase::TryEnterVaultingTransition(const FVector& EndPoint)
{
	if (CompareCustomMovement(CustomMovementModes::Default) || CompareCustomMovement(CustomMovementModes::Grappling))
	{
		const FVector DirToVaultTarget = EndPoint - GetActorLocation();
		const FRotator RotationToTarget = (DirToVaultTarget / DirToVaultTarget.Size()).Rotation();
		SetActorRotation({ 0.f, RotationToTarget.Yaw, 0.f });
		SetCustomMovement(CustomMovementModes::Vaulting);
	}
}

void APlayerBase::StartVaulting()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	SetCustomMovement(CustomMovementModes::Vaulting);

	if (!CompareHookState(HookCurrentState::GrappleDetached))
		HookDetach();
}

void APlayerBase::EndVaulting()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleDefaultStandingHeight);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if (CompareCustomMovement(CustomMovementModes::Vaulting))
		SetCustomMovement(CustomMovementModes::Default);
}

void APlayerBase::SwordStartAttack()
{
	if (SwordPtr && CompareCustomMovement(CustomMovementModes::Attacking))
	{
		SetAttackState(AttackState::Attack1);
		SwordPtr->StartSwing();
		HookDelayTimer = 0.f;
		GetCharacterMovement()->MaxWalkSpeed = SwordAttackWalkSpeed;
	}
}

void APlayerBase::SwordEndAttack()
{
#ifdef _DEBUG_SWORD
	UE_LOG(LogTemp, Warning, TEXT("Sword attack end"));
#endif

	GetCharacterMovement()->MaxWalkSpeed = InitialPlayerSpeed;
	if (SwordPtr && CompareCustomMovement(CustomMovementModes::Attacking))
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		SwordPtr->EndSwing();

		AttackPressedWhileSwinging = 0;
		AttackCurrentComboNR = 0;
		HookDelayTimer = HookMaxDelay;

		SetCustomMovement(CustomMovementModes::Default);
		SetAttackStateTransition(AttackStateTransition::Default);
		SetAttackState(AttackState::Default);
	}
}
void APlayerBase::OnBeginCameraTrigger(ACameraRotateTrigger* Trigger)
{
	if (AssociatedTriggerBox)
		AssociatedTriggerBox->ResetToDefault();
	AssociatedTriggerBox = Trigger;
}

void APlayerBase::OnEndCameraTrigger(ACameraRotateTrigger* Trigger) const
{
	if (AssociatedTriggerBox == Trigger)
		AssociatedTriggerBox->Reverse = true;
	else
		Trigger->Reverse = false;
}

void APlayerBase::OnVaultingBegin_TransitionStart()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingBegin_TransitionStart"));
#endif
	StartVaulting();
}

void APlayerBase::OnVaultingBegin_TransitionInterrupted()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingBegin_TransitionInterrupted"));
#endif
	EndVaulting();
}

void APlayerBase::OnVaultingBegin_TransitionEnd()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingBegin_TransitionEnd"));
#endif
}

void APlayerBase::OnVaultingEnd_TransitionStart()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingEnd_TransitionStart"));
#endif
	EndVaulting();
}

void APlayerBase::OnVaultingEnd_TransitionInterrupted()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingEnd_TransitionInterrupted"));
#endif
}

void APlayerBase::OnVaultingEnd_TransitionEnd()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnVaultingEnd_TransitionEnd"));
#endif
}

void APlayerBase::TryEnterSwordAttackTransition()
{	
	if (!CompareHookState(HookCurrentState::GrappleDetached))
		return;
	
	if (CompareCustomMovement(CustomMovementModes::Attacking))
		HandleAttackCombo();
	
	if (CompareCustomMovement(CustomMovementModes::Default) && !GetCharacterMovement()->IsFalling())
		SetCustomMovement(CustomMovementModes::Attacking);
}

void APlayerBase::OnSwordAttackBegin_TransitionStart()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("Sword Attack Begin Transition Start"));
#endif
	SwordStartAttack();
}

void APlayerBase::OnSwordAttackBegin_TransitionInterrupted()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("Sword Attack Begin Transition Interrupted"));
#endif
	SwordEndAttack();
}

void APlayerBase::OnSwordAttackBegin_TransitionEnd()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("OnSwordAttackBegin_TransitionEnd"));
#endif
}

void APlayerBase::OnSwordAttackEnd_TransitionStart()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("Sword Attack End Transition Start"));
#endif
	SwordEndAttack();
}

void APlayerBase::OnSwordAttackEnd_TransitionInterrupted()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("Sword Attack End Transition Interrupted"));
#endif
	SwordEndAttack();
}

void APlayerBase::OnSwordAttackEnd_TransitionEnd()
{
#ifdef _DEBUG_ANIM_NOTIFY
	UE_LOG(LogTemp, Warning, TEXT("Sword Attack End Transition End"));
#endif
	SwordEndAttack();
}
void APlayerBase::SetAttackStateTransition(AttackStateTransition::AttackStateTransition NewState)
{
	_AttackStateTransition = NewState;
}

void APlayerBase::SetAttackState(AttackState::AttackState NewState)
{
	_AttackState = NewState;
}

bool APlayerBase::CompareAttackState(AttackState::AttackState NewState) const
{
	return _AttackState == NewState;
}

bool APlayerBase::CompareAttackStateTransition(AttackStateTransition::AttackStateTransition NewState) const
{
	return NewState == _AttackStateTransition;
}

void APlayerBase::HandleAttackCombo()
{
	if (!SwordPtr)
		return;

	AttackPressedWhileSwinging++;
	
	switch (_AttackState)
	{
		case AttackState::Attack1:
		{
			SetAttackStateTransition(AttackStateTransition::Attack2);
			AttackCurrentComboNR++;
			SwordPtr->ComboSwing();
		}
		break;
		case AttackState::Attack2:
		{
			SetAttackStateTransition(AttackStateTransition::Attack1);
			AttackCurrentComboNR++;
			SwordPtr->ComboSwing();
		}
		break;
		default:
		break;
	}
}

void APlayerBase::OnAttack1End()
{
	if (CompareAttackStateTransition(AttackStateTransition::Attack2))
		SetAttackState(AttackState::Attack2);
}

void APlayerBase::OnAttack2End()
{
	if (CompareAttackStateTransition(AttackStateTransition::Attack1))
		SetAttackState(AttackState::Attack1);
}

bool APlayerBase::IsAttack1() const
{
	return CompareAttackState(AttackState::Attack1);
}

bool APlayerBase::CanTransitionToAttack1() const
{
	return CompareAttackStateTransition(AttackStateTransition::Attack1);
}

bool APlayerBase::IsAttack2() const
{
	return CompareAttackState(AttackState::Attack2);
}

void APlayerBase::HookUpdate(const float DeltaTime)
{
	if (HookCurrentState::GrappleDetached == _HookCurrentState)
	{
		HookDelayTimer += DeltaTime;
		return;
	}

	if (!Hook)
		return;
	
	//Out of range scenario
	if (HookMaxRange < GetDistanceToHook() && !CompareHookState(HookCurrentState::GrappleTo))
		SetHookState(HookCurrentState::GrappleReturn);

	if (HookDetachTimer < 0.f)
	{
		HookDelete();
		return;
	}	
	HookDetachTimer -= DeltaTime;

	switch (_HookCurrentState)
	{
		case HookCurrentState::GrappleTo:
		{
			GrappleTo(DeltaTime);
		}
		break;
		case HookCurrentState::GrappleReturn:
		{
			GrappleReturn(DeltaTime);	
		}
		break;
		default:
		break;
	}

	
}

void APlayerBase::HookFire()
{
	if (!Hook || !CompareHookState(HookCurrentState::GrappleFired))
		return;
	
	HookDelayTimer = 0.f;
	const auto PlayerController = Cast<APlayerController>(GetController());
	ReturnIfNull(PlayerController);

#ifdef SREENSPACEGRAPPLEDIR
	const auto MouseToPlayerDir = GetMouseScreenDirection();
	const FVector Direction = { MouseToPlayerDir.X, MouseToPlayerDir.Y, 0.f };
#else
	const auto Dir = CalculateGrappleFireDirection();
	//UE_LOG(LogTemp, Warning, TEXT("Fired: %s"), *Dir.ToString());
#endif
		
	ShadowProjectionCamera->HideActorComponents(Hook);
	Hook->GetCollisionComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Hook->GetProjectileMovementComponent()->bRotationFollowsVelocity = true;
	Hook->GetProjectileMovementComponent()->SetUpdatedComponent(Hook->GetCollisionComponent());
	Hook->SetMeshRotation({ 90.f, 0.f, 180.f });
	Hook->ShootDirection(Dir);
	GrappleCable->SetVisibility(true);
	SetHookState(HookCurrentState::GrappleInAir);
	Hook->GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

#ifdef _DEBUG_GRAPPLE
			UE_LOG(LogTemp, Error, TEXT("Shooting attached component"));
#endif	
}

void APlayerBase::TryEnterHookFireTransition()
{
	if (!CompareCustomMovement(CustomMovementModes::Default))
		return;

	if (HookDelayTimer <= HookMaxDelay)
		return;

	SetHookState(HookCurrentState::GrappleFired);
	SetCustomMovement(CustomMovementModes::Grappling);
}

void APlayerBase::GrappleTo(const float DeltaTime)
{
	const FRotator Rotation = { 0.f, GetRotationToHook().Yaw, 0.f };
	SetActorRotation(Rotation);
	CalculatePlayerVelocityWhileRetracting(DeltaTime);

	if (GetDistanceToHook() < HookInRange)
		HookDelete();
}

void APlayerBase::GrappleReturn(const float DeltaTime)
{
	FVector DirectionToHook = GetDirectionToHook();
	DirectionToHook /= DirectionToHook.Size();

	Hook->GetCollisionComponent()->SetWorldRotation(DirectionToHook.Rotation());
	Hook->GetProjectileMovementComponent()->Velocity = -DirectionToHook * (HookReturnSpeed + GetCharacterMovement()->Velocity.Size());

	if (GetDistanceToHook() < HookInRange)
		HookDelete();
}

bool APlayerBase::CompareHookState(HookCurrentState::HookCurrentState NewState) const
{
	return NewState == _HookCurrentState;
}

void APlayerBase::SetHookState(HookCurrentState::HookCurrentState NewState)
{
	_HookCurrentState = NewState;
}

HookCurrentState::HookCurrentState APlayerBase::GetHookState() const
{
	return _HookCurrentState;
}

void APlayerBase::SetCustomMovement(CustomMovementModes::CustomMovementModes NewState)
{
	_CustomMovementMode = NewState;
}

CustomMovementModes::CustomMovementModes APlayerBase::GetCustomMovement() const
{
	return _CustomMovementMode;
}

bool APlayerBase::CompareCustomMovement(CustomMovementModes::CustomMovementModes NewState) const
{
	return _CustomMovementMode == NewState;
}

void APlayerBase::GrappleInit()
{
	HookDelayTimer = 0.f;
	HookDetachTimer = HookDetachMaxTimer;
	//Cable Component Related
	GrappleCable->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CharacterHookSpawnPoint);
	GrappleCable->SetVisibility(false);
	GrappleCable->bEnableStiffness = true;
	GrappleCable->NumSegments = 2.f;
	GrappleCable->SetEnableGravity(false);
	
	FActorSpawnParameters Params; const FHitResult Hit;
	Params.Owner = this;
	Params.Instigator = this;
	Hook = GetWorld()->SpawnActor<AHookProjectile>(HookProjectileClass, Params);
	assert(Hook);
	Hook->SetSpeed(HookFireSpeed);
	Hook->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CharacterHookSpawnPoint);
	Hook->SetMeshRotation({ 90.f, 0.f, 270.f });
	Hook->GetProjectileMovementComponent()->StopSimulating(Hit);
	GrappleCable->SetAttachEndToComponent(Hook->GetCableAttachmentPoint());
	GrappleCable->SetVisibility(false);
}

void APlayerBase::WidgetInit(_Inout_ APlayerController* PlayerController)
{
	PlayerController->bShowMouseCursor = true;
	WidgetCursorNotLocked = CreateWidget(PlayerController, WidgetClassCursorNoneLock);
	WidgetCursorLocked = CreateWidget(PlayerController, WidgetClassCursorLocked);
	WidgetMenu = CreateWidget(PlayerController, WidgetClassMenu);
	WidgetSword = CreateWidget(PlayerController, WidgetClassSword);
	
	PlayerController->SetMouseCursorWidget(EMouseCursor::Default, WidgetMenu);
	PlayerController->SetMouseCursorWidget(EMouseCursor::Crosshairs, WidgetCursorLocked);
	PlayerController->SetMouseCursorWidget(EMouseCursor::CardinalCross, WidgetSword);
	PlayerController->SetMouseCursorWidget(EMouseCursor::Hand , WidgetCursorNotLocked);
	
	PlayerController->DefaultMouseCursor = EMouseCursor::Default;
	PlayerController->CurrentMouseCursor = EMouseCursor::Default;
}

void APlayerBase::SetMouseCursor(APlayerController* PlayerController, const EMouseCursor::Type NewType)
{
	PlayerController->CurrentMouseCursor = NewType;
}

void APlayerBase::InteractInit(APlayerControllerBase* PlayerController)
{
	const auto PlayerControllerHUDRef = PlayerController->HUD;	
	if(PlayerControllerHUDRef)
	{
		DialogueUIPtr = Cast<UDialogueUI>(CreateWidget(PlayerController, DialogueUIWidget));
	}
	
	if(InteractionPopup)
	{
		InteractionWidget = CreateWidget(PlayerController, InteractionPopup);
		if(InteractionWidget)
		{
			InteractionWidget->AddToViewport();
			InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
#ifdef _DEBUG_INERACTION
			UE_LOG(LogTemp, Error, TEXT("NPC Interaction Widget not set"));
#endif
		}		
	}
}

void APlayerBase::CameraInit()
{
	CameraInitialPosition = SpringArm->TargetOffset;
	CameraInitialRotation = SpringArm->GetRelativeRotation();
}

void APlayerBase::CharacterInit()
{
	CurrentHealth = MaxHealth;
	GetCharacterMovement()->AirControl = DefaultAirControl;
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleDefaultStandingHeight);
	GetCapsuleComponent()->SetCapsuleRadius(CapsuleDefaultRadius);
	InitialPlayerSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void APlayerBase::VaultInit()
{	
	VaultBoxExtent->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::OnVaultBoxBeginOverlap);
}

void APlayerBase::SwordInit()
{
	SwordPtr = GetWorld()->SpawnActor<APlayerSword>(SwordClass);
	if (SwordPtr)
	{
		SwordPtr->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CharacterSwordSocket);
		SwordPtr->SetActorHiddenInGame(false);
		SwordPtr->SetOwner(this);
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn sword"));
	}
}

void APlayerBase::ShadowProjectionInit()
{
	TArray<AActor*> CaptureActors, HiddenActors;
	TArray<TWeakObjectPtr<UPrimitiveComponent>> HiddenComponents;
	const float ShadowMapTextureSize = 512.f;
	
	CaptureActors.Push(this);
	CaptureActors.Push(SwordPtr);
	CaptureActors.Push(Hook);
	HiddenActors.Push(this);
	HiddenActors.Push(SwordPtr);
	HiddenActors.Push(Hook);
	HiddenComponents.Push(GrappleCable);
	ShadowProjectionTexture->SizeX = ShadowMapTextureSize;
	ShadowProjectionTexture->SizeY = ShadowMapTextureSize;
	ShadowProjectionTexture->ClearColor = FLinearColor(0.f, 0.f, 0.f, 1.f);
	ShadowProjectionTexture->AddressX = TextureAddress::TA_Clamp;
	ShadowProjectionTexture->AddressY = TextureAddress::TA_Clamp;
	ShadowProjectionTexture->RenderTargetFormat = ETextureRenderTargetFormat::RTF_R32f;
	
	DepthProjectionTexture->SizeX = ShadowProjectionTexture->SizeX;
	DepthProjectionTexture->SizeY = ShadowProjectionTexture->SizeY;
	DepthProjectionTexture->ClearColor = ShadowProjectionTexture->ClearColor;
	DepthProjectionTexture->AddressX = ShadowProjectionTexture->AddressX;
	DepthProjectionTexture->AddressY = ShadowProjectionTexture->AddressY;
	DepthProjectionTexture->RenderTargetFormat = ShadowProjectionTexture->RenderTargetFormat;
	
	ShadowProjectionCamera->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	ShadowProjectionCamera->ProjectionType = ECameraProjectionMode::Orthographic;
	ShadowProjectionCamera->CaptureSource = ESceneCaptureSource::SCS_SceneDepth;
	ShadowProjectionCamera->ShowOnlyActors = CaptureActors;
	ShadowProjectionCamera->HiddenComponents = HiddenComponents;
	ShadowProjectionCamera->OrthoWidth = ShadowMapTextureSize;
	ShadowProjectionCamera->CustomNearClippingPlane = 0.f;
	ShadowProjectionCamera->bCaptureEveryFrame = true;
	ShadowProjectionCamera->bCaptureOnMovement = false;
	ShadowProjectionCamera->MaxViewDistanceOverride = 10000.f;
	ShadowProjectionCamera->TextureTarget = ShadowProjectionTexture;
	
	DepthProjectionCamera->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	DepthProjectionCamera->ProjectionType = ShadowProjectionCamera->ProjectionType;
	DepthProjectionCamera->CaptureSource = ShadowProjectionCamera->CaptureSource;
	DepthProjectionCamera->HiddenActors = HiddenActors;
	DepthProjectionCamera->OrthoWidth = ShadowProjectionCamera->OrthoWidth;
	DepthProjectionCamera->CustomNearClippingPlane = ShadowProjectionCamera->CustomNearClippingPlane;
	DepthProjectionCamera->bCaptureEveryFrame = ShadowProjectionCamera->bCaptureEveryFrame;
	DepthProjectionCamera->bCaptureOnMovement = ShadowProjectionCamera->bCaptureOnMovement;
	DepthProjectionCamera->MaxViewDistanceOverride = ShadowProjectionCamera->MaxViewDistanceOverride;
	DepthProjectionCamera->TextureTarget = DepthProjectionTexture;
}

void APlayerBase::InteractLoad()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::InteractionActive);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerBase::InteractionNotActive);

	bPlayerIsTalking = false;
	PlayerCanInteract = false;
	AssociatedNPC = nullptr;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(FName("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());

}

void APlayerBase::CharacterLoad()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Make Character move towards input direction
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
}

void APlayerBase::HandleDeath()
{
	//GetCharacterMovement()->DisableMovement();
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	const auto PController = Cast<APlayerController>(GetController());
	if (PController)
	{
		DisableInput(PController);
		DeadScreen = CreateWidget(PController, DeadScreenClass);
		DeadScreen->AddToViewport();
	}
	GetWorldTimerManager().SetTimer(MyTimerHandle, this, &APlayerBase::RestartLevel, 3.f, false);
}

void APlayerBase::RestartLevel()
{
	const auto PController = Cast<APlayerController>(GetController());
	if (DeadScreen)
	{
		if (DeadScreen->IsInViewport())
			DeadScreen->RemoveFromViewport();
	}
	
	if (PController)
	{
		bRestartGame = true;
		LoadCheckpoint();
		/*PController->RestartLevel();*/
	}

	

}

void APlayerBase::HookDetach()
{
	if (HookCurrentState::GrappleDetached == _HookCurrentState || HookCurrentState::GrappleFired == _HookCurrentState || HookCurrentState::GrappleReturn == _HookCurrentState)
		return;
#ifdef _DEBUG_GRAPPLE
	UE_LOG(LogTemp, Warning, TEXT("HookDetach"));
#endif
	if (Hook)
	{
		Hook->GetProjectileMovementComponent()->SetUpdatedComponent(Hook->GetCollisionComponent());
		Hook->GetProjectileMovementComponent()->bRotationFollowsVelocity = false;

		GrappleDetachPhysicsComponent();			
	}
	_HookCurrentState = HookCurrentState::GrappleReturn;
}

void APlayerBase::GrappleDetachPhysicsComponent()
{
	ReturnIfNull(AttachedComponent);
	
	AttachedComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	const auto Actor = AttachedComponent->GetOwner();
	if (Actor)
		if (const auto AIBB = Cast<AAIBehaviorBase>(Actor))
			AIBB->OnDetach();

	AttachedComponent = nullptr;		
}

void APlayerBase::OnFirePressed()
{
	TryEnterSwordAttackTransition();
}

FVector APlayerBase::GetDirectionToHook() const
{
	if (!Hook) return FVector::ZeroVector;
	const FVector HookLocation = Hook->GetCollisionComponent()->GetComponentLocation();
	const FVector ThisLocation = GetActorLocation();
	return HookLocation - ThisLocation;
}

FRotator APlayerBase::GetRotationToHook() const
{
	return GetDirectionToHook().Rotation();
}

float APlayerBase::GetDistanceToHook() const
{
	return GetDirectionToHook().Size();
}

void APlayerBase::HookDelete()
{
#ifdef _DEBUG_GRAPPLE
	UE_LOG(LogTemp, Warning, TEXT("HookDetach"));
#endif
	
	HookDetachTimer = HookDetachMaxTimer;
	GrappleCable->SetVisibility(false);
	const FHitResult Hit;
	ShadowProjectionCamera->ClearHiddenComponents();
	ShadowProjectionCamera->HideComponent(GrappleCable);
	Hook->GetProjectileMovementComponent()->StopSimulating(Hit);
	
	Hook->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CharacterHookSpawnPoint);
	Hook->SetMeshRotation({ 90.f, 0.f, 270.f });
	GetCharacterMovement()->AirControl = DefaultAirControl;
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;

	GrappleDetachPhysicsComponent();
	
	_HookCurrentState = HookCurrentState::GrappleDetached;
	if (CustomMovementModes::Grappling == _CustomMovementMode)
	{
		_CustomMovementMode = CustomMovementModes::Default;
	}
}

void APlayerBase::CalculatePlayerVelocityWhileRetracting(const float DeltaTime)
{
	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		GetCharacterMovement()->AirControl = 0.0f;
		GetCharacterMovement()->GroundFriction = 0.f;
		GetCharacterMovement()->BrakingFriction = 0.f;
		GetCharacterMovement()->FallingLateralFriction = 0.f;
	}
	const FVector PlayerVelocity = GetCharacterMovement()->Velocity;
	const FVector DirectionToHook = GetDirectionToHook();
	FVector DirToHookUnit = DirectionToHook / DirectionToHook.Size();
	const FVector PlayerVelocityDir = PlayerVelocity / PlayerVelocity.Size();
	
	
	if (PlayerVelocity.IsNearlyZero())
	{
#ifdef _DEBUG_GRAPPLE
		UE_LOG(LogTemp, Warning, TEXT("PlayerVelocity:nearly zero"));
#endif
		LaunchCharacter(GetActorUpVector() * HookOnAttachedLaunchForce, false, false);
		GetCharacterMovement()->Velocity += DirToHookUnit * HookRetractStartSpeed;
		return;
	}
#ifdef _DEBUG_GRAPPLE
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (PlayerVelocityDir * 200.f), FColor::Blue, false, -1, 0, 2.f);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (DirToHookUnit * 200.f), FColor::Red, false, -1, 0, 2.f);
#endif
	const float ProjectedSpeed = Dot3(PlayerVelocity, DirToHookUnit);
#ifdef _DEBUG_GRAPPLE
	//UE_LOG(LogTemp, Warning, TEXT("Currentspeed: %f"), CurrentSpeed);
	UE_LOG(LogTemp, Warning, TEXT("ProjectedSpeed: %f"), ProjectedSpeed);
#endif

#ifdef _DEBUG_GRAPPLE
	//UE_LOG(LogTemp, Warning, TEXT("DotProj: %f"), DotProj);
#endif
	const float DistanceToHook = DirectionToHook.Size();
	const float x = DistanceToHook / InitialDistanceToHook;
	const float a = 300.f;
	const float b = 1000.f;
	const float c = 0;
	const float WishSpeed = QuadraticFormula(x, a, b, c);
	
	GetCharacterMovement()->Velocity += DirToHookUnit * WishSpeed;
	
	if (ProjectedSpeed > HookRetractMaxSpeed)
	{
		GetCharacterMovement()->Velocity *= 0.8f;
	}
	
#ifdef _DEBUG_GRAPPLE
	UE_LOG(LogTemp, Warning, TEXT("WishSpeed: %f"), WishSpeed);
	UE_LOG(LogTemp, Warning, TEXT("DistanceToHook: %f"), DistanceToHook);
#endif
	
	if (DistanceToHook < 150.f)
	{
#ifdef _DEBUG_GRAPPLE
		UE_LOG(LogTemp, Warning, TEXT("Reducing speed"));
#endif
		GetCharacterMovement()->Velocity *= 0.1f;
	}	
}

float APlayerBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                              AActor* DamageCauser)
{
	const auto IncomingDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//OnPlayerKnockBack(DamageCauser->GetActorLocation(), PlayerOnKnockBackValue);
	
	CurrentHealth -= FMath::Min(CurrentHealth, IncomingDamage);
	if (CurrentHealth <= 0.f)
		HandleDeath();
	
	if (DamageCauser->IsA(APowerUpActor::StaticClass()))
	{
		float IncomingHealth;

		FString CurrentHealthString;
		
		if(IncomingDamage)
		{
			IncomingHealth = IncomingDamage;
			
			if(CurrentHealth == MaxHealth)
			{
				IncomingHealth = 0.f;
				return IncomingHealth;				
			}
			
			if (IncomingDamage && CurrentHealth <= MaxHealth)
			{
				IncomingHealth = IncomingDamage;
				return IncomingHealth;			
			}


			if ((CurrentHealth+(-IncomingDamage)) >= MaxHealth)
			{
				IncomingHealth = 0.f;
				return 0.f;
			}

			return 0.f;
		}

	}

	if (!(DamageCauser->IsA(APowerUpActor::StaticClass())))
	{
		return IncomingDamage;
	}

	return 0.f;
	
}

void APlayerBase::OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Hook->GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (HookCurrentState::GrappleReturn == _HookCurrentState)
		return;

#ifdef _DEBUG_GRAPPLE
		UE_LOG(LogTemp, Warning, TEXT("collision type: %i"), AttachedComponentType);
#endif
	AttachedComponentType = OtherComp->GetCollisionObjectType();
	switch (AttachedComponentType)
	{
	case ECollisionChannel::ECC_GameTraceChannel2:
		{
			OnHookHitGrappleTo();
		}
		break;
		case ECollisionChannel::ECC_PhysicsBody:
		{
			OnHookHitGrappleInAttachedComponent(OtherComp);
		}
		break;
		default:
		{
			_HookCurrentState = HookCurrentState::GrappleReturn;				
		}
		break;
	}
		
#ifdef _DEBUG_GRAPPLE
		UE_LOG(LogTemp, Warning, TEXT("Collision type %i"), AttachedComponentType);
#endif
}
void APlayerBase::OnHookHitGrappleTo()
{
	FHitResult Hit;
	Hook->GetProjectileMovementComponent()->StopSimulating(Hit);
	GetCharacterMovement()->AirControl = 0.0f;
	InitialDistanceToHook = GetDistanceToHook();
	LaunchCharacter(GetActorUpVector() * HookOnAttachedLaunchForce, false, false);
	_HookCurrentState = HookCurrentState::GrappleTo;
}

void APlayerBase::OnHookHitGrappleInAttachedComponent(UPrimitiveComponent* OtherComp)
{
	UE_LOG(LogTemp, Error, TEXT("OnHookHitGrappleInAttachedComponent"));
	AttachedComponent = OtherComp;
	AttachedComponent->AttachToComponent(Hook->GetCollisionComponent(), FAttachmentTransformRules::KeepWorldTransform);
	
	if (AttachedComponent->GetOwner()->IsA(AAIBehaviorBase::StaticClass()))
	{
		const auto AIBB = Cast<AAIBehaviorBase>(AttachedComponent->GetOwner());
		if (AIBB)
		{
			AIBB->OnAttach();
		}
	}

	_HookCurrentState = HookCurrentState::GrappleReturn;
}


void APlayerBase::OnUpdatePlayerCustomTexture2DUniform(FViewUniformShaderParameters& ViewUniformShaderParameters)
{
	if (ShadowProjectionTexture->TextureReference.TextureReferenceRHI.IsValid())
	{
		ViewUniformShaderParameters.PawnCustomTexture = ShadowProjectionTexture->TextureReference.TextureReferenceRHI;
	}
	if (DepthProjectionTexture->TextureReference.TextureReferenceRHI.IsValid())
	{
		ViewUniformShaderParameters.PawnCustomTexture1 = DepthProjectionTexture->TextureReference.TextureReferenceRHI;
	}	
}

void APlayerBase::OnUpdatePlayerCustomCbUniform(TStaticArray<FVector4, 8>& DataPins)
{
	FMinimalViewInfo MinimalViewInfo;
	FMatrix View, Project, ViewProject;
	ShadowProjectionCamera->GetCameraView(UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), MinimalViewInfo);
	UGameplayStatics::GetViewProjectionMatrix(MinimalViewInfo, View, Project, ViewProject);
	const auto MVP = (View* Project).GetTransposed();
	const auto ViewMatrix = View.GetTransposed();
	DataPins[0] = FVector4(MVP.M[0][0], MVP.M[0][1], MVP.M[0][2], MVP.M[0][3]);
	DataPins[1] = FVector4(MVP.M[1][0], MVP.M[1][1], MVP.M[1][2], MVP.M[1][3]);
	DataPins[2] = FVector4(MVP.M[2][0], MVP.M[2][1], MVP.M[2][2], MVP.M[2][3]);
	DataPins[3] = FVector4(MVP.M[3][0], MVP.M[3][1], MVP.M[3][2], MVP.M[3][3]);

	DataPins[4] = GetActorLocation();//FVector4(ViewMatrix.M[0][0], ViewMatrix.M[0][1], ViewMatrix.M[0][2], ViewMatrix.M[0][3]);
	DataPins[5] = FVector4(ViewMatrix.M[1][0], ViewMatrix.M[1][1], ViewMatrix.M[1][2], ViewMatrix.M[1][3]);
	DataPins[6] = FVector4(ViewMatrix.M[2][0], ViewMatrix.M[2][1], ViewMatrix.M[2][2], ViewMatrix.M[2][3]);
	DataPins[7] = FVector4(ViewMatrix.M[3][0], ViewMatrix.M[3][1], ViewMatrix.M[3][2], ViewMatrix.M[3][3]);
}

void APlayerBase::LoadPlayerData()
{
	
	const auto GameInstance = Cast<UHookGameInstance>(GetGameInstance());

	if(GameInstance)
	{
		if(GameInstance->bLevelIsLoaded)
		{
			GameInstance->bLevelIsLoaded = false;
			GameInstance->LoadPlayerData(this);
		}

		if(GameInstance->bCheckpointIsLoaded)
		{
			GameInstance->bCheckpointIsLoaded = false;
			GameInstance->LoadPlayerCheckpointData(this);
		}
	}

}

void APlayerBase::OnFireReleased()
{
	
}

void APlayerBase::OnUsePressed()
{
#ifdef _DEBUG_INPUT	
	UE_LOG(LogTemp, Warning, TEXT("On use pressed"));
#endif
	UE_LOG(LogTemp, Warning, TEXT("On use pressed"));
	if (SkillTreeFCRI)
		SkillTreeFCRI->AddExperience(500);
	SetActorLocation(GetActorLocation() - FVector(0.f, 0.f, 10.f));
}

void APlayerBase::OnUseReleased()
{
}

void APlayerBase::OnSkillTreePressed()
{
	if (SkillTreeFCRI)
		SkillTreeFCRI->OnSkillTreePressed();
}

void APlayerBase::OnJumpPressed()
{
	if (!CompareAttackState(AttackState::Default))
		return;
	
	Jump();
}

void APlayerBase::OnJumpReleased()
{
	StopJumping();
}

void APlayerBase::OnRightClickPressed()
{
	TryEnterHookFireTransition();
}

void APlayerBase::OnRightClickReleased()
{
	HookDetach();
}

void APlayerBase::CursorUpdate(const float DeltaTime)
{
	const auto PlayerController = Cast<APlayerController>(GetController());
	ReturnIfNull(PlayerController);

	if (bInMenu)
	{
		SetMouseCursor(PlayerController, EMouseCursor::Default);
		return;
	}

	FHitResult Hit;	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectQuery;
	ObjectQuery.Push(EObjectTypeQuery::ObjectTypeQuery8);
	const bool bHit = PlayerController->GetHitResultUnderCursorForObjects(ObjectQuery, true, Hit);

	const auto ImpactPoint = Hit.ImpactPoint;
	const auto PlayerLocation = GetActorLocation();
	const auto PlayerToImpact = ImpactPoint - PlayerLocation;

	if (bHit && PlayerToImpact.Size() < HookMaxRange)
		SetMouseCursor(PlayerController, EMouseCursor::Crosshairs);
	else
		SetMouseCursor(PlayerController, EMouseCursor::Hand);
	
}

bool APlayerBase::CanTransitionToAttack2() const
{
	return CompareAttackStateTransition(AttackStateTransition::Attack2);
}

void APlayerBase::OnHookFire()
{
#ifdef _DEBUG_GRAPPLE	
	UE_LOG(LogTemp, Warning, TEXT("OnHookFireCalled"));
#endif
	HookFire();
}

bool APlayerBase::IsSliding() const
{
	return GetCharacterMovement()->CustomMovementMode == CustomMovementModes::Sliding;
}

bool APlayerBase::IsAttacking() const
{
	return _CustomMovementMode == CustomMovementModes::Attacking;
}

bool APlayerBase::IsGrappleFired() const
{
	return _HookCurrentState == HookCurrentState::GrappleFired;
}

bool APlayerBase::IsGrappleReturning() const
{
	return _HookCurrentState == HookCurrentState::GrappleReturn;
}

bool APlayerBase::IsGrapplingTo() const
{
	return _HookCurrentState == HookCurrentState::GrappleTo;
}

bool APlayerBase::IsGrappling() const
{
	return _CustomMovementMode == CustomMovementModes::Grappling;
}

bool APlayerBase::IsHookInAir()
{
	return _HookCurrentState == HookCurrentState::GrappleInAir;
}

void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &THIS::OnMoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &THIS::OnMoveRight);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &THIS::OnFirePressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &THIS::OnFireReleased);
	PlayerInputComponent->BindAction(TEXT("RightClick"), IE_Pressed, this, &THIS::OnRightClickPressed);
	PlayerInputComponent->BindAction(TEXT("RightClick"), IE_Released, this, &THIS::OnRightClickReleased);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &THIS::OnJumpPressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &THIS::OnJumpReleased);

	PlayerInputComponent->BindAction(TEXT("HealthDebug"), IE_Pressed, this, &THIS::OnUsePressed);
	PlayerInputComponent->BindAction(TEXT("HealthDebug"), IE_Released, this, &THIS::OnUseReleased);
	PlayerInputComponent->BindAction(TEXT("SkillTree"), IE_Pressed, this, &THIS::OnSkillTreePressed);
	
	//Interact
	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &THIS::InteractWithNPC);

	//Save and load DEBUG
	PlayerInputComponent->BindAction(TEXT("DebugSave"), IE_Pressed, this, &THIS::SaveGame);

	PlayerInputComponent->BindAction(TEXT("DebugLoad"), IE_Pressed, this, &THIS::LoadGame);
}

bool APlayerBase::CanVault() const
{
	return _CustomMovementMode == CustomMovementModes::Vaulting;
}

bool APlayerBase::IsDead() const
{
	return CurrentHealth <= 0.f;
}

float APlayerBase::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

float APlayerBase::GetHookDelayPercent() const
{
	return HookDelayTimer / HookMaxDelay;
}

void APlayerBase::OnMoveForward(const float Value)
{

	if ((Controller != nullptr) && (Value != 0.0f) && !bPlayerIsTalking)
	{		
		const FRotator YawRotation(0, SpringArm->GetRelativeRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerBase::OnMoveRight(const float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !bPlayerIsTalking)
	{
		const FRotator YawRotation(0, SpringArm->GetRelativeRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

USkillTree* APlayerBase::GetSkillTree() const
{
	return SkillTreeFCRI;
}

void APlayerBase::DebugUpdate(const float DeltaTime) const
{
#ifdef _DEBUG_VAULTING
	DrawDebugCapsule(GetWorld(), GetActorLocation(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), GetCapsuleComponent()->GetScaledCapsuleRadius(), GetActorRotation().Quaternion(), FColor::Red, false, -1, 0, 2.f);
#endif
#ifdef _DEBUG_GRAPPLE
	UE_LOG(LogTemp, Warning, TEXT("Current state: %i"), _HookCurrentState);
#endif
#ifdef _DEBUG_ANIM_STATES
	const int LocomotionIndex = GetMesh()->GetAnimInstance()->GetStateMachineIndex(TEXT("Locomotion"));
	const auto StateName = GetMesh()->GetAnimInstance()->GetCurrentStateName(LocomotionIndex);
	UE_LOG(LogTemp, Warning, TEXT("state name: %s"), *StateName.ToString());
#endif
}

void APlayerBase::CreateComponents()
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	SpringArm->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArm);

	GrappleCable = CreateDefaultSubobject<UCableComponent>(TEXT("Grapple Cable"));
	
	ShadowProjectionCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Shadow Projection Camera"));
	DepthProjectionCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Depth Projection Camera"));
	ShadowProjectionCameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Shadow Projection Arm"));
	
	ShadowProjectionCameraArm->SetupAttachment(RootComponent);
	ShadowProjectionCamera->SetupAttachment(ShadowProjectionCameraArm);
	DepthProjectionCamera->SetupAttachment(ShadowProjectionCameraArm);

	VaultCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Vault Capsule"));
	VaultBoxExtent = CreateDefaultSubobject<UBoxComponent>(TEXT("Vault BoxExtent"));

	VaultCapsule->SetCapsuleHalfHeight(CapsuleDefaultStandingHeight);
	VaultCapsule->SetCapsuleRadius(CapsuleDefaultRadius);
	VaultBoxExtent->SetupAttachment(RootComponent);
	VaultCapsule->SetupAttachment(VaultBoxExtent);
	VaultBoxExtent->bHiddenInGame = false;

	SkillTreeFCRI = CreateDefaultSubobject<USkillTree>(TEXT("Skill tree"));
	if(!SkillTreeFCRI)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create skilltree"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("created skilltree"));
	}
}


void APlayerBase::SaveGame()
{
	UHookGameInstance* GameInstanceRef = Cast<UHookGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ReturnIfNull(GameInstanceRef);
	GameInstanceRef->SaveGameData(this);
}

void APlayerBase::SaveLatestCheckpoint()
{
	UHookGameInstance* GameInstanceRef = Cast<UHookGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ReturnIfNull(GameInstanceRef);
	GameInstanceRef->SaveLatestCheckpoint(this);
}
void APlayerBase::LoadCheckpoint()
{
	UHookGameInstance* GameInstanceRef = Cast<UHookGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ReturnIfNull(GameInstanceRef);
	GameInstanceRef->LoadLatestCheckpoint(this);
}

void APlayerBase::LoadGame()
{
	UHookGameInstance* GameInstanceRef = Cast<UHookGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ReturnIfNull(GameInstanceRef);
	GameInstanceRef->LoadGameData(this);
}

float APlayerBase::GetPlayerToHookRotation() const
{
	const FRotator A = (GetDirectionToHook() / GetDistanceToHook()).Rotation();
	const FRotator B = GetActorRotation();
	FRotator Delta = A - B;
	Delta.Normalize();
	return FMath::ClampAngle(Delta.Yaw, -179.f, 179.f);
}

float APlayerBase::GetCurrentHealth() const
{
	return CurrentHealth;
}

float APlayerBase::GetMaxHealth() const
{
	return MaxHealth;
}

void APlayerBase::InteractionActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp && (OtherActor->IsA(AAIRobotSidekick::StaticClass())))
	{
#ifdef _DEBUG_INTERACTION
		UE_LOG(LogTemp, Warning, TEXT("Player is now in the interaction area and can interact"));
#endif
		PlayerCanInteract = true;
	}
}

void APlayerBase::InteractionNotActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex)
{
	if (PlayerCanInteract)
	{
		PlayerCanInteract = false;
#ifdef _DEBUG_INTERACTION
		UE_LOG(LogTemp, Warning, TEXT("Player is not in the interaction area anymore"));
#endif
	}
}

void APlayerBase::OnIncomingXP(int32 Amount) const
{
	SkillTreeFCRI->AddExperience(Amount); 
}

void APlayerBase::GeneratePlayerLines(UDataTable* PlayerLines)
{
	TArray<FName> PlayerOptions = PlayerLines->GetRowNames();

	for (auto It : PlayerOptions)
	{
		FDialogue* Dialog = RetrieveDialogue(PlayerLines, It);

		if (Dialog)
		{
			Questions.Add(Dialog->QuestionText);
		}
	}
	DialogueUIPtr->QuestionsForHUD = Questions;
	AvailableLines = PlayerLines;	
}

void APlayerBase::OnPlayerKnockBack(const FVector& KnockBackFrom, float KnockBackValue) const
{
	const auto PlayerLocation = GetActorLocation();
	const auto TargetToPlayer = PlayerLocation - KnockBackFrom;
	const auto Direction = TargetToPlayer / TargetToPlayer.Size();
	GetCharacterMovement()->Velocity += Direction * KnockBackValue;
}

void APlayerBase::Talk(FString Excerpt, TArray<FSubtitle>& Subtitles)
{
	TArray<FName> PlayerOptions = AvailableLines->GetRowNames();
	for (auto It : PlayerOptions)
	{
		FDialogue* Dialogue = RetrieveDialogue(AvailableLines, It);

		if (Dialogue && Dialogue->QuestionText == Excerpt)
		{
#ifdef _DEBUG_INTERACTION
			UE_LOG(LogTemp, Error, TEXT("Dialogue found and excerpt"));
#endif
			AudioComp->SetSound(Dialogue->DialogueSoundEffect);
			AudioComp->Play();

			Subtitles = Dialogue->Subtitles;
			
			if (DialogueUIPtr && AssociatedNPC && Dialogue->bShouldAIRespond)
			{				
				TArray<FSubtitle> SubtitlesToDisplay;

				float TotalSubsTime = 0.f;

				for (int32 i = 0; i < Subtitles.Num(); i++)
				{
					TotalSubsTime += Subtitles[i].SubtitleDisplayTime;
				}

				TotalSubsTime += SpeakerNPCDelay;
				AssociatedNPC->AnswerPlayerCharacter(It, SubtitlesToDisplay, TotalSubsTime);

			}
			else if (!Dialogue->bShouldAIRespond) InteractWithNPC();
			break;
		}
	}
}

void APlayerBase::SetInteractableNPC(AAIRobotSidekick* NPC)
{	
	InteractableNPC = NPC;	
	if(InteractableNPC)
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	else
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);	
}

void APlayerBase::DisplayInteractMessage()
{
#ifdef _DEBUG_INTERACTION
	UE_LOG(LogTemp, Warning, TEXT("DisplayInteractMessage"));
#endif
	const auto PlayerControllerReference = Cast<APlayerControllerBase>(Controller);
	ReturnIfNull(PlayerControllerReference);

	const auto PlayerHUDRef = PlayerControllerReference->HUD;
	ReturnIfNull(PlayerHUDRef);
	
	ReturnIfNull(InteractionWidget);
	ReturnIfNull(PlayerCanInteract);
						
	if(InteractableNPC && !bNPCFollowPlayer)
	{
		NPCPosition = InteractableNPC->GetActorLocation();
	}
				
	FVector2D PositionInViewport;
	PlayerControllerReference->ProjectWorldLocationToScreen(NPCPosition, PositionInViewport);
	
	const FVector2D SizeOfInteractHUD{ InteractSizeX,InteractSizeY };
	InteractionWidget->SetDesiredSizeInViewport(SizeOfInteractHUD);
	PositionInViewport.X -= 50;
	PositionInViewport.Y -= 90;
	InteractionWidget->SetPositionInViewport(PositionInViewport);										
}

void APlayerBase::OnVaultBoxBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult)
{
#ifdef _DEBUG_VAULTING
	UE_LOG(LogTemp, Warning, TEXT("Overlap"));
#endif
	TArray<AActor*> HitActor;
	VaultCapsule->GetOverlappingActors(HitActor);
	if (HitActor.Num())
		return;

	if (OtherComp->GetCollisionProfileName().ToString() == "WaterBodyCollision")
		return;
#ifdef _DEBUG_VAULTING
	UE_LOG(LogTemp, Warning, TEXT("OnVaultBoxBeginOverlap can vault"));
#endif _DEBUG_VAULTING

	const auto VaultCapsuleLocation = VaultCapsule->GetComponentLocation();
	const auto VaultCapsHalfHeight = VaultCapsule->GetScaledCapsuleHalfHeight()*2.f;
	const auto VaultCapsRadius = VaultCapsule->GetScaledCapsuleRadius();
	const auto TraceEnd = VaultCapsule->GetComponentLocation() + (FVector::DownVector * VaultCapsHalfHeight);
	FHitResult SphereHitResult;
	const TArray<AActor*> IgnoreActors;

#ifndef _DEBUG_VAULTING
	const auto bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), VaultCapsuleLocation, TraceEnd, VaultCapsRadius, ETraceTypeQuery::TraceTypeQuery1, true, IgnoreActors, EDrawDebugTrace::None, SphereHitResult, true, FLinearColor::Red);
#else
	const auto bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), VaultCapsuleLocation, TraceEnd, VaultCapsRadius, ETraceTypeQuery::TraceTypeQuery1, true, IgnoreActors, EDrawDebugTrace::ForDuration, SphereHitResult, true, FLinearColor::Red);
#endif

	if (!bHit)
		return;

	if (Dot3(SphereHitResult.ImpactNormal, FVector::UpVector) < 0.5f)
		return;
	
	if (GetCharacterMovement()->IsFalling())
		TryEnterVaultingTransition(VaultCapsule->GetComponentLocation());
}

void APlayerBase::InteractWithNPC()
{
	ReturnIfNull(PlayerCanInteract);
	
#ifdef _DEBUG_INTERACTION
		UE_LOG(LogTemp, Warning, TEXT("You pressed E to interact with the NPC"));
#endif
	bPlayerIsTalking = !bPlayerIsTalking;
	ToggleUI();
	if (bPlayerIsTalking && AssociatedNPC)
	{
		const FVector Location = AssociatedNPC->GetActorLocation();
		const FVector TargetLocation = GetActorLocation();

		AssociatedNPC->SetActorRotation((TargetLocation - Location).Rotation());
	}
}

void APlayerBase::ToggleUI()
{
	const auto PlayerControllerRef = Cast<APlayerControllerBase>(Controller);
	ReturnIfNull(PlayerControllerRef);
		
	if(bPlayerIsTalking)
	{
#ifdef _DEBUG_INTERACTION
		UE_LOG(LogTemp, Warning, TEXT("PlayerisNowTalking"));
#endif
		const auto PlayerControllerHUDRef = PlayerControllerRef->HUD;
		if (PlayerControllerHUDRef->IsInViewport())
		{
			if(DialogueUIPtr->IsInViewport()&&!bPlayerIsTalking)
			{				
				UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerControllerRef);
				PlayerControllerRef->bShowMouseCursor = true;
				PlayerControllerRef->bEnableClickEvents = true;
				PlayerControllerRef->bEnableClickEvents = true;
				PlayerControllerRef->bEnableMouseOverEvents = true;
				PlayerControllerRef->SetMouseCursorWidget(EMouseCursor::Crosshairs, WidgetCursorNotLocked);
				DialogueUIPtr->RemoveFromViewport();
#ifdef _DEBUG_INTERACTION
				UE_LOG(LogTemp, Warning, TEXT("DialogueUIHUDRemoved"));
#endif
			}	
			else
			{		
				const auto PlayerLines = AssociatedNPC->GetPlayerLines();		
				DialogueUIPtr->Show();
				UWidgetBlueprintLibrary::SetInputMode_GameAndUI(PlayerControllerRef);
				PlayerControllerRef->bShowMouseCursor = true;
				PlayerControllerRef->bEnableClickEvents = true;
				PlayerControllerRef->bEnableMouseOverEvents = true;	
			}
		}
	}else
	{	
		DialogueUIPtr->RemoveFromViewport();
	}		
}

FDialogue* APlayerBase::RetrieveDialogue(UDataTable* TableToSearch, FName RowName)
{
	if (!TableToSearch)
	{
		return nullptr;
	}

	FString ContextString;
	return TableToSearch->FindRow<FDialogue>(RowName, ContextString);
}