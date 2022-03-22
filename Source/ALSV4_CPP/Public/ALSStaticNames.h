// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning

#pragma once

namespace ALS
{
	namespace AnimInstance
	{
		static const FName NAME_BasePose_CLF(TEXT("BasePose_CLF"));
		static const FName NAME_BasePose_N(TEXT("BasePose_N"));
		static const FName NAME_Enable_FootIK_R(TEXT("Enable_FootIK_R"));
		static const FName NAME_Enable_FootIK_L(TEXT("Enable_FootIK_L"));
		static const FName NAME_Enable_HandIK_L(TEXT("Enable_HandIK_L"));
		static const FName NAME_Enable_HandIK_R(TEXT("Enable_HandIK_R"));
		static const FName NAME_Enable_Transition(TEXT("Enable_Transition"));
		static const FName NAME_FootLock_L(TEXT("FootLock_L"));
		static const FName NAME_FootLock_R(TEXT("FootLock_R"));
		static const FName NAME_Grounded___Slot(TEXT("Grounded Slot"));
		static const FName NAME_Layering_Arm_L(TEXT("Layering_Arm_L"));
		static const FName NAME_Layering_Arm_L_Add(TEXT("Layering_Arm_L_Add"));
		static const FName NAME_Layering_Arm_L_LS(TEXT("Layering_Arm_L_LS"));
		static const FName NAME_Layering_Arm_R(TEXT("Layering_Arm_R"));
		static const FName NAME_Layering_Arm_R_Add(TEXT("Layering_Arm_R_Add"));
		static const FName NAME_Layering_Arm_R_LS(TEXT("Layering_Arm_R_LS"));
		static const FName NAME_Layering_Hand_L(TEXT("Layering_Hand_L"));
		static const FName NAME_Layering_Hand_R(TEXT("Layering_Hand_R"));
		static const FName NAME_Layering_Head_Add(TEXT("Layering_Head_Add"));
		static const FName NAME_Layering_Spine_Add(TEXT("Layering_Spine_Add"));
		static const FName NAME_Mask_AimOffset(TEXT("Mask_AimOffset"));
		static const FName NAME_Mask_LandPrediction(TEXT("Mask_LandPrediction"));
		static const FName NAME__ALSCharacterAnimInstance__RotationAmount(TEXT("RotationAmount"));
		static const FName NAME_VB___foot_target_l(TEXT("VB foot_target_l"));
		static const FName NAME_VB___foot_target_r(TEXT("VB foot_target_r"));
		static const FName NAME_W_Gait(TEXT("W_Gait"));
		static const FName NAME__ALSCharacterAnimInstance__root(TEXT("root"));
	}

	namespace BaseCharacter
	{
		static const FName NAME_FP_Camera(TEXT("FP_Camera"));
		static const FName NAME_Pelvis(TEXT("Pelvis"));
		static const FName NAME_RagdollPose(TEXT("RagdollPose"));
		static const FName NAME_RotationAmount(TEXT("RotationAmount"));
		static const FName NAME_YawOffset(TEXT("YawOffset"));
		static const FName NAME_pelvis(TEXT("pelvis"));
		static const FName NAME_root(TEXT("root"));
		static const FName NAME_spine_03(TEXT("spine_03"));
	}

	namespace CameraManager
	{
		static const FName NAME_CameraBehavior(TEXT("CameraBehavior"));
		static const FName NAME_CameraOffset_X(TEXT("CameraOffset_X"));
		static const FName NAME_CameraOffset_Y(TEXT("CameraOffset_Y"));
		static const FName NAME_CameraOffset_Z(TEXT("CameraOffset_Z"));
		static const FName NAME_Override_Debug(TEXT("Override_Debug"));
		static const FName NAME_PivotLagSpeed_X(TEXT("PivotLagSpeed_X"));
		static const FName NAME_PivotLagSpeed_Y(TEXT("PivotLagSpeed_Y"));
		static const FName NAME_PivotLagSpeed_Z(TEXT("PivotLagSpeed_Z"));
		static const FName NAME_PivotOffset_X(TEXT("PivotOffset_X"));
		static const FName NAME_PivotOffset_Y(TEXT("PivotOffset_Y"));
		static const FName NAME_PivotOffset_Z(TEXT("PivotOffset_Z"));
		static const FName NAME_RotationLagSpeed(TEXT("RotationLagSpeed"));
		static const FName NAME_Weight_FirstPerson(TEXT("Weight_FirstPerson"));
	}

	namespace Footstep
	{
		static const FName NAME_Mask_FootstepSound(TEXT("Mask_FootstepSound"));
		static const FName NAME_FootstepType(TEXT("FootstepType"));
		static const FName NAME_Foot_R(TEXT("Foot_R"));
	}

	namespace MantleComponent
	{
		static const FName NAME_MantleEnd(TEXT("MantleEnd"));
		static const FName NAME_MantleUpdate(TEXT("MantleUpdate"));
		static const FName NAME_MantleTimeline(TEXT("MantleTimeline"));
		static const FName NAME_IgnoreOnlyPawn(TEXT("IgnoreOnlyPawn"));
	}
}