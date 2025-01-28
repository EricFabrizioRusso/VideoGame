// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraShakeComponent.h"


UCameraShakeComponent::UCameraShakeComponent() {


    OscillationDuration = 0.20f;  // Duración del shake
    OscillationBlendInTime = 0.02f;
    OscillationBlendOutTime = 0.02f;

    RotOscillation.Pitch.Amplitude = 1.0f;
    RotOscillation.Pitch.Frequency = 5.0f;

    RotOscillation.Yaw.Amplitude = 0.5f;  // Amplitud del shake en el eje Yaw
    RotOscillation.Yaw.Frequency = 10.0f; // Frecuencia del shake en el eje Yaw

    RotOscillation.Roll.Amplitude = 0.0f;
    RotOscillation.Roll.Frequency = 0.0f;

 

}


void UCameraShakeComponent::SetOscillationDuration(float NewDuration)
{
    OscillationDuration = NewDuration;
}