#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include <iostream>

enum class Camera_Walk {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default values
const float YAW = -90.0f;
const float PITCH = 0.0f;
//const float SPEED = 2.5f;
//const float TURN_SENSITIVITY = 0.5f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:

    glm::vec3 CameraPos;
    glm::vec3 PivotPos;
    glm::vec3 WorldUp;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    float Yaw;
    float Pitch;
    // camera options
    //const float MovementSpeed = 2.5f;;
    //const float TurnRotateSensitivity = 0.5f;
    //const float WalkRotateSensitivity = 0.5f;
    float Zoom;

    float DistanceToPivot;
    const float MinDistance = 0.1f;
    const float MaxDistance = 15.0f;

    // constructor with vectors
    Camera(glm::vec3 pivotPos = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 cameraOffset = glm::vec3(0.0f, 0.0f, 6.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
        float yaw = YAW, 
        float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), Zoom(ZOOM), PivotPos(glm::vec3(0.0f, 0.0f, 0.0f))
    {
        PivotPos = pivotPos;
        CameraPos = pivotPos + cameraOffset;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;

        DistanceToPivot = glm::distance(CameraPos, PivotPos);
        updateCamDirVec();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(CameraPos, CameraPos + Front, Up);
        //return MyLookAt(Position, Position + Front, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void SwitchToWalkMode(bool isWalkMode)
    {
        if (isWalkMode)
        {
            EnableWalkMode();
        }
        else
        {
            DisableWalkMode();
        }
    }


    void FocusToOrigin()
    {
        PivotPos = glm::vec3(0.0f, 0.0f, 0.0f);
        DistanceToPivot = 6.0f;
        CameraPos = PivotPos - Front * DistanceToPivot;
    }


    //Turntable Mode: Camera rotate around pivot
    // --------------------------------------------------------------------------------------
    void TurntableRotate(float xoffset, float yoffset)
    {
        float factor = 0.5f;
        xoffset *= factor;
        yoffset *= factor;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped

        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        updateCamDirVec();
        updateCamPosVecTurntable();
        //std::cout << "Yaw: " << Yaw << " Pitch: " << Pitch << std::endl;
        //debugLogVec3(Front);
    }

     
    void TurntableTranslate(float xoffset, float yoffset)
    {
        float factor = 0.5f * 0.01f;
        xoffset *= factor;
        yoffset *= factor;

        glm::vec3 tempPivotPos = PivotPos;
        PivotPos += (xoffset * Right + yoffset * Up);

        updateCamPosVecTurntable();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void TurntableZoom(float yoffset)
    {
        float dynamicFac = DistanceToPivot * 0.2f + 0.1f;

        DistanceToPivot -= dynamicFac * yoffset;

        if (DistanceToPivot < MinDistance)
        {
            DistanceToPivot = MinDistance;
        }
        else if (DistanceToPivot > MaxDistance)
        {
            DistanceToPivot = MaxDistance;
        }

        updateCamPosVecTurntable();

        //debugLog(DistanceToPivot);
        //debugLogVec3(CameraPos);
    }

    // Walk Mode: Control the camera like FPS game
    // --------------------------------------------------------------------------------------
    void WalkTranslate(Camera_Walk dir, float deltaTime)
    {
        float velocity = 5.0f * deltaTime;
        if (dir == Camera_Walk::FORWARD)
            PivotPos += Front * velocity;
        if (dir == Camera_Walk::BACKWARD)
            PivotPos -= Front * velocity;
        if (dir == Camera_Walk::LEFT)
            PivotPos -= Right * velocity;
        if (dir == Camera_Walk::RIGHT)
            PivotPos += Right * velocity;
        if (dir == Camera_Walk::UP)
            PivotPos += WorldUp * velocity;
        if (dir == Camera_Walk::DOWN)
            PivotPos -= WorldUp * velocity;

        updateCamPosVecWalk();
    }

    void WalkRotate(float xoffset, float yoffset)
    {
        float walkRotateSensitivity = 0.15f;
        xoffset *= walkRotateSensitivity;
        yoffset *= walkRotateSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped

        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCamDirVec();
    }

    void WalkZoom(float yoffset)
    {
        float const zoomSensitivity = 2.0f;
        Zoom -= (float)yoffset * zoomSensitivity;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 90.0f)
            Zoom = 90.0f;
    }

private:
    void EnableWalkMode()
    {
        PivotPos = CameraPos;
    }

    void DisableWalkMode()
    {
        PivotPos = CameraPos + DistanceToPivot * Front;
    }

    void updateCamPosVecTurntable()
    {
        CameraPos = PivotPos - DistanceToPivot * Front;
    }

    void updateCamPosVecWalk()
    {
        CameraPos = PivotPos;
    }

    void updateCamDirVec()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));//感觉这里的Yaw，Pitch等在Turntable中没有用，但是如果要做两种模式的切换的画，需要考虑到。
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }

    void debugLog(glm::vec3 v)
    {
        std::cout << v.x << " " << v.y << " " << v.z << std::endl;
    }

    void debugLog(float v)
    {
        std::cout << v << std::endl;
    }
};
#endif