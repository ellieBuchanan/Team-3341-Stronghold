#include "Drive.h"
#include "../RobotMap.h"
#include <math.h>
#include "Commands/ArcadeDrive.h"
#include "../CommandBase.h"

#define max(x, y) (((x) > (y)) ? (x) : (y))

Drive::Drive() :
    Subsystem("Drive"), left(new Talon(DRIVE_LEFT)), right(new Talon(DRIVE_RIGHT)),
    encoderLeft(new Encoder(ENCODER_LEFT_1, ENCODER_LEFT_2)),
    encoderRight(new Encoder(ENCODER_RIGHT_1, ENCODER_RIGHT_2)), mult(1.0),
    rotationCurve(true)

{
    encoderLeft->SetDistancePerPulse(1.0);
    encoderRight->SetDistancePerPulse(1.0);
}

void Drive::setMult(float m)
{
    mult = m;
}

void Drive::ResetEncoders()
{
    encoderLeft->Reset();
    encoderRight->Reset();
}

void Drive::arcadeDrive(float moveValue, float rotateValue)
{
    float leftMotorOutput;
    float rightMotorOutput;

    moveValue = Drive::Limit(moveValue, 1.0) * mult;
    rotateValue = Drive::Limit(rotateValue, 1.0);

    // Standard ArcadeDrive algorithm from Google
    if(moveValue > 0.0)
    {
        if(rotateValue > 0.0)
        {
            leftMotorOutput = moveValue - rotateValue;
            rightMotorOutput = max(moveValue, rotateValue);
        }
        else
        {
            leftMotorOutput = max(moveValue, -rotateValue);
            rightMotorOutput = moveValue + rotateValue;
        }
    }
    else
    {
        if(rotateValue > 0.0)
        {
            leftMotorOutput = - max(-moveValue, rotateValue);
            rightMotorOutput = moveValue + rotateValue;
        }
        else
        {
            leftMotorOutput = moveValue - rotateValue;
            rightMotorOutput = - max(-moveValue, -rotateValue);
        }
    }

    float limitedL = Drive::Limit(leftMotorOutput, 1.0);
    float limitedR = Drive::Limit(rightMotorOutput, 1.0);

    left->Set(limitedL);
    right->Set(limitedR);
}

float Drive::Limit(float num, float max)
{
    if(num > max)
        return max;

    if(num < -max)
        return -max;

    return num;
}

double Drive::GetDistance()
{
    // Average of both encoders (must negate to get proper direction)
    // TODO: test to see if negation is necessary
    return 
    (
        (double) ((encoderLeft->Get()) / 1090.0) -
        (double) ((encoderRight->Get()) / 1090.0)
    ) / -2.0;
}

double Drive::GetRate()
{
    // Average of both encoder rates (must negate to get proper direction)
    // TODO: test to see if negation is necessary
    return 
    (
        (double) ((encoderLeft->GetRate()) / 1090.0) - 
        (double) ((encoderRight->GetRate()) / 1090.0)
    ) / -2.0;
                 
}

void Drive::toggleRotationCurve()
{
    isUsingRotationCurve = !isUsingRotationCurve;
}

bool Drive::usingRotationCurve()
{
    return isUsingRotationCurve;
}

void Drive::InitDefaultCommand()
{
    SetDefaultCommand(new ArcadeDrive());
}
