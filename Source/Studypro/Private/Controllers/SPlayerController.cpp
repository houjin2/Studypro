// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/SPlayerController.h"

ASPlayerController::ASPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
    //������ Tick�� true���� �Է��� ó�� ����
}

void ASPlayerController::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameOnly InputModeGameOnly;   //�÷��̽� �ٷ� �����ϼ� �ְ� ��
    SetInputMode(InputModeGameOnly);
}



