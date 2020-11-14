/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.hpp
Purpose: generate camera and write code in here not cpp
Language: c++
Platform: win64
Project: yongmin.cho_CS300_1
Author: walt cho, yongmin.cho, 180003919
Creation date: 2019.10.04
End Header --------------------------------------------------------*/
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
	LOOKDOWN, 
	LOOKUP, 
	LOOKLEFT, 
	LOOKRIGHT,
	INITIALIZE
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    //LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    //keyboard input
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
		float velocity = MovementSpeed * deltaTime;
		float angVel = velocity * 40.f;

		//if (direction == INITIALIZE) {
		//	Position = initPos_;
		//	Yaw = initYaw_;
		//	Pitch = initPitch_;
		//}

		if (direction == LOOKUP)
			Pitch += angVel;

		if (direction == LOOKDOWN)
			Pitch -= angVel;

		if (direction == LOOKRIGHT)
			Yaw += angVel;

		if (direction == LOOKLEFT)
			Yaw -= angVel;

		//if (direction == FORWARD)
		//	position_ += front_ * velocity;

		//if (direction == BACKWARD)
		//	position_ -= front_ * velocity;

		/*if (direction == LEFT)
			position_ -= right_ * velocity;

		if (direction == RIGHT)
			position_ += right_ * velocity;*/

		if (Pitch >= 90.f)
			Pitch = 89.9f;

		else if (Pitch <= -90.f)
			Pitch = -89.9f;

    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;

		    updateCameraVectors();
        
    }

    // limit 
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }
    void updateCameraVectors()
    {
        glm::vec3 front;
        float pitch = Pitch;
        float yaw = Yaw;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Front = glm::normalize(front);
        //// Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};